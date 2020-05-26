#pragma once
#include <functional>
#include <iomanip>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "circle_buffer.hpp"
#include "defination.hpp"
#include "ipv4_addr.hpp"
#include "logger.hpp"
#include "mac_addr.hpp"
#include "protocol.hpp"
#include "utils.hpp"

namespace mstack {

template <typename CurrentPacketType, typename UpperPacketType>
class base_hook_funcs {
public:
    template <typename DEV>
    void hook_register_dev(DEV& dev)
    {
        return;
    }

    void
    hook_register_protocol(protocol_interface<CurrentPacketType>& in_protocol)
    {
        DLOG(INFO) << "[REGISTER] " << in_protocol.tag();
        return;
    }

    void
    hook_dispatch(std::optional<CurrentPacketType>& in_packet)
    {
        in_packet.reset();
        return;
    }

    void hook_gather(std::optional<CurrentPacketType>& in_packet)
    {
        in_packet.reset();
        return;
    }

    std::optional<UpperPacketType>
    hook_forward_to_upper(std::optional<CurrentPacketType> in_packet)
    {
        return std::nullopt;
    }

    std::optional<CurrentPacketType>
    hook_forward_from_upper(std::optional<UpperPacketType> in_packet)
    {
        return std::nullopt;
    }
};

template <typename CurrentPacketType, typename UpperPacketType,
    typename HookFuncs = base_hook_funcs<CurrentPacketType, UpperPacketType>>
class layer {
private:
    using packet_provider_current_layer_type = std::function<std::optional<CurrentPacketType>(void)>;
    using packet_provider_upper_layer_type = std::function<std::optional<UpperPacketType>(void)>;
    using packet_receiver_current_layer_type = std::function<void(CurrentPacketType)>;
    using packet_receiver_upper_layer_type = std::function<void(UpperPacketType)>;

    std::optional<packet_receiver_upper_layer_type> _forward_to_upper_layer_func;
    std::optional<packet_provider_upper_layer_type> _packet_provider_upper_layer_func;

    std::unordered_map<int, packet_receiver_current_layer_type> _protocols;
    std::vector<packet_provider_current_layer_type> _packet_providers;

    circle_buffer<CurrentPacketType> packet_queue;

    HookFuncs hook_funcs;

private:
    layer() = default;
    ~layer() = default;

public:
    layer(const layer&) = delete;
    layer(layer&&) = delete;
    layer& operator=(const layer&) = delete;
    layer& operator=(layer&&) = delete;

    static layer& instance()
    {
        static layer instance;
        return instance;
    }
    template <typename DEV>
    void register_dev(DEV& dev)
    {
        this->hook_funcs.hook_register_dev(dev);

        dev.register_provider_func([&]() { return std::move(instance().gather_packet()); });
        dev.register_receiver_func([&](CurrentPacketType in_packet) {
            instance().receive(std::move(in_packet));
        });
    }

    template <typename OtherLayer>
    void register_upper_layer(OtherLayer& layer)
    {
        _forward_to_upper_layer_func = [&layer](UpperPacketType packet) { layer.receive(std::move(packet)); };
        _packet_provider_upper_layer_func = [&layer]() { return layer.gather_packet(); };
    }

    void register_protocol(protocol_interface<CurrentPacketType>& protocol)
    {

        this->hook_funcs.hook_register_protocol(protocol);

        _packet_providers.push_back([&protocol]() { return protocol.gather_packet(); });
        _protocols[protocol.proto()] = [&protocol](CurrentPacketType packet) { protocol.receive(std::move(packet)); };

        protocol.register_forward_layer([&](CurrentPacketType packet) { this->_dispatch(std::move(packet)); });
        DLOG(INFO) << "[REGISTER PROTOCOL] " << protocol.proto();
    }

    void receive(CurrentPacketType in_packet)
    {

        std::optional<CurrentPacketType> in_packet_ = std::optional<CurrentPacketType>(std::move(in_packet));

        std::optional<int> proto = get_proto<CurrentPacketType>(in_packet_);

        if (!proto) {
            DLOG(ERROR) << "[NULL PROTO]";
            return;
        }

        // forward to upper layer
        if (this->_protocols.find(proto.value()) == this->_protocols.end()) {
            if (_forward_to_upper_layer_func) {
                DLOG(INFO) << "[FORWARD PACKET] " << std::setiosflags(std::ios::uppercase) << std::hex << proto.value();

                std::optional<UpperPacketType> forward_packet = this->hook_funcs.hook_forward_to_upper(std::move(in_packet_));

                if (forward_packet) {
                    _forward_to_upper_layer_func.value()(std::move(forward_packet.value()));
                } else {
                    DLOG(INFO) << "[FORWARD ERROR]";
                }

            } else {
                DLOG(ERROR) << "[UNKNOWN PROTOCOL] " << std::setiosflags(std::ios::uppercase) << std::hex << proto.value();
            }
            return;
        }

        this->hook_funcs.hook_dispatch(in_packet_);

        this->_dispatch(std::move(in_packet_));
    }

    void _dispatch(std::optional<CurrentPacketType> in_packet)
    {

        if (!in_packet)
            return;

        std::optional<int> proto = get_proto<CurrentPacketType>(in_packet);

        if (!proto) {
            DLOG(ERROR) << "[NULL PROTO]";
            return;
        }

        this->_protocols[proto.value()](std::move(in_packet.value()));
    }

    std::optional<CurrentPacketType> gather_packet()
    {

        if (this->packet_queue.empty()) {
            for (auto packet_provider : this->_packet_providers) {
                std::optional<CurrentPacketType> in_packet = packet_provider();
                if (in_packet) {
                    packet_queue.push_back(std::move(in_packet.value()));
                }
            }
            // LOG(INFO) << "[TEST1]";
            if (_packet_provider_upper_layer_func) {
                // LOG(INFO) << "[TEST2]";
                std::optional<UpperPacketType> in_packet = _packet_provider_upper_layer_func.value()();
                // LOG(INFO) << "[TEST3]";
                std::optional<CurrentPacketType> in_packet_ = this->hook_funcs.hook_forward_from_upper(std::move(in_packet));
                // LOG(INFO) << "[TEST4]";
                if (in_packet_) {
                    // LOG(INFO) << "[TEST5]";
                    packet_queue.push_back(std::move(in_packet_.value()));
                }
            }
        }

        std::optional<CurrentPacketType> out_packet = std::move(this->packet_queue.pop_front());

        this->hook_funcs.hook_gather(out_packet);
        // LOG(INFO) << "[TEST6]";
        return std::move(out_packet);
    }
};

}; // namespace mstack
#pragma once
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>

#include "circle_buffer.hpp"
#include "mac_addr.hpp"
#include "ipv4_addr.hpp"
#include "logger.hpp"
#include "protocol.hpp"
#include "utils.hpp"
#include "defination.hpp"

namespace mstack {


template <typename CurrentPacketType>
class base_hook_funcs {
public:
    template<typename DEV>
    void hook_register_dev(DEV& dev) {
        return dev;
    }

    virtual void
    hook_register_protocol(protocol_interface<CurrentPacketType>& protocol)
    {
        DLOG(INFO) << "[REGISTER] " << protocol.tag();
        return;
    }

    virtual void
    hook_dispatch(std::optional<CurrentPacketType>& packet)
    {
        return;
    }

    template<typename OtherPacketType>
    std::optional<OtherPacketType> hook_gather(std::optional<CurrentPacketType> packet)
    {
        return std::move(std::nullopt);
    }
};

template <typename OtherPacketType, typename CurrentPacketType,
    typename HookFuncs = base_hook_funcs<CurrentPacketType>>
class layer {
private:
    using packet_provider_type = std::function<std::optional<CurrentPacketType>(void)>;
    using packet_receiver_type = std::function<void(CurrentPacketType)>;
    
    std::optional<packet_receiver_type> _forward_to_layer_func;
    std::unordered_map<int, packet_receiver_type> _protocols;
    std::vector<packet_provider_type> _packet_providers;
    circle_buffer<OtherPacketType> packet_queue;
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
        dev.register_receiver_func([&](raw_packet raw_packet) { 
            instance().receive(std::move(raw_packet)); 
        });
    }

    template <typename OtherLayer>
    void register_forward_layer(OtherLayer& layer){
        _forward_to_layer_func = [&layer](CurrentPacketType packet){ layer.receive(std::move(packet)); };
    }

    void register_protocol(protocol_interface<CurrentPacketType>& protocol)
    {

        this->hook_funcs.hook_register_protocol(protocol);

        _packet_providers.push_back([&protocol]() { return protocol.gather_packet(); });
        _protocols[protocol.proto()] = [&protocol](CurrentPacketType packet) { protocol.receive(std::move(packet)); };

        protocol.register_forward_layer([&](CurrentPacketType packet){ this->_dispatch(std::move(packet)); });
        DLOG(INFO) << "[REGISTER PROTOCOL] " << protocol.proto();

    }

    
    void receive(OtherPacketType packet)
    {  

        std::optional<CurrentPacketType> packet_ = make_packet<CurrentPacketType>(std::move(packet));

        std::optional<int> proto = get_proto<CurrentPacketType>(packet_);
        if (!proto) {
            DLOG(ERROR) << "[UNKOWN PROTO]";
            return;
        }

        if (this->_protocols.find(*proto) == this->_protocols.end()) {
            if (_forward_to_layer_func) {
                DLOG(INFO) << "[FORWARD PACKET] " << std::setiosflags(std::ios::uppercase) << std::hex << packet_->_proto;
                _forward_to_layer_func.value()(std::move(packet_.value()));
            } else {
                DLOG(ERROR) << "[UNKNOWN PROTOCOL] " << std::setiosflags(std::ios::uppercase) << std::hex << *proto;
            }
            return;
        }

        this->hook_funcs.hook_dispatch(packet_);

        this->_dispatch(std::move(packet_));
    }

    void _dispatch(std::optional<CurrentPacketType> packet)
    {

        if (!packet)
            return;

        std::optional<int> proto = get_proto<CurrentPacketType>(packet);

        if(!proto) {
            DLOG(ERROR) << "[UNKOWN PROTO]";
            return;
        }

        this->_protocols[*proto](std::move(packet.value()));
    }

    std::optional<OtherPacketType> gather_packet()
    {

        if (this->packet_queue.empty()) {
            for (auto packet_provider : this->_packet_providers) {
                std::optional<CurrentPacketType> packet = packet_provider();
                if (packet) {
                    std::optional<OtherPacketType> packet_ = this->hook_funcs.hook_gather(std::move(packet));
                    
                    if(packet_) {
                        packet_queue.push_back(std::move(packet_.value()));
                    }
                }
            }
        }

        if (this->packet_queue.empty()) {
            return std::nullopt;
        }

        OtherPacketType packet = std::move(this->packet_queue.pop_front()); 

        return std::make_optional<OtherPacketType>(std::move(packet));
    }
};

}; // namespace mstack
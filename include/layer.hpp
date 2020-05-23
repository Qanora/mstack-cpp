#pragma once
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "circle_buffer.hpp"
#include "ethernet.hpp"
#include "logger.hpp"
#include "packet.hpp"
#include "protocol.hpp"
#include "utils.hpp"

namespace mstack {

class raw_packet {
public:
    using raw_proto = int;
    raw_proto proto; // TUNTAP_DEV
    packet payload;
};

class l2_packet {
    using l2_proto = int;
    std::optional<mac_addr> remote_mac_addr;
    l2_proto proto;
    packet payload;
};

// class l3_packet {
//     ip_addr remote_ip_addr;
//     l3_proto proto;
//     buffer payload
// };

// class l4_packet {
//     ip_addr remote_ip_addr;
//     port remote_port;
//     l4_proto proto;
//     buffer payload;
// };

template <typename CurrentPacketType>
class base_hook_funcs {
public:
    virtual std::optional<protocol_interface<CurrentPacketType>>
    hook_register(std::optional<protocol_interface<CurrentPacketType>> protocol)
    {
        return protocol;
    }
    virtual std::optional<CurrentPacketType>
    hook_dispatch(std::optional<CurrentPacketType> packet)
    {
        return packet;
    }
    virtual std::optional<CurrentPacketType>
    hook_gather(std::optional<CurrentPacketType> packet)
    {
        return packet;
    }
};

template <typename OtherPacketType, typename CurrentPacketType,
    typename HookFuncs = base_hook_funcs<CurrentPacketType>>
class layer {
private:
    using packet_provider_type = std::function<std::optional<OtherPacketType>(void)>;
    using packet_receiver_type = std::function<void(CurrentPacketType)>;
    std::unordered_map<int, packet_receiver_type> _protocols;
    std::vector<packet_provider_type> _packet_providers;
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

    void register_packet_privder(protocol_interface<CurrentPacketType> protocol)
    {

        std::optional<protocol_interface<CurrentPacketType>> protocol_ = this->hook_funcs.hook_register(protocol);
        if (!protocol_)
            return;

        this->_packet_provider.push_back([&protocol_]() { return protocol_->gather_packet(); });
        _protocols[protocol_->proto] = [&protocol_](CurrentPacketType packet) { protocol_->receive(packet); };
    }

    void receive(OtherPacketType packet)
    {

        std::optional<int> proto = get_proto<CurrentPacketType>(packet);

        if (!proto) {
            DLOG(INFO) << "[UNKNOWN PACKET]";
            return;
        }
        if (this->_protocols.find(proto) == this->_protocols.end()) {
            DLOG(INFO) << "[UNKNOWN PROTOCOL]";
            return;
        }
        std::optional<CurrentPacketType> packet_ = make_packet<CurrentPacketType>(packet);

        packet_ = this->hook_funcs.hook_dispatch(packet_);

        this->_dispatch(packet_);
    }

    void _dispatch(std::optional<CurrentPacketType> packet)
    {

        if (!packet)
            return;

        std::optional<int> proto = get_proto<CurrentPacketType>(*packet);

        CHECK(this->_protocols.find(proto) != this->_protocols.end())
            << "[UNKNOW PROTCOL]";

        this->_protocols[proto].receive(packet);
    }

    std::optional<CurrentPacketType> gather_packet()
    {

        if (this->packet_queue.empty()) {
            for (auto packet_provider : this->__packet_providers) {
                std::optional<CurrentPacketType> packet = packet_provider();
                if (!packet) {
                    packet_queue.push_back(*packet);
                }
            }
        }

        if (this->packet_queue.empty()) {
            return std::nullopt;
        }

        std::optional<CurrentPacketType> packet = packet_queue.front();
        packet_queue.pop_front();

        packet = this->hook_funcs.hook_gather(packet);

        return packet;
    }
};

// class l3_hook : public base_hook_funcs<l3_packet> {
// public:
//     struct ipv4_reassemble {
//     };
// };

// class l3_layer<l2_packet, l3_packet, l3_hook> {
// };

// class l4_layer<l3_packet, l4_packet> {
// };

}; // namespace mstack
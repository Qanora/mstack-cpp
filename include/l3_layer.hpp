#pragma once
#include "layer.hpp"
#include "ipv4_addr.hpp"
#include <optional>
namespace mstack {



class l3_hook : public base_hook_funcs<l3_packet> {
public:
    template<typename DEV>
    void hook_register_dev(DEV& dev) {
        return;
    }
    
    virtual void
    hook_register_protocol(protocol_interface<l3_packet>& protocol)
    {
        DLOG(INFO) << "[REGISTER] " << protocol.tag();
        return;
    }

    virtual void
    hook_dispatch(std::optional<l3_packet>& packet)
    {
        return;
    }

    virtual std::optional<l2_packet>
    hook_gather(std::optional<l3_packet> packet)
    {
        return std::move(std::nullopt);
    }
};


using l3_layer = mstack::layer<l2_packet, l3_packet, l3_hook>;

template<>
std::optional<l3_packet> make_packet(l2_packet in_packet) {
    DLOG(INFO) << "[PACKET: L2 -> L3] " << std::setiosflags(std::ios::uppercase) << std::hex << in_packet._proto;
    return std::make_optional<l3_packet>(std::nullopt, std::nullopt, in_packet._proto, std::move(in_packet._payload));
}

template <>
std::optional<int> get_proto(std::optional<l3_packet>& packet)
{
    DLOG(INFO) << "[PROTO L3_PACKET] " << std::setiosflags(std::ios::uppercase) << std::hex << packet->_proto;
    return packet->_proto;
}

};

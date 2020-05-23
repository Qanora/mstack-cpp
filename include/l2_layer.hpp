#pragma once
#include "layer.hpp"
#include "tuntap.hpp"
#include "ethernet.hpp"
#include <iomanip>
#include <optional>

namespace mstack {

class l2_hook : public base_hook_funcs<l2_packet> {
    struct arpv4_request {
    };
    struct arpv4_cache {
    };
};

using l2_layer = mstack::layer<raw_packet, l2_packet, l2_hook>;

template<>
std::optional<l2_packet> make_packet(raw_packet in_packet) {
    DLOG(INFO) << "[PACKET: RAW -> L2]";

    eth_hdr* e = in_packet.payload->get_header<eth_hdr>();
    
    in_packet.payload->add_offset<eth_hdr, 0>();

    // DLOG(INFO) << mac_addr(e->remote_mac);

    return std::make_optional<l2_packet>(mac_addr(e->remote_mac),  e->ethernet_type, std::move(in_packet.payload));
}

template <>
std::optional<int> get_proto(std::optional<l2_packet>& packet)
{
    DLOG(INFO) << "[PROTO L2_PACKET] " << std::setiosflags(std::ios::uppercase) << std::hex << packet->_proto;
    
    return packet->_proto;
}

};

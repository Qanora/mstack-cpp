#pragma once
#include <optional>

#include "arp.hpp"
#include "ipv4.hpp"
#include "ipv4_addr.hpp"
#include "layer.hpp"
#include "mac_addr.hpp"

namespace mstack {

class l2_hook : public base_hook_funcs<l2_packet, l3_packet> {
public:
    arp_hook_t arp_hook;

    template <typename DEV>
    void hook_register_dev(DEV& dev)
    {
        arp_hook.add(dev);
        return;
    }

    void
    hook_register_protocol(protocol_interface<l2_packet>& protocol)
    {
        DLOG(INFO) << "[REGISTER] " << protocol.tag();
        return;
    }

    void
    hook_dispatch(std::optional<l2_packet>& in_packet)
    {
        return;
    }

    void
    hook_gather(std::optional<l2_packet>& in_packet)
    {
        if (!in_packet) {
            return;
        }
        arp_hook.arp_filter(in_packet);
        return;
    }

    std::optional<l3_packet>
    hook_forward_to_upper(std::optional<l2_packet> in_packet)
    {
        if (!in_packet) {
            return std::nullopt;
        }
        ipv4_header_t in_ipv4_packet;
        in_ipv4_packet.consume(in_packet->_payload->get_pointer());
        in_packet->_payload->add_offset(ipv4_header_t::size());
        return l3_packet(in_ipv4_packet.src_ip_addr, in_ipv4_packet.dst_ip_addr, in_ipv4_packet.proto_type, std::move(in_packet->_payload));
    }

    std::optional<l2_packet>
    hook_forward_from_upper(std::optional<l3_packet> in_packet)
    {
        if (!in_packet) {
            return std::nullopt;
        }
        if (in_packet->_src_ipv4_addr && in_packet->_dst_ipv4_addr && in_packet->_proto == IPV4_PROTO) {
            std::optional<mac_addr_t> src_mac_addr = arp_hook.query_arp_cache((in_packet->_src_ipv4_addr).value());
            std::optional<mac_addr_t> dst_mac_addr = arp_hook.query_arp_cache((in_packet->_dst_ipv4_addr).value());
            if (!src_mac_addr || !dst_mac_addr) {
                // TODO send arp request
                LOG(INFO) << "[ARP QUERY FAIL]";
                return std::nullopt;
            }
            return l2_packet(src_mac_addr, dst_mac_addr, IPV4_PROTO, std::move(in_packet->_payload));
        } else {
            LOG(INFO) << "[L3 PACKET NOT FILL]";
            return std::nullopt;
        }
    }
};

using l2_layer = mstack::layer<l2_packet, l3_packet, l2_hook>;

template <>
std::optional<int> get_proto(std::optional<l2_packet>& packet)
{
    DLOG(INFO) << "[PROTO L2_PACKET] " << std::setiosflags(std::ios::uppercase) << std::hex << packet->_proto;
    return packet->_proto;
}
};

#pragma once
#include <optional>
#include "icmp.hpp"
#include "ipv4.hpp"
#include "ipv4_addr.hpp"
#include "layer.hpp"
#include "packets.hpp"
#include "tcb.hpp"
#include "tcp.hpp"
namespace mstack {

class l3_hook : public base_hook_funcs<l3_packet, l4_packet> {
 public:
  icmp_hook_t icmp_hook;
  ipv4_hook_t ipv4_hook;

  void hook_register_protocol(protocol_interface<l3_packet>& in_protocol) {
    DLOG(INFO) << "[REGISTER] " << in_protocol.tag();
    return;
  }

  void hook_dispatch(std::optional<l3_packet>& in_packet) { return; }

  void hook_gather(std::optional<l3_packet>& in_packet) {
    if (!in_packet) {
      return;
    }
    if (in_packet->_proto == icmp::PROTO) {
      icmp_hook.icmp_filter(in_packet);
    }
    ipv4_hook.ipv4_filter(in_packet);
    return;
  }

  std::optional<l4_packet> hook_forward_to_upper(
      std::optional<l3_packet> in_packet) {
    if (!in_packet) {
      return std::nullopt;
    }
    if (in_packet->_proto == tcp::PROTO) {
      in_packet->_payload->add_offset(ipv4_header_t::size());
      tcp_header_t tcp_header;
      tcp_header.consume(in_packet->_payload->get_pointer());
      half_connect_id_t remote_info(in_packet->_src_ipv4_addr.value(),
                                    tcp_header.src_port);
      half_connect_id_t local_info(in_packet->_dst_ipv4_addr.value(),
                                   tcp_header.dst_port);
      return l4_packet(remote_info, local_info, in_packet->_proto,
                       std::move(in_packet->_payload));
    }
    return std::nullopt;
  }

  std::optional<l3_packet> hook_forward_from_upper(
      std::optional<l4_packet> in_packet) {
    if (!in_packet) {
      return std::nullopt;
    }
    if (in_packet->_proto == tcp::PROTO) {
      auto out_packet = std::make_optional<l3_packet>(
          in_packet->_local_info->ip_addr, in_packet->_remote_info->ip_addr,
          in_packet->_proto, std::move(in_packet->_payload));
      return out_packet;
    }
    return std::nullopt;
  }
};

using l3_layer = mstack::layer<l3_packet, l4_packet, l3_hook>;

template <>
std::optional<int> get_proto(std::optional<l3_packet>& packet) {
  DLOG(INFO) << "[PROTO L3_PACKET] " << std::setiosflags(std::ios::uppercase)
             << std::hex << packet->_proto;
  return packet->_proto;
}
};  // namespace mstack

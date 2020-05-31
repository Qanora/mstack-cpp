#pragma once
#include "base_protocol.hpp"
#include "packets.hpp"
#include "tcp_header.hpp"
namespace mstack {
class tcp : public base_protocol<ipv4_packet, tcp_packet_t, tcp> {
public:
        constexpr static int PROTO = 0x06;

        virtual int id() { return PROTO; }

        virtual std::optional<ipv4_packet> make_packet(tcp_packet_t in_packet) {
                uint32_t sum = 0;

                sum += utils::ntoh(in_packet.local_info->ipv4_addr->get_raw_ipv4());
                sum += utils::ntoh(in_packet.remote_info->ipv4_addr->get_raw_ipv4());
                sum += utils::ntoh(in_packet.proto);
                sum += utils::ntoh((uint16_t)in_packet.buffer->get_remaining_len());

                tcp_header_t tcp_header = tcp_header_t::consume(in_packet.buffer->get_pointer());

                uint16_t checksum = utils::checksum(in_packet.buffer->get_pointer(),
                                                    in_packet.buffer->get_remaining_len(), sum);

                tcp_header.checksum = checksum;
                tcp_header.produce(in_packet.buffer->get_pointer());

                ipv4_packet out_ipv4 = {.src_ipv4_addr = in_packet.local_info->ipv4_addr.value(),
                                        .dst_ipv4_addr = in_packet.remote_info->ipv4_addr.value(),
                                        .proto         = in_packet.proto,
                                        .buffer        = std::move(in_packet.buffer)};
                return std::move(out_ipv4);
        }

        virtual std::optional<tcp_packet_t> make_packet(ipv4_packet in_packet) {
                auto tcp_header = tcp_header_t::consume(in_packet.buffer->get_pointer());
                DLOG(INFO) << "[RECEIVE] " << tcp_header;
                ipv4_port_t  remote_info    = {.ipv4_addr = in_packet.src_ipv4_addr.value(),
                                           .port_addr = tcp_header.src_port};
                ipv4_port_t  local_info     = {.ipv4_addr = in_packet.dst_ipv4_addr.value(),
                                          .port_addr = tcp_header.dst_port};
                tcp_packet_t out_tcp_packet = {.proto       = PROTO,
                                               .remote_info = remote_info,
                                               .local_info  = local_info,
                                               .buffer      = std::move(in_packet.buffer)};
                return std::move(out_tcp_packet);
        }
};  // namespace mstack
};  // namespace mstack
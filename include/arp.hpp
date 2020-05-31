#pragma once
#include "arp_cache.hpp"
#include "arp_header.hpp"
#include "base_protocol.hpp"
#include "defination.hpp"
#include "logger.hpp"
#include "packets.hpp"
namespace mstack {

class arp : public base_protocol<ethernetv2_packet, ipv4_packet, arp> {
public:
        static constexpr uint16_t PROTO = 0x0806;
        arp_cache_t               arp_cache;

        virtual int id() { return PROTO; }

        std::optional<mac_addr_t> query_by_ipv4(ipv4_addr_t ipv4_addr) {
                return arp_cache.query_arp_cache(ipv4_addr);
        }

        template <typename DEV>
        void register_dev(DEV& dev) {
                arp_cache.register_dev(dev);
        }

        void send_reply(arpv4_header_t& in_arp) {
                auto dev_mac_addr  = arp_cache.query_dev_mac_addr(TUNTAP_DEV);
                auto dev_ipv4_addr = arp_cache.query_dev_ipv4_addr(TUNTAP_DEV);
                if (!dev_mac_addr || !dev_ipv4_addr) {
                        DLOG(ERROR) << "[UNKONWN DEV] " << TUNTAP_DEV;
                        return;
                }

                struct arpv4_header_t out_arp;
                out_arp.hw_type    = 0x0001;
                out_arp.proto_type = 0x0800;
                out_arp.hw_size    = 0x06;
                out_arp.proto_size = 0x04;
                out_arp.opcode     = 0x02;

                out_arp.src_mac_addr  = dev_mac_addr.value();
                out_arp.src_ipv4_addr = dev_ipv4_addr.value();

                out_arp.dst_mac_addr  = in_arp.src_mac_addr;
                out_arp.dst_ipv4_addr = in_arp.src_ipv4_addr;

                arp_cache.add_arp_cache(in_arp.src_ipv4_addr, in_arp.src_mac_addr);
                auto out_buffer = std::make_unique<base_packet>(arpv4_header_t::size());
                out_arp.produce(out_buffer->get_pointer());

                ethernetv2_packet out_packet = {.src_mac_addr = out_arp.src_mac_addr,
                                                .dst_mac_addr = out_arp.dst_mac_addr,
                                                .proto        = PROTO,
                                                .buffer       = std::move(out_buffer)};

                this->enter_send_queue(std::move(out_packet));
                DLOG(INFO) << "[ARP] SEND ARP REPLY" << out_arp;
                return;
        };

        virtual std::optional<ipv4_packet> make_packet(ethernetv2_packet in_packet) {
                auto in_arp = arpv4_header_t::consume(in_packet.buffer->get_pointer());
                if (in_arp.opcode == 0x0001) {
                        send_reply(in_arp);
                }
                return std::nullopt;
        }
};
}  // namespace mstack
#pragma once
#include "ipv4_addr.hpp"
#include "utils.hpp"

namespace mstack {
struct ipv4_header_t {
        uint8_t     version : 4;
        uint8_t     header_length : 4;
        uint8_t     type_of_service;
        uint16_t    total_length;
        uint16_t    id;
        uint16_t    NOP : 1;
        uint16_t    DF : 1;
        uint16_t    MF : 1;
        uint16_t    frag_offset : 13;
        uint8_t     ttl;
        uint8_t     proto_type;
        uint16_t    header_checksum;
        ipv4_addr_t src_ip_addr;
        ipv4_addr_t dst_ip_addr;

        ipv4_header_t() {
                version         = 0;
                header_length   = 0;
                type_of_service = 0;
                total_length    = 0;
                id              = 0;
                NOP             = 0;
                DF              = 0;
                MF              = 0;
                frag_offset     = 0;
                ttl             = 0;
                proto_type      = 0;
                header_checksum = 0;
        }

        static constexpr size_t size() {
                return 1 + 1 + 2 + 2 + 2 + 1 + 1 + 2 + ipv4_addr_t::size() * 2;
        }

        static ipv4_header_t consume(uint8_t* ptr) {
                ipv4_header_t ipv4_header;
                uint8_t       version_header_length = utils::consume<uint8_t>(ptr);
                ipv4_header.version                 = version_header_length >> 4;
                ipv4_header.header_length           = version_header_length & 0xF;
                ipv4_header.type_of_service         = utils::consume<uint8_t>(ptr);
                ipv4_header.total_length            = utils::consume<uint16_t>(ptr);
                ipv4_header.id                      = utils::consume<uint16_t>(ptr);
                uint16_t flags_farg_offset          = utils::consume<uint16_t>(ptr);
                ipv4_header.NOP                     = flags_farg_offset >> 15;
                ipv4_header.DF                      = (flags_farg_offset >> 14) & 0x1;
                ipv4_header.MF                      = (flags_farg_offset >> 13) & 0x1;
                ipv4_header.frag_offset             = flags_farg_offset & ((1 << 13) - 1);
                ipv4_header.ttl                     = utils::consume<uint8_t>(ptr);
                ipv4_header.proto_type              = utils::consume<uint8_t>(ptr);
                ipv4_header.header_checksum         = utils::consume<uint16_t>(ptr);
                ipv4_header.src_ip_addr.consume(ptr);
                ipv4_header.dst_ip_addr.consume(ptr);
                return ipv4_header;
        }

        void produce(uint8_t* ptr) {
                utils::produce<uint8_t>(ptr, version << 4 | header_length);
                utils::produce<uint8_t>(ptr, type_of_service);
                utils::produce<uint16_t>(ptr, total_length);
                utils::produce<uint16_t>(ptr, id);
                utils::produce<uint16_t>(ptr, NOP << 15 | DF << 14 | MF << 13 | frag_offset);
                utils::produce<uint8_t>(ptr, ttl);
                utils::produce<uint8_t>(ptr, proto_type);
                utils::produce<uint16_t>(ptr, header_checksum);
                src_ip_addr.produce(ptr);
                dst_ip_addr.produce(ptr);
        }
        friend std::ostream& operator<<(std::ostream& out, ipv4_header_t& m) {
                using u = uint32_t;
                out << "[IPV4 PACKET] ";
                out << m.total_length << " ";
                out << m.src_ip_addr;
                out << " -> " << m.dst_ip_addr << " ";
                out << "PROTO: " << std::hex << u(m.proto_type);
                return out;
        }
};
}  // namespace mstack
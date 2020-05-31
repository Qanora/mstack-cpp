#pragma once
#include "ipv4_addr.hpp"
#include "mac_addr.hpp"
#include "utils.hpp"

namespace mstack {
struct arpv4_header_t {
        uint16_t    hw_type    = 0;
        uint16_t    proto_type = 0;
        uint8_t     hw_size    = 0;
        uint8_t     proto_size = 0;
        uint16_t    opcode     = 0;
        mac_addr_t  src_mac_addr;
        ipv4_addr_t src_ipv4_addr;
        mac_addr_t  dst_mac_addr;
        ipv4_addr_t dst_ipv4_addr;

        static constexpr size_t size() {
                return 2 + 2 + 1 + 1 + 2 + mac_addr_t::size() * 2 + ipv4_addr_t::size() * 2;
        }

        static arpv4_header_t consume(uint8_t* ptr) {
                arpv4_header_t arpv4_header;
                arpv4_header.hw_type    = utils::consume<uint16_t>(ptr);
                arpv4_header.proto_type = utils::consume<uint16_t>(ptr);
                arpv4_header.hw_size    = utils::consume<uint8_t>(ptr);
                arpv4_header.proto_size = utils::consume<uint8_t>(ptr);
                arpv4_header.opcode     = utils::consume<uint16_t>(ptr);
                arpv4_header.src_mac_addr.consume(ptr);
                arpv4_header.src_ipv4_addr.consume(ptr);
                arpv4_header.dst_mac_addr.consume(ptr);
                arpv4_header.dst_ipv4_addr.consume(ptr);
                return arpv4_header;
        }

        void produce(uint8_t* ptr) {
                utils::produce<uint16_t>(ptr, hw_type);
                utils::produce<uint16_t>(ptr, proto_type);
                utils::produce<uint8_t>(ptr, hw_size);
                utils::produce<uint8_t>(ptr, proto_size);
                utils::produce<uint16_t>(ptr, opcode);
                src_mac_addr.produce(ptr);
                src_ipv4_addr.produce(ptr);
                dst_mac_addr.produce(ptr);
                dst_ipv4_addr.produce(ptr);
        }
        friend std::ostream& operator<<(std::ostream& out, arpv4_header_t& m) {
                out << m.opcode << " ";
                out << m.src_mac_addr << " " << m.src_ipv4_addr;
                out << " -> " << m.dst_mac_addr << " " << m.dst_ipv4_addr;
                return out;
        }
};
}  // namespace mstack
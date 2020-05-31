#pragma once
#include "utils.hpp"

namespace mstack {
struct icmp_header_t {
        uint8_t  proto_type = 0;
        uint8_t  code       = 0;
        uint16_t checksum   = 0;
        uint16_t id         = 0;
        uint16_t seq        = 0;

        static constexpr size_t size() { return 1 + 1 + 2 + 2 + 2; }

        static icmp_header_t consume(uint8_t* ptr) {
                icmp_header_t icmp_header;
                icmp_header.proto_type = utils::consume<uint8_t>(ptr);
                icmp_header.code       = utils::consume<uint8_t>(ptr);
                icmp_header.checksum   = utils::consume<uint16_t>(ptr);
                icmp_header.id         = utils::consume<uint16_t>(ptr);
                icmp_header.seq        = utils::consume<uint16_t>(ptr);
                return icmp_header;
        }

        uint8_t* produce(uint8_t* ptr) {
                utils::produce<uint8_t>(ptr, proto_type);
                utils::produce<uint8_t>(ptr, code);
                utils::produce<uint16_t>(ptr, checksum);
                utils::produce<uint16_t>(ptr, id);
                utils::produce<uint16_t>(ptr, seq);
                return ptr;
        }
        friend std::ostream& operator<<(std::ostream& out, icmp_header_t& m) {
                using u = uint32_t;
                out << "[ICMP PACKET] ";
                out << u(m.proto_type) << " ";
                out << u(m.code) << " ";
                out << u(m.id) << " ";
                out << u(m.seq);
                return out;
        };
};
}  // namespace mstack
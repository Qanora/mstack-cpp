#pragma once
#include <iomanip>

#include "mac_addr.hpp"
#include "utils.hpp"

namespace mstack {

struct ethernetv2_header_t {
        mac_addr_t dst_mac_addr;
        mac_addr_t src_mac_addr;
        uint16_t   proto = 0;

        static constexpr size_t size() {
                return mac_addr_t::size() + mac_addr_t::size() + 2;
        }

        static ethernetv2_header_t consume(uint8_t* ptr) {
                ethernetv2_header_t ethernet_header;
                ethernet_header.dst_mac_addr.consume(ptr);
                ethernet_header.src_mac_addr.consume(ptr);
                ethernet_header.proto = utils::consume<uint16_t>(ptr);
                return ethernet_header;
        }

        void produce(uint8_t* ptr) {
                dst_mac_addr.produce(ptr);
                src_mac_addr.produce(ptr);
                utils::produce<uint16_t>(ptr, proto);
        }
};

std::ostream& operator<<(std::ostream& out, ethernetv2_header_t& m) {
        out << "[ETHERENT PACKET] ";
        out << "DST: " << mac_addr_t(m.dst_mac_addr)
            << " SRC: " << mac_addr_t(m.src_mac_addr);
        out << " TYPE: " << std::setiosflags(std::ios::uppercase) << std::hex
            << int(m.proto);
        return out;
}
};  // namespace mstack
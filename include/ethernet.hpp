#pragma once
#include <iomanip>
#include "mac_addr.hpp"
#include "utils.hpp"

namespace mstack {

struct ethernet_header_t {
  mac_addr_t dst_mac_addr;
  mac_addr_t src_mac_addr;
  uint16_t ethernet_type;

  static constexpr size_t size() {
    return mac_addr_t::size() + mac_addr_t::size() + 2;
  }

  void consume(uint8_t* ptr) {
    dst_mac_addr.consume(ptr);
    src_mac_addr.consume(ptr);
    ethernet_type = util::consume<uint16_t>(ptr);
  }

  void produce(uint8_t* ptr) {
    dst_mac_addr.produce(ptr);
    src_mac_addr.produce(ptr);
    util::produce<uint16_t>(ptr, ethernet_type);
  }
};

std::ostream& operator<<(std::ostream& out, ethernet_header_t& m) {
  out << "[ETHERENT PACKET] ";
  out << "DST: " << mac_addr_t(m.dst_mac_addr)
      << " SRC: " << mac_addr_t(m.src_mac_addr);
  out << " TYPE: " << std::setiosflags(std::ios::uppercase) << std::hex
      << int(m.ethernet_type);
  return out;
}
};  // namespace mstack
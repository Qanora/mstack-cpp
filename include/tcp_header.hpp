#pragma once
#include "ipv4_addr.hpp"
namespace mstack {
using port_addr_t = uint16_t;
struct tcp_fake_header_t {
  ipv4_addr_t src_ip_addr;
  ipv4_addr_t dst_ip_addr;
  uint8_t NOP;
  uint8_t proto;
  uint16_t length;
  static constexpr size_t size() { return ipv4_addr_t::size() * 2 + 1 + 1 + 2; }
  void produce(uint8_t* ptr) {
    src_ip_addr.produce(ptr);
    dst_ip_addr.produce(ptr);
    util::produce<uint8_t>(ptr, NOP);
    util::produce<uint8_t>(ptr, proto);
    util::produce<uint16_t>(ptr, length);
  }
};

struct tcp_header_t {
  port_addr_t src_port;
  port_addr_t dst_port;
  uint32_t seq_no;
  uint32_t ack_no;

  uint16_t header_length : 4;
  uint16_t NOP : 6;
  uint16_t URG : 1;
  uint16_t ACK : 1;
  uint16_t PSH : 1;
  uint16_t RST : 1;
  uint16_t SYN : 1;
  uint16_t FIN : 1;

  uint16_t window_size;
  uint16_t checksum;
  uint16_t urgent_pointer;

  static constexpr size_t size() { return 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2; }

  void consume(uint8_t* ptr) {
    src_port = util::consume<port_addr_t>(ptr);
    dst_port = util::consume<port_addr_t>(ptr);
    seq_no = util::consume<uint32_t>(ptr);
    ack_no = util::consume<uint32_t>(ptr);
    uint16_t header_length_flags = util::consume<uint16_t>(ptr);
    header_length = header_length_flags >> 12;
    NOP = (header_length_flags >> 6) & ((1 << 6) - 1);
    URG = (header_length_flags >> 5) & 0x1;
    ACK = (header_length_flags >> 4) & 0x1;
    PSH = (header_length_flags >> 3) & 0x1;
    RST = (header_length_flags >> 2) & 0x1;
    SYN = (header_length_flags >> 1) & 0x1;
    FIN = (header_length_flags >> 0) & 0x1;
    window_size = util::consume<uint16_t>(ptr);
    checksum = util::consume<uint16_t>(ptr);
    urgent_pointer = util::consume<uint16_t>(ptr);
  }

  void produce(uint8_t* ptr) {
    util::produce<port_addr_t>(ptr, src_port);
    util::produce<port_addr_t>(ptr, dst_port);
    util::produce<uint32_t>(ptr, seq_no);
    util::produce<uint32_t>(ptr, ack_no);
    util::produce<uint16_t>(ptr, header_length << 12 | NOP << 6 | URG << 5 |
                                     ACK << 4 | PSH << 3 | RST << 2 | SYN << 1 |
                                     FIN);
    util::produce<uint16_t>(ptr, window_size);
    util::produce<uint16_t>(ptr, checksum);
    util::produce<uint16_t>(ptr, urgent_pointer);
  }
};
std::ostream& operator<<(std::ostream& out, tcp_header_t& m) {
  out << m.src_port;
  out << " -> " << m.dst_port;
  out << " SEQ_NO: " << m.seq_no;
  out << " ACK_NO: " << m.ack_no;
  out << " [ ";
  if (m.ACK == 1) out << "ACK ";
  if (m.SYN == 1) out << "SYN ";
  if (m.PSH == 1) out << "PSH ";
  if (m.RST == 1) out << "RST ";
  if (m.FIN == 1) out << "FIN ";
  out << " ]";

  return out;
}
}  // namespace mstack
#pragma once
#include <memory>
#include <optional>
#include <unordered_map>
#include "ethernet.hpp"
#include "packets.hpp"
#include "protocol.hpp"
namespace mstack {

struct arpv4_header_t {
  uint16_t hw_type;
  uint16_t proto_type;
  uint8_t hw_size;
  uint8_t proto_size;
  uint16_t opcode;
  mac_addr_t src_mac_addr;
  ipv4_addr_t src_ip_addr;
  mac_addr_t dst_mac_addr;
  ipv4_addr_t dst_ip_addr;

  static constexpr size_t size() {
    return 2 + 2 + 1 + 1 + 2 + mac_addr_t::size() * 2 + ipv4_addr_t::size() * 2;
  }

  void consume(uint8_t* ptr) {
    hw_type = util::consume<uint16_t>(ptr);
    proto_type = util::consume<uint16_t>(ptr);
    hw_size = util::consume<uint8_t>(ptr);
    proto_size = util::consume<uint8_t>(ptr);
    opcode = util::consume<uint16_t>(ptr);
    src_mac_addr.consume(ptr);
    src_ip_addr.consume(ptr);
    dst_mac_addr.consume(ptr);
    dst_ip_addr.consume(ptr);
  }

  void produce(uint8_t* ptr) {
    util::produce<uint16_t>(ptr, hw_type);
    util::produce<uint16_t>(ptr, proto_type);
    util::produce<uint8_t>(ptr, hw_size);
    util::produce<uint8_t>(ptr, proto_size);
    util::produce<uint16_t>(ptr, opcode);
    src_mac_addr.produce(ptr);
    src_ip_addr.produce(ptr);
    dst_mac_addr.produce(ptr);
    dst_ip_addr.produce(ptr);
  }
};

std::ostream& operator<<(std::ostream& out, arpv4_header_t& m) {
  out << m.opcode << " ";
  out << m.src_mac_addr << " " << m.src_ip_addr;
  out << " -> " << m.dst_mac_addr << " " << m.dst_ip_addr;
  return out;
}

class arp : public protocol_interface<l2_packet> {
 public:
  static constexpr int PROTO = 0x0806;
  virtual int proto() { return PROTO; }

  virtual std::string tag() { return "ARP"; }

  static arp& instance() {
    static arp instance;
    return instance;
  }

  virtual void receive(l2_packet packet) {
    struct arpv4_header_t arp_header;
    arp_header.consume(packet._payload->get_pointer());
    if (arp_header.opcode == 0x0001) {
      DLOG(INFO) << "[ARP REQUEST] " << arp_header;
      make_response_packet(arp_header);
    }
  }

  void make_response_packet(arpv4_header_t& in_header) {
    std::unique_ptr<packet> out_packet =
        std::make_unique<packet>(arpv4_header_t::size());

    struct arpv4_header_t arp_header;

    arp_header.hw_type = 0x0001;
    arp_header.proto_type = 0x0800;
    arp_header.hw_size = 0x06;
    arp_header.proto_size = 0x04;
    arp_header.opcode = 0x02;

    //? FIXME avoid move uninit value
    arp_header.src_mac_addr = in_header.src_mac_addr;
    arp_header.src_ip_addr = in_header.src_ip_addr;

    arp_header.dst_mac_addr = in_header.src_mac_addr;
    arp_header.dst_ip_addr = in_header.src_ip_addr;

    arp_header.produce(out_packet->get_pointer());

    l2_packet out_l2_packet = l2_packet(std::nullopt, arp_header.dst_mac_addr,
                                        PROTO, std::move(out_packet));
    DLOG(INFO) << "[ENQUEUE ARP] " << out_l2_packet;
    packet_queue.push_back(std::move(out_l2_packet));
  }
};

struct arp_hook_t {
  std::unordered_map<int, mac_addr_t> mac_addr_map;
  std::unordered_map<int, ipv4_addr_t> ipv4_addr_map;
  std::unordered_map<ipv4_addr_t, mac_addr_t> arp_cache;

  void add_arp_cache(ipv4_addr_t ipv4_addr, mac_addr_t mac_addr) {
    arp_cache[ipv4_addr] = mac_addr;
  }

  std::optional<mac_addr_t> query_arp_cache(ipv4_addr_t ipv4_addr) {
    if (arp_cache.find(ipv4_addr) == arp_cache.end()) {
      return std::nullopt;
    }
    return arp_cache[ipv4_addr];
  }

  template <typename DEV>
  void add(DEV& dev) {
    std::optional<mac_addr_t> mac_addr = dev.get_mac_addr();
    if (mac_addr) {
      mac_addr_map[DEV::TAG] = mac_addr.value();
      DLOG(INFO) << "[REGISTER MAC] " << DEV::TAG << " "
                 << mac_addr_map[DEV::TAG];
    }

    std::optional<ipv4_addr_t> ipv4_addr = dev.get_ipv4_addr();
    if (ipv4_addr) {
      ipv4_addr_map[DEV::TAG] = ipv4_addr.value();
      DLOG(INFO) << "[REGISTER IPV4] " << DEV::TAG << " "
                 << ipv4_addr_map[DEV::TAG];
    }

    if (mac_addr && ipv4_addr) {
      add_arp_cache(ipv4_addr.value(), mac_addr.value());
    }
  }

  std::optional<mac_addr_t> query_dev_mac_addr(int tag) {
    if (mac_addr_map.find(tag) == mac_addr_map.end()) {
      DLOG(ERROR) << "[UNKONWN MAC]";
      return std::nullopt;
    }
    return mac_addr_map[tag];
  }

  std::optional<ipv4_addr_t> query_dev_ipv4_addr(int tag) {
    if (ipv4_addr_map.find(tag) == ipv4_addr_map.end()) {
      DLOG(ERROR) << "[UNKONWN IPV4]";
      return std::nullopt;
    }
    return ipv4_addr_map[tag];
  }

  void arp_filter(std::optional<l2_packet>& in_packet) {
    if (in_packet->_proto == arp::PROTO) {
      // 1. query net dev mac
      std::optional<mac_addr_t> dev_mac_addr = query_dev_mac_addr(TUNTAP_DEV);
      std::optional<ipv4_addr_t> dev_ipv4_addr =
          query_dev_ipv4_addr(TUNTAP_DEV);

      if (!dev_mac_addr || !dev_ipv4_addr) {
        DLOG(ERROR) << "[UNKNOWN DEV]";
        in_packet.reset();
        return;
      }

      // 2. prepare insert net dev mac
      struct arpv4_header_t in_arp_header;
      in_arp_header.consume(in_packet->_payload->get_pointer());

      DLOG(INFO) << "[START ARP HOOK] " << in_packet.value();
      DLOG(INFO) << " -- " << in_arp_header;

      struct arpv4_header_t out_arp_header = in_arp_header;
      out_arp_header.src_mac_addr = dev_mac_addr.value();
      out_arp_header.src_ip_addr = dev_ipv4_addr.value();
      out_arp_header.produce(in_packet->_payload->get_pointer());

      in_packet->_src_mac_addr = dev_mac_addr.value();
      DLOG(INFO) << " -- " << out_arp_header;
      DLOG(INFO) << "[END   ARP HOOK] " << in_packet.value();

      // 3. add arp cache
      add_arp_cache(in_arp_header.dst_ip_addr, in_arp_header.dst_mac_addr);
    }
  };
};
};  // namespace mstack
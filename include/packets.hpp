#pragma once
#include <iomanip>
#include <memory>
#include <optional>
#include "defination.hpp"
#include "ipv4_addr.hpp"
#include "mac_addr.hpp"
#include "packet.hpp"
namespace mstack {
using port_addr_t = uint16_t;

struct half_connect_id_t {
  ipv4_addr_t ip_addr;
  port_addr_t port_addr;
  half_connect_id_t(ipv4_addr_t ip_addr, port_addr_t port_addr)
      : ip_addr(ip_addr), port_addr(port_addr) {}
  bool operator==(const half_connect_id_t& rhs) const {
    return ip_addr == rhs.ip_addr && port_addr == rhs.port_addr;
  };
  friend std::ostream& operator<<(std::ostream& out, half_connect_id_t info) {
    using u = uint32_t;
    out << "(";
    out << info.ip_addr << " ";
    out << u(info.port_addr) << ")";
    return out;
  }
};

struct full_connect_id_t {
  half_connect_id_t remote_info;
  half_connect_id_t local_info;
  full_connect_id_t(half_connect_id_t remote_info, half_connect_id_t local_info)
      : remote_info(remote_info), local_info(local_info) {}
  bool operator==(const full_connect_id_t& rhs) const {
    return remote_info == rhs.remote_info && local_info == rhs.local_info;
  };
  friend std::ostream& operator<<(std::ostream& out, full_connect_id_t info) {
    using u = uint32_t;
    out << "R: " << info.remote_info;
    out << " L: " << info.local_info;
    return out;
  }
};

struct l4_packet {
  using l4_proto = uint16_t;
  static constexpr int tag = L4_PACKET;
  l4_proto _proto;
  std::optional<half_connect_id_t> _remote_info;
  std::optional<half_connect_id_t> _local_info;
  std::unique_ptr<packet> _payload;

  l4_packet(std::optional<half_connect_id_t> remote_info,
            std::optional<half_connect_id_t> local_info, l4_proto pr,
            std::unique_ptr<packet> pa)
      : _remote_info(remote_info),
        _local_info(local_info),
        _proto(pr),
        _payload(std::move(pa)){};

  friend std::ostream& operator<<(std::ostream& out, l4_packet& p) {
    out << "R: ";
    if (p._remote_info) {
      out << p._remote_info.value();
    } else {
      out << "NONE";
    }
    out << " L: ";
    if (p._local_info) {
      out << p._local_info.value();
    } else {
      out << "NONE";
    }
    out << " PROTO: " << std::setiosflags(std::ios::uppercase) << std::hex
        << p._proto;
    return out;
  };
};

struct l3_packet {
  using l3_proto = uint16_t;
  static constexpr int tag = L3_PACKET;
  l3_proto _proto;

  std::optional<ipv4_addr_t> _src_ipv4_addr;
  std::optional<ipv4_addr_t> _dst_ipv4_addr;
  std::unique_ptr<packet> _payload;

  l3_packet(std::optional<ipv4_addr_t> sip, std::optional<ipv4_addr_t> dip,
            l3_proto pr, std::unique_ptr<packet> pa)
      : _src_ipv4_addr(sip),
        _dst_ipv4_addr(dip),
        _proto(pr),
        _payload(std::move(pa)){};

  friend std::ostream& operator<<(std::ostream& out, l3_packet& p) {
    if (p._src_ipv4_addr) {
      out << p._src_ipv4_addr.value();
    } else {
      out << "NONE";
    }
    out << " -> ";
    if (p._dst_ipv4_addr) {
      out << p._dst_ipv4_addr.value();
    } else {
      out << "NONE";
    }
    out << " PROTO: " << std::setiosflags(std::ios::uppercase) << std::hex
        << p._proto;
    return out;
  };
};

struct l2_packet {
  using l2_proto = uint16_t;
  static constexpr int tag = L2_PACKET;

  std::optional<mac_addr_t> _src_mac_addr;
  std::optional<mac_addr_t> _dst_mac_addr;
  l2_proto _proto;
  std::unique_ptr<packet> _payload;

  l2_packet(std::optional<mac_addr_t> smac, std::optional<mac_addr_t> dmac,
            l2_proto pr, std::unique_ptr<packet> pa)
      : _src_mac_addr(smac),
        _dst_mac_addr(dmac),
        _proto(pr),
        _payload(std::move(pa)) {}

  friend std::ostream& operator<<(std::ostream& out, l2_packet& p) {
    if (p._src_mac_addr) {
      out << p._src_mac_addr.value();
    } else {
      out << "NONE";
    }
    out << " -> ";
    if (p._dst_mac_addr) {
      out << p._dst_mac_addr.value();
    } else {
      out << "NONE";
    }
    out << " PROTO: " << std::setiosflags(std::ios::uppercase) << std::hex
        << p._proto;
    return out;
  }
};

using raw_packet = std::unique_ptr<uint8_t[]>;

struct tcp_packet {
  uint32_t seq_no;
  raw_packet payload;
};
};  // namespace mstack
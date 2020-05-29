#pragma once
#include <chrono>
#include <functional>
#include "circle_buffer.hpp"
#include "ipv4_addr.hpp"

namespace mstack {
using port_addr_t = uint16_t;
struct sender_t {
  uint32_t unacknowledged;
  uint32_t next;
  uint32_t window;
  int8_t window_sale;
  uint16_t mss;
  std::chrono::milliseconds rttvar;
  std::chrono::milliseconds srtt;
  std::chrono::milliseconds rto;
  uint32_t cwnd;
  uint32_t ssthresh;
  uint16_t dupacks;
  uint16_t retransmits;
  uint16_t backoff;
};

struct receiver_t {
  uint32_t next;
  uint32_t window;
  uint8_t window_scale;
  uint16_t mss;
};

struct full_tcb_t {
  int state;
  full_connect_id_t connect_id;
  circle_buffer<tcp_packet> sender_queue;
  circle_buffer<tcp_packet> receiver_queue;
  circle_buffer<l4_packet> ctl_packet;
  sender_t sender;
  receiver_t receiver;

  bool can_send() { return true; }

  void enqueue_packet(l4_packet in_packet) {
    ctl_packet.push_back(std::move(in_packet));
  }

  std::optional<l4_packet> make_l4_packet(std::optional<tcp_packet> in_packet) {
    return std::nullopt;
  }

  std::optional<l4_packet> gather_packet() {
    if (!ctl_packet.empty()) {
      return std::move(ctl_packet.pop_front());
    }
    if (can_send()) {
      return make_l4_packet(std::move(receiver_queue.pop_front()));
    }
    return std::nullopt;
  }

  full_tcb_t(full_connect_id_t connect_id) : connect_id(connect_id) {}
  friend std::ostream& operator<<(std::ostream& out, full_tcb_t& m) {
    out << m.connect_id << " ";
    out << state_to_string(m.state);
    return out;
  }
};

struct half_tcb_t {
  int state;
  half_connect_id_t connect_id;
  circle_buffer<std::shared_ptr<full_tcb_t>> tcb_queue;
  sender_t sender;
  receiver_t receiver;
  half_tcb_t(half_connect_id_t connect_id) : connect_id(connect_id) {}
};
};  // namespace mstack

namespace std {
template <>
struct hash<mstack::half_connect_id_t> {
  size_t operator()(const mstack::half_connect_id_t& half_connect_id) const {
    return hash<mstack::ipv4_addr_t>{}(half_connect_id.ip_addr) ^
           hash<mstack::port_addr_t>{}(half_connect_id.port_addr);
  };
};
template <>
struct hash<mstack::full_connect_id_t> {
  size_t operator()(const mstack::full_connect_id_t& full_connect_id) const {
    return hash<mstack::half_connect_id_t>{}(full_connect_id.remote_info) ^
           hash<mstack::half_connect_id_t>{}(full_connect_id.local_info);
  }
};

};  // namespace std
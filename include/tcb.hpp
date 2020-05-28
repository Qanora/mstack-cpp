#pragma once
#include <chrono>
#include <functional>
#include "circle_buffer.hpp"
#include "ipv4_addr.hpp"
#include "packets.hpp"

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
  sender_t sender;
  receiver_t receiver;
  full_tcb_t(full_connect_id_t connect_id) : connect_id(connect_id) {}
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
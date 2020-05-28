#pragma once
#include <chrono>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include "circle_buffer.hpp"
#include "defination.hpp"
#include "packets.hpp"
#include "tcb.hpp"

namespace mstack {
class tcb_manager_t {
  std::unordered_map<full_connect_id_t, std::shared_ptr<full_tcb_t>> full_tcbs;
  std::unordered_map<half_connect_id_t, std::shared_ptr<half_tcb_t>> half_tcbs;

 public:
  static tcb_manager_t& instance() {
    static tcb_manager_t instance;
    return instance;
  }

  std::optional<std::shared_ptr<half_tcb_t>> query_half_tcb(
      half_connect_id_t half_connect_id) {
    if (half_tcbs.find(half_connect_id) != half_tcbs.end()) {
      return half_tcbs[half_connect_id];
    }
    return std::nullopt;
  }

  std::optional<std::shared_ptr<half_tcb_t>> register_half_tcb(
      half_connect_id_t half_connect_id) {
    std::optional<std::shared_ptr<half_tcb_t>> tcb =
        query_half_tcb(half_connect_id);
    if (tcb) {
      return std::nullopt;
    }

    half_tcbs[half_connect_id] = std::make_shared<half_tcb_t>(half_connect_id);
    half_tcbs[half_connect_id]->state = TCP_LISTEN;
    return query_half_tcb(half_connect_id);
  }

  std::optional<std::shared_ptr<full_tcb_t>> query_and_register_full_tcb(
      full_connect_id_t full_connect_id) {
    if (full_tcbs.find(full_connect_id) != full_tcbs.end()) {
      DLOG(INFO) << "[FIND FULL CONNECT]";
      return full_tcbs[full_connect_id];
    }
    DLOG(INFO) << "[NO FULL CONNECT]";
    if (half_tcbs.find(full_connect_id.local_info) != half_tcbs.end()) {
      DLOG(INFO) << "[FIND HALF CONNECT]";
      register_full_tcb(full_connect_id);
      DLOG(INFO) << "[REGISTER FULL CONNECT] " << full_connect_id;
      full_tcbs[full_connect_id]->state = TCP_LISTEN;
      return full_tcbs[full_connect_id];
    }
    return std::nullopt;
  }

  std::optional<std::shared_ptr<full_tcb_t>> query_full_tcb(
      full_connect_id_t full_connect_id) {
    if (full_tcbs.find(full_connect_id) != full_tcbs.end()) {
      return full_tcbs[full_connect_id];
    }
    return std::nullopt;
  }

  std::optional<std::shared_ptr<full_tcb_t>> register_full_tcb(
      full_connect_id_t full_connect_id) {
    std::optional<std::shared_ptr<full_tcb_t>> tcb =
        query_full_tcb(full_connect_id);

    if (tcb) {
      return std::nullopt;
    }

    full_tcbs[full_connect_id] = std::make_shared<full_tcb_t>(full_connect_id);
    return query_full_tcb(full_connect_id);
  }
};
};  // namespace mstack
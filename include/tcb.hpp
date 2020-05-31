#pragma once
#include <chrono>
#include <functional>
#include <memory>

#include "circle_buffer.hpp"
#include "defination.hpp"
#include "ipv4_addr.hpp"
#include "packets.hpp"

namespace mstack {
using port_addr_t = uint16_t;
struct send_state_t {
        uint32_t unacknowledged = 0;
        uint32_t next           = 0;
        uint32_t window         = 0;
        int8_t   window_sale    = 0;
        uint16_t mss            = 0;
        uint32_t cwnd           = 0;
        uint32_t ssthresh       = 0;
        uint16_t dupacks        = 0;
        uint16_t retransmits    = 0;
        uint16_t backoff        = 0;
        // std::chrono::milliseconds rttvar;
        // std::chrono::milliseconds srtt;
        // std::chrono::milliseconds rto;
};

struct receive_state_t {
        uint32_t next         = 0;
        uint32_t window       = 0;
        uint8_t  window_scale = 0;
        uint16_t mss          = 0;
};

struct tcb_t : public std::enable_shared_from_this<tcb_t> {
        std::shared_ptr<circle_buffer<std::shared_ptr<tcb_t>>> _active_tcbs;

        int                         state;
        std::optional<ipv4_port_t>  remote_info;
        std::optional<ipv4_port_t>  local_info;
        circle_buffer<raw_packet>   send_queue;
        circle_buffer<raw_packet>   receive_queue;
        circle_buffer<tcp_packet_t> ctl_packets;
        send_state_t                send;
        receive_state_t             receive;

        tcb_t(std::shared_ptr<circle_buffer<std::shared_ptr<tcb_t>>> active_tcbs,
              ipv4_port_t                                            remote_info,
              ipv4_port_t                                            local_info)
            : _active_tcbs(active_tcbs),
              remote_info(remote_info),
              local_info(local_info),
              state(TCP_CLOSED) {}

        void enqueue_send(raw_packet packet) {
                send_queue.push_back(std::move(packet));
                active_self();
        }

        void active_self() { _active_tcbs->push_back(shared_from_this()); }

        bool can_send() { return true; }

        std::optional<tcp_packet_t> make_packet() { return std::nullopt; }
        std::optional<tcp_packet_t> gather_packet() {
                if (!ctl_packets.empty()) {
                        return std::move(ctl_packets.pop_front());
                }
                if (can_send()) {
                        return make_packet();
                }
                return std::nullopt;
        }
};
};  // namespace mstack
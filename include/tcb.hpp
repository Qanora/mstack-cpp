#pragma once
#include <chrono>
#include <functional>
#include <memory>

#include "base_packet.hpp"
#include "circle_buffer.hpp"
#include "defination.hpp"
#include "ipv4_addr.hpp"
#include "packets.hpp"
#include "tcp_header.hpp"
namespace mstack {
using port_addr_t = uint16_t;
struct send_state_t {
        uint32_t                  unacknowledged = 0;
        uint32_t                  next           = 0;
        uint32_t                  window         = 0;
        int8_t                    window_sale    = 0;
        uint16_t                  mss            = 0;
        uint32_t                  cwnd           = 0;
        uint32_t                  ssthresh       = 0;
        uint16_t                  dupacks        = 0;
        uint16_t                  retransmits    = 0;
        uint16_t                  backoff        = 0;
        std::chrono::milliseconds rttvar;
        std::chrono::milliseconds srtt;
        std::chrono::milliseconds rto;
};

struct receive_state_t {
        uint32_t next         = 0;
        uint32_t window       = 0;
        uint8_t  window_scale = 0;
        uint16_t mss          = 0;
};

struct tcb_t : public std::enable_shared_from_this<tcb_t> {
        std::shared_ptr<circle_buffer<std::shared_ptr<tcb_t>>>                _active_tcbs;
        std::optional<std::shared_ptr<circle_buffer<std::shared_ptr<tcb_t>>>> _listener;
        int                                                                   state;
        int                                                                   next_state;
        std::optional<ipv4_port_t>                                            remote_info;
        std::optional<ipv4_port_t>                                            local_info;
        circle_buffer<raw_packet>                                             send_queue;
        circle_buffer<raw_packet>                                             receive_queue;
        circle_buffer<tcp_packet_t>                                           ctl_packets;
        send_state_t                                                          send;
        receive_state_t                                                       receive;

        tcb_t(std::shared_ptr<circle_buffer<std::shared_ptr<tcb_t>>>                active_tcbs,
              std::optional<std::shared_ptr<circle_buffer<std::shared_ptr<tcb_t>>>> listener,
              ipv4_port_t                                                           remote_info,
              ipv4_port_t                                                           local_info)
            : _active_tcbs(active_tcbs),
              _listener(listener),
              remote_info(remote_info),
              local_info(local_info),
              state(TCP_CLOSED) {}

        void enqueue_send(raw_packet packet) {
                send_queue.push_back(std::move(packet));
                active_self();
        }

        void listen_finish() {
                if (this->_listener) {
                        _listener.value()->push_back(shared_from_this());
                }
        }

        void active_self() { _active_tcbs->push_back(shared_from_this()); }

        bool can_send() { return true; }

        std::optional<std::unique_ptr<base_packet>> prepare_data_optional(int& option_len) {
                return std::nullopt;
        }

        std::optional<tcp_packet_t> make_packet() {
                tcp_header_t                 out_tcp;
                std::unique_ptr<base_packet> out_buffer;

                int option_len = 0;

                std::optional<std::unique_ptr<base_packet>> data_buffer =
                        prepare_data_optional(option_len);

                if (data_buffer) {
                        out_buffer = std::move(data_buffer.value());
                } else {
                        out_buffer = std::make_unique<base_packet>(tcp_header_t::size());
                }

                out_tcp.src_port = local_info->port_addr.value();
                out_tcp.dst_port = remote_info->port_addr.value();
                out_tcp.ack_no   = receive.next;
                out_tcp.seq_no   = send.unacknowledged;

                // TODO
                out_tcp.window_size   = 0xFAF0;
                out_tcp.header_length = (tcp_header_t::size() + option_len) / 4;

                out_tcp.ACK = 1;

                if (this->next_state == TCP_SYN_RECEIVED) {
                        out_tcp.SYN = 1;
                }

                out_tcp.produce(out_buffer->get_pointer());
                tcp_packet_t out_packet = {.proto       = 0x06,
                                           .remote_info = this->remote_info,
                                           .local_info  = this->local_info,
                                           .buffer      = std::move(out_buffer)};
                if (this->next_state != this->state) {
                        this->state = this->next_state;
                }
                return std::move(out_packet);
        }

        std::optional<tcp_packet_t> gather_packet() {
                if (!ctl_packets.empty()) {
                        return std::move(ctl_packets.pop_front());
                }
                if (can_send()) {
                        return make_packet();
                }
                return std::nullopt;
        }

        friend std::ostream& operator<<(std::ostream& out, tcb_t& m) {
                out << m.remote_info.value();
                out << " -> ";
                out << m.local_info.value();
                out << " ";
                out << state_to_string(m.state);
                return out;
        }
};
};  // namespace mstack
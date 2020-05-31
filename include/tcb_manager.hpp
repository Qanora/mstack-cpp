#pragma once
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "circle_buffer.hpp"
#include "packets.hpp"
#include "tcb.hpp"
#include "tcp_transmit.hpp"
namespace mstack {
class tcb_manager {
private:
        tcb_manager() : active_tcbs(std::make_shared<circle_buffer<std::shared_ptr<tcb_t>>>()) {}
        ~tcb_manager() = default;
        std::shared_ptr<circle_buffer<std::shared_ptr<tcb_t>>> active_tcbs;
        std::unordered_map<two_ends_t, std::shared_ptr<tcb_t>> tcbs;
        std::unordered_set<ipv4_port_t>                        active_ports;

public:
        tcb_manager(const tcb_manager&) = delete;
        tcb_manager(tcb_manager&&)      = delete;
        tcb_manager& operator=(const tcb_manager&) = delete;
        tcb_manager& operator=(tcb_manager&&) = delete;

        static tcb_manager& instance() {
                static tcb_manager instance;
                return instance;
        }

public:
        int id() { return 0x06; }

        std::optional<tcp_packet_t> gather_packet() {
                while (!active_tcbs->empty()) {
                        std::optional<std::shared_ptr<tcb_t>> tcb = active_tcbs->pop_front();
                        if (!tcb) continue;
                        std::optional<tcp_packet_t> tcp_packet = tcb.value()->gather_packet();
                        if (tcp_packet) return tcp_packet;
                }
                return std::nullopt;
        }

        void register_tcb(two_ends_t& two_end) {
                DLOG(INFO) << "[REGISTER TCB] " << two_end;
                if (!two_end.remote_info || !two_end.local_info) {
                        DLOG(FATAL) << "[EMPTY TCB]";
                }
                std::shared_ptr<tcb_t> tcb = std::make_shared<tcb_t>(
                        this->active_tcbs, two_end.remote_info.value(), two_end.local_info.value());
                tcbs[two_end] = tcb;
        }

        void receive(tcp_packet_t in_packet) {
                two_ends_t two_end = {.remote_info = in_packet.remote_info,
                                      .local_info  = in_packet.local_info};
                if (tcbs.find(two_end) != tcbs.end()) {
                        tcp_transmit::tcp_in(tcbs[two_end], in_packet);
                } else if (active_ports.find(in_packet.local_info.value()) != active_ports.end()) {
                        register_tcb(two_end);

                        if (tcbs.find(two_end) != tcbs.end()) {
                                tcp_transmit::tcp_in(tcbs[two_end], in_packet);
                        } else {
                                DLOG(ERROR) << "[REGISTER TCB FAIL]";
                        }

                } else {
                        DLOG(ERROR) << "[RECEIVE UNKNOWN TCP PACKET]";
                }
        }
};
}  // namespace mstack
#pragma once
#include <functional>
#include <iomanip>

#include "circle_buffer.hpp"
namespace mstack {

template <typename UnderPacketType, typename UpperPacketType, typename ChildType>
class base_protocol {
private:
        using packet_from_upper_type = std::function<std::optional<UpperPacketType>(void)>;
        using packet_to_upper_type   = std::function<void(UpperPacketType)>;
        std::unordered_map<int, packet_to_upper_type> _protocols;
        std::vector<packet_from_upper_type>           _packet_providers;
        circle_buffer<UnderPacketType>                packet_queue;

private:
        // base_protocol()  = default;
        // ~base_protocol() = default;

public:
        // base_protocol(const base_protocol&) = delete;
        // base_protocol(base_protocol&&)      = delete;
        // base_protocol& operator=(const base_protocol&) = delete;
        // base_protocol& operator=(base_protocol&&) = delete;
        static ChildType& instance() {
                static ChildType instance;
                return instance;
        }
        virtual int id() = 0;

        template <typename UpperProtocol>
        void register_upper_protocol(UpperProtocol& upper_protocol) {
                _packet_providers.push_back(
                        [&upper_protocol]() { return upper_protocol.gather_packet(); });
                _protocols[upper_protocol.id()] = [&upper_protocol](UpperPacketType packet) {
                        upper_protocol.receive(std::move(packet));
                };
        }

        virtual std::optional<UnderPacketType> make_packet(UpperPacketType in_packet) {
                return std::nullopt;
        }
        virtual std::optional<UpperPacketType> make_packet(UnderPacketType in_packet) {
                return std::nullopt;
        }

        void receive(UnderPacketType in_packet) {
                std::optional<UpperPacketType> in_packet_ = make_packet(std::move(in_packet));
                if (!in_packet_) return;
                dispatch(std::move(in_packet_));
        }

        void enter_send_queue(UnderPacketType in_packet) {
                packet_queue.push_back(std::move(in_packet));
        }

        void dispatch(std::optional<UpperPacketType> in_packet) {
                if (!in_packet) return;

                if (this->_protocols.find(in_packet->proto) == this->_protocols.end()) {
                        DLOG(INFO) << "[UNKNOWN PACKET] " << std::setiosflags(std::ios::uppercase)
                                   << std::hex << in_packet->proto;
                        return;
                }
                this->_protocols[in_packet->proto](std::move(in_packet.value()));
        }

        std::optional<UnderPacketType> gather_packet() {
                if (this->packet_queue.empty()) {
                        for (auto packet_provider : this->_packet_providers) {
                                std::optional<UpperPacketType> in_packet = packet_provider();
                                if (!in_packet) continue;
                                std::optional<UnderPacketType> in_packet_ =
                                        make_packet(std::move(in_packet.value()));
                                if (!in_packet_) continue;
                                packet_queue.push_back(std::move(in_packet_.value()));
                        }
                }
                return std::move(this->packet_queue.pop_front());
        }
};
};  // namespace mstack

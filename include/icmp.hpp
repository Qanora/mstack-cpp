#include "base_protocol.hpp"
#include "icmp_header.hpp"
#include "packets.hpp"
namespace mstack {
class icmp : public base_protocol<ipv4_packet, nop_packet, icmp> {
public:
        static constexpr uint16_t PROTO = 0x01;

        virtual int id() { return PROTO; }

        void make_icmp_reply(ipv4_packet& in_packet) {
                icmp_header_t in_icmp_header = icmp_header_t::consume(in_packet.buffer->get_pointer());

                icmp_header_t out_icmp_header;
                out_icmp_header.seq        = in_icmp_header.seq;
                out_icmp_header.id         = in_icmp_header.id;
                int total_len = in_packet.buffer->get_remaining_len();

                std::unique_ptr<base_packet> out_buffer = std::make_unique<base_packet>(total_len);
                uint8_t* payload_pointer = out_icmp_header.produce(out_buffer->get_pointer());
                in_packet.buffer->export_payload(payload_pointer, icmp_header_t::size());

                uint8_t* pointer  = out_buffer->get_pointer();
                uint16_t checksum = utils::checksum(pointer, out_buffer->get_remaining_len(), 0);

                out_icmp_header.checksum = checksum;
                out_icmp_header.produce(pointer);
                
                DLOG(INFO) << out_icmp_header;
                ipv4_packet out_packet = {.src_ipv4_addr = in_packet.dst_ipv4_addr,
                                          .dst_ipv4_addr = in_packet.src_ipv4_addr,
                                          .proto         = in_packet.proto,
                                          .buffer        = std::move(out_buffer)};
                DLOG(INFO) << "[SEND ICMP REPLY]";
                this->enter_send_queue(std::move(out_packet));
        }

        virtual std::optional<nop_packet> make_packet(ipv4_packet in_packet) {
                icmp_header_t in_icmp_header =
                        icmp_header_t::consume(in_packet.buffer->get_pointer());
                DLOG(INFO) << "[RECEIVED ICMP] " << in_icmp_header;
                if (in_icmp_header.proto_type == 0x08) {
                        make_icmp_reply(in_packet);
                }
                return std::nullopt;
        }

        virtual std::optional<ipv4_packet> make_packet(nop_packet in_packet) {
                return std::nullopt;
        }
};
}  // namespace mstack
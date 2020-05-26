#pragma once
#include "packet.hpp"
#include "packets.hpp"
#include "protocol.hpp"
#include "utils.hpp"

namespace mstack {

struct icmp_header_t {
    uint8_t proto_type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;

    static constexpr size_t size()
    {
        return 1 + 1 + 2 + 2 + 2;
    }

    uint8_t* consume(uint8_t* ptr)
    {
        proto_type = util::consume<uint8_t>(ptr);
        code = util::consume<uint8_t>(ptr);
        checksum = util::consume<uint16_t>(ptr);
        id = util::consume<uint16_t>(ptr);
        seq = util::consume<uint16_t>(ptr);
        return ptr;
    }

    uint8_t* produce(uint8_t* ptr)
    {
        util::produce<uint8_t>(ptr, proto_type);
        util::produce<uint8_t>(ptr, code);
        util::produce<uint16_t>(ptr, checksum);
        util::produce<uint16_t>(ptr, id);
        util::produce<uint16_t>(ptr, seq);
        return ptr;
    }
};

std::ostream& operator<<(std::ostream& out, icmp_header_t& m)
{
    using u = uint32_t;
    out << "[ICMP PACKET] ";
    out << u(m.proto_type) << " ";
    out << u(m.code) << " ";
    out << u(m.id) << " ";
    out << u(m.seq);
    return out;
};

class icmp : public protocol_interface<l3_packet> {
public:
    static constexpr int PROTO = 0x01;
    virtual int proto()
    {
        return PROTO;
    }

    virtual std::string tag()
    {
        return "ICMP";
    }

    static icmp& instance()
    {
        static icmp instance;
        return instance;
    }

    virtual void receive(l3_packet in_packet)
    {
        // DLOG(INFO) << "[RECEIVED ICMP] " << in_packet;
        icmp_header_t in_icmp_header;
        in_icmp_header.consume(in_packet._payload->get_pointer());

        if (in_icmp_header.proto_type == 0x08) {
            make_icmp_reply(std::move(in_packet));
        }
    }

    void make_icmp_reply(l3_packet in_l3_packet)
    {

        icmp_header_t in_icmp_header;
        in_icmp_header.consume(in_l3_packet._payload->get_pointer());

        icmp_header_t out_icmp_header;
        out_icmp_header.proto_type = 0x00;
        out_icmp_header.seq = in_icmp_header.seq;
        out_icmp_header.code = 0x00;
        out_icmp_header.id = in_icmp_header.id;
        out_icmp_header.checksum = 0x0000;

        int total_len = in_l3_packet._payload->get_remaining_len();

        std::unique_ptr<packet> out_packet = std::make_unique<packet>(total_len);
        uint8_t* payload_pointer = out_icmp_header.produce(out_packet->get_pointer());
        in_l3_packet._payload->export_payload(payload_pointer, icmp_header_t::size());

        l3_packet l3_out_packet(in_l3_packet._dst_ipv4_addr, in_l3_packet._src_ipv4_addr, PROTO, std::move(out_packet));
        packet_queue.push_back(std::move(l3_out_packet));
    }
};

struct icmp_hook_t {
    void icmp_filter(std::optional<l3_packet>& in_packet)
    {
        if (in_packet->_proto != icmp::PROTO) {
            DLOG(ERROR) << "[ERROR PROTOCOL]";
            return;
        }
        uint8_t* pointer = in_packet->_payload->get_pointer();
        uint16_t checksum = util::checksum(pointer, in_packet->_payload->get_remaining_len(), 0);
        icmp_header_t in_icmp_packet;
        in_icmp_packet.consume(pointer);
        in_icmp_packet.checksum = checksum;
        in_icmp_packet.produce(pointer);
        // DLOG(INFO) << "[ICMP FILTER] " << in_packet.value();
    }
};
}
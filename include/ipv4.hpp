#pragma once
#include "packets.hpp"
#include "protocol.hpp"
#include "utils.hpp"

namespace mstack {
struct ipv4_header_t {
    uint8_t version : 4;
    uint8_t header_length : 4;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t id;
    uint16_t NOP : 1;
    uint16_t DF : 1;
    uint16_t MF : 1;
    uint16_t frag_offset : 13;
    uint8_t ttl;
    uint8_t proto_type;
    uint16_t header_checksum;
    ipv4_addr_t src_ip_addr;
    ipv4_addr_t dst_ip_addr;

    static constexpr size_t size()
    {
        return 1 + 1 + 2 + 2 + 2 + 1 + 1 + 2 + ipv4_addr_t::size() * 2;
    }

    void consume(uint8_t* ptr)
    {
        uint8_t version_header_length = util::consume<uint8_t>(ptr);
        version = version_header_length >> 4;
        header_length = version_header_length & 0xF;
        type_of_service = util::consume<uint8_t>(ptr);
        total_length = util::consume<uint16_t>(ptr);
        id = util::consume<uint16_t>(ptr);
        uint16_t flags_farg_offset = util::consume<uint16_t>(ptr);
        NOP = flags_farg_offset >> 15;
        DF = (flags_farg_offset >> 14) & 0x1;
        MF = (flags_farg_offset >> 13) & 0x1;
        frag_offset = flags_farg_offset & ((1 << 13) - 1);
        ttl = util::consume<uint8_t>(ptr);
        proto_type = util::consume<uint8_t>(ptr);
        header_checksum = util::consume<uint16_t>(ptr);
        src_ip_addr.consume(ptr);
        dst_ip_addr.consume(ptr);
    }

    void produce(uint8_t* ptr)
    {
        util::produce<uint8_t>(ptr, version << 4 | header_length);
        util::produce<uint8_t>(ptr, type_of_service);
        util::produce<uint16_t>(ptr, total_length);
        util::produce<uint16_t>(ptr, id);
        util::produce<uint16_t>(ptr, NOP << 15 | DF << 14 | MF << 13 | frag_offset);
        util::produce<uint8_t>(ptr, ttl);
        util::produce<uint8_t>(ptr, proto_type);
        util::produce<uint16_t>(ptr, header_checksum);
        src_ip_addr.produce(ptr);
        dst_ip_addr.produce(ptr);
    }
};

std::ostream& operator<<(std::ostream& out, ipv4_header_t& m)
{
    using u = uint32_t;
    out << "[IPV4 PACKET] ";
    out << m.total_length << " ";
    out << m.src_ip_addr;
    out << " -> " << m.dst_ip_addr << " ";
    out << "PROTO: " << std::hex << u(m.proto_type);
    return out;
};

struct ipv4_hook_t {
    uint16_t id = 0;

    void ipv4_filter(std::optional<l3_packet>& in_packet)
    {
        in_packet->_payload->reflush_packet(ipv4_header_t::size());
        ipv4_header_t out_ipv4_header;

        out_ipv4_header.version = 0x4;
        out_ipv4_header.header_length = 0x5;
        out_ipv4_header.total_length = in_packet->_payload->get_total_len() + ipv4_header_t::size();
        out_ipv4_header.id = id++;
        out_ipv4_header.NOP = 0x0;
        out_ipv4_header.DF = 0x0;
        out_ipv4_header.MF = 0x0;
        out_ipv4_header.frag_offset = 0x00;
        out_ipv4_header.ttl = 0x40;
        out_ipv4_header.proto_type = in_packet->_proto;
        out_ipv4_header.header_checksum = 0x0000;
        out_ipv4_header.src_ip_addr = (in_packet->_src_ipv4_addr).value();
        out_ipv4_header.dst_ip_addr = (in_packet->_dst_ipv4_addr).value();

        uint8_t* pointer = in_packet->_payload->get_pointer();
        out_ipv4_header.produce(pointer);
        uint16_t checksum = util::checksum(pointer, ipv4_header_t::size(), 0);
        out_ipv4_header.header_checksum = checksum;
        out_ipv4_header.produce(pointer);
        in_packet->_proto = IPV4_PROTO;
        // LOG(INFO) << out_ipv4_header;
    }
};
}
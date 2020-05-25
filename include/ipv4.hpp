#pragma once
#include "utils.hpp"
#include "protocol.hpp"
#include "packets.hpp"
#include "icmp.hpp"

namespace mstack{
    struct ipv4_header_t
    {
        uint8_t version : 4;
        uint8_t header_length : 4;
        uint8_t type_of_service;
        uint16_t total_length;
        uint16_t id;
        uint16_t NOP : 1;
        uint16_t DF: 1;
        uint16_t MF: 1;
        uint16_t frag_offset: 13;
        uint8_t ttl;
        uint8_t proto_type;
        uint16_t header_checksum;
        ipv4_addr_t src_ip_addr;
        ipv4_addr_t dst_ip_addr;

        static constexpr size_t size() {
            return 1 + 1 + 2 + 2 + 2 + 1 + 1 + 2 + ipv4_addr_t::size() * 2;
        }

        void consume(uint8_t* ptr){
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

        void produce(uint8_t* ptr){
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
        out << m.src_ip_addr;
        out << " -> " << m.dst_ip_addr << " ";
        out << "PROTO: " << std::hex << u(m.proto_type);
        return out;
    };

    class ipv4 : public protocol_interface<l3_packet> {
    public:

        static constexpr int PROTO = 0x0800;
        virtual int proto(){
            return PROTO;
        }

        virtual std::string tag(){
            return "IPV4";
        }

        static ipv4& instance()
        {
            static ipv4 instance;
            return instance;
        }
        
        virtual void receive(l3_packet in_packet){
            DLOG(INFO) << "[RECEIVED IPV4] " << in_packet;
            ipv4_header_t in_ipv4_header;
            in_ipv4_header.consume(in_packet._payload->get_pointer());
            DLOG(INFO) << in_ipv4_header;
            if(in_ipv4_header.proto_type == icmp::PROTO) {
                if (_forward_to_layer_func){
                    DLOG(INFO) << "[IPV4 -> ICMP]";
                    in_packet._payload->add_offset(ipv4_header_t::size());
                    l3_packet forward_packet(in_ipv4_header.src_ip_addr, in_ipv4_header.dst_ip_addr, in_ipv4_header.proto_type, std::move(in_packet._payload));
                    _forward_to_layer_func.value()(std::move(forward_packet));
                }
            }
        }


    };
}
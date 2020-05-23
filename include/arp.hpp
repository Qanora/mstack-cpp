#pragma once
#include <ios>
#include "protocol.hpp"
#include "layer.hpp"
#include "ethernet.hpp"
#include "ipv4.hpp"

namespace mstack{

    struct arpv4_hdr{
        uint16_t hard_type;
        uint16_t proto;
        uint8_t hard_size;
        uint8_t proto_size;
        uint16_t op;
        uint8_t remote_mac[6];
        uint32_t remote_ip;
        uint8_t local_mac[6];
        uint32_t local_ip;
    }__attribute__((__packed__));

    std::ostream& operator<<(std::ostream& out, arpv4_hdr& m)
    {
        using u = uint32_t;
        out << "OP: " << m.op << " ";
        out << "LOCAL: " << mac_addr(m.local_mac) <<", " << ipv4_addr(m.local_ip);
        out << " REMOTE: " << mac_addr(m.remote_mac) <<", " << ipv4_addr(m.remote_ip);
        return out;
    }
    class arp : public protocol_interface<l2_packet> {
    public:
        virtual std::string tag(){
            return "ARP";
        }
        static arp& instance()
        {
            static arp instance;
            return instance;
        }
        
        virtual int proto() {
            return 0x0608;
        }
        virtual void receive(l2_packet packet){
            struct arpv4_hdr* arp_packet = packet._payload->get_header<arpv4_hdr>();
            arp_packet->remote_ip = util::ntoh(arp_packet->remote_ip);
            arp_packet->local_ip = util::ntoh(arp_packet->local_ip);

            
            DLOG(INFO) << "[ARP RECEIVE] " <<  *arp_packet;
        }
    };
};
#pragma once
#include <ios>
#include <memory>
#include "protocol.hpp"
#include "layer.hpp"
#include "ethernet.hpp"
#include "ipv4.hpp"

namespace mstack{

    struct arpv4_header_t{
        uint16_t hw_type;
        uint16_t proto_type;
        uint8_t hw_size;
        uint8_t proto_size;
        uint16_t opcode;
        mac_addr_t src_mac_addr;
        ipv4_addr_t src_ip_addr;
        mac_addr_t dst_mac_addr;
        ipv4_addr_t dst_ip_addr;
    
        static constexpr size_t size() {
            return 2 + 2 + 1 + 1 + 2 + mac_addr_t::size() * 2 + ipv4_addr_t::size() * 2;
        }

        void consume(uint8_t* ptr){
            hw_type = util::consume<uint16_t>(ptr);
            proto_type = util::consume<uint16_t>(ptr);
            hw_size = util::consume<uint8_t>(ptr);
            proto_size = util::consume<uint8_t>(ptr);
            opcode = util::consume<uint16_t>(ptr);
            src_mac_addr.consume(ptr);
            src_ip_addr.consume(ptr);
            dst_mac_addr.consume(ptr);
            dst_ip_addr.consume(ptr);
        }

        void produce(uint8_t* ptr){
            util::produce<uint16_t>(ptr, hw_type);
            util::produce<uint16_t>(ptr, proto_type);
            util::produce<uint8_t>(ptr, hw_size);
            util::produce<uint8_t>(ptr, proto_size);
            util::produce<uint16_t>(ptr, opcode);
            src_mac_addr.produce(ptr);
            src_ip_addr.produce(ptr);
            dst_mac_addr.produce(ptr);
            dst_ip_addr.produce(ptr);
        }
    };

    std::ostream& operator<<(std::ostream& out, arpv4_header_t& m)
    {
        using u = uint32_t;
        out << "OP: " << m.opcode << " ";
        out << "LOCAL: " << mac_addr_t(m.dst_mac_addr) <<", " << ipv4_addr_t(m.dst_ip_addr);
        out << " REMOTE: " << mac_addr_t(m.src_mac_addr) <<", " << ipv4_addr_t(m.src_ip_addr);
        return out;
    }

    class arp : public protocol_interface<l2_packet> {
    public:
        virtual int proto(){
            return 0x0806;
        }
        virtual std::string tag(){
            return "ARP";
        }

        static arp& instance()
        {
            static arp instance;
            return instance;
        }
        
        virtual void receive(l2_packet packet){
            struct arpv4_header_t arp_header;
            arp_header.consume(packet._payload->get_pointer());
            // DLOG(INFO) << "[ARP RECEIVE] " << arp_header;
            if(arp_header.opcode == 0x0001){
                DLOG(INFO) << "[ARP REQUEST] " << arp_header;
                // make_response_packet(arp_header);
            }

        }

        // void make_response_packet(arpv4_header_t* in_header) {


        //     packet out_packet = packet<arpv4_header_t>();

        //     struct arpv4_header_t* out_header = out_packet.get_header<arpv4_header_t>();

        //     out_header->hw_type = 0x0001;
        //     out_header->proto_type = 0x0800;
        //     out_header->hw_size = 0x06;
        //     out_header->proto_size = 0x02;
        //     out_header->opcode = 0x02

        //     out_header->src_ip = in_header.dst_ip;
        //     std::copy(in_header->src_mac, in_header->src_mac + in_header->hw_size, out_packet->dst_mac);
        //     out_header->dst_ip = in_header.src_ip;

        //     l2_packet out_l2_packet = l2_packet>(in_header->src_mac, PROTO, std::move(out_packet));

        //     packet_queue.push_back(std::move(out_l2_packet));

        // }  
    };

    struct arp_hook{

        std::unordered_map<int, mac_addr_t> hw_addr_map;

        template<typename DEV>
        void add(DEV& dev) {
            std::array<uint8_t, DEV::HW_SIZE> hw_addr = dev.get_hw_addr(); 
            hw_addr_map[DEV::TAG] = mac_addr_t(hw_addr);
            DLOG(INFO) << "[REGISTER DEV] " << DEV::TAG << " " << hw_addr_map[DEV::TAG];
        }

        mac_addr_t& query(int tag){
            if(hw_addr_map.find(tag) != hw_addr_map.end()) {
                DLOG(ERROR) << "[UNKONWN DEV]";
            }
            return hw_addr_map[tag];
        }

        void arp_filter(std::optional<l2_packet> packet){

        }
        
        std::optional<raw_packet> arp_convert(std::optional<l2_packet> packet){
            if(packet->_proto == arp::PROTO) {
                vector<uint8> dev_hw_addr = dev_hw_addr.query(TUNTAP_DEV);
                struct arpv4_hdr* arp_packet = packet._payload->get_header<arpv4_hdr>();
                std::copy(begin(dev_hw_addr), end(dev_hw_addr), arp_packet->src_mac);
                
                packet._payload.reflush_packet<eth_hdr>();

                struct eth_hdr* eth_packet = packet._payload->get_header<eth_hdr>();

                std::copy(begin(packet->_remote_mac_addr), end(packet->_remote_mac_addr), eth_packet->dst_mac_addr);
                std::copy(begin(dev_hw_addr), end(dev_hw_addr), eth_packet->src_mac_addr);

                raw_packet out_packet(TUNTAP_DEV, std::move(packet._payload));
                return std::move(out_packet)
            }
        }
    }
};
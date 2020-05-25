#pragma once
#include "ipv4_addr.hpp"
#include "mac_addr.hpp"
#include "packet.hpp"
#include <iomanip>

namespace mstack{
struct l3_packet {
    using l3_proto = uint16_t;
    static constexpr int tag = L3_PACKET;

    l3_proto _proto;
    std::optional<ipv4_addr_t> _src_ipv4_addr;
    std::optional<ipv4_addr_t> _dst_ipv4_addr;
    std::unique_ptr<packet> _payload;

    l3_packet(std::optional<ipv4_addr_t> saddr, std::optional<ipv4_addr_t> daddr, 
            l3_proto pr,std::unique_ptr<packet>  pa): _src_ipv4_addr(saddr),
            _dst_ipv4_addr(daddr), _proto(pr), _payload(std::move(pa)){};
    friend std::ostream& operator<<(std::ostream& out, l3_packet& p)
    {
        if(p._src_ipv4_addr) {
            out << p._src_ipv4_addr.value();
        } else{
            out << "NONE";
        }
        out << " -> ";
        if (p._dst_ipv4_addr) {
            out << p._dst_ipv4_addr.value();
        } else {
            out << "NONE";
        }
        out << " PROTO: " <<  std::setiosflags(std::ios::uppercase)<< std::hex << p._proto;
        return out;
    };
};

struct l2_packet {
    using l2_proto = uint16_t;
    static constexpr int tag = L2_PACKET;

    std::optional<ipv4_addr_t> _dst_ipv4_addr;
    std::optional<ipv4_addr_t> _src_ipv4_addr;
    l2_proto _proto;

    std::unique_ptr<packet> _payload;

    l2_packet(std::optional<ipv4_addr_t> ip, l2_proto pr, std::unique_ptr<packet> pa) : _dst_ipv4_addr(ip), _proto(pr), _payload(std::move(pa)){}
    
    // l2_packet(l2_packet&) = delete;
    // l2_packet& operator=(l2_packet&) = delete;

    // l2_packet(l2_packet&& other){
    //     _payload = std::move(other._payload);
    //     _remote_ipv4_addr = std::move(other._remote_ipv4_addr);
    //     _proto = std::move(other._proto);
    // }

    // l2_packet& operator=(l2_packet&& other){
    //     _payload = std::move(other._payload);
    //     _remote_ipv4_addr = std::move(other._remote_ipv4_addr);
    //     _proto = std::move(other._proto);
    //     return *this;
    // }

    friend std::ostream& operator<<(std::ostream& out, l2_packet& p)
    {
        if(p._src_ipv4_addr) {
            out << p._src_ipv4_addr.value();
        } else{
            out << "NONE";
        }
        out << " -> ";
        if (p._dst_ipv4_addr) {
            out << p._dst_ipv4_addr.value();
        } else {
            out << "NONE";
        }
        out << " PROTO: " <<  std::setiosflags(std::ios::uppercase)<< std::hex << p._proto;
        return out;
    }
};

struct raw_packet {
    using raw_proto = uint16_t;
    static constexpr int tag = RAW_PACKET;

    raw_proto _proto; // TUNTAP_DEV
    std::unique_ptr<packet> _payload;

    // raw_packet& operator=(raw_packet& other) = delete;
    // raw_packet(raw_packet& other) = delete;

    // raw_packet& operator=(raw_packet&& other){
    //     _proto = std::move(other._proto);
    //     _payload = std::move(other._payload);
    //     return *this;
    // }

    // raw_packet(raw_packet&& other){
    //     _proto = std::move(other._proto);
    //     _payload = std::move(other._payload);
    // }
    raw_packet(raw_proto pr, std::unique_ptr<packet> pa): _proto(pr), _payload(std::move(pa)){}
};
}
#pragma once
#include "defination.hpp"
#include "ipv4_addr.hpp"
#include "mac_addr.hpp"
#include "packet.hpp"

#include <iomanip>
#include <memory>
#include <optional>
namespace mstack {

struct l4_packet {
    using l4_proto = uint16_t;
    using port_addr_t = uint16_t;
    static constexpr int tag = L4_PACKET;
    l4_proto _proto;

    std::optional<ipv4_addr_t> _src_ipv4_addr;
    std::optional<ipv4_addr_t> _dst_ipv4_addr;
    std::optional<port_addr_t> _src_port_addr;
    std::optional<port_addr_t> _dst_port_addr;
    std::unique_ptr<packet> _payload;

    l4_packet(std::optional<ipv4_addr_t> sip, std::optional<ipv4_addr_t> dip,
        std::optional<port_addr_t> sport, std::optional<port_addr_t> dport,
        l4_proto pr, std::unique_ptr<packet> pa)
        : _src_ipv4_addr(sip)
        , _dst_ipv4_addr(dip)
        , _src_port_addr(sport)
        , _dst_port_addr(dport)
        , _proto(pr)
        , _payload(std::move(pa)){};

    friend std::ostream& operator<<(std::ostream& out, l4_packet& p)
    {
        if (p._src_ipv4_addr && p._src_port_addr) {
            out << p._src_ipv4_addr.value() << " ";
            out << p._src_port_addr.value();
        } else {
            out << "NONE";
        }
        out << " -> ";
        if (p._dst_ipv4_addr && p._dst_port_addr) {
            out << p._dst_ipv4_addr.value() << " ";
            out << p._dst_port_addr.value();
        } else {
            out << "NONE";
        }
        out << " PROTO: " << std::setiosflags(std::ios::uppercase) << std::hex << p._proto;
        return out;
    };
};

struct l3_packet {
    using l3_proto = uint16_t;
    static constexpr int tag = L3_PACKET;
    l3_proto _proto;

    std::optional<ipv4_addr_t> _src_ipv4_addr;
    std::optional<ipv4_addr_t> _dst_ipv4_addr;
    std::unique_ptr<packet> _payload;

    l3_packet(std::optional<ipv4_addr_t> sip, std::optional<ipv4_addr_t> dip,
        l3_proto pr, std::unique_ptr<packet> pa)
        : _src_ipv4_addr(sip)
        , _dst_ipv4_addr(dip)
        , _proto(pr)
        , _payload(std::move(pa)){};

    friend std::ostream& operator<<(std::ostream& out, l3_packet& p)
    {
        if (p._src_ipv4_addr) {
            out << p._src_ipv4_addr.value();
        } else {
            out << "NONE";
        }
        out << " -> ";
        if (p._dst_ipv4_addr) {
            out << p._dst_ipv4_addr.value();
        } else {
            out << "NONE";
        }
        out << " PROTO: " << std::setiosflags(std::ios::uppercase) << std::hex << p._proto;
        return out;
    };
};

struct l2_packet {
    using l2_proto = uint16_t;
    static constexpr int tag = L2_PACKET;

    std::optional<mac_addr_t> _src_mac_addr;
    std::optional<mac_addr_t> _dst_mac_addr;
    l2_proto _proto;
    std::unique_ptr<packet> _payload;

    l2_packet(std::optional<mac_addr_t> smac, std::optional<mac_addr_t> dmac,
        l2_proto pr, std::unique_ptr<packet> pa)
        : _src_mac_addr(smac)
        , _dst_mac_addr(dmac)
        , _proto(pr)
        , _payload(std::move(pa))
    {
    }

    friend std::ostream& operator<<(std::ostream& out, l2_packet& p)
    {
        if (p._src_mac_addr) {
            out << p._src_mac_addr.value();
        } else {
            out << "NONE";
        }
        out << " -> ";
        if (p._dst_mac_addr) {
            out << p._dst_mac_addr.value();
        } else {
            out << "NONE";
        }
        out << " PROTO: " << std::setiosflags(std::ios::uppercase) << std::hex << p._proto;
        return out;
    }
};
}
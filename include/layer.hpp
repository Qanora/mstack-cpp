#pragma once
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include "circle_buffer.hpp"
#include "ethernet.hpp"
#include "logger.hpp"
#include "packet.hpp"
#include "protocol.hpp"
#include "utils.hpp"
#include "defination.hpp"

namespace mstack {

class raw_packet {
public:
    using raw_proto = uint16_t;
    static constexpr int tag = RAW_PACKET;
    raw_proto proto; // TUNTAP_DEV
    std::unique_ptr<packet> payload;
    raw_packet(raw_proto pr, std::unique_ptr<packet> pa): proto(pr), payload(std::move(pa)){}
};

class l2_packet {
public:
    using l2_proto = uint16_t;
    std::optional<mac_addr> _remote_mac_addr;
    l2_proto _proto;
    std::unique_ptr<packet> _payload;
    static constexpr int tag = L2_PACKET;
    l2_packet(std::optional<mac_addr> m, l2_proto pr, std::unique_ptr<packet> pa) : _remote_mac_addr(m), _proto(pr), _payload(std::move(pa)){}
    friend std::ostream& operator<<(std::ostream& out, l2_packet& p)
    {
        out << "remote_mac_addr: ";
        if(p._remote_mac_addr) {
            out << *(p._remote_mac_addr);
        } else{
            out << "None";
        }
        out << "proto: " << p._proto;
        return out;
    }
};


// class l3_packet {
//     ip_addr remote_ip_addr;
//     l3_proto proto;
//     buffer payload
// };

// class l4_packet {
//     ip_addr remote_ip_addr;
//     port remote_port;
//     l4_proto proto;
//     buffer payload;
// };

template <typename CurrentPacketType>
class base_hook_funcs {
public:
    virtual void
    hook_register(protocol_interface<CurrentPacketType>& protocol)
    {
        DLOG(INFO) << "[REGISTER] " << protocol.tag();
    }

    virtual std::optional<CurrentPacketType>
    hook_dispatch(std::optional<CurrentPacketType> packet)
    {
        return packet;
    }

    virtual std::optional<CurrentPacketType>
    hook_gather(std::optional<CurrentPacketType> packet)
    {
        return packet;
    }
};

template <typename OtherPacketType, typename CurrentPacketType,
    typename HookFuncs = base_hook_funcs<CurrentPacketType>>
class layer {
private:
    using packet_provider_type = std::function<std::optional<CurrentPacketType>(void)>;
    using packet_receiver_type = std::function<void(CurrentPacketType)>;
    std::unordered_map<int, packet_receiver_type> _protocols;
    std::vector<packet_provider_type> _packet_providers;
    circle_buffer<CurrentPacketType> packet_queue;
    HookFuncs hook_funcs;

private:
    layer() = default;
    ~layer() = default;

public:
    layer(const layer&) = delete;
    layer(layer&&) = delete;
    layer& operator=(const layer&) = delete;
    layer& operator=(layer&&) = delete;

    static layer& instance()
    {
        static layer instance;
        return instance;
    }

    template <typename DEV>
    void register_dev(DEV& dev)
    {
        dev.register_provider_func([&]() { return std::move(instance().gather_packet()); });
        dev.register_receiver_func([&](raw_packet raw_packet) { instance().receive(std::move(raw_packet)); });
    }

    void register_protocol(protocol_interface<CurrentPacketType>& protocol)
    {

        // std::optional<std::reference_wrapper<protocol_interface<CurrentPacketType>>> 
        this->hook_funcs.hook_register(protocol);

        //_packet_providers.push_back([&protocol]() { return protocol.gather_packet(); });
        _protocols[protocol.proto()] = [&protocol](CurrentPacketType packet) { protocol.receive(std::move(packet)); };
    }

    void receive(OtherPacketType packet)
    {
        std::optional<CurrentPacketType> packet_ = make_packet<CurrentPacketType>(std::move(packet));
        
        std::optional<int> proto = get_proto<CurrentPacketType>(packet_);

        if (!proto) {
            DLOG(ERROR) << "[UNKNOWN PROTO]";
            return;
        }

        if (this->_protocols.find(*proto) == this->_protocols.end()) {
            DLOG(ERROR) << "[UNKNOWN PROTOCOL] " << std::setiosflags(std::ios::uppercase) << std::hex << *proto;
            return;
        }

        packet_ = this->hook_funcs.hook_dispatch(std::move(packet_));

        this->_dispatch(std::move(packet_));
    }

    void _dispatch(std::optional<CurrentPacketType> packet)
    {

        if (!packet)
            return;

        std::optional<int> proto = get_proto<CurrentPacketType>(packet);

        if(!proto) {
            DLOG(ERROR) << "[DISPATCH FAIL]";
            return;
        }
        CHECK(this->_protocols.find(*proto) != this->_protocols.end())
            << "[UNKNOW PROTCOL]";

        this->_protocols[*proto](std::move(*packet));
    }

    std::optional<OtherPacketType> gather_packet()
    {

        if (this->packet_queue.empty()) {
            for (auto packet_provider : this->_packet_providers) {
                std::optional<CurrentPacketType> packet = packet_provider();
                if (!packet) {
                    packet_queue.push_back(std::move(*packet));
                }
            }
        }

        if (this->packet_queue.empty()) {
            return std::nullopt;
        }

        std::optional<CurrentPacketType> packet = packet_queue.pop_front(); 

        packet = this->hook_funcs.hook_gather(std::move(packet));
        if(!packet){
            return std::nullopt;
        }
        std::optional<OtherPacketType> packet_ = make_packet<OtherPacketType>(std::move(*packet));      

        return packet_;
    }
};

// class l3_hook : public base_hook_funcs<l3_packet> {
// public:
//     struct ipv4_reassemble {
//     };
// };

// class l3_layer<l2_packet, l3_packet, l3_hook> {
// };

// class l4_layer<l3_packet, l4_packet> {
// };

}; // namespace mstack
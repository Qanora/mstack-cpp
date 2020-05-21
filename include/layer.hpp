#include <functional>
#include <glog/logging.h>
#include <optional>
#include <vector>

namespace mstack {

class raw_packet {
    raw_proto proto;
    buffer payload;
};

class l2_packet {
    mac_addr remote_mac_addr;
    l2_proto proto;
    buffer payload;
};

class l3_packet {
    ip_addr remote_ip_addr;
    l3_proto proto;
    buffer payload
};

class l4_packet {
    ip_addr remote_ip_addr;
    port remote_port;
    l4_proto proto;
    buffer payload;
};

template <typname CurrentPacketType>
class base_hook_funcs {
public:
    virtual std::optional<protocol_interface>
    hook_register(std::optional<protocol_interface> protocol)
    {
        return protocol
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
    typename ChildType,
    typename HookFuncs = base_hook_funcs<CurrentPacketType>>
class layer {
private:
    using packet_provider_type = std::function<optional<OtherPacketType>()>;
    using packet_receiver_type = std::function<optional<(CurrentPacketType)>>;
    unordered_map<int, packet_receiver_type> _protocols;
    vector<packet_provider_type> _packet_providers;
    circle_buffer<CurrentPacketType> packet_queue;
    HookFuncs hook_funcs;

public:
    void register_packet_privder(protocol_interface<CurrentPacketType> protocol)
    {

        std::optional<protocol_interface<CurrentPacketType>> protocol = this->hook_funcs.hook_register(protocol);
        if (!protocol)
            return;

        this->_packet_provider.push_back((*protocols).gather_packet);
        _protocols[protocols->proto] = (*protocols).receive;
    }

    void receive(OtherPacketType packet)
    {

        std::optional<int> proto = get_proto<CurrentPacketType>(packet);

        if (!proto) {
            LOG(INFO) << "[UNKNOWN PACKET]" return;
        }
        if (this->_protocols.find(proto) == this->_protocols.end()) {
            LOG(INFO) << "[UNKNOWN PROTOCOL]";
            return;
        }
        std::optional<CurrentPacketType> packet = make_packet<CurrentPacketType>(packet);

        std::optional<CurrentPacketType> packet = this->hook_funcs.hook_dispatch(packet);

        this->_dispatch(packet);
    }

    void _dispatch(std::optional<CurrentPacketType> packet)
    {

        if (!packet)
            return;

        std::optional<int> proto = get_proto<CurrentPacketType>(*packet);

        CHECK(this->_protocols.find(proto) != this->_protocols.end())
            << "[UNKNOW PROTCOL]";

        this->_protocols[proto].receive(packet);
    }

    std::optional<CurrentPacketType> gather_packet()
    {

        if (this->packet_queue.empty()) {
            for (auto packet_provider : this->__packet_providers) {
                std::optional<CurrentPacketType> packet = packet_provider();
                if (!packet) {
                    packet_queue.push_back(*packet);
                }
            }
        }

        if (this->packet_queue.empty()) {
            return std::nullopt;
        }

        std::optional<CurrentPacketType> packet = packet_queue.front();
        packet_queue.pop_front();

        packet = this->hook_funcs.hook_gather(packet);

        return packet;
    }
};

class l2_hook : public base_hook_funcs<l2_packet> {
    struct arpv4_request {
    };
    struct arpv4_cache {
    };
};

class l2_layer<raw_packet, l2_packet, l2_layer, l2_hook> {
};

class l3_hook : public base_hook_funcs<l3_packet> {
public:
    struct ipv4_reassemble {
    };
};

class l3_layer<l2_packet, l3_packet, l3_layer, l3_hook> {
};

class l4_layer<l3_packet, l4_packet, l4_layer> {
};

}; // namespace mstack
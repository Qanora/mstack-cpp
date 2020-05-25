
#pragma once
#include "circle_buffer.hpp"
#include <optional>
namespace mstack {

template <typename CurrentPacketType>
class protocol_interface {
private:
    // protocol_interface() = default;
    // ~protocol_interface() = default;
    // protocol_interface(const protocol_interface&) = default;
    // protocol_interface(protocol_interface&&) = default;
    // protocol_interface& operator=(const protocol_interface&) = default;
    //   protocol_interface& operator=(protocol_interface&&) = default;  
public:
    circle_buffer<CurrentPacketType> packet_queue;
    using packet_receiver_type = std::function<void(CurrentPacketType)>;
    std::optional<packet_receiver_type> _forward_to_layer_func;
    
    virtual int proto(){
        return 0x0000;
    }

    virtual std::string tag() {
        return "NULL PROTO";
    }

    virtual void receive(CurrentPacketType packet){
        return;
    }

    virtual std::optional<CurrentPacketType> gather_packet()
    {
        if (this->packet_queue.empty()) {
            return std::nullopt;
        }
        CurrentPacketType packet = std::move(packet_queue.pop_front());
        return std::make_optional<CurrentPacketType>(std::move(packet));
    }
    
    void register_forward_layer(packet_receiver_type forward_to_layer_func){
         _forward_to_layer_func = forward_to_layer_func;
    }
};

template <typename CurrentPacketType>
std::optional<int> get_proto(std::optional<CurrentPacketType>& packet)
{
    DLOG(ERROR) << "[UNKNOWN PROTO] " << CurrentPacketType::tag;
    return std::nullopt;
}
}; // namespace mstack
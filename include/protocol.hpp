
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
    virtual std::string tag(){
        return "NULL";
    }
    virtual int proto() {
        DLOG(ERROR) << "[NULL PROTO]";
        return 0;
    }
    virtual void receive(CurrentPacketType packet){
        return;
    }

    virtual std::optional<CurrentPacketType> gather_packet()
    {
        if (this->packet_queue.empty()) {
            return std::nullopt;
        }
        auto packet = packet_queue.pop_front();
        return packet;
    }
};

template <typename CurrentPacketType>
std::optional<int> get_proto(std::optional<CurrentPacketType>& packet)
{
    DLOG(ERROR) << "[UNKNOWN PROTO] " << CurrentPacketType::tag;
    return std::nullopt;
}
}; // namespace mstack
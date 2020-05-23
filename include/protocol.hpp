
#pragma once
#include "circle_buffer.hpp"
namespace mstack {

template <typename CurrentPacketType>
class protocol_interface {
public:
    circle_buffer<CurrentPacketType> packet_queue;

    virtual void receive(CurrentPacketType packet) = 0;

    virtual CurrentPacketType gather_packet()
    {
        if (this->packet_queue.empty()) {
            return std::nullopt;
        }
        auto packet = packet_queue.front();
        packet_queue.pop_front();
        return packet;
    }
};

template <typename CurrentPacketType>
int get_proto(CurrentPacketType packet)
{
    return 0;
}
}; // namespace mstack
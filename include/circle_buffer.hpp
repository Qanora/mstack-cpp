#pragma once
#include <optional>
namespace mstack {
template <typename PacketType>
class circle_buffer {
public:
    bool empty(){ return true; }
    void push_back(PacketType packet){}
    std::optional<PacketType> pop_front(){ return std::nullopt; }
};
}
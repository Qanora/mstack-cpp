#pragma once
#include <optional>
#include <memory>
#include <queue>
namespace mstack {
template <typename PacketType>
class circle_buffer {
private:
    std::queue<PacketType> packets;
public:
    bool empty(){ return packets.size() == 0; }
    void push_back(PacketType packet){
        packets.push(std::move(packet));
    }
    std::optional<PacketType> pop_front(){
        // auto packet = std::move(packets.front());
        // packets.pop();
        // return packet.get();
        return std::nullopt;
    }
};
}
#pragma once
#include <memory>
#include <optional>
#include <queue>
namespace mstack {
template <typename PacketType>
class circle_buffer {
private:
    std::queue<PacketType> packets;

public:
    bool empty() { return packets.size() == 0; }
    void push_back(PacketType packet)
    {
        packets.push(std::move(packet));
    }

    std::optional<PacketType> pop_front()
    {
        if (empty()) {
            return std::nullopt;
        }
        auto packet = std::move(packets.front());
        packets.pop();
        return std::move(packet);
    }
};
}
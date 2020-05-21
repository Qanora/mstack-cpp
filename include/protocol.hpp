
namespace mstack {

template <CurrentPacketType>
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
}

template <CurrentPacketType>
std::optional<int> get_proto(CurrentPacketType packet)
{
    return;
}

template <CurrentPacketType, OtherPacketType>
std::optional<CurrentPacketType> make_packet(OtherPacketType packet)
{
    return;
}

} // namespace mstack
}
;
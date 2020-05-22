#include "logger.hpp"
#include <memory>

namespace mstack {

class packet {
private:
    std::vector<unique_ptr<pair<int, char[]>>> _data_stack;
    std::unique_ptr<char[]> _raw_data;
    int _total_len;
    int _head;
    int _len;

public:
    packet(char* buf, int len)
        : _raw_data(std::make_unique<char[]>(len))
        , _head(0)
        , _len(len)
    {
        std::copy(buf, buf + len, begin(_raw_data.get()));
    }
    packet(int len)
        : _raw_data(std::make_unique<char[]>(len))
        , _head(0)
        , _len(len)
    {
    }

    ~packet() = default;
    packet(packet&) = delete;
    packet(packet&& other)
    {
        *this = std::move(other);
    }
    packet& operator=(packet&) = delete;
    packet& operator=(packet&& other)
    {
        raw_data = std::move(other.raw_data);
        std::swap(head, other.head);
        std::swap(len, other.len);
    }

public:
    void reflush_packet(int len)
    {
        _data_stack.push_back({ _len, std::move(_raw_data) });
        _head = len;
        _len = 0;
        _raw_data = sdt::make_unique<char[]>(len);
    }

    void export_data(char* buf, int& len)
    {
        if (_total_len > len) {
            len = 0;
            return;
        }
        int index = 0;
        for (int i = 0; i < _data_stack.size(); i++) {
            for (int j = 0; j < _data_stack[i].first; j++) {
                buf[index++] = data_stack[i].second[j];
            }
        }
        len = index;
    }
};

template <typename From, typename To>
To make_packet(From packet)
{
    LOG(ERROR) << "UNKNOW PACKET";
    return To();
}

template <>
l2_packet make_packet(raw_packet in_packet)
{
}

template <>
raw_packet make_packet(l2_packet, in_packet)
{
}
}
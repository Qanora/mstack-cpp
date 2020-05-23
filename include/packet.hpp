#pragma once
#include "logger.hpp"

#include <memory>
#include <optional>
namespace mstack {

class packet {
private:
    std::vector<std::pair<int, std::unique_ptr<char[]>>> _data_stack;
    std::unique_ptr<char[]> _raw_data;
public:
    int _data_stack_len;
    int _head;
    int _len;

public:
    packet(char* buf, int len)
        : _raw_data(std::make_unique<char[]>(len))
        , _head(0)
        , _len(len)
    {
        std::copy(buf, buf + len, _raw_data.get());
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
        _raw_data = std::move(other._raw_data);
        std::swap(_head, other._head);
        std::swap(_len, other._len);
        std::swap(_data_stack_len, other._data_stack_len);
    }
    packet& operator=(packet&) = delete;
    packet& operator=(packet&& other)
    {
        _raw_data = std::move(other._raw_data);
        std::swap(_head, other._head);
        std::swap(_len, other._len);
        std::swap(_data_stack_len, other._data_stack_len);
    }

public:
    template<typename HeaderType>
    HeaderType* get_header(){
        char* pointer = _raw_data.get() + _head;
        return reinterpret_cast<HeaderType*>(pointer);
    }

    template<typename HeaderType>
    char* get_option(){
        char* pointer = _raw_data.get() + _head + sizeof(HeaderType);
        return pointer;
    }


    template<typename HeaderType, int options_len>
    void add_offset() {
        int offset = sizeof(HeaderType) + options_len;
        _head += offset;
    }

    void reflush_packet(int len)
    {
        _data_stack_len += _len;
        _data_stack.push_back({ _len, std::move(_raw_data) });

        _head = 0;
        _len = len;
        _raw_data = std::make_unique<char[]>(len);
    }

    void export_data(char* buf, int& len)
    {
        //!FIXME: _data_stack_len + now stack len; 
        if (_data_stack_len > len) {
            len = 0;
            return;
        }
        int index = 0;
        for (int i = 0; i < _data_stack.size(); i++) {
            for (int j = 0; j < _data_stack[i].first; j++) {
                buf[index++] = _data_stack[i].second[j];
            }
        }
        len = index;
    }
};

template <typename To, typename From>
std::optional<To> make_packet(From packet)
{
    DLOG(ERROR) << "[UNKNOW PACKET] " << From::tag << "->" << To::tag;
    return std::nullopt;
}


// template <>
// l2_packet make_packet(raw_packet in_packet)
// {
// }

// template <>
// raw_packet make_packet(l2_packet, in_packet)
// {
// }
}
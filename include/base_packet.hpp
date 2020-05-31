#pragma once
#include <memory>
#include <optional>

#include "logger.hpp"
namespace mstack {

class base_packet {
private:
        std::vector<std::pair<int, std::unique_ptr<uint8_t[]>>> _data_stack;
        std::unique_ptr<uint8_t[]>                              _raw_data;

public:
        int _data_stack_len;
        int _head;
        int _len;

public:
        base_packet(uint8_t* buf, int len)
            : _raw_data(std::make_unique<uint8_t[]>(len)), _head(0), _len(len), _data_stack_len(0) {
                std::copy(buf, buf + len, _raw_data.get());
        }

        base_packet(int len)
            : _raw_data(std::make_unique<uint8_t[]>(len)),
              _head(0),
              _len(len),
              _data_stack_len(0) {}

        ~base_packet()             = default;
        base_packet(base_packet&)  = delete;
        base_packet(base_packet&&) = delete;
        base_packet& operator=(base_packet&) = delete;
        base_packet& operator=(base_packet&&) = delete;

public:
        uint8_t* get_pointer() { return _raw_data.get() + _head; }

        int get_remaining_len() { return _len - _head; }

        int get_total_len() { return _data_stack_len; }

        void add_offset(int offset) { _head += offset; }

        void reflush_packet(int len) {
                _data_stack_len += _len;
                _data_stack.push_back({_len, std::move(_raw_data)});

                _head     = 0;
                _len      = len;
                _raw_data = std::make_unique<uint8_t[]>(len);
        }

        void export_payload(uint8_t* buf, int len) {
                int index = 0;
                for (int i = _head + len; i < _len; i++) {
                        buf[index++] = _raw_data[i];
                }
        }

        void export_data(uint8_t* buf, int& len) {
                if (_data_stack_len > len) {
                        len = 0;
                        return;
                }
                reflush_packet(1);
                int index = 0;
                for (int i = _data_stack.size() - 1; i >= 0; i--) {
                        for (int j = 0; j < _data_stack[i].first; j++) {
                                buf[index++] = _data_stack[i].second[j];
                        }
                }
                len = index;
        }
};

}  // namespace mstack
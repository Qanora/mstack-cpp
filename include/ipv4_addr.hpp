#pragma once
#include "utils.hpp"
#include <functional>
namespace mstack {
class ipv4_addr_t {
private:
    uint32_t _ipv4;

public:
    ipv4_addr_t() = default;
    ~ipv4_addr_t() = default;
    ipv4_addr_t(const ipv4_addr_t&) = default;
    ipv4_addr_t(ipv4_addr_t&&) = default;
    ipv4_addr_t& operator=(const ipv4_addr_t&) = default;
    ipv4_addr_t& operator=(ipv4_addr_t&&) = default;
    ipv4_addr_t(uint32_t ipv4)
        : _ipv4(ipv4)
    {
    }
    ipv4_addr_t(std::string ipv4)
    {
        ipv4.push_back('.');
        _ipv4 = 0;
        int start = 0;
        for (int i = 0; i < ipv4.size(); i++) {
            if (ipv4[i] == '.') {
                _ipv4 <<= 8;
                std::string tmp = ipv4.substr(start, i - start + 1);
                uint8_t tmp_int = std::stoi(tmp);
                _ipv4 |= tmp_int;
                start = i + 1;
            }
        }
    }

    uint32_t get_raw_ipv4() const
    {
        return _ipv4;
    }
    bool operator==(const ipv4_addr_t& other) const
    {
        return _ipv4 == other._ipv4;
    }

    size_t operator()(const ipv4_addr_t& p) const
    {
        return std::hash<uint32_t>()(p._ipv4);
    }

    static constexpr size_t size()
    {
        return 4;
    }

    void consume(uint8_t*& ptr)
    {
        _ipv4 = util::consume<uint32_t>(ptr);
    }

    void produce(uint8_t*& ptr)
    {
        util::produce<uint32_t>(ptr, _ipv4);
    }

    friend std::ostream& operator<<(std::ostream& out, ipv4_addr_t ipv4)
    {
        out << util::format("%u.%u.%u.%u",
            (ipv4._ipv4 >> 24) & 0xFF,
            (ipv4._ipv4 >> 16) & 0xFF,
            (ipv4._ipv4 >> 8) & 0xFF,
            (ipv4._ipv4 >> 0) & 0xFF);
        return out;
    }
};
};

namespace std {
template <>
struct hash<mstack::ipv4_addr_t> {
    size_t operator()(const mstack::ipv4_addr_t& ipv4_addr) const
    {
        return hash<uint32_t>{}(ipv4_addr.get_raw_ipv4());
    }
};
};
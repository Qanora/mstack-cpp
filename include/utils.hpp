#pragma once
#include <fmt/printf.h>

#include <cstdint>
#include <cstdio>
namespace mstack {
namespace utils {

template <typename... A>
inline std::string format(std::string format, A&&... a) {
        std::ostringstream os;
        ::fmt::fprintf(os, format, std::forward<A>(a)...);
        return os.str();
}

template <typename... A>
inline static int run_cmd(std::string fmt, A&&... a) {
        std::string cmd = format(fmt, std::forward<A>(a)...);
        DLOG(INFO) << "[EXEC COMMAND]: " << cmd;
        return system(cmd.c_str());
}

static int set_interface_route(std::string dev, std::string cidr) {
        return run_cmd("ip route add dev %s %s", dev, cidr);
}

static int set_interface_address(std::string dev, std::string cidr) {
        return run_cmd("ip address add dev %s local %s", dev, cidr);
}

static int set_interface_up(std::string dev) { return run_cmd("ip link set dev %s up", dev); }

uint32_t ntoh(uint32_t value) {
        return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
               (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;
}

uint16_t ntoh(uint16_t value) { return (value & 0x00FF) << 8 | (value & 0xFF00) >> 8; }

uint8_t ntoh(uint8_t value) { return value; }

template <typename T>
inline T consume(uint8_t*& ptr) {
        T ret = *(reinterpret_cast<T*>(ptr));
        ptr += sizeof(T);
        return ntoh(ret);
}

template <typename T>
inline void produce(uint8_t*& p, T t) {
        T* ptr_ = reinterpret_cast<T*>(p);
        *ptr_   = ntoh(t);
        p += sizeof(T);
}

uint32_t sum_every_16bits(uint8_t* addr, int count) {
        uint32_t sum = 0;

        uint16_t* ptr = reinterpret_cast<uint16_t*>(addr);

        while (count > 1) {
                /*  This is the inner loop */
                sum += *ptr++;
                count -= 2;
        }

        /*  Add left-over byte, if any */
        if (count > 0) sum += *(uint8_t*)ptr;

        return sum;
}

uint16_t checksum(uint8_t* addr, int count, int start_sum) {
        uint32_t sum = start_sum;

        sum += sum_every_16bits(addr, count);

        /*  Fold 32-bit sum to 16 bits */
        while (sum >> 16)
                sum = (sum & 0xffff) + (sum >> 16);

        uint16_t ret = ~sum;
        return ntoh(ret);
}
};  // namespace utils
};  // namespace mstack
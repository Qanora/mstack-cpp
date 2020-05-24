#pragma once

#include <cstdarg>
#include <iostream>

#include <fmt/printf.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
namespace mstack {
namespace util {

    static int init_logger(int argc, char* argv[])
    {
        FLAGS_logtostderr = true;
        FLAGS_minloglevel = 0;
        FLAGS_colorlogtostderr = true;

        gflags::ParseCommandLineFlags(&argc, &argv, true);
        google::InitGoogleLogging(argv[0]);
    }

    template <typename... A>
    inline std::string format(std::string format, A&&... a)
    {
        std::ostringstream os;
        ::fmt::fprintf(os, format, std::forward<A>(a)...);
        return os.str();
    }

    template <typename... A>
    inline static int run_cmd(std::string fmt, A&&... a)
    {
        std::string cmd = format(fmt, std::forward<A>(a)...);
        DLOG(INFO) << "[EXEC COMMAND]: " << cmd;
        return system(cmd.c_str());
    }

    static int set_interface_route(std::string dev, std::string cidr)
    {
        return run_cmd("ip route add dev %s %s", dev, cidr);
    }

    static int set_interface_address(std::string dev, std::string cidr)
    {
        return run_cmd("ip address add dev %s local %s", dev, cidr);
    }

    static int set_interface_up(std::string dev)
    {
        return run_cmd("ip link set dev %s up", dev);
    }


    uint32_t ntoh(uint32_t value){
        return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | 
                (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24; 
    }

    uint16_t ntoh(uint16_t value){
        return (value & 0x00FF) << 8 | (value & 0xFF00) >> 8;
    }

    uint8_t ntoh(uint8_t value){
        return value;
    }

    template <typename T>
    inline T consume(uint8_t*& ptr) {
        T ret = *(reinterpret_cast<T*>(ptr));
        ptr += sizeof(T);
        return ntoh(ret);
    }

    template <typename T>
    inline void produce(uint8_t*& p, T& t) {
        T* ptr_ = reinterpret_cast<T*>(t);
        *ptr_ = ntoh(t);
        p += sizeof(T);
    }

    std::string print_mm(uint8_t* ptr, int len){
        std::ostringstream os;
        for(int i = 0; i < len; i++){
            os << format("%02X", *(ptr + i));
        }
        return os.str();
    }
}; // namespace util
};
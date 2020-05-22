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
        LOG(INFO) << "[EXEC COMMAND]: " << cmd;
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
}; // namespace util
};
#pragma once

#include <iostream>

#include <cstdarg>

#include <gflags/gflags.h>
#include <glog/logging.h>

namespace util {

static int init_logger(int argc, char* argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
}

inline static int run_cmd(const char* cmd, ...)
{
    static constexpr int BUFLEN = 100;
    va_list ap;

    char buf[BUFLEN];

    va_start(ap, cmd);
    vsnprintf(buf, BUFLEN, cmd, ap);
    va_end(ap);

    LOG(INFO) << "[EXEC COMMAND]: " << buf;
    return system(buf);
}

static int set_interface_route(std::string dev, std::string cidr)
{
    return run_cmd("ip route add dev %s %s", dev.c_str(), cidr.c_str());
}

static int set_interface_address(std::string dev, std::string cidr)
{
    return run_cmd("ip address add dev %s local %s", dev.c_str(), cidr.c_str());
}

static int set_interface_up(std::string dev)
{
    return run_cmd("ip link set dev %s up", dev.c_str());
}
}; // namespace util
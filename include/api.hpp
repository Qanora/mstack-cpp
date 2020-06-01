#pragma once
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "arp.hpp"
#include "ethernet.hpp"
#include "icmp.hpp"
#include "ipv4.hpp"
#include "socket_manager.hpp"
#include "tcb_manager.hpp"
#include "tcp.hpp"
#include "tuntap.hpp"

namespace mstack {
int init_logger(int argc, char* argv[]) {
        FLAGS_logtostderr      = true;
        FLAGS_minloglevel      = 0;
        FLAGS_colorlogtostderr = true;

        gflags::ParseCommandLineFlags(&argc, &argv, true);
        google::InitGoogleLogging(argv[0]);
}

void init_stack(int argc, char* argv[]) {
        init_logger(argc, argv);
        auto& tuntap_dev = tuntap<1500>::instance();
        tuntap_dev.set_ipv4_addr(std::string("192.168.1.1"));

        auto& ethernetv2 = ethernetv2::instance();
        tuntap_dev.register_upper_protocol(ethernetv2);

        auto& arpv4 = arp::instance();
        ethernetv2.register_upper_protocol(arpv4);

        arpv4.register_dev(tuntap_dev);

        auto& ipv4 = ipv4::instance();
        ethernetv2.register_upper_protocol(ipv4);

        auto& icmp = icmp::instance();
        ipv4.register_upper_protocol(icmp);

        auto& tcp = tcp::instance();
        ipv4.register_upper_protocol(tcp);

        auto& tcb_manager = tcb_manager::instance();
        tcp.register_upper_protocol(tcb_manager);

        tuntap_dev.run();
};

int socket(int proto, ipv4_addr_t ipv4_addr, port_addr_t port_addr) {
        auto& socket_manager = socket_manager::instance();
        return socket_manager.register_socket(proto, ipv4_addr, port_addr);
}
int listen(int fd) {
        auto& socket_manager = socket_manager::instance();
        return socket_manager.listen(fd);
}
int accept(int fd) {
        auto& socket_manager = socket_manager::instance();
        return socket_manager.accept(fd);
}
int read(int fd, char* buf, int& len) {
        auto& socket_manager = socket_manager::instance();
        return socket_manager.read(fd, buf, len);
}

int write(int fd, char* buf, int& len) {
        auto& socket_manager = socket_manager::instance();
        return socket_manager.write(fd, buf, len);
}

}  // namespace mstack
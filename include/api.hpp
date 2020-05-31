#pragma once
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "arp.hpp"
#include "ethernet.hpp"
#include "icmp.hpp"
#include "ipv4.hpp"
#include "tcb_manager.hpp"
#include "tcp.hpp"
#include "tuntap.hpp"
namespace mstack {
static int init_logger(int argc, char* argv[]) {
        FLAGS_logtostderr      = true;
        FLAGS_minloglevel      = 0;
        FLAGS_colorlogtostderr = true;

        gflags::ParseCommandLineFlags(&argc, &argv, true);
        google::InitGoogleLogging(argv[0]);
}
void listen(std::string ipv4, int port) {
        auto&       tcb_manager = tcb_manager::instance();
        ipv4_port_t ipv4_port   = {.ipv4_addr = ipv4_addr_t(ipv4), .port_addr = port};
        tcb_manager.listen_port(ipv4_port);
};
void init_stack() {
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

        listen(std::string("192.168.1.1"), 30000);
        tuntap_dev.run();
};

}  // namespace mstack
#include <iostream>

#include "tuntap.hpp"
#include "utils.hpp"
#include "l2_layer.hpp"
#include "l3_layer.hpp"
//#include "ipv4.hpp"
#include "arp.hpp"
int main(int argc, char* argv[])
{

    mstack::util::init_logger(argc, argv);

    auto &l2_layer = mstack::l2_layer::instance();

    using tuntap = mstack::tuntap<1500>;
    auto &tuntap_dev = tuntap::instance();
    tuntap_dev.set_ipv4_addr(mstack::ipv4_addr_t("192.168.1.1"));
    
    l2_layer.register_dev(tuntap_dev);

    auto &arp = mstack::arp::instance();
    l2_layer.register_protocol(arp);

    auto &l3_layer = mstack::l3_layer::instance();
    l2_layer.register_upper_layer(l3_layer);

    auto &icmp = mstack::icmp::instance();
    l3_layer.register_protocol(icmp);  
      
    tuntap_dev.run();

    // auto &l2_layer = mstack::l2_layer::instance();
    // l2_layer.register_dev(tuntap_dev);

    // std::string ret = util::format("%d", 1);
    // std::cout << ret << std::endl;
    // mstack::mac_addr mac("0e:85:90:53:66:fe");
    // std::cout << mac << std::endl;
    sleep(100);
}
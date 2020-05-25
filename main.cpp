#include <iostream>

#include "tuntap.hpp"
#include "utils.hpp"
#include "l2_layer.hpp"
#include "ipv4.hpp"
#include "arp.hpp"
int main(int argc, char* argv[])
{

    mstack::util::init_logger(argc, argv);

    using tuntap = mstack::tuntap<1500>;

    auto &tuntap_dev = tuntap::instance();
    
    tuntap_dev.set_ipv4_addr(mstack::ipv4_addr_t("192.168.1.1"));
    auto &l2_layer = mstack::l2_layer::instance();

    auto &arp = mstack::arp::instance();
    

    l2_layer.register_protocol(arp);
    l2_layer.register_dev(tuntap_dev);

    tuntap_dev.run();

    // auto &l2_layer = mstack::l2_layer::instance();
    // l2_layer.register_dev(tuntap_dev);

    // std::string ret = util::format("%d", 1);
    // std::cout << ret << std::endl;
    // mstack::mac_addr mac("0e:85:90:53:66:fe");
    // std::cout << mac << std::endl;
    sleep(100);
}
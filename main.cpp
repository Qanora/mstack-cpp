#include <iostream>

// #include "tuntap.hpp"
#include "utils.hpp"
#include "ethernet.hpp"
#include "layer.hpp"

int main(int argc, char* argv[])
{

    mstack::util::init_logger(argc, argv);
    using tuntap = mstack::device::tuntap;

    auto &tuntap_dev = tuntap::instance();
    
    l2

    // std::string ret = util::format("%d", 1);
    // std::cout << ret << std::endl;

    // mstack::mac_addr mac("0e:85:90:53:66:fe");
    // std::cout << mac << std::endl;          
    sleep(100);
}
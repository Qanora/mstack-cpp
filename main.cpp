#include <iostream>

#include "tuntap.hpp"
#include "utils.hpp"

int main(int argc, char* argv[])
{

    util::init_logger(argc, argv);
    using tuntap = mstack::device::tuntap;
    std::cout << tuntap::instance().is_available() << std::endl;
    sleep(100);
}
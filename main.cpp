#include "api.hpp"

int main(int argc, char* argv[])
{
    auto stack = std::async(std::launch::async, mstack::init_stack, argc, argv);
    int fd = mstack::socket();
    mstack::bind(fd, mstack::ipv4_addr_t("192.168.1.1"), 30000);
    mstack::listen(fd);
}
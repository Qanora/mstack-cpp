#include <future>
#include <iostream>

#include "api.hpp"
int main(int argc, char* argv[]) {
        auto stack = std::async(std::launch::async, mstack::init_stack, argc, argv);
        int  fd    = mstack::socket(0x06, mstack::ipv4_addr_t("192.168.1.1"), 30000);
        mstack::listen(fd);
        char buf[2000];
        int  size = 2000;
        int  cfd  = mstack::accept(fd);
        while (true) {
                size = 2000;
                mstack::read(cfd, buf, size);
                std::cout << "read size: " << size << std::endl;
                for (int i = 0; i < size; i++) {
                        std::cout << buf[i];
                }
                std::cout << std::endl;
        }
}
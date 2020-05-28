#include <iostream>
#include "api.hpp"

int main(int argc, char* argv[])
{
    auto stack = std::async(std::launch::async, mstack::init_stack, argc, argv);
}
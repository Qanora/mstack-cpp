# mstack-cpp 🧐

mstack-cpp is a Linux user-space TCP / IP stack based on TUN / TAP devices implemented in C++20.

This project aims to implement a **SIMPLE** and **READABLE** TCP / IP user-space protocol stack. Although it is not sound and cannot be used on a large scale, it is an excellent opportunity to explore the composition of the TCP / IP protocol stack. At the same time, it can help us to understand the various trade-offs on the architecture of the protocol stack.

Unlike C-style procedural programming, C++ has features such as object-oriented programming and meta-programming. This project uses c++ skills as much as possible to show a more explicit and more modern protocol stack.

## Prerequisites
  + You MUST have ```/dev/net/tun``` device based on linux
  + CMake
  + fmt
  + glog

## Running
```
mkdir build && cd build && cmake .. && make && sudo ./mstack
```
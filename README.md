# mstack-cpp 🧐

[![Build Status](https://travis-ci.org/Qanora/mstack-cpp.svg?branch=master)](https://travis-ci.org/Qanora/mstack-cpp)
[![GitHub license](https://img.shields.io/github/license/sultan99/react-on-lambda.svg)](https://github.com/Qanora/mstack-cpp/blob/master/LICENSE)

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

## Current Feature
  + Ethernet II frame
  + ARP reply, simple caching
  + ICMP reply
  + IPv4 packet handling, checksum
  + TCPv4 Handshake
  + TCP RFC793 "Segment Arrives"

## Doing Feature
  + ARP request
  + TCP RFC6298 Retransmission calculation
  + TCP RFC793 User Timeout
  + TCP data transmission
  + TCP Window Management
  + TCP Congestion Control
  + Server socket API calls (bind, accept...)
## Upcoming Feature
  + IP Fragmentation
  + IP/ICMP Diagnostics
  + TCP Selective Acknowledgments (SACK)
  + TCP Silly Window Syndrome Avoidance
  + TCP Zero-Window Probes
  + ...

## References
  + [level-ip](https://github.com/saminiir/level-ip)
  + [seastar](https://github.com/scylladb/seastar)
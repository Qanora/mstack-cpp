#pragma once
namespace mstack {
static constexpr int TUNTAP_DEV = 0x01;

constexpr static int TCP_CLOSED       = 0x10;
constexpr static int TCP_LISTEN       = 0x11;
constexpr static int TCP_SYN_SENT     = 0x12;
constexpr static int TCP_SYN_RECEIVED = 0x13;
constexpr static int TCP_ESTABLISHED  = 0x14;
constexpr static int TCP_FIN_WAIT_1   = 0x15;
constexpr static int TCP_FIN_WAIT_2   = 0x16;
constexpr static int TCP_CLOSE_WAIT   = 0x17;
constexpr static int TCP_CLOSING      = 0x18;
constexpr static int TCP_LAST_ACK     = 0x19;
constexpr static int TCP_TIME_WAIT    = 0x20;
};  // namespace mstack
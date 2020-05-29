#pragma once
namespace mstack {
constexpr static int TUNTAP_DEV = 0x01;
constexpr static int L2_PACKET = 0x02;
constexpr static int L3_PACKET = 0x03;
constexpr static int L4_PACKET = 0x04;
constexpr static int IPV4_PROTO = 0x0800;

constexpr static int TCP_PROTO = 0x06;
constexpr static int TCP_URG = (1 << 5);
constexpr static int TCP_ACK = (1 << 4);
constexpr static int TCP_PSH = (1 << 3);
constexpr static int TCP_RST = (1 << 2);
constexpr static int TCP_SYN = (1 << 1);
constexpr static int TCP_FIN = (1 << 0);

constexpr static int TCP_CLOSED = 0x10;
constexpr static int TCP_LISTEN = 0x11;
constexpr static int TCP_SYN_SENT = 0x12;
constexpr static int TCP_SYN_RECEIVED = 0x13;
constexpr static int TCP_ESTABLISHED = 0x14;
constexpr static int TCP_FIN_WAIT_1 = 0x15;
constexpr static int TCP_FIN_WAIT_2 = 0x16;
constexpr static int TCP_CLOSE_WAIT = 0x17;
constexpr static int TCP_CLOSING = 0x18;
constexpr static int TCP_LAST_ACK = 0x19;
constexpr static int TCP_TIME_WAIT = 0x20;

constexpr static int SOCKET_UNCONNECTED = 0x21;
constexpr static int SOCKET_CONNECTING = 0x22;
constexpr static int SOCKET_CONNECTED = 0x23;

std::string state_to_string(int state) {
  switch (state) {
    case TCP_CLOSED:
      return "TCP_CLOSED";
    case TCP_LISTEN:
      return "TCP_LISTEN";
    case TCP_SYN_SENT:
      return "TCP_SYN_SENT";
    case TCP_SYN_RECEIVED:
      return "TCP_SYN_RECEIVED";
    case TCP_ESTABLISHED:
      return "TCP_ESTABLISHED";
    case TCP_FIN_WAIT_1:
      return "TCP_FIN_WAIT_1";
    case TCP_FIN_WAIT_2:
      return "TCP_FIN_WAIT_2";
    case TCP_CLOSE_WAIT:
      return "TCP_CLOSE_WAIT";
    case TCP_CLOSING:
      return "TCP_CLOSING";
    case TCP_LAST_ACK:
      return "TCP_LAST_ACK";
    case TCP_TIME_WAIT:
      return "TCP_TIME_WAIT";
  }
}
};  // namespace mstack
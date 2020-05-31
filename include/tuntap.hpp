#pragma once

#include <linux/if_tun.h>
#include <net/if.h>
#include <poll.h>
#include <sys/ioctl.h>

#include <functional>
#include <optional>

#include "file_desc.hpp"
#include "ipv4.hpp"
#include "ipv4_addr.hpp"
#include "mac_addr.hpp"
#include "packets.hpp"
#include "utils.hpp"

namespace mstack {

template <int mtu>
class tuntap {
public:
        constexpr static int MTU = mtu;
        constexpr static int TAG = TUNTAP_DEV;

private:
        file_desc                  _fd;
        std::optional<mac_addr_t>  _mac_addr;
        std::optional<ipv4_addr_t> _ipv4_addr;
        std::string                _dev_name = "tap0";

        bool    _available = false;
        uint8_t _buf[MTU];

        using packet_provider_type = std::function<std::optional<raw_packet>(void)>;
        using packet_receiver_type = std::function<void(raw_packet)>;

        std::optional<packet_provider_type> _provider_func;
        std::optional<packet_receiver_type> _receiver_func;

private:
        ~tuntap() = default;

        tuntap() { init(); }

public:
        tuntap(const tuntap&) = delete;
        tuntap(tuntap&&)      = delete;
        tuntap& operator=(const tuntap&) = delete;
        tuntap& operator=(tuntap&& x) = delete;

        static tuntap& instance() {
                static tuntap instance;
                return instance;
        }

        operator bool() { return _available; }

private:
        void set_mac_addr() {
                struct ifreq ifr;

                strcpy(ifr.ifr_name, _dev_name.c_str());

                int err = _fd.ioctl(SIOCGIFHWADDR, ifr);

                if (err < 0) {
                        LOG(FATAL) << "[HW FAIL]";
                }
                std::array<uint8_t, 6> hw_addr;
                for (int i = 0; i < 6; ++i) {
                        hw_addr[i] = ifr.ifr_addr.sa_data[i];
                }
                _mac_addr = mac_addr_t(hw_addr);
        }

        void init() {
                auto fd = std::move(
                        file_desc::open("/dev/net/tun", file_desc::RDWR | file_desc::NONBLOCK));
                if (!fd) {
                        LOG(FATAL) << "[INIT FAIL] ";
                        return;
                }

                // ? something error
                _fd = std::move(fd.value());

                DLOG(INFO) << "[DEV FD] " << _fd.get_fd();

                struct ifreq ifr;

                ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
                strcpy(ifr.ifr_ifrn.ifrn_name, _dev_name.c_str());

                int err = _fd.ioctl(TUNSETIFF, ifr);

                if (err < 0) {
                        LOG(FATAL) << "[INIT FAIL] ";
                        return;
                }

                if (utils::set_interface_up(_dev_name) != 0) {
                        LOG(FATAL) << "[SET UP] ";
                        return;
                }

                set_mac_addr();

                DLOG(INFO) << "[INIT MAC] " << _mac_addr.value();

                utils::set_interface_route(_dev_name, "192.168.1.0/24");
                _available = true;
        }

        raw_packet encode_raw_packet(uint8_t* buf, int len) {
                std::unique_ptr<base_packet> out_packet = std::make_unique<base_packet>(buf, len);
                raw_packet                   r_packet   = {.buffer = std::move(out_packet)};
                return std::move(r_packet);
        }

        void decode_raw_packet(raw_packet& r_packet, uint8_t* buf, int& len) {
                r_packet.buffer->export_data(buf, len);
        }

public:
        std::optional<mac_addr_t> get_mac_addr() { return _mac_addr; }

        std::optional<ipv4_addr_t> get_ipv4_addr() { return _ipv4_addr; }

        void set_ipv4_addr(ipv4_addr_t ipv4_addr) { _ipv4_addr = ipv4_addr; }

        template <typename Protocol>
        void register_upper_protocol(Protocol& protocol) {
                _provider_func = [&protocol]() { return protocol.gather_packet(); };
                _receiver_func = [&protocol](raw_packet r_packet) {
                        protocol.receive(std::move(r_packet));
                };
        }

        void run() {
                if (!_fd) {
                        LOG(FATAL) << "[FILE DESC FAIL]";
                        return;
                }

                int base_fd = _fd.get_fd();

                if (base_fd < 0) {
                        LOG(FATAL) << "[BASE FILE DESC FAIL]";
                        return;
                }

                struct pollfd pollevent[1];

                pollevent[0].fd = base_fd;

                pollevent[0].events = POLLIN | POLLOUT;

                DLOG(INFO) << "[TUNTAP LOOP]";
                while (_available) {
                        int ret = poll(pollevent, 1, -1);
                        if (ret == -1) {
                                LOG(FATAL) << "[POLL FAIL]";
                                return;
                        }

                        if (pollevent[0].revents & POLLOUT) {
                                if (_provider_func) {
                                        std::optional<raw_packet> r_packet =
                                                _provider_func.value()();

                                        if (r_packet) {
                                                int len = MTU;

                                                decode_raw_packet(r_packet.value(),
                                                                  reinterpret_cast<uint8_t*>(_buf),
                                                                  len);

                                                DLOG(INFO) << "[TUNTAP WRITE] " << len;
                                                write(pollevent[0].fd, _buf, len);
                                        }
                                } else {
                                        LOG(FATAL) << "[NO PROVIDER FUNC]";
                                }
                        }
                        if (pollevent[0].revents & POLLIN) {
                                if (_receiver_func) {
                                        int len = MTU;

                                        int n = read(pollevent[0].fd, _buf, len);

                                        DLOG(INFO) << "[TUNTAP RECEIVE] " << n;

                                        raw_packet r_packet = encode_raw_packet(
                                                reinterpret_cast<uint8_t*>(_buf), n);

                                        _receiver_func.value()(std::move(r_packet));
                                } else {
                                        LOG(FATAL) << "[NO RECEIVER FUNC]";
                                }
                        }
                }
        }
};
};  // namespace mstack
#pragma once

#include "defination.hpp"
#include "ethernet.hpp"
#include "file_desc.hpp"
#include "ipv4.hpp"
#include "ipv4_addr.hpp"
#include "mac_addr.hpp"
#include "packets.hpp"
#include "utils.hpp"
#include <functional>
#include <linux/if_tun.h>
#include <net/if.h>
#include <optional>
#include <poll.h>
#include <sys/ioctl.h>

namespace mstack {

template <int mtu>
class tuntap {
public:
    constexpr static int MTU = mtu;
    constexpr static int TAG = TUNTAP_DEV;

private:
    file_desc _fd;
    std::optional<mac_addr_t> _mac_addr;
    std::optional<ipv4_addr_t> _ipv4_addr;
    std::string _dev_name = "tap0";

    bool _available = false;
    uint8_t _buf[MTU];

    using packet_provider_type = std::function<std::optional<l2_packet>(void)>;
    using packet_receiver_type = std::function<void(l2_packet)>;

    std::optional<packet_provider_type> _provider_func;
    std::optional<packet_receiver_type> _receiver_func;

private:
    ~tuntap() = default;

    tuntap()
    {
        init();
    }

public:
    tuntap(const tuntap&) = delete;
    tuntap(tuntap&&) = delete;
    tuntap& operator=(const tuntap&) = delete;
    tuntap& operator=(tuntap&& x) = delete;

    static tuntap& instance()
    {
        static tuntap instance;
        return instance;
    }

    operator bool()
    {
        return _available;
    }

private:
    void set_mac_addr()
    {

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

    void init()
    {
        auto fd = std::move(file_desc::open("/dev/net/tun", file_desc::RDWR | file_desc::NONBLOCK));
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

        if (util::set_interface_up(_dev_name) != 0) {
            LOG(FATAL) << "[SET UP] ";
            return;
        }

        set_mac_addr();

        DLOG(INFO) << "[INIT MAC] " << _mac_addr.value();

        util::set_interface_route(_dev_name, "192.168.1.0/24");
        _available = true;
    }

    l2_packet encode_l2_packet(uint8_t* buf, int len)
    {
        ethernet_header_t ethernet_header;
        ethernet_header.consume(buf);
        std::unique_ptr<packet> out_packet = std::make_unique<packet>(buf, len);
        out_packet->add_offset(ethernet_header_t::size());
        l2_packet l2_packet(ethernet_header.src_mac_addr, ethernet_header.dst_mac_addr, ethernet_header.ethernet_type, std::move(out_packet));
        return std::move(l2_packet);
    }

    void decode_l2_packet(l2_packet& l2_packet, uint8_t* buf, int& len)
    {

        ethernet_header_t ethernet_header;
        ethernet_header.src_mac_addr = l2_packet._src_mac_addr.value();
        ethernet_header.dst_mac_addr = l2_packet._dst_mac_addr.value();
        ethernet_header.ethernet_type = l2_packet._proto;

        l2_packet._payload->reflush_packet(ethernet_header_t::size());
        ethernet_header.produce(l2_packet._payload->get_pointer());
        l2_packet._payload->export_data(buf, len);
    }

public:
    std::optional<mac_addr_t> get_mac_addr()
    {
        return _mac_addr;
    }

    std::optional<ipv4_addr_t> get_ipv4_addr()
    {
        return _ipv4_addr;
    }

    void set_ipv4_addr(ipv4_addr_t ipv4_addr)
    {
        _ipv4_addr = ipv4_addr;
    }
    void register_provider_func(packet_provider_type func)
    {
        _provider_func = func;
    }
    void register_receiver_func(packet_receiver_type func)
    {
        _receiver_func = func;
    }

    void run()
    {

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

                    std::optional<l2_packet> l2_packet = _provider_func.value()();

                    if (l2_packet) {

                        if (l2_packet->_src_mac_addr && l2_packet->_dst_mac_addr) {
                            int len = MTU;

                            decode_l2_packet(l2_packet.value(), reinterpret_cast<uint8_t*>(_buf), len);

                            DLOG(INFO) << "[TUNTAP WRITE] " << len;
                            write(pollevent[0].fd, _buf, len);
                        } else {
                            DLOG(ERROR) << "[L2 PACKET NOT FILL]";
                        }
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

                    l2_packet l2_packet = encode_l2_packet(reinterpret_cast<uint8_t*>(_buf), n);

                    _receiver_func.value()(std::move(l2_packet));
                } else {
                    LOG(FATAL) << "[NO RECEIVER FUNC]";
                }
            }
        }
    }
};
}; // namespace mstack
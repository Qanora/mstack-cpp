#pragma once

#include "defination.hpp"
// #include "device.hpp"
#include "ethernet.hpp"
#include "file_desc.hpp"
#include "layer.hpp"
#include "utils.hpp"

#include <functional>
#include <linux/if_tun.h>
#include <net/if.h>
#include <optional>
#include <poll.h>
#include <sys/ioctl.h>

namespace mstack {
namespace device {
    template <int mtu>
    class tuntap {
    private:
        std::optional<file_desc> _fd;
        std::optional<mac_addr> _mac_addr;
        std::string _dev_name = "tap0";
        const int _mtu = mtu;
        bool _available = false;
        char _buf[mtu];

        using packet_provider_type = std::function<std::optional<raw_packet>(void)>;
        using packet_receiver_type = std::function<void(raw_packet)>;

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

    private:
        void init()
        {
            _fd = std::move(file_desc::open("/dev/net/tun", file_desc::RDWR | file_desc::NONBLOCK));
            if (!_fd) {
                LOG(FATAL) << "[INIT FAIL] ";
                return;
            }
            struct ifreq ifr;
            ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
            strcpy(ifr.ifr_ifrn.ifrn_name, _dev_name.c_str());

            int err = _fd->ioctl(TUNSETIFF, ifr);

            if (err < 0) {
                LOG(FATAL) << "[INIT FAIL] ";
                return;
            }

            if (util::set_interface_up(_dev_name) != 0) {
                LOG(FATAL) << "[SET UP] ";
                return;
            }
            get_mac_addr();

            DLOG(INFO) << "[INIT MAC] " << _mac_addr.value();

            util::set_interface_route(_dev_name, "192.168.1.0/24");
            _available = true;
        }

    public:
        mac_addr get_mac_addr() {
            if(_mac_addr) return _mac_addr.value();
            
            struct ifreq ifr;
            strcpy(ifr.ifr_name, _dev_name.c_str());
            int err = _fd->ioctl(SIOCGIFHWADDR, ifr);
            if (err < 0) {
                LOG(FATAL) << "[MAC FAIL]";
            }
            uint8_t m[6];
            for (int i = 0; i < 6; ++i) {
                m[i] = ifr.ifr_addr.sa_data[i];
            }
            _mac_addr = mac_addr(m);
        }

        bool is_available() { return _available; }

        void register_provider_func(packet_provider_type func)
        {
            _provider_func = func;
        }
        void register_receiver_func(packet_receiver_type func)
        {
            _receiver_func = func;
        }

        raw_packet encode_raw_packet(char* buf, int len)
        {
            raw_packet raw_packet(TUNTAP_DEV, std::move(std::make_unique<packet>(buf, len)));
            return raw_packet;
        }

        void decode_raw_packet(raw_packet& raw_packet, char* buf, int& len)
        {
            if (raw_packet.proto != TUNTAP_DEV) {
                DLOG(ERROR) << "[NO TUNTAP DEV]";
                return;
            }
            raw_packet.payload->export_data(buf, len);
        }

        void run()
        {

            if (!_fd) {
                LOG(FATAL) << "[FILE DESC FAIL]";
                return;
            }

            int base_fd = _fd->get_fd();

            if (base_fd < 0) {
                LOG(FATAL) << "[BASE FILE DESC FAIL]";
                return;
            }

            struct pollfd pollevent[1];

            pollevent[0].fd = base_fd;

            pollevent[0].events = POLLIN | POLLOUT;

            DLOG(INFO) << "[TUNTAP LOOP]";
            while (is_available()) {

                int ret = poll(pollevent, 1, -1);
                if (ret == -1) {
                    LOG(FATAL) << "[POLL FAIL]";
                    return;
                }

                if (pollevent[0].revents & POLLOUT) {
                    if (_provider_func) {

                        std::optional<raw_packet> raw_packet = _provider_func.value()();

                        if (raw_packet) {

                            int len = _mtu;

                            decode_raw_packet(raw_packet.value(), _buf, len);

                            write(pollevent[0].fd, _buf, len);
                        }
                    }
                }

                if (pollevent[0].revents & POLLIN) {

                    if (_receiver_func) {

                        int len = _mtu;

                        int n = read(pollevent[0].fd, _buf, len);

                        DLOG(INFO) << "[TUNTAP RECEIVE] " << n;

                        raw_packet raw_packet = encode_raw_packet(_buf, n);

                        _receiver_func.value()(std::move(raw_packet));
                    }
                }
            }
        }
    };
}; // namespace device
} // namespace mstack
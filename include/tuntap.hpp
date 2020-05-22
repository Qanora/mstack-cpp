#pragma once

#include "defination.hpp"
#include "device.hpp"
#include "file_desc.hpp"
#include "utils.hpp"

#include <optional>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <poll.h>
#include <sys/ioctl.h>

namespace mstack {
namespace device {
    class tuntap {
    private:
        std::optional<file_desc> fd;
        std::string dev_name = "tap0";
        constexpr int mtu = 1500;
        bool available = false;
        char[mtu] buf;

        using packet_provider_type = std::function<optional<raw_packet>()>;
        using packet_receiver_type = std::function<(raw_packet)>> ;

        std::optional<packet_provider_type> provider_func;
        std::optional<packet_receiver_type> receiver_func;

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
            fd = file_desc::open("/dev/net/tun", file_desc::RDWR | file_desc::NONBLOCK);
            if (!fd) {
                LOG(ERROR) << "[INIT FAIL] ";
                return;
            }
            struct ifreq ifr;
            ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
            strcpy(ifr.ifr_ifrn.ifrn_name, dev_name.c_str());

            int err = fd->ioctl(TUNSETIFF, ifr);

            if (err < 0) {
                LOG(ERROR) << "[INIT FAIL] ";
                return;
            }

            if (util::set_interface_up(dev_name) != 0) {
                LOG(ERROR) << "[SET UP] ";
                return;
            }

            available = true;
        }

    public:
        bool is_available() { return available; }

        void register_provider_func(packet_provider_type func)
        {
            provider_func = func;
        }
        void register_receiver_func(packet_receiver_type func)
        {
            receiver_func = func;
        }

        l2_packet encode_raw_packet(char* buf, int len)
        {
            raw_packet raw_packet;
            raw_packet.proto = TUNTAP_DEV;
            raw_packet.payload = std::move(packet(buf, len));
        }

        void decode_raw_packet(raw_packet raw_packet, char* buf, int& len)
        {
            if (raw_packet.proto != TUNTAP_DEV) {
                LOG(ERROR) << "NO TUNTAP DEV";
                return;
            }
            raw_packet.payload.export(buf, len);
        }

        void run()
        {

            if (!fd) {
                LOG(ERROR) << "[FILE DESC FAIL]";
                return;
            }

            int base_fd = fd->get_fd();

            if (base_fd < 0) {
                LOG(ERROR) << "[BASE FILE DESC FAIL]" return;
            }

            strcut pollfd pollevent[1];

            pollevent[0].fd = base_fd;

            pollevent[0].events = POLLIN | POLLOUT;

            while (is_available()) {

                int ret = poll(&pollevent, 1, -1);
                if (ret == -1) {
                    LOG(ERROR) << "[POLL FAIL]" return;
                }

                if (pollevent[0].revents & POLLOUT) {
                    if (provider_func) {

                        raw_packet raw_packet = provider_func();

                        if (raw_packet) {

                            int len = mtu;

                            decode_raw_packet(raw_packet, buf, len);

                            write(pollevent[0].fd, buf, len);
                        }
                    }
                }

                if (pollevent[0].revents & POLLIN) {
                    if (receiver_func) {

                        int len = mtu;

                        n = read(pollevent[0].fd, buf, len);

                        raw_packet raw_packet = encode_raw_packet(buf, len);

                        receiver_func(raw_packet);
                    }
                }
            }
        }
    };
}; // namespace device
} // namespace mstack
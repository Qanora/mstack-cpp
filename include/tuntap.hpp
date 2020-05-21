#pragma once

#include "file_desc.hpp"
#include "utils.hpp"
#include <optional>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <sys/ioctl.h>

namespace mstack {
namespace device {
    class tuntap {
    private:
        std::optional<file_desc> fd;
        std::string dev_name = "tap0";
        bool available;
        ~tuntap() = default;

        tuntap()
            : available(false)
        {
            init();
        }

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
        static tuntap& instance()
        {
            static tuntap instance;
            return instance;
        }
        bool is_available() { return available; }
        tuntap(const tuntap&) = delete;
        tuntap(tuntap&& x) = delete;
        tuntap& operator=(const tuntap&) = delete;
        tuntap& operator=(tuntap&& x) = delete;
    };
}; // namespace device
} // namespace mstack
#pragma once
#include <unordered_map>

#include "ipv4_addr.hpp"
#include "logger.hpp"
#include "mac_addr.hpp"

namespace mstack {
struct arp_cache_t {
        std::unordered_map<int, mac_addr_t>         mac_addr_map;
        std::unordered_map<int, ipv4_addr_t>        ipv4_addr_map;
        std::unordered_map<ipv4_addr_t, mac_addr_t> arp_cache;
        void add_arp_cache(ipv4_addr_t ipv4_addr, mac_addr_t mac_addr) {
                DLOG(INFO) << "[ADD ARP CACHE] " << ipv4_addr << "-" << mac_addr;
                arp_cache[ipv4_addr] = mac_addr;
        }

        std::optional<mac_addr_t> query_arp_cache(ipv4_addr_t ipv4_addr) {
                if (arp_cache.find(ipv4_addr) == arp_cache.end()) {
                        return std::nullopt;
                }
                return arp_cache[ipv4_addr];
        }

        template <typename DEV>
        void register_dev(DEV& dev) {
                std::optional<mac_addr_t> mac_addr = dev.get_mac_addr();
                if (mac_addr) {
                        mac_addr_map[DEV::TAG] = mac_addr.value();
                        DLOG(INFO) << "[REGISTER DEV MAC] " << DEV::TAG << " "
                                   << mac_addr_map[DEV::TAG];
                }

                std::optional<ipv4_addr_t> ipv4_addr = dev.get_ipv4_addr();
                if (ipv4_addr) {
                        ipv4_addr_map[DEV::TAG] = ipv4_addr.value();
                        DLOG(INFO) << "[REGISTER DEV IPV4] " << DEV::TAG << " "
                                   << ipv4_addr_map[DEV::TAG];
                }

                if (mac_addr && ipv4_addr) {
                        add_arp_cache(ipv4_addr.value(), mac_addr.value());
                } else {
                        DLOG(FATAL) << "[REGISTER DEV FAIL] " << DEV::TAG;
                }
        }

        std::optional<mac_addr_t> query_dev_mac_addr(int tag) {
                if (mac_addr_map.find(tag) == mac_addr_map.end()) {
                        DLOG(ERROR) << "[UNKONWN DEV MAC]";
                        return std::nullopt;
                }
                return mac_addr_map[tag];
        }

        std::optional<ipv4_addr_t> query_dev_ipv4_addr(int tag) {
                if (ipv4_addr_map.find(tag) == ipv4_addr_map.end()) {
                        DLOG(ERROR) << "[UNKONWN DEV IPV4]";
                        return std::nullopt;
                }
                return ipv4_addr_map[tag];
        }
};
}  // namespace mstack
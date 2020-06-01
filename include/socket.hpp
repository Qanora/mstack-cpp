#pragma once
#include <optional>

#include "circle_buffer.hpp"
#include "defination.hpp"
#include "packets.hpp"
#include "tcb.hpp"
namespace mstack {
struct socket_t {
        int                                   fd;
        int                                   state = SOCKET_UNCONNECTED;
        int                                   proto;
        std::optional<ipv4_port_t>            local_info;
        std::optional<ipv4_port_t>            remote_info;
        std::optional<std::shared_ptr<tcb_t>> tcb;
};

struct listener_t {
        listener_t() : acceptors(std::make_shared<circle_buffer<std::shared_ptr<tcb_t>>>()) {}
        int                                                    fd;
        int                                                    state = SOCKET_UNCONNECTED;
        int                                                    proto;
        std::shared_ptr<circle_buffer<std::shared_ptr<tcb_t>>> acceptors;
        std::optional<ipv4_port_t>                             local_info;
};
}  // namespace mstack
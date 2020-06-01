#include <unordered_map>

#include "defination.hpp"
#include "socket.hpp"
#include "tcb_manager.hpp"
namespace mstack {
class socket_manager {
private:
        socket_manager()  = default;
        ~socket_manager() = default;
        std::unordered_map<uint16_t, std::shared_ptr<socket_t>>   sockets;
        std::unordered_map<uint16_t, std::shared_ptr<listener_t>> listeners;

public:
        socket_manager(const socket_manager&) = delete;
        socket_manager(socket_manager&&)      = delete;
        socket_manager& operator=(const socket_manager&) = delete;
        socket_manager& operator=(socket_manager&&) = delete;

        static socket_manager& instance() {
                static socket_manager instance;
                return instance;
        }

        int register_socket(int proto, ipv4_addr_t ipv4_addr, port_addr_t port_addr) {
                for (int i = 1; i < 65535; i++) {
                        if (sockets.find(i) == sockets.end()) {
                                ipv4_port_t local_info = {.ipv4_addr = ipv4_addr,
                                                          .port_addr = port_addr};

                                std::shared_ptr<socket_t> socket = std::make_shared<socket_t>();
                                socket->proto                    = proto;
                                socket->local_info               = local_info;
                                sockets[i]                       = socket;
                                return i;
                        }
                }
                return -1;
        }

        int listen(int fd) {
                if (sockets.find(fd) == sockets.end()) {
                        return -1;
                }
                std::shared_ptr<listener_t> listener = std::make_shared<listener_t>();
                listener->local_info                 = sockets[fd]->local_info;
                listener->proto                      = sockets[fd]->proto;
                listener->state                      = SOCKET_CONNECTING;
                listener->fd                         = fd;
                listeners[fd]                        = listener;
                auto& tcb_manager                    = tcb_manager::instance();
                tcb_manager.listen_port(listener->local_info.value(), listener);
        };

        int accept(int fd) {
                if (listeners.find(fd) == listeners.end()) {
                        return -1;
                }
                while (listeners[fd]->acceptors->empty()) {
                };
                for (int i = 1; i < 65535; i++) {
                        if (sockets.find(i) == sockets.end()) {
                                std::optional<std::shared_ptr<tcb_t>> tcb =
                                        listeners[fd]->acceptors->pop_front();
                                std::shared_ptr<socket_t> socket = std::make_shared<socket_t>();
                                socket->local_info               = tcb.value()->local_info;
                                socket->remote_info              = tcb.value()->remote_info;
                                socket->proto                    = listeners[fd]->proto;
                                socket->state                    = SOCKET_CONNECTED;
                                socket->tcb                      = tcb;
                                socket->fd                       = i;
                                sockets[i]                      = socket;
                                return i;
                        }
                }
                return -1;
        }

        int read(int fd, char* buf, int& len) {
                if (sockets.find(fd) == sockets.end()) {
                        len = 0;
                        return -1;
                }
                std::shared_ptr<socket_t> socket = sockets[fd];
                while (socket->tcb.value()->receive_queue.empty()) {
                };
                raw_packet r_packet =
                        std::move(socket->tcb.value()->receive_queue.pop_front().value());
                r_packet.buffer->export_data(reinterpret_cast<uint8_t*>(buf), len);
        }

        int write(int fd, char* buf, int& len) {
                if (sockets.find(fd) == sockets.end()) {
                        return -1;
                }
                std::shared_ptr<socket_t>    socket = sockets[fd];
                std::unique_ptr<base_packet> out_buffer =
                        std::make_unique<base_packet>(reinterpret_cast<uint8_t*>(buf), len);
                raw_packet r_packet = {.buffer = std::move(out_buffer)};
                socket->tcb.value()->send_queue.push_back(std::move(r_packet));
        }
};
};  // namespace mstack
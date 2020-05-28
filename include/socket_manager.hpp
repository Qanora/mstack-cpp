#include "unordered_map"
#include "tcb_manager.hpp"
#include "defination.hpp"
#include "tcb.hpp"
namespace mstack{

    struct socket_t{
        int fd;
        int state = SOCKET_UNCONNECTED;
        int proto;
        std::optional<half_connect_id_t> remote_info;
        std::optional<half_connect_id_t> local_info;
        std::optional<std::shared_ptr<full_tcb_t>> full_tcb;
        std::optional<std::shared_ptr<half_tcb_t>> half_tcb;
    };

    class socket_manager_t{
        tcb_manager_t &tcb_manager = tcb_manager_t::instance();
        std::array<socket_t, 200> sockets;
    public:
        static socket_manager_t& instance()
        {
            static socket_manager_t instance;
            return instance;
        }

        int register_socket(){
            for(int i = 0; i < 200; i++){
                if(sockets[i].state == SOCKET_UNCONNECTED){
                    return i;
                }
            }
            return -1;
        }

        void set_socket_proto(int fd, int proto) {
            if(fd < 0 || fd >= 200) return;
            sockets[fd].proto = proto;
        }

        void set_socket_remote_info(int fd, half_connect_id_t remote_info) {
            if(fd < 0 || fd >= 200) return;
            sockets[fd].remote_info = remote_info;
        }

        void set_socket_local_info(int fd, half_connect_id_t local_info) {
            if(fd < 0 || fd >= 200) return;
            sockets[fd].local_info = local_info;
        }

        void listen(int fd){
            if(fd < 0 || fd >= 200) return;
            if(sockets[fd].state != SOCKET_UNCONNECTED) return;
            std::optional<std::shared_ptr<half_tcb_t>> tcb = tcb_manager.register_half_tcb(sockets[fd].local_info.value());
            sockets[fd].half_tcb = tcb;
        }

        void connect(int fd){
            if(fd < 0 || fd >= 200) return;
            if(sockets[fd].state != SOCKET_UNCONNECTED) return;
            full_connect_id_t full_connect(sockets[fd].remote_info.value(), sockets[fd].local_info.value());
            std::optional<std::shared_ptr<full_tcb_t>> tcb = tcb_manager.register_full_tcb(full_connect);
            sockets[fd].full_tcb = tcb;
            while(tcb.value()->state != TCP_ESTABLISHED){}
        }
    };
}
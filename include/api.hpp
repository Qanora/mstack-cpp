#include "tuntap.hpp"
#include <future>

#include "arp.hpp"
#include "icmp.hpp"
#include "l2_layer.hpp"
#include "l3_layer.hpp"
#include "l4_layer.hpp"
#include "socket_manager.hpp"
#include "tcp.hpp"
#include "utils.hpp"
namespace mstack {

static int init_stack(int argc, char* argv[])
{

    mstack::util::init_logger(argc, argv);

    auto& l2_layer = mstack::l2_layer::instance();

    using tuntap = mstack::tuntap<1500>;
    auto& tuntap_dev = tuntap::instance();
    tuntap_dev.set_ipv4_addr(mstack::ipv4_addr_t("192.168.1.1"));

    l2_layer.register_dev(tuntap_dev);

    auto& arp = mstack::arp::instance();
    l2_layer.register_protocol(arp);

    auto& l3_layer = mstack::l3_layer::instance();
    l2_layer.register_upper_layer(l3_layer);

    auto& icmp = mstack::icmp::instance();
    l3_layer.register_protocol(icmp);

    auto& l4_layer = mstack::l4_layer::instance();
    l3_layer.register_upper_layer(l4_layer);

    auto& tcp = mstack::tcp::instance();
    l4_layer.register_protocol(tcp);

    tuntap_dev.run();
};

static int socket()
{
    socket_manager_t& socket_manager = socket_manager_t::instance();
    int fd = socket_manager.register_socket();
    return fd;
}

static void listen(int fd)
{
    socket_manager_t& socket_manager = socket_manager_t::instance();
    socket_manager.listen(fd);
}

static void accept(int fd)
{
}

static void bind(int fd, ipv4_addr_t ipv4_addr, port_addr_t port_addr)
{
    socket_manager_t& socket_manager = socket_manager_t::instance();
    half_connect_id_t local_info(ipv4_addr, port_addr);
    socket_manager.set_socket_local_info(fd, local_info);
}
};
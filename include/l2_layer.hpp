#include "layer.hpp"
#include "tuntap.hpp"
namespace mstack {

class l2_hook : public base_hook_funcs<l2_packet> {
    struct arpv4_request {
    };
    struct arpv4_cache {
    };
};

class l2_layer : public mstack::layer<raw_packet, l2_packet, l2_hook> {
public:
    template <typename DEV>
    void register_dev(DEV& dev)
    {
        dev.register_provider_func([&]() { return this->gather_packet(); });
        dev.register_receiver_func([&](raw_packet& raw_packet) { this->receive(raw_packet); });
    }
};
};
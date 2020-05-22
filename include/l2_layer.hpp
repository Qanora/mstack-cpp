#include "layer.hpp"
namespace mstack {

class l2_hook : public base_hook_funcs<l2_packet> {
    struct arpv4_request {
    };
    struct arpv4_cache {
    };
};

class l2_layer : public layer<raw_packet, l2_packet, l2_hook> {
};
};
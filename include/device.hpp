#include "l2_layer.hpp"
#include <functional>
#include <optional>
#include <vector>
namespace mstack {
namespace device {
    class device {
    private:
        using packet_provider_type = std::function<optional<l2_packet>()>;
        using packet_receiver_type = std::function<(raw_packet)>> ;
        std::optional<packet_provider_type> provider_func;
        std::optional<packet_receiver_type> receiver_func;
        void register_provider_func(packet_provider_type func)
        {
            provider_func = func;
        }
        void register_receiver_func(packet_receiver_type func)
        {
            receiver_func = func;
        }

    public:
        device() = delete;
        ~device() = delete;
        device(const device&) = delete;
        device(device&&) = delete;
        device& operator=(const device&) = delete;
        device& operator=(device&&) = delete;
        virutal void run() = 0;
    };
};
};
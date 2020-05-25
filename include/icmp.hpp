namespace mstack{

struct icmp_header_t
{
    uint8_t proto_type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;

    static constexpr size_t size() {
        return 1 + 1 + 2 + 2 + 2;
    }

    void consume(uint8_t* ptr){
        proto_type = util::consume<uint8_t>(ptr);
        code = util::consume<uint8_t>(ptr);
        checksum = util::consume<uint16_t>(ptr);
        id = util::consume<uint16_t>(ptr);
        seq = util::consume<uint16_t>(ptr);
    }

    void produce(uint8_t* ptr){
        util::produce<uint8_t>(ptr, proto_type);
        util::produce<uint8_t>(ptr, code);
        util::produce<uint16_t>(ptr, checksum);
        util::produce<uint16_t>(ptr, id);
        util::produce<uint16_t>(ptr, seq);
    }
};

std::ostream& operator<<(std::ostream& out, icmp_header_t& m)
{
    using u = uint32_t;
    out << "[ICMP PACKET] ";
    out << u(m.proto_type) << " ";
    out << u(m.code) << " ";
    out << u(m.id) << " ";
    out << u(m.seq);
    return out;
};

class icmp : public protocol_interface<l3_packet> {
    public:
        static constexpr int PROTO = 0x01;
        virtual int proto(){
            return PROTO;
        }

        virtual std::string tag(){
            return "ICMP";
        }

        static icmp& instance()
        {
            static icmp instance;
            return instance;
        }
        
        virtual void receive(l3_packet in_packet){
            DLOG(INFO) << "[RECEIVED ICMP] " << in_packet;
            icmp_header_t in_icmp_header;
            in_icmp_header.consume(in_packet._payload->get_pointer());
            DLOG(INFO) << in_icmp_header;
        }
};
}
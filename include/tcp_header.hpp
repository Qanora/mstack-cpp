#pragma once
#include "utils.hpp"
namespace mstack {
struct tcp_header_t {
        using port_addr_t = uint16_t;
        port_addr_t src_port;
        port_addr_t dst_port;
        uint32_t    seq_no;
        uint32_t    ack_no;

        uint16_t header_length : 4;
        uint16_t NOP : 6;
        uint16_t URG : 1;
        uint16_t ACK : 1;
        uint16_t PSH : 1;
        uint16_t RST : 1;
        uint16_t SYN : 1;
        uint16_t FIN : 1;

        uint16_t window_size;
        uint16_t checksum;
        uint16_t urgent_pointer;

        static constexpr size_t size() { return 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2; }

        tcp_header_t() {
                src_port       = 0;
                dst_port       = 0;
                seq_no         = 0;
                ack_no         = 0;
                header_length  = 0;
                NOP            = 0;
                URG            = 0;
                ACK            = 0;
                PSH            = 0;
                RST            = 0;
                SYN            = 0;
                FIN            = 0;
                window_size    = 0;
                checksum       = 0;
                urgent_pointer = 0;
        }
        static tcp_header_t consume(uint8_t* ptr) {
                tcp_header_t tcp_header;
                tcp_header.src_port          = utils::consume<port_addr_t>(ptr);
                tcp_header.dst_port          = utils::consume<port_addr_t>(ptr);
                tcp_header.seq_no            = utils::consume<uint32_t>(ptr);
                tcp_header.ack_no            = utils::consume<uint32_t>(ptr);
                uint16_t header_length_flags = utils::consume<uint16_t>(ptr);
                tcp_header.header_length     = header_length_flags >> 12;
                tcp_header.NOP               = (header_length_flags >> 6) & ((1 << 6) - 1);
                tcp_header.URG               = (header_length_flags >> 5) & 0x1;
                tcp_header.ACK               = (header_length_flags >> 4) & 0x1;
                tcp_header.PSH               = (header_length_flags >> 3) & 0x1;
                tcp_header.RST               = (header_length_flags >> 2) & 0x1;
                tcp_header.SYN               = (header_length_flags >> 1) & 0x1;
                tcp_header.FIN               = (header_length_flags >> 0) & 0x1;
                tcp_header.window_size       = utils::consume<uint16_t>(ptr);
                tcp_header.checksum          = utils::consume<uint16_t>(ptr);
                tcp_header.urgent_pointer    = utils::consume<uint16_t>(ptr);
                return tcp_header;
        }

        void produce(uint8_t* ptr) {
                utils::produce<port_addr_t>(ptr, src_port);
                utils::produce<port_addr_t>(ptr, dst_port);
                utils::produce<uint32_t>(ptr, seq_no);
                utils::produce<uint32_t>(ptr, ack_no);
                utils::produce<uint16_t>(ptr, header_length << 12 | NOP << 6 | URG << 5 | ACK << 4 |
                                                      PSH << 3 | RST << 2 | SYN << 1 | FIN);
                utils::produce<uint16_t>(ptr, window_size);
                utils::produce<uint16_t>(ptr, checksum);
                utils::produce<uint16_t>(ptr, urgent_pointer);
        }
        friend std::ostream& operator<<(std::ostream& out, tcp_header_t& m) {
                out << "[TCP PACKET] ";
                out << m.src_port;
                out << " -> " << m.dst_port;
                out << " SEQ_NO: " << m.seq_no;
                out << " ACK_NO: " << m.ack_no;
                out << " [ ";
                if (m.ACK == 1) out << "ACK ";
                if (m.SYN == 1) out << "SYN ";
                if (m.PSH == 1) out << "PSH ";
                if (m.RST == 1) out << "RST ";
                if (m.FIN == 1) out << "FIN ";
                out << " ]";

                return out;
        }
};
}  // namespace mstack
#pragma once
namespace mstack{
    class ipv4_addr{
    private:
        uint32_t _ipv4;
    public:
        ipv4_addr() = default;
        ~ipv4_addr() = default;
        ipv4_addr(const ipv4_addr&) = default;
        ipv4_addr(ipv4_addr&&) = default;
        ipv4_addr& operator=(const ipv4_addr&) = default;
        ipv4_addr& operator=(ipv4_addr&&) = default;
        ipv4_addr(uint32_t ipv4) : _ipv4(ipv4){}
        ipv4_addr(std::string ipv4) {
            ipv4.push_back('.');
            _ipv4 = 0;
            int start = 0;
            for(int i = 0; i < ipv4.size(); i++){
                if(ipv4[i] == '.') {
                    _ipv4 <<= 8;
                    std::string tmp = ipv4.substr(start, i-start+1);
                    uint8_t tmp_int = std::stoi(tmp);
                    _ipv4 |= tmp_int; 
                    start = i + 1; 
                }
            }
        }
        friend std::ostream& operator<<(std::ostream& out, ipv4_addr ipv4)
        {
            out << util::format("%u.%u.%u.%u", 
                                (ipv4._ipv4 >> 24) & 0xFF,
                                (ipv4._ipv4 >> 16) & 0xFF,
                                (ipv4._ipv4 >> 8) & 0xFF,
                                (ipv4._ipv4 >> 0) & 0xFF);
            return out;
        }
    };
}
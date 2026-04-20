#ifndef PCAP_PARSER_H
#define PCAP_PARSER_H

#include <stdio.h>
#include <vector>
#include <string>
#include <stdexcept>

#define WPCAP
#define HAVE_REMOTE
#include <pcap.h>

struct Packet {
    double timestamp;
    uint32_t size;
    std::string protocol;
};

class PcapParser {
public:
    std::vector<Packet> parse(const std::string& filename);
};

#endif
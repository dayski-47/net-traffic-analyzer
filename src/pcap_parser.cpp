#include "pcap_parser.h"

static std::vector<Packet> packets;

static void packetHandler(u_char *args, 
                          const struct pcap_pkthdr *header,
                          const u_char *packet){

    Packet p;
    p.timestamp = header->ts.tv_sec + header->ts.tv_usec / 1e6;
    p.size = header->len;
    p.protocol = "UNKNOWN";  // ip/tcp parsing stage

    packets.push_back(p);
}

std::vector<Packet> PcapParser::parse(const std::string& filename) {
    char errbuf[PCAP_ERRBUF_SIZE];

    pcap_t *handle = pcap_open_offline(filename.c_str(), errbuf);
    if (!handle) {
        throw std::runtime_error(errbuf);
    }

    packets.clear();


    // read file

    if (pcap_loop(handle, 0, packetHandler, nullptr) < 0){
        pcap_close(handle);
        throw std::runtime_error("Error reading PCAP file");
    }

    pcap_close(handle);

    return packets;
}
            
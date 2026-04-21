#ifndef WINDOWING_H
#define WINDOWING_H


#include <vector>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "pcap_parser.h"

struct Window {
    double startTime;
    int packetCount;
    int totalBytes;
    int tcpCount;
    int udpCount;
};

class Windowing{
public:
    std::vector<Window> createWindows(
        const std::vector<Packet>& packets,
        double windowSize
    );

};




#endif
#include "windowing.h"

std::vector<Window> Windowing::createWindows(const std::vector<Packet>& packets, double windowSize){
    std::vector<Window> windows;

    if (packets.empty() || windowSize <= 0.0){
        return windows;
    }

    double startTime = packets.front().timestamp;
    double endTime   = packets.back().timestamp;

    int numWindows = static_cast<int>(std::ceil(endTime - startTime) / windowSize) + 1;

    windows.resize(numWindows);

    for (int i = 0; i < numWindows; i++){
        windows[i].startTime = startTime + i * windowSize;
        windows[i].packetCount = 0;
        windows[i].totalBytes  = 0;
        windows[i].tcpCount    = 0;
        windows[i].udpCount    = 0;
    }


    for (const auto& packet : packets){
        int index = static_cast<int>((packet.timestamp - startTime) / windowSize);

        if (index < 0 || index >= numWindows) {
            continue;
        }

        windows[index].packetCount++;
        windows[index].totalBytes+= packet.size;

        if (packet.protocol == "TCP") {
            windows[index].tcpCount++;
        } else if (packet.protocol == "UDP"){
            windows[index].udpCount++;
        }
    }
    
    return windows;
}
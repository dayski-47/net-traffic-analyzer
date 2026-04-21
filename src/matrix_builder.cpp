#include "matrix_builder.h"



std::vector<std::vector<double>> MatrixBuilder::buildMatrix(const std::vector<Window>& windows){
    std::vector<std::vector<double>> A;
    A.reserve(windows.size());

    for (const auto& w : windows){
        std::vector<double> row = {
            static_cast<double>(w.packetCount),
            static_cast<double>(w.totalBytes),
            static_cast<double>(w.tcpCount),
            static_cast<double>(w.udpCount)
            };

        A.push_back(row);
    }
    return A;

}
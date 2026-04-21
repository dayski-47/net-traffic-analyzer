#include <iostream>
#include <vector>

#include "pcap_parser.h"
#include "windowing.h"
#include "matrix_builder.h"
#include "linear_algebra.h"
#include "cli_dashboard.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./analyzer <file.pcap>\n";
        return 1;
    }

    std::string filename = argv[1];

    // ---- 1. Parse PCAP ----
    PcapParser parser;
    std::vector<Packet> packets = parser.parse(filename);

    if (packets.empty()) {
        std::cerr << "No packets found.\n";
        return 1;
    }

    // ---- 2. Windowing ----
    Windowing windowing;
    std::vector<Window> windows =
        windowing.createWindows(packets, 1.0);

    if (windows.empty()) {
        std::cerr << "No windows generated.\n";
        return 1;
    }

    // ---- 3. Build matrix ----
    MatrixBuilder builder;
    std::vector<std::vector<double>> A =
        builder.buildMatrix(windows);

    // ---- 4. Define weights (x) ----

    std::vector<double> x = {0.5, 0.3, 0.1, 0.1};

    // ---- 5. Linear algebra ----
    LinearAlgebra la;
    std::vector<double> y = la.multiply(A, x);

    // ---- 6. CLI visualization ----
    CLIDashboard cli;

    // Frame render 
    cli.render(A, y);

    return 0;
}
// #include <iostream>
// #include <vector>

// #include "pcap_parser.h"
// #include "windowing.h"
// #include "matrix_builder.h"
// #include "linear_algebra.h"
// #include "cli_dashboard.h"

// int main(int argc, char* argv[]) {
//     if (argc < 2) {
//         std::cerr << "Usage: ./analyzer <file.pcap>\n";
//         return 1;
//     }

//     std::string filename = argv[1];

//     // ---- 1. Parse PCAP ----
//     PcapParser parser;
//     std::vector<Packet> packets = parser.parse(filename);

//     if (packets.empty()) {
//         std::cerr << "No packets found.\n";
//         return 1;
//     }

//     // ---- 2. Windowing ----
//     Windowing windowing;
//     std::vector<Window> windows =
//         windowing.createWindows(packets, 1.0);

//     if (windows.empty()) {
//         std::cerr << "No windows generated.\n";
//         return 1;
//     }

//     // ---- 3. Build matrix ----
//     MatrixBuilder builder;
//     std::vector<std::vector<double>> A =
//         builder.buildMatrix(windows);

//     // ---- 4. Define weights (x) ----

//     std::vector<double> x = {0.5, 0.3, 0.1, 0.1};

//     // ---- 5. Linear algebra ----
//     LinearAlgebra la;
//     std::vector<double> y = la.multiply(A, x);

//     // ---- 6. CLI visualization ----
//     CLIDashboard cli;

//     // Frame render 
//     cli.render(A, y);

//     return 0;
// }

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <csignal>
#include <atomic>
#include "pcap_parser.h"
#include "windowing.h"
#include "matrix_builder.h"
#include "linear_algebra.h"
#include "cli_dashboard.h"
 
static std::atomic<bool> running(true);
 
void handle_signal(int) {
    running = false;
}
 
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./analyzer <file.pcap> [interval_ms]\n";
        std::cerr << "  interval_ms: refresh rate in milliseconds (default 500)\n";
        return 1;
    }
 
    std::string filename = argv[1];
    int interval_ms = (argc >= 3) ? std::stoi(argv[2]) : 500;
 
    std::signal(SIGINT, handle_signal);
 
    PcapParser parser;
    std::vector<Packet> packets = parser.parse(filename);
    if (packets.empty()) {
        std::cerr << "No packets found in: " << filename << "\n";
        return 1;
    }
 
    Windowing windowing;
    std::vector<Window> windows = windowing.createWindows(packets, 1.0);
    if (windows.empty()) {
        std::cerr << "No windows generated.\n";
        return 1;
    }
 
    MatrixBuilder builder;
    std::vector<std::vector<double>> A_full = builder.buildMatrix(windows);
 
    std::vector<double> x = {0.5, 0.3, 0.1, 0.1};
 
    LinearAlgebra la;
    std::vector<double> y_full = la.multiply(A_full, x);
 
    CLIDashboard cli;
    size_t frame = 1;
 
    while (running) {
        std::vector<std::vector<double>> A(A_full.begin(), A_full.begin() + frame);
        std::vector<double> y(y_full.begin(), y_full.begin() + frame);
 
        cli.render(A, y);
 
        frame++;
        if (frame > A_full.size()) frame = 1;
 
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
 
    std::cout << "\033[?25h\033[0m\nAnalyzer stopped.\n";
    return 0;
}

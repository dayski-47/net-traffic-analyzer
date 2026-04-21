#include "cli_dashboard.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

static double mean(const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}

void CLIDashboard::render(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& y
) {
    if (A.empty() || y.empty()) return;

    size_t last = y.size() - 1;

    // ---- baseline ----
    double mu = mean(y);
    double current = y[last];
    double deviation = std::abs(current - mu);

    std::string trend = "STABLE";
    if (last > 0) {
        if (y[last] > y[last - 1]) trend = "UP";
        else if (y[last] < y[last - 1]) trend = "DOWN";
    }

    // ---- clear screen ----
    std::cout << "\033[2J\033[H";

    // ---- header ----
    std::cout << "=== NETWORK MATRIX SIGNAL ANALYZER ===\n\n";

    std::cout << "Current Score: " << current << "\n";
    std::cout << "Baseline Avg : " << mu << "\n";
    std::cout << "Deviation    : " << deviation << "\n";
    std::cout << "Trend        : " << trend << "\n\n";

    // ---- feature breakdown (last window row) ----
    const auto& row = A[last];

    std::cout << "Feature Breakdown (Window " << last << ")\n";

    std::cout << "Packets: ";
    for (int i = 0; i < (int)row[0] && i < 50; i++) std::cout << "#";
    std::cout << " (" << row[0] << ")\n";

    std::cout << "Bytes  : ";
    int bytesBars = static_cast<int>(row[1] / 100);
    for (int i = 0; i < std::min(bytesBars, 50); i++) std::cout << "#";
    std::cout << " (" << row[1] << ")\n";

    std::cout << "TCP    : ";
    for (int i = 0; i < (int)row[2]; i++) std::cout << "#";
    std::cout << " (" << row[2] << ")\n";

    std::cout << "UDP    : ";
    for (int i = 0; i < (int)row[3]; i++) std::cout << "#";
    std::cout << " (" << row[3] << ")\n\n";

    // ---- signal history ----
    std::cout << "Signal (y = Ax)\n";
    double maxVal = *std::max_element(y.begin(), y.end());
    if (maxVal == 0) maxVal = 1.0;

    for (size_t i = 0; i < y.size(); i++) {
        int bar = static_cast<int>((y[i] / maxVal) * 40);

        std::cout << "[" << std::setw(2) << i << "] ";

        for (int j = 0; j < bar; j++) std::cout << "#";

        std::cout << " " << y[i] << "\n";
    }

    // ---- math reference ----
    std::cout << "\nModel: y = Ax\n";
}
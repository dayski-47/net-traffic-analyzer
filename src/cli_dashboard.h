#ifndef CLI_DASHBOARD_H
#define CLI_DASHBOARD_H

#include <vector>

class CLIDashboard {
public:
    void render(
        const std::vector<std::vector<double>>& A,
        const std::vector<double>& y
    );
};

#endif
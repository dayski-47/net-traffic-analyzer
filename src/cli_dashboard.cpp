/* #include "cli_dashboard.h"
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
    const std::vector<double>& y) {
    if (A.empty() || y.empty()) return;

    size_t last = std::min(A.size(), y.size()) - 1;
    double current = y[last];
   
    // ---- baseline ----
    double mu = mean(y);
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

    double packets = row.size() > 0 ? row[0] : 0;
    double bytes   = row.size() > 1 ? row[1] : 0;
    double tcp     = row.size() > 2 ? row[2] : 0;
    double udp     = row.size() > 3 ? row[3] : 0;

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
} */


#include "cli_dashboard.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <string>
#include <vector>
 
#ifdef _WIN32
#include <windows.h>
static void enableWindowsAnsi() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleOutputCP(CP_UTF8);
}
#endif
 
// ─────────────────────────────────────────────
//  Basic 16-color ANSI only (works in CMD/PS)
// ─────────────────────────────────────────────
namespace ansi {
    inline std::string clear()           { return "\033[2J\033[H"; }
    inline std::string move(int r,int c) { return "\033["+std::to_string(r)+";"+std::to_string(c)+"H"; }
    inline std::string reset()           { return "\033[0m"; }
    inline std::string bold()            { return "\033[1m"; }
 
    // standard 16-color foreground codes
    inline std::string black()   { return "\033[30m"; }
    inline std::string red()     { return "\033[31m"; }
    inline std::string green()   { return "\033[32m"; }
    inline std::string yellow()  { return "\033[33m"; }
    inline std::string blue()    { return "\033[34m"; }
    inline std::string magenta() { return "\033[35m"; }
    inline std::string cyan()    { return "\033[36m"; }
    inline std::string white()   { return "\033[37m"; }
    inline std::string grey()    { return "\033[90m"; }  // bright black
    inline std::string bgreen()  { return "\033[92m"; }  // bright green
    inline std::string byellow() { return "\033[93m"; }  // bright yellow
    inline std::string bcyan()   { return "\033[96m"; }  // bright cyan
    inline std::string bwhite()  { return "\033[97m"; }  // bright white
 
    // background
    inline std::string bg_black()  { return "\033[40m"; }
    inline std::string bg_white()  { return "\033[47m"; }
    inline std::string bg_grey()   { return "\033[100m"; } // bright black bg
    inline std::string bg_cyan()   { return "\033[46m"; }
}
 
static int termWidth = 110;
 
// ─────────────────────────────────────────────
//  Stat helpers
// ─────────────────────────────────────────────
static double vecMean(const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}
static double vecMax(const std::vector<double>& v) {
    if (v.empty()) return 1.0;
    return *std::max_element(v.begin(), v.end());
}
 
// ─────────────────────────────────────────────
//  ASCII box drawing (plain + chars only)
// ─────────────────────────────────────────────
//  +-------[ TITLE ]-------+
//  |                       |
//  +-----------------------+
 
static void drawBox(int row, int col, int h, int w, const std::string& title) {
    // top border
    std::cout << ansi::move(row, col) << ansi::cyan() << "+";
    if (!title.empty()) {
        std::string t = "[ " + title + " ]";
        int tlen = (int)t.size();
        int rem  = w - 2 - tlen;
        int left = rem / 2, right = rem - left;
        for (int i = 0; i < left;  i++) std::cout << "-";
        std::cout << ansi::bcyan() << ansi::bold() << t << ansi::reset() << ansi::cyan();
        for (int i = 0; i < right && i+left+tlen < w-2; i++) std::cout << "-";
    } else {
        for (int i = 0; i < w-2; i++) std::cout << "-";
    }
    std::cout << "+" << ansi::reset();
 
    // sides
    for (int r = 1; r < h-1; r++) {
        std::cout << ansi::move(row+r, col)     << ansi::cyan() << "|" << ansi::reset();
        std::cout << ansi::move(row+r, col+w-1) << ansi::cyan() << "|" << ansi::reset();
    }
 
    // bottom border
    std::cout << ansi::move(row+h-1, col) << ansi::cyan() << "+";
    for (int i = 0; i < w-2; i++) std::cout << "-";
    std::cout << "+" << ansi::reset();
}
 
// Clear box interior to spaces
static void clearBox(int row, int col, int h, int w) {
    std::string blank(w-2, ' ');
    for (int r = 1; r < h-1; r++)
        std::cout << ansi::move(row+r, col+1) << blank;
}
 
// ─────────────────────────────────────────────
//  ASCII bar   [####.......]
// ─────────────────────────────────────────────
static void drawBar(int pct, int width) {
    int filled = std::min(width, (pct * width) / 100);
 
    std::string color;
    if      (pct < 50) color = ansi::bgreen();
    else if (pct < 80) color = ansi::byellow();
    else               color = ansi::red();
 
    std::cout << ansi::grey() << "[" << color;
    for (int i = 0; i < filled;        i++) std::cout << "#";
    std::cout << ansi::grey();
    for (int i = filled; i < width;    i++) std::cout << ".";
    std::cout << "]" << ansi::reset();
}
 
// ─────────────────────────────────────────────
//  ASCII sparkline using _.-^*
// ─────────────────────────────────────────────
static const char SPARK[] = { '_', '.', '-', '~', '^', '*', '#', '@' };
 
static void drawSparkline(const std::vector<double>& data, int width) {
    if (data.empty()) { std::cout << std::string(width, ' '); return; }
    double mx = vecMax(data);
    if (mx == 0) mx = 1.0;
    int start = (int)data.size() > width ? (int)data.size() - width : 0;
    std::cout << ansi::bcyan();
    for (int i = start; i < (int)data.size(); i++) {
        int idx = (int)((data[i] / mx) * 7.0);
        std::cout << SPARK[std::max(0,std::min(7,idx))];
    }
    int printed = (int)data.size() - start;
    for (int i = printed; i < width; i++) std::cout << ' ';
    std::cout << ansi::reset();
}
 
// ─────────────────────────────────────────────
//  ASCII signal graph (vertical bars of |)
// ─────────────────────────────────────────────
static void drawSignalGraph(int row, int col, int h, int w,
                             const std::vector<double>& y) {
    int innerW = w - 2;
    int innerH = h - 2;
    double mx = vecMax(y);
    if (mx == 0) mx = 1.0;
 
    int start = (int)y.size() > innerW ? (int)y.size() - innerW : 0;
 
    // build a 2D char grid
    std::vector<std::string> grid(innerH, std::string(innerW, ' '));
 
    for (int i = start; i < (int)y.size(); i++) {
        int col_i = i - start;
        double frac = y[i] / mx;
        int barH = (int)(frac * innerH);
        barH = std::min(barH, innerH);
        for (int b = 0; b < barH; b++) {
            int grid_r = innerH - 1 - b;
            // pick char by height
            if (b == barH - 1)     grid[grid_r][col_i] = '^';
            else if (b > barH*2/3) grid[grid_r][col_i] = '|';
            else                   grid[grid_r][col_i] = ':';
        }
    }
 
    for (int r = 0; r < innerH; r++) {
        std::cout << ansi::move(row+1+r, col+1);
        // color top row bright, rest dim
        double rowFrac = 1.0 - (double)r / innerH;
        if      (rowFrac > 0.8) std::cout << ansi::red();
        else if (rowFrac > 0.5) std::cout << ansi::byellow();
        else                    std::cout << ansi::bgreen();
        std::cout << grid[r] << ansi::reset();
    }
}
 
// ─────────────────────────────────────────────
//  Stat line helper
// ─────────────────────────────────────────────
static void statLine(int row, int col,
                     const std::string& label,
                     const std::string& value,
                     const std::string& valColor = "") {
    std::cout << ansi::move(row, col)
              << ansi::grey()  << std::left << std::setw(15) << label
              << (valColor.empty() ? ansi::bwhite() : valColor)
              << ansi::bold()  << value
              << ansi::reset();
}
 
// ─────────────────────────────────────────────
//  MAIN RENDER
// ─────────────────────────────────────────────
void CLIDashboard::render(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& y)
{
#ifdef _WIN32
    static bool ansiEnabled = false;
    if (!ansiEnabled) { enableWindowsAnsi(); ansiEnabled = true; }
#endif
 
    if (A.empty() || y.empty()) return;
 
    size_t last = std::min(A.size(), y.size()) - 1;
    const auto& row = A[last];
 
    double current = y[last];
    double mu      = vecMean(y);
    double mx      = vecMax(y);
    double dev     = std::abs(current - mu);
 
    // trend
    std::string trend    = "STABLE";
    std::string trendCol = ansi::cyan();
    if (last > 0) {
        if      (y[last] > y[last-1]) { trend = "/\\ UP";   trendCol = ansi::bgreen();  }
        else if (y[last] < y[last-1]) { trend = "\\/ DOWN"; trendCol = ansi::red();     }
    }
 
    // alert
    double sigPct = (mx > 0) ? (current / mx) * 100.0 : 0;
    std::string alert, alertCol;
    if      (sigPct < 50) { alert = "[*] NORMAL";   alertCol = ansi::bgreen();  }
    else if (sigPct < 80) { alert = "[!] ELEVATED"; alertCol = ansi::byellow(); }
    else                  { alert = "[X] CRITICAL"; alertCol = ansi::red();     }
 
    auto feat = [&](int i) -> double {
        return ((int)row.size() > i) ? row[i] : 0.0;
    };
    double packets = feat(0), bytes = feat(1),
           tcp     = feat(2), udp   = feat(3);
 
    int pkt_pct = std::min(100, (int)(packets / 100.0  * 100));
    int byt_pct = std::min(100, (int)(bytes   / 50000.0* 100));
    int tcp_pct = std::min(100, (int)(tcp     / 50.0   * 100));
    int udp_pct = std::min(100, (int)(udp     / 50.0   * 100));
 
    // clear screen
    std::cout << "\033[2J\033[H" << ansi::reset();
 
    // ── Title bar ────────────────────────────
    {
        std::string title = "=== NET MATRIX SIGNAL ANALYZER ===";
        int pad = (termWidth - (int)title.size()) / 2;
        std::cout << ansi::move(1, pad)
                  << ansi::bcyan() << ansi::bold() << title << ansi::reset();
    }
 
    // ── STATUS box  (rows 3-11, cols 1-38) ──
    {
        int r=3, c=1, h=9, w=38;
        drawBox(r, c, h, w, "STATUS");
        clearBox(r, c, h, w);
 
        statLine(r+1, c+2, "Signal Score:", std::to_string((int)current));
        statLine(r+2, c+2, "Baseline Avg:", std::to_string((int)mu));
        statLine(r+3, c+2, "Deviation:",    std::to_string((int)dev),  ansi::byellow());
        statLine(r+4, c+2, "Trend:",        trend,                      trendCol);
        statLine(r+5, c+2, "Alert:",        alert,                      alertCol);
 
        std::cout << ansi::move(r+7, c+2) << ansi::grey() << "Hist: ";
        drawSparkline(y, w - 10);
    }
 
    // ── FEATURES box (rows 3-11, cols 40-110) ─
    {
        int r=3, c=40, h=9, w=70;
        drawBox(r, c, h, w, "FEATURES  [window " + std::to_string(last) + "]");
        clearBox(r, c, h, w);
 
        int barW = w - 30;
        auto featureRow = [&](int row_, const std::string& lbl, double val, int pct) {
            std::cout << ansi::move(r+row_, c+2)
                      << ansi::grey() << std::left << std::setw(10) << lbl;
            drawBar(pct, barW);
            std::cout << ansi::bwhite() << ansi::bold()
                      << " " << std::right << std::setw(7) << (int)val
                      << ansi::reset();
        };
 
        featureRow(1, "PACKETS", packets, pkt_pct);
        featureRow(3, "BYTES",   bytes,   byt_pct);
        featureRow(5, "TCP",     tcp,     tcp_pct);
        featureRow(7, "UDP",     udp,     udp_pct);
    }
 
    // ── SIGNAL GRAPH box (rows 13-27) ─────────
    {
        int r=13, c=1, h=15, w=termWidth;
        drawBox(r, c, h, w, "SIGNAL GRAPH  y = Ax");
        clearBox(r, c, h, w);
        drawSignalGraph(r, c, h, w, y);
 
        // y-axis labels on right
        std::cout << ansi::move(r+1,   c+w-10) << ansi::grey()
                  << std::right << std::setw(8) << std::fixed << std::setprecision(1) << mx;
        std::cout << ansi::move(r+h/2, c+w-10) << ansi::grey()
                  << std::right << std::setw(8) << std::fixed << std::setprecision(1) << mx/2.0;
        std::cout << ansi::move(r+h-2, c+w-10) << ansi::grey()
                  << std::right << std::setw(8) << "0.0";
    }
 
    // ── SIGNAL LOG table (rows 29-37) ─────────
    {
        int r=29, c=1, h=9, w=termWidth;
        int show = (int)std::min((size_t)6, y.size());
        drawBox(r, c, h, w, "SIGNAL LOG  (last " + std::to_string(show) + " samples)");
        clearBox(r, c, h, w);
 
        // headers
        std::cout << ansi::move(r+1, c+2)
                  << ansi::cyan() << ansi::bold()
                  << std::left
                  << std::setw(6)  << "IDX"
                  << std::setw(12) << "SCORE"
                  << std::setw(12) << "DELTA"
                  << std::setw(8)  << "PCT"
                  << "BAR"
                  << ansi::reset();
 
        int startI = (int)y.size() - show;
        for (int i = 0; i < show; i++) {
            int idx    = startI + i;
            double val = y[idx];
            double delta = (idx > 0) ? (val - y[idx-1]) : 0.0;
            int pct      = (mx > 0) ? (int)((val / mx) * 100) : 0;
            int miniBar  = (pct * 25) / 100;
 
            std::string deltaCol = (delta > 0) ? ansi::bgreen() :
                                   (delta < 0) ? ansi::red()    : ansi::grey();
            std::string deltaStr = (delta >= 0 ? "+" : "") + std::to_string((int)delta);
 
            std::string barCol = pct < 50 ? ansi::bgreen() :
                                 pct < 80 ? ansi::byellow() : ansi::red();
 
            std::cout << ansi::move(r+2+i, c+2)
                      << ansi::grey()   << std::left  << std::setw(6)  << idx
                      << ansi::bwhite() << ansi::bold()
                      << std::setw(12) << std::fixed << std::setprecision(2) << val
                      << ansi::reset()
                      << deltaCol       << std::left  << std::setw(12) << deltaStr
                      << ansi::grey()   << std::setw(8)
                      << (std::to_string(pct) + "%")
                      << barCol;
            for (int b = 0; b < miniBar; b++) std::cout << "|";
            std::cout << ansi::reset();
        }
    }
 
    // ── Footer / hotkey bar ───────────────────
    {
        std::cout << ansi::move(39, 1)
                  << ansi::grey() << " Model: "
                  << ansi::bcyan() << ansi::bold() << "y = Ax"
                  << ansi::reset() << ansi::grey()
                  << "   Samples: " << y.size()
                  << "   Window: "  << last
                  << ansi::reset();
 
        // hotkey strip
        std::cout << ansi::move(40, 1)
                  << ansi::bg_grey() << std::string(termWidth, ' ');
        std::cout << ansi::move(40, 1) << ansi::bg_grey();
 
        auto hk = [&](const std::string& key, const std::string& lbl) {
            std::cout << ansi::bg_cyan() << ansi::black() << ansi::bold()
                      << " " << key << " "
                      << ansi::reset() << ansi::bg_grey() << ansi::grey()
                      << " " << lbl << "  ";
        };
        hk("Ctrl+C", "Quit");
        hk("r",      "Refresh");
        hk("f",      "Freeze");
        std::cout << ansi::reset();
    }
 
    std::cout << ansi::move(41, 1) << std::flush;
}

#include "ui/TableRenderer.hpp"

#include <array>
#include <string>

#include "core/Types.hpp"
#include "ui/Layout.hpp"
#include "ui/Terminal.hpp"

namespace married::ui {
namespace {

constexpr std::array<int, 4> kWidths{4, 30, 30, 6};
constexpr int kStartRow = 8;
constexpr int kStartCol = 2;

void drawHeader() {
    moveCursor(kStartRow, kStartCol);
    write(padRight("No.", kWidths[0]));
    moveCursor(kStartRow, kStartCol + kWidths[0] + 1);
    write(padRight("Marina", kWidths[1]));
    moveCursor(kStartRow, kStartCol + kWidths[0] + kWidths[1] + 2);
    write(padRight("Roman", kWidths[2]));
    moveCursor(kStartRow, kStartCol + kWidths[0] + kWidths[1] + kWidths[2] + 3);
    write(padRight("Match", kWidths[3]));
}

void drawSeparator() {
    moveCursor(kStartRow + 1, kStartCol);
    write(std::string(kScreenWidth - 4, '-'));
}

}  // namespace

void renderAttemptsTable(const std::vector<married::Attempt>& attempts) {
    drawHeader();
    drawSeparator();

    int y = kStartRow + 2;
    for (const auto& att : attempts) {
        moveCursor(y, kStartCol);
        write(padRight(std::to_string(att.number), kWidths[0]));
        write(" ");
        write(padRight(ellipsize(att.d1, kWidths[1]), kWidths[1]));
        write(" ");
        write(padRight(ellipsize(att.d2, kWidths[2]), kWidths[2]));
        write(" ");

        setMatchColor(att.match);
        write(padRight(att.match ? "YES" : "no", kWidths[3]));
        resetColor();
        ++y;
    }
}

}  // namespace married::ui

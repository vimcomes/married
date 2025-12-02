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

std::string formatRow(const married::Attempt& att) {
    std::string row;
    row.reserve(kWidths[0] + kWidths[1] + kWidths[2] + 3);
    row += padRight(std::to_string(att.number), kWidths[0]);
    row += " ";
    row += padRight(ellipsize(att.d1, kWidths[1]), kWidths[1]);
    row += " ";
    row += padRight(ellipsize(att.d2, kWidths[2]), kWidths[2]);
    row += " ";
    return row;
}

}  // namespace

void renderAttemptsTable(const std::vector<married::Attempt>& attempts) {
    writeAt(kStartRow, kStartCol, padRight("No.", kWidths[0]));
    writeAt(kStartRow, kStartCol + kWidths[0] + 1, padRight("Marina", kWidths[1]));
    writeAt(kStartRow, kStartCol + kWidths[0] + kWidths[1] + 2, padRight("Roman", kWidths[2]));
    writeAt(kStartRow, kStartCol + kWidths[0] + kWidths[1] + kWidths[2] + 3,
            padRight("Match", kWidths[3]));

    drawHorizontalLine(kStartRow + 1, kStartCol, kScreenWidth - 4);

    int y = kStartRow + 2;
    const int maxRows = kScreenHeight - 2 - y;
    int shown = 0;
    for (const auto& att : attempts) {
        if (shown >= maxRows) break;
        const int rowY = y + shown;
        writeAt(rowY, kStartCol, formatRow(att));
        setMatchColor(att.match);
        writeAt(rowY, kStartCol + kWidths[0] + kWidths[1] + kWidths[2] + 3,
                padRight(att.match ? "YES" : "no", kWidths[3]));
        resetColor();
        ++shown;
    }
}

}  // namespace married::ui

#include "ui/HudRenderer.hpp"

#include <string>
#include <vector>

#include "core/Types.hpp"
#include "ui/Layout.hpp"
#include "ui/TableRenderer.hpp"
#include "ui/Terminal.hpp"

namespace married::ui {
namespace {

void drawBorder() {
    const std::string horizontal(kScreenWidth - 2, '-');
    moveCursor(0, 0);
    write("+" + horizontal + "+");
    for (int row = 1; row < kScreenHeight - 1; ++row) {
        moveCursor(row, 0);
        write("|");
        moveCursor(row, kScreenWidth - 1);
        write("|");
    }
    moveCursor(kScreenHeight - 1, 0);
    write("+" + horizontal + "+");
}

}  // namespace

void renderHeader(int attempts, int matches, int tickMs) {
    moveCursor(1, 2);
    write("Att " + std::to_string(attempts) + " | Match " + std::to_string(matches) + "/" +
          std::to_string(kTargetMatches) + " | Tick " + std::to_string(tickMs) + "ms");
    moveCursor(2, 2);
    write("Fastest - | Slowest - | Showing " + std::to_string(kRecentLimit));
}

void renderProgress(int matches) {
    constexpr int barWidth = 20;
    int filled = (matches * barWidth) / kTargetMatches;
    moveCursor(3, 2);
    write("Progress: [");
    setBarColor();
    for (int i = 0; i < barWidth; ++i) {
        write(i < filled ? "=" : " ");
    }
    resetColor();
    write("]");
}

void renderCurrentDesires(const std::string& d1, const std::string& d2) {
    const int desireWidth = 26;
    moveCursor(5, 2);
    write("Current desires:");
    moveCursor(6, 2);
    setAccentColor();
    write(" " + padRight("Marina", 8) + " " + padRight(ellipsize(d1, desireWidth), desireWidth) + " | " +
          padRight("Roman", 8) + " " + padRight(ellipsize(d2, desireWidth), desireWidth));
    resetColor();
}

void renderStatusLine() {
    moveCursor(kScreenHeight - 2, 2);
    write("[q] quit");
}

void renderFrame(const married::History& history, const std::string& d1, const std::string& d2, int tickMs) {
    clearScreen();
    drawBorder();
    renderHeader(history.attemptsCount(), history.matchesCount(), tickMs);
    renderProgress(history.matchesCount());
    renderCurrentDesires(d1, d2);
    renderAttemptsTable(history.attempts());
    renderStatusLine();
    flush();
}

void renderSummary(const married::History& history) {
    clearScreen();
    drawBorder();

    moveCursor(1, 2);
    write("Top matched desires:");
    const auto top = history.topMatches();
    int y = 3;
    for (std::size_t i = 0; i < top.size() && y < kScreenHeight - 6; ++i) {
        moveCursor(y, 2);
        const auto countStr = std::to_string(top[i].second);
        const auto indexStr = std::to_string(i + 1);
        const std::string indexField = (indexStr.size() < 2 ? std::string(2 - indexStr.size(), ' ') : "") + indexStr;
        const std::string countField = (countStr.size() < 2 ? std::string(2 - countStr.size(), ' ') : "") + countStr;
        write(indexField + ") " + padRight(ellipsize(top[i].first, 52), 52) + " x" + countField);
        ++y;
    }

    const char* art[] = {
        "   O     O  ",
        "  /|\\   /|\\ ",
        "  / \\   / \\ ",
        "  |       | ",
        " / \\     / \\"};
    int artStart = kScreenHeight - 8;
    for (int i = 0; i < 5 && artStart + i < kScreenHeight - 2; ++i) {
        moveCursor(artStart + i, 2);
        write(art[i]);
    }

    moveCursor(kScreenHeight - 2, 2);
    write("Finished after " + std::to_string(history.attemptsCount()) + " attempts. Press any key to exit.");
    flush();
}

}  // namespace married::ui

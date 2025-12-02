#include "ui/HudRenderer.hpp"

#include <format>
#include <string>
#include <vector>

#include "core/Types.hpp"
#include "ui/Layout.hpp"
#include "ui/TableRenderer.hpp"
#include "ui/Terminal.hpp"

namespace married::ui {
namespace {

void drawFrameBorder() { drawBorder(kScreenWidth, kScreenHeight); }

}  // namespace

std::string formatSeconds(int valueMs) {
    const double seconds = static_cast<double>(valueMs) / 1000.0;
    return std::format("{:.1f}s", seconds);
}

std::string formatTimingField(const std::string& label, int valueMs, bool available) {
    if (!available) return label + " -";
    return label + " " + formatSeconds(valueMs);
}

void renderHeader(const married::History& history, int tickMs) {
    writeAt(1, 2, "Att " + std::to_string(history.attemptsCount()) + " | Match " +
                       std::to_string(history.matchesCount()) + "/" + std::to_string(kTargetMatches) +
                       " | Tick " + std::to_string(tickMs) + "ms");

    const bool hasTiming = history.hasTiming();
    const std::string fastest = formatTimingField("Fastest", history.fastestMs(), hasTiming);
    const std::string slowest = formatTimingField("Slowest", history.slowestMs(), hasTiming);
    const std::string average = formatTimingField("Avg", history.averageMs(), hasTiming);
    writeAt(2, 2, fastest + " | " + slowest + " | " + average + " | Showing " +
                      std::to_string(kRecentLimit));
}

void renderProgress(int matches) {
    constexpr int barWidth = 20;
    int filled = (matches * barWidth) / kTargetMatches;
    writeAt(3, 2, "Progress: [");
    setBarColor();
    for (int i = 0; i < barWidth; ++i) {
        write(i < filled ? "=" : " ");
    }
    resetColor();
    write("]");
}

void renderCurrentDesires(const std::string& d1, const std::string& d2) {
    const int desireWidth = 26;
    writeAt(5, 2, "Current desires:");
    setAccentColor();
    writeAt(6, 2, " " + padRight("Marina", 8) + " " + padRight(ellipsize(d1, desireWidth), desireWidth) +
                      " | " + padRight("Roman", 8) + " " +
                      padRight(ellipsize(d2, desireWidth), desireWidth));
    resetColor();
}

void renderStatusLine() { writeAt(kScreenHeight - 2, 2, "[q] quit"); }

void renderFrame(const married::History& history, const std::string& d1, const std::string& d2, int tickMs) {
    clearScreen();
    drawFrameBorder();
    renderHeader(history, tickMs);
    renderProgress(history.matchesCount());
    renderCurrentDesires(d1, d2);
    renderAttemptsTable(history.attempts());
    renderStatusLine();
    flush();
}

void renderSummary(const married::History& history) {
    clearScreen();
    drawFrameBorder();

    writeAt(1, 2, "Top matched desires:");
    const auto top = history.topMatches();
    int y = 3;
    for (std::size_t i = 0; i < top.size() && y < kScreenHeight - 6; ++i) {
        const auto indexStr = std::to_string(i + 1);
        const auto countStr = std::to_string(top[i].second);
        const std::string indexField =
            (indexStr.size() < 2 ? std::string(2 - indexStr.size(), ' ') : "") + indexStr;
        const std::string countField =
            (countStr.size() < 2 ? std::string(2 - countStr.size(), ' ') : "") + countStr;
        writeAt(y, 2, indexField + ") " + padRight(ellipsize(top[i].first, 52), 52) + " x" + countField);
        ++y;
    }

    const char* art[] = {
        "  O        O",
        " /|\\     /|\\",
        " / \\     / \\",
        "  |        |",
        " / \\     / \\"};
    int artStart = kScreenHeight - 8;
    for (int i = 0; i < 5 && artStart + i < kScreenHeight - 2; ++i) {
        writeAt(artStart + i, 2, art[i]);
    }

    writeAt(kScreenHeight - 2, 2,
            "Finished after " + std::to_string(history.attemptsCount()) + " attempts. Press any key to exit.");
    flush();
}

}  // namespace married::ui

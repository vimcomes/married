#include "ui/Ui.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <curses.h>
#include <string>
#include <vector>

#include "core/Types.h"

namespace {

std::string ellipsize(const std::string& text, int width) {
    if (width <= 0) return "";
    if (static_cast<int>(text.size()) <= width) return text;
    if (width <= 3) return text.substr(0, static_cast<std::size_t>(width));
    return text.substr(0, static_cast<std::size_t>(width - 3)) + "...";
}

void draw_border(int width, int height) {
    box(stdscr, 0, 0);
    mvhline(0, width, ' ', std::max(0, getmaxx(stdscr) - width));
    mvvline(height, 0, ' ', std::max(0, getmaxy(stdscr) - height));
}

enum ColorPair : short {
    kPairNormal = 1,
    kPairGood = 2,
    kPairBad = 3,
    kPairAccent = 4,
    kPairBar = 5
};

bool colorsEnabled() {
    static bool initialized = false;
    static bool enabled = false;
    if (!initialized) {
        initialized = true;
        enabled = has_colors();
        if (enabled) {
            start_color();
            use_default_colors();
            init_pair(kPairNormal, COLOR_WHITE, -1);
            init_pair(kPairGood, COLOR_GREEN, -1);
            init_pair(kPairBad, COLOR_RED, -1);
            init_pair(kPairAccent, COLOR_CYAN, -1);
            init_pair(kPairBar, COLOR_YELLOW, -1);
        }
    }
    return enabled;
}

void draw_header(int attempts, int matches, int tickMs) {
    mvprintw(1, 2, "Att %d | Match %d/%d | Tick %dms", attempts, matches, kTargetMatches, tickMs);
    mvprintw(2, 2, "Fastest - | Slowest - | Showing %d", kRecentLimit);
}

void draw_progress(int matches) {
    constexpr int barWidth = 20;
    int filled = (matches * barWidth) / kTargetMatches;
    mvprintw(3, 2, "Progress: [");
    if (colorsEnabled()) attron(COLOR_PAIR(kPairBar));
    for (int i = 0; i < barWidth; ++i) {
        addch(i < filled ? '=' : ' ');
    }
    if (colorsEnabled()) attroff(COLOR_PAIR(kPairBar));
    printw("]");
}

void draw_current(int y, const std::string& d1, const std::string& d2) {
    const int desireWidth = 26;
    mvprintw(y, 2, "Current desires:");
    if (colorsEnabled()) attron(COLOR_PAIR(kPairAccent));
    mvprintw(y + 1, 2, " %-8s %-26s | %-8s %-26s", "Marina",
             ellipsize(d1, desireWidth).c_str(), "Roman", ellipsize(d2, desireWidth).c_str());
    if (colorsEnabled()) attroff(COLOR_PAIR(kPairAccent));
}

void draw_table_header(int y, int x, const std::array<int, 4>& widths) {
    mvaddnstr(y, x, "No.", widths[0]);
    mvaddnstr(y, x + widths[0] + 1, "Marina", widths[1]);
    mvaddnstr(y, x + widths[0] + widths[1] + 2, "Roman", widths[2]);
    mvaddnstr(y, x + widths[0] + widths[1] + widths[2] + 3, "Match", widths[3]);
}

void draw_attempts(int y_start, int width, const std::vector<Attempt>& attempts) {
    const std::array<int, 4> widths{4, 30, 30, 6};
    const int x = 2;
    draw_table_header(y_start, x, widths);
    mvhline(y_start + 1, x, ACS_HLINE, width - 4);

    int y = y_start + 2;
    const int maxRows = kScreenHeight - 2 - y;
    int shown = 0;
    for (const auto& att : attempts) {
        if (shown >= maxRows) break;
        mvprintw(y + shown, x, "%-*d %-*s %-*s ", widths[0],
                 att.number, widths[1],
                 ellipsize(att.d1, widths[1]).c_str(), widths[2],
                 ellipsize(att.d2, widths[2]).c_str());
        if (colorsEnabled()) {
            attron(COLOR_PAIR(att.match ? kPairGood : kPairBad));
        }
        mvprintw(y + shown, x + widths[0] + widths[1] + widths[2] + 3, "%-*s", widths[3],
                 att.match ? "YES" : "no");
        if (colorsEnabled()) {
            attroff(COLOR_PAIR(att.match ? kPairGood : kPairBad));
        }
        ++shown;
    }
}

void draw_status() {
    mvprintw(kScreenHeight - 2, 2, "[q] quit");
}

} // namespace

void init_ncurses() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    colorsEnabled();
}

void shutdown_ncurses() {
    endwin();
}

void render_frame(int attemptsCount,
                  int matchCount,
                  const std::string& d1,
                  const std::string& d2,
                  const std::vector<Attempt>& attempts,
                  int tickMs) {
    erase();
    draw_border(kScreenWidth, kScreenHeight);
    draw_header(attemptsCount, matchCount, tickMs);
    draw_progress(matchCount);
    draw_current(5, d1, d2);
    draw_attempts(8, kScreenWidth, attempts);
    draw_status();
    refresh();
}

void draw_summary(const std::vector<std::pair<std::string, int>>& matched, int attemptsCount) {
    erase();
    draw_border(kScreenWidth, kScreenHeight);
    mvprintw(1, 2, "Top matched desires:");
    int y = 3;
    for (std::size_t i = 0; i < matched.size() && y < kScreenHeight - 6; ++i) {
        mvprintw(y, 2, "%2zu) %-52s x%2d", i + 1,
                 ellipsize(matched[i].first, 52).c_str(), matched[i].second);
        ++y;
    }

    // Simple ASCII art (fits width)
    const char* art[] = {
        "   O     O  ",
        "  /|\\   /|\\ ",
        "  / \\   / \\ ",
        "  |       | ",
        " / \\     / \\"};
    int artStart = kScreenHeight - 8;
    for (int i = 0; i < 5 && artStart + i < kScreenHeight - 2; ++i) {
        mvprintw(artStart + i, 2, "%s", art[i]);
    }

    mvprintw(kScreenHeight - 2, 2, "Finished after %d attempts. Press any key to exit.", attemptsCount);
    refresh();
}

#include "ui/Terminal.hpp"

#include <algorithm>
#include <string>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <curses.h>
#endif

namespace married::ui {
namespace {

#ifdef _WIN32
HANDLE g_out = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE g_in = GetStdHandle(STD_INPUT_HANDLE);
DWORD g_defaultOutMode = 0;
DWORD g_defaultInMode = 0;
WORD g_defaultAttributes = 0;

void setCursorVisibility(bool visible) {
    CONSOLE_CURSOR_INFO info{};
    GetConsoleCursorInfo(g_out, &info);
    info.bVisible = visible;
    SetConsoleCursorInfo(g_out, &info);
}

void setColor(WORD attributes) {
    SetConsoleTextAttribute(g_out, attributes);
}

COORD makeCoord(int row, int col) {
    COORD c;
    c.X = static_cast<SHORT>(col);
    c.Y = static_cast<SHORT>(row);
    return c;
}

#else
enum ColorPair : short {
    kPairNormal = 1,
    kPairGood = 2,
    kPairBad = 3,
    kPairAccent = 4,
    kPairBar = 5
};

bool initColors() {
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
#endif

}  // namespace

void initializeTerminal() {
#ifdef _WIN32
    GetConsoleMode(g_out, &g_defaultOutMode);
    GetConsoleMode(g_in, &g_defaultInMode);
    DWORD outMode = g_defaultOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    DWORD inMode = g_defaultInMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(g_out, outMode);
    SetConsoleMode(g_in, inMode);

    CONSOLE_SCREEN_BUFFER_INFO info{};
    GetConsoleScreenBufferInfo(g_out, &info);
    g_defaultAttributes = info.wAttributes;

    setCursorVisibility(false);
#else
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    initColors();
#endif
}

void shutdownTerminal() {
#ifdef _WIN32
    setColor(g_defaultAttributes);
    setCursorVisibility(true);
    SetConsoleMode(g_out, g_defaultOutMode);
    SetConsoleMode(g_in, g_defaultInMode);
#else
    endwin();
#endif
}

void hideCursor() {
#ifdef _WIN32
    setCursorVisibility(false);
#else
    curs_set(0);
#endif
}

void showCursor() {
#ifdef _WIN32
    setCursorVisibility(true);
#else
    curs_set(1);
#endif
}

void clearScreen() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info{};
    GetConsoleScreenBufferInfo(g_out, &info);
    DWORD size = static_cast<DWORD>(info.dwSize.X) * static_cast<DWORD>(info.dwSize.Y);
    COORD origin{0, 0};
    DWORD written = 0;
    FillConsoleOutputCharacterA(g_out, ' ', size, origin, &written);
    FillConsoleOutputAttribute(g_out, g_defaultAttributes, size, origin, &written);
    moveCursor(0, 0);
#else
    erase();
#endif
}

void moveCursor(int row, int col) {
#ifdef _WIN32
    SetConsoleCursorPosition(g_out, makeCoord(row, col));
#else
    move(row, col);
#endif
}

void drawBorder(int width, int height) {
#ifdef _WIN32
    const std::string horizontal(width - 2, '-');
    moveCursor(0, 0);
    write("+" + horizontal + "+");
    for (int y = 1; y < height - 1; ++y) {
        moveCursor(y, 0);
        write("|");
        moveCursor(y, width - 1);
        write("|");
    }
    moveCursor(height - 1, 0);
    write("+" + horizontal + "+");
#else
    box(stdscr, 0, 0);
    mvhline(0, width, ' ', std::max(0, getmaxx(stdscr) - width));
    mvvline(height, 0, ' ', std::max(0, getmaxy(stdscr) - height));
#endif
}

void drawHorizontalLine(int row, int col, int count) {
#ifdef _WIN32
    moveCursor(row, col);
    write(std::string(static_cast<std::size_t>(count), '-'));
#else
    mvhline(row, col, ACS_HLINE, count);
#endif
}

void write(const std::string& text) {
#ifdef _WIN32
    DWORD written = 0;
    WriteConsoleA(g_out, text.c_str(), static_cast<DWORD>(text.size()), &written, nullptr);
#else
    addnstr(text.c_str(), static_cast<int>(text.size()));
#endif
}

void writeAt(int row, int col, const std::string& text) {
    moveCursor(row, col);
    write(text);
}

void setMatchColor(bool match) {
#ifdef _WIN32
    setColor(match ? FOREGROUND_GREEN | FOREGROUND_INTENSITY
                   : FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
    if (initColors()) attron(COLOR_PAIR(match ? kPairGood : kPairBad));
#endif
}

void setAccentColor() {
#ifdef _WIN32
    setColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
    if (initColors()) attron(COLOR_PAIR(kPairAccent));
#endif
}

void setBarColor() {
#ifdef _WIN32
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
    if (initColors()) attron(COLOR_PAIR(kPairBar));
#endif
}

void resetColor() {
#ifdef _WIN32
    setColor(g_defaultAttributes);
#else
    if (initColors()) {
        attrset(COLOR_PAIR(kPairNormal));
    } else {
        attrset(A_NORMAL);
    }
#endif
}

bool pollKey(char& ch) {
#ifdef _WIN32
    if (_kbhit() == 0) return false;
    int c = _getch();
    if (c == 0 || c == 0xE0) return false;
    ch = static_cast<char>(c);
    return true;
#else
    int c = getch();
    if (c == ERR) return false;
    ch = static_cast<char>(c);
    return true;
#endif
}

char waitKey() {
    char ch{};
#ifdef _WIN32
    ch = static_cast<char>(_getch());
#else
    setBlocking(true);
    ch = static_cast<char>(getch());
    setBlocking(false);
#endif
    return ch;
}

void setBlocking(bool blocking) {
#ifdef _WIN32
    DWORD mode = g_defaultInMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    if (!blocking) {
        mode &= ~ENABLE_PROCESSED_INPUT;
    }
    SetConsoleMode(g_in, mode);
#else
    nodelay(stdscr, blocking ? FALSE : TRUE);
#endif
}

void flush() {
#ifdef _WIN32
    // WriteConsoleA is immediate; nothing additional needed.
#else
    refresh();
#endif
}

}  // namespace married::ui

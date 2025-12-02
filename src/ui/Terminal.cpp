#include "ui/Terminal.hpp"

#include <iostream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace married::ui {
namespace {

#ifdef _WIN32
HANDLE g_out = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE g_in = GetStdHandle(STD_INPUT_HANDLE);
DWORD g_defaultOutMode = 0;
DWORD g_defaultInMode = 0;
WORD g_defaultAttributes = 0;

void enableVirtualTerminal() {
    GetConsoleMode(g_out, &g_defaultOutMode);
    DWORD outMode = g_defaultOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(g_out, outMode);

    CONSOLE_SCREEN_BUFFER_INFO info{};
    GetConsoleScreenBufferInfo(g_out, &info);
    g_defaultAttributes = info.wAttributes;

    GetConsoleMode(g_in, &g_defaultInMode);
    DWORD inMode = g_defaultInMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(g_in, inMode);
}

void restoreConsole() {
    SetConsoleMode(g_out, g_defaultOutMode);
    SetConsoleMode(g_in, g_defaultInMode);
    SetConsoleTextAttribute(g_out, g_defaultAttributes);
}

void setAnsiColor(short color) {
    std::cout << "\033[" << color << "m";
}
#else
termios g_origTerm{};
int g_origFlags = -1;
bool g_termReady = false;

void enableRawMode() {
    if (g_termReady) return;
    g_termReady = true;
    tcgetattr(STDIN_FILENO, &g_origTerm);
    termios raw = g_origTerm;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    g_origFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, g_origFlags | O_NONBLOCK);
}

void restoreTerm() {
    if (!g_termReady) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &g_origTerm);
    if (g_origFlags != -1) {
        fcntl(STDIN_FILENO, F_SETFL, g_origFlags);
    }
    g_termReady = false;
}

void setAnsiColor(short color) {
    std::cout << "\033[" << color << "m";
}
#endif

}  // namespace

void initializeTerminal() {
#ifdef _WIN32
    enableVirtualTerminal();
#else
    enableRawMode();
#endif
    hideCursor();
    clearScreen();
    flush();
}

void shutdownTerminal() {
    resetColor();
    showCursor();
    flush();
#ifdef _WIN32
    restoreConsole();
#else
    restoreTerm();
#endif
}

void hideCursor() { std::cout << "\033[?25l"; }

void showCursor() { std::cout << "\033[?25h"; }

void clearScreen() {
    std::cout << "\033[2J";
    moveCursor(0, 0);
}

void moveCursor(int row, int col) {
    std::cout << "\033[" << row + 1 << ";" << col + 1 << "H";
}

void setMatchColor(bool match) { setAnsiColor(match ? 32 : 31); }

void setAccentColor() { setAnsiColor(36); }

void setBarColor() { setAnsiColor(33); }

void resetColor() { setAnsiColor(0); }

bool pollKey(char& ch) {
#ifdef _WIN32
    if (_kbhit() == 0) return false;
    int c = _getch();
    if (c == 0 || c == 0xE0) return false;
    ch = static_cast<char>(c);
    return true;
#else
    unsigned char c{};
    ssize_t n = ::read(STDIN_FILENO, &c, 1);
    if (n <= 0) return false;
    ch = static_cast<char>(c);
    return true;
#endif
}

void write(const std::string& text) { std::cout << text; }

void flush() { std::cout.flush(); }

}  // namespace married::ui

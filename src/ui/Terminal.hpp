#pragma once

#include <string>

namespace married::ui {

void initializeTerminal();
void shutdownTerminal();

void hideCursor();
void showCursor();
void clearScreen();
void moveCursor(int row, int col);

void setMatchColor(bool match);
void setAccentColor();
void setBarColor();
void resetColor();

bool pollKey(char& ch);
void write(const std::string& text);
void flush();

}  // namespace married::ui

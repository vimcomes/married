#pragma once

#include <string>

namespace married::ui {

void initializeTerminal();
void shutdownTerminal();

void hideCursor();
void showCursor();
void clearScreen();
void moveCursor(int row, int col);
void drawBorder(int width, int height);
void drawHorizontalLine(int row, int col, int count);
void write(const std::string& text);
void writeAt(int row, int col, const std::string& text);

void setMatchColor(bool match);
void setAccentColor();
void setBarColor();
void resetColor();

bool pollKey(char& ch);
char waitKey();
void setBlocking(bool blocking);
void flush();

}  // namespace married::ui

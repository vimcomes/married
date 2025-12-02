#include "core/Input.h"

#include <curses.h>

bool handle_input() {
    int ch = getch();
    if (ch == 'q' || ch == 'Q') return false;
    return true;
}

#include "app/App.h"
#include "ui/Ui.h"

int main() {
    init_ncurses();
    {
        App app;
        app.run();
    }
    shutdown_ncurses();
    return 0;
}

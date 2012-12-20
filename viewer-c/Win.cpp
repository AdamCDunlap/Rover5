#include <unistd.h>
#include "Win.h"

static bool usecurses = isatty(1); // 1 is stdout

Win::Win() {
    if (usecurses) {
        initscr();
        curs_set(0);

        int maxy, maxx;
        getmaxyx(stdscr, maxy, maxx);

        winds[Win::stmicros] = newwin(4, 15, 0, 0);
        mvwaddstr(winds[Win::stmicros], 1, 1, "Start Micros:");

        winds[Win::endmicros] = newwin(4, 15, 4, 0);
        mvwaddstr(winds[Win::endmicros], 1, 1, "End Micros:");

        winds[Win::encdsts] = newwin(5, 18, 0, 40);
        mvwaddstr(winds[Win::encdsts], 1,  1, "Enc Dists:");
        mvwaddstr(winds[Win::encdsts], 2,  1, "FR");
        mvwaddstr(winds[Win::encdsts], 2, 15, "FL");
        mvwaddstr(winds[Win::encdsts], 3,  1, "BR");
        mvwaddstr(winds[Win::encdsts], 3, 15, "BL");

        winds[Win::pows] = newwin(5, 17, 0, 60);
        mvwaddstr(winds[Win::pows], 1,  1, "Motor Powers:");
        mvwaddstr(winds[Win::pows], 2,  1, "FR");
        mvwaddstr(winds[Win::pows], 2, 14, "FL");
        mvwaddstr(winds[Win::pows], 3,  1, "BR");
        mvwaddstr(winds[Win::pows], 3, 14, "BL");

        winds[Win::pos] = newwin(7, 15, 10, 0);
        mvwaddstr(winds[Win::pos], 1, 1, "Reported Pos:");
        mvwaddstr(winds[Win::pos], 2, 3, "X");
        mvwaddstr(winds[Win::pos], 3, 3, "Y");
        mvwaddstr(winds[Win::pos], 4, 1, "ang");

        winds[Win::errs] = newwin(4, 10, maxy-4, maxx-10);
        mvwaddstr(winds[Win::errs], 1, 1, "Errors:");

        for (size_t i=0; i<n; i++) {
            box(winds[i], 0, 0);
            wrefresh(winds[i]);
        }
    }
}

Win::~Win() {
    if (usecurses) {
        for (size_t i=0; i<n; i++) {
            delwin(winds[i]);
        }
        endwin();
    }
}

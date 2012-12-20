#ifndef WIN_H
#define WIN_H
#include <curses.h>

class Win {
public:
    enum names {stmicros, endmicros, encdsts, pows, pos, errs, n};

    Win();
    ~Win();
    WINDOW* operator[](size_t i) { return winds[i]; }
private:
    WINDOW* winds[n];
};

#endif//WIN_H

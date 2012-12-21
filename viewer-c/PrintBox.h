#ifndef PRINTSCREEN_H
#define PRINTSCREEN_H

#include <vector>
#include <curses.h>

class PrintBox {
public:
// "Constructor"
    static PrintBox* NewBox(int width, int height, const char* label);

    static void refreshAll();

    int refresh() { return wrefresh(win); }
    int box(chtype verch, chtype horch) { return ::box(win, verch, horch); }
    int mvprintw(int y, int x, const char* fmt, ...) {
        va_list ap;
        int ret;
        va_start(ap, fmt);
        wmove(win, y+2, x+1);
        ret = vw_printw(win, fmt, ap);
        va_end(ap);
        return ret;
    }
    int printw(const char* fmt, ...) {
        va_list ap;
        int ret;
        va_start(ap, fmt);
        ret = vw_printw(win, fmt, ap);
        va_end(ap);
        return ret;
    }
    int move(int y, int x) {
        return wmove(win, y, x);
    }
    int printf(const char* fmt, ...) {
        va_list ap;
        int ret;
        wmove(win, 2, 1);
        va_start(ap, fmt);
        ret = vw_printw(win, fmt, ap);
        va_end(ap);
        return ret;
    }


    WINDOW* getWin() { return win; }
private:
    // Private constructor and destructor
    PrintBox(int height, int width, int ypos, int xpos, const char* label);
    ~PrintBox();

    WINDOW* win;

    static std::vector<PrintBox*> boxes;
    static int nextxpos;
    static int nextypos;
    static int maxHeight;
    static bool firstRun;
};

#endif//PRINTSCREEN_H

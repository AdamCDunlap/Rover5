#ifndef PRINTSCREEN_H
#define PRINTSCREEN_H

#include <vector>
#include <curses.h>
#include <cstring>

class PrintBox {
public:
    PrintBox(int height, int width, const char* label);
    PrintBox(int height, const char* label) : PrintBox(height, strlen(label), label) {}
    PrintBox(const char* label) : PrintBox(1, label) {}
    ~PrintBox();

    static void refreshAll();

    int refresh();
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
    int move(int y, int x) { return wmove(win, y, x); }
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
    WINDOW* win;
    const char* label;

    static std::vector<PrintBox*> boxes;
    static int nextxpos;
    static int nextypos;
    static int maxHeight;
    static int numClasses;
};

#endif//PRINTSCREEN_H

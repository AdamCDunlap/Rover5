#include "PrintBox.h"
using namespace std;

PrintBox::PrintBox(int height, int width, const char* label) {
    if (numClasses == 0) {
        initscr();
        curs_set(0);
        noecho();
        nodelay(stdscr, true);
        //keypad(stdscr, true);
    }

    win = newwin(height+3, width+2, nextypos, nextxpos);
    this->label = label;
    refresh();
    boxes.push_back(this);

    // For border
    width += 2;
    height += 3;


    if (height > maxHeight) maxHeight = height;
    nextxpos += width;
    if (nextxpos > 80) {
        nextxpos = 0;
        nextypos += maxHeight;
        maxHeight = 0;
    }
    numClasses++;
}

PrintBox::~PrintBox() {
    delwin(win);
    numClasses--;
    if (numClasses == 0) {
        endwin();
    }
}

int PrintBox::refresh() {
    // First redraw the box and text
    box(0, 0);
    mvprintw(-1, 0, label);
    return wrefresh(win);
}

void PrintBox::refreshAll() {
    for (PrintBox* box : boxes) {
        box->refresh();
    }
}

vector<PrintBox*> PrintBox::boxes;
int PrintBox::nextxpos = 0;
int PrintBox::nextypos = 0;
int PrintBox::maxHeight = 3;
int PrintBox::numClasses = 0;

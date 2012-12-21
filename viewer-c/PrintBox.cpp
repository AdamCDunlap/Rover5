#include "PrintBox.h"
using namespace std;

// "Constructor"
PrintBox* PrintBox::NewBox(int height, int width, const char* label) {
    if (firstRun) {
        initscr();
        curs_set(0);
        firstRun = false;
    }

    PrintBox* tmp = new PrintBox(height, width, nextypos, nextxpos, label);
    // For border
    width += 2;
    height += 3;

    boxes.push_back(tmp);

    if (height > maxHeight) maxHeight = height;
    nextxpos += width;
    if (nextxpos > 80) {
        nextxpos = 0;
        nextypos += maxHeight;
        maxHeight = 0;
    }

    return tmp;
}

void PrintBox::refreshAll() {
    for (PrintBox* box : boxes) {
        box->refresh();
    }
}

PrintBox::PrintBox(int height, int width, int ypos, int xpos, const char* label) {
    win = newwin(height+3, width+2, ypos, xpos);
    ::box(win, 0, 0);
    mvprintw(-1, 0, label);
}

vector<PrintBox*> PrintBox::boxes;
int PrintBox::nextxpos = 0;
int PrintBox::nextypos = 0;
int PrintBox::maxHeight = 3;
bool PrintBox::firstRun = true;

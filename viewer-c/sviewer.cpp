#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#include "Rover5.h"

#include "PrintBox.h"
bool usecurses = isatty(1); // 1 is stdout

// stty the port
// read from the port
// parse data
// display data
// ?do math

bool checkForValue(int fd, char w);
int openPort(const char* ardpath);
void runOffKB();

Rover5 virtualBot;

int main(int argc, char* argv[]) {

// Set up serial port
    const char* ardpath;
    if (argc >= 2) ardpath = argv[1];
    else ardpath = "/dev/ttyACM0";

    int ardfd = openPort(ardpath);
    virtualBot.SetArdFd(ardfd);

    while (true) {

        // Give these weird starting values so I'll know if they weren't
        //  changed
        uint32_t stmicros   = 1234567890;
        uint32_t endmicros  = 1234567890;
        int32_t  encdsts[4] = {1234567890, 1234567890, 1234567890, 1234567890};
        int16_t  pows[4]    = {12345,12345,12345,12345};
        int32_t  xpos = 1234567890, ypos = 1234567890;
        int32_t  xdist= 1234567890, ydist= 1234567890;
        uint16_t ang = 12345;
        int16_t  spds[4];


        // {{{ Read Serial Port
        startreading:
        if (checkForValue(ardfd, '0')) goto startreading;
        read(ardfd, &stmicros, 4);
        if (checkForValue(ardfd, '1')) goto startreading;
        read(ardfd, &encdsts[Rover5::FL], 4);
        if (checkForValue(ardfd, '2')) goto startreading;
        read(ardfd, &encdsts[Rover5::FR], 4);
        if (checkForValue(ardfd, '3')) goto startreading;
        read(ardfd, &encdsts[Rover5::BL], 4);
        if (checkForValue(ardfd, '4')) goto startreading;
        read(ardfd, &encdsts[Rover5::BR], 4);
        if (checkForValue(ardfd, '5')) goto startreading;

        read(ardfd, &pows[Rover5::FL], 2);
        if (checkForValue(ardfd, '6')) goto startreading;
        read(ardfd, &pows[Rover5::FR], 2);
        if (checkForValue(ardfd, '7')) goto startreading;
        read(ardfd, &pows[Rover5::BL], 2);
        if (checkForValue(ardfd, '8')) goto startreading;
        read(ardfd, &pows[Rover5::BR], 2);
        if (checkForValue(ardfd, '9')) goto startreading;

        read(ardfd, &xpos, 4);
        if (checkForValue(ardfd, 'A')) goto startreading;
        read(ardfd, &ypos, 4);
        if (checkForValue(ardfd, 'B')) goto startreading;
        read(ardfd, &ang, 4);
        if (checkForValue(ardfd, 'C')) goto startreading;

        read(ardfd, &xdist, 4);
        if (checkForValue(ardfd, 'D')) goto startreading;
        read(ardfd, &ydist, 4);
        if (checkForValue(ardfd, 'E')) goto startreading;

        read(ardfd, &spds[Rover5::FL], 2);
        if (checkForValue(ardfd, 'F')) goto startreading;
        read(ardfd, &spds[Rover5::FR], 2);
        if (checkForValue(ardfd, 'G')) goto startreading;
        read(ardfd, &spds[Rover5::BL], 2);
        if (checkForValue(ardfd, 'H')) goto startreading;
        read(ardfd, &spds[Rover5::BR], 2);
        if (checkForValue(ardfd, 'I')) goto startreading;

        read(ardfd, &endmicros, 4);
        if (checkForValue(ardfd, 'J')) goto startreading;
        // }}}


        if (usecurses) {
            static PrintBox stmicrosBox("Start Micros:");
            stmicrosBox.printf("%10u", stmicros);

            static PrintBox endmicrosBox("End Micros:");
            endmicrosBox.printf("%10u", endmicros);

            static PrintBox encdstsBox(2, 16, "Enc Dists:");
            encdstsBox.mvprintw(0,  0, "FL");
            encdstsBox.mvprintw(0,  2, "% 5d", encdsts[Rover5::FL]);
            encdstsBox.mvprintw(0, 14, "FR");
            encdstsBox.mvprintw(0,  8, "% 5d", encdsts[Rover5::FR]);
            encdstsBox.mvprintw(1,  0, "BL");
            encdstsBox.mvprintw(1,  2, "% 5d", encdsts[Rover5::BL]);
            encdstsBox.mvprintw(1, 14, "BR");
            encdstsBox.mvprintw(1,  8, "% 5d", encdsts[Rover5::BR]);

            static PrintBox powsBox(2, 15, "Motor Powers:");
            powsBox.mvprintw(0,  0, "FL");
            powsBox.mvprintw(0,  2, "% 4d", pows[Rover5::FL]);
            powsBox.mvprintw(0, 13, "FR");
            powsBox.mvprintw(0,  8, "% 4d", pows[Rover5::FR]);
            powsBox.mvprintw(1,  0, "BL");
            powsBox.mvprintw(1,  2, "% 4d", pows[Rover5::BL]);
            powsBox.mvprintw(1, 13, "BR");
            powsBox.mvprintw(1,  8, "% 4d", pows[Rover5::BR]);

            static PrintBox posBox(3, 13, "Reported Pos:");
            posBox.mvprintw(0,  2,   "x:");
            posBox.mvprintw(0,  4, "% 5d", xpos);
            posBox.mvprintw(1,  2,   "y:");
            posBox.mvprintw(1,  4, "% 5d", ypos);
            posBox.mvprintw(2,  0, "ang:");
            posBox.mvprintw(2,  4, "% 5u", ang);

            static PrintBox distBox(2, "Reported Dist:");
            distBox.mvprintw(0, 0, "x:");
            distBox.mvprintw(1, 0, "y:");
            distBox.mvprintw(0, 2, "% 5d", xdist);
            distBox.mvprintw(1, 2, "% 5d", ydist);

            static PrintBox spdsBox(2, 15, "Motor Speeds:");
            spdsBox.mvprintw(0,  0, "FL");
            spdsBox.mvprintw(0,  2, "% 5d", spds[Rover5::FL]);
            spdsBox.mvprintw(0, 13, "FR");
            spdsBox.mvprintw(0,  8, "% 5d", spds[Rover5::FR]);
            spdsBox.mvprintw(1,  0, "BL");
            spdsBox.mvprintw(1,  2, "% 5d", spds[Rover5::BL]);
            spdsBox.mvprintw(1, 13, "BR");
            spdsBox.mvprintw(1,  8, "% 5d", spds[Rover5::BR]);

            PrintBox::refreshAll();
        }
        else
        {
            printf(
                "%10u %4d %4d %4d %4d % 4d % 4d % 4d % 4d %4d %4d %5u %5d %5d %10u\n",
                stmicros,
                encdsts[Rover5::FL],
                encdsts[Rover5::FR],
                encdsts[Rover5::BL],
                encdsts[Rover5::BR],
                pows[Rover5::FL],
                pows[Rover5::FR],
                pows[Rover5::BL],
                pows[Rover5::BR],
                xpos,
                ypos,
                ang,
                xdist,
                ydist,
                endmicros
            );
        }
        if (usecurses) {
            runOffKB();
        }
        virtualBot.UpdateEncoders();
    }

    return 0;
}

void runOffKB() {
    static PrintBox chBox("Most recent character:");
    int ch = getch();
    if (ch != ERR) { // If something was actually typed
        chBox.printf("%4d", ch);
        static int xpwr = 0, ypwr = 0, zpwr = 0;
        switch (ch) {
        case 'w': ypwr = +128; break;
        case 'a': xpwr = -128; break;
        case 's': ypwr = -128; break;
        case 'd': xpwr = +128; break;
        case 'e': zpwr = +128; break;
        case 'q': zpwr = -128; break;
        default:  ypwr = xpwr = zpwr = 0; break;
        }
        virtualBot.Run(xpwr, ypwr, zpwr);
    }
}

bool checkForValue(int fd, const char w) {
    static unsigned int numerrs = 0;
    char c = 123;
    read(fd, &c, 1);
    if (c != w) {
        numerrs++;
        if (!usecurses) {
            cerr << "Exp: "
                << (int)w << " (" << (char)w << ")"
                << " act: "
                << (int)c << ". " << numerrs << " errs" <<  endl;
            //if (++numerrs >= 100) {
            //    cout << "There have been a LOT of wrong values. Exiting" << endl;
            //    exit(3);
            //}
        }
        return true;
    }
    if (usecurses) {
        static PrintBox errsBox("errs:");
        errsBox.printf("%5u", numerrs);
    }
    return false; // no error
}

int openPort(const char* ardpath) {
    int ardfd = open(ardpath, O_RDWR);
    if (ardfd == -1) {
        cout << "open on `" << ardpath << "' failed." << endl;
        exit(2);
    }

    char sttycmd[256] = "stty -F ";
    strcat(sttycmd, ardpath);
    strcat(sttycmd, " cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
    //cout << "Running system(" << sttycmd << ")" << endl;
    if (system(sttycmd)) {
        cerr << "system(" << sttycmd << ") failed" << endl;
        exit(2);
    }
    return ardfd;
}

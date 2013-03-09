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
#include "Utility.h"
#include "PrintBox.h"

bool checkForValue(int fd, char w);
int openPort(const char* ardpath);
void runOffKB();

bool usecurses = isatty(1); // 1 is stdout
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

        memcpy(virtualBot.ticks, encdsts, sizeof(encdsts));
        virtualBot.UpdateEncoders();

// {{{ Print data
        if (usecurses) {
            static PrintBox stmicrosBox("rStart Micros:");
            stmicrosBox.mvprintw(0,0,"%10u", stmicros);

            static PrintBox endmicrosBox("rEnd Micros:");
            endmicrosBox.mvprintw(0,0,"%10u", endmicros);

            static PrintBox encdstsBox(2, 16, "rEnc Dists:");
            encdstsBox.mvprintw(0,0,"FL % 5d% 5d FR", encdsts[Rover5::FL], encdsts[Rover5::FR]);
            encdstsBox.mvprintw(1,0,"BL % 5d% 5d BR", encdsts[Rover5::BL], encdsts[Rover5::BR]);

            static PrintBox powsBox(2, 15, "rMotor Powers:");
            powsBox.mvprintw(0,0,"FL % 4d% 4d FR", pows[Rover5::FL], pows[Rover5::FR]);
            powsBox.mvprintw(1,0,"BL % 4d% 4d BR", pows[Rover5::BL], pows[Rover5::BR]);

            static PrintBox posBox(3, 13, "rPos:");
            posBox.mvprintw(0,2,  "x: % 8d", xpos);
            posBox.mvprintw(1,2,  "x: % 8d", ypos);
            posBox.mvprintw(2,0,"ang: % 8u", ang);

            static PrintBox distBox(2, 9, "rDist:");
            distBox.mvprintw(0,0,"x: % 5d", xdist);
            distBox.mvprintw(1,0,"y: % 5d", ydist);

            static PrintBox spdsBox(2, 21, "rMotor Speeds:");
            spdsBox.mvprintw(0,0,"FL % 7d % 7d FR", spds[Rover5::FL], spds[Rover5::FR]);
            spdsBox.mvprintw(1,0,"BL % 7d % 7d BR", spds[Rover5::BL], spds[Rover5::BR]);

            double calcSpds[4];
            virtualBot.GetSpeeds(calcSpds);
            static PrintBox calcSpdsBox(2, 31, "Calc'd Speeds:");
            calcSpdsBox.mvprintw(0,0,"FL % 7.4f % 7.4f FR", calcSpds[Rover5::FL], calcSpds[Rover5::FR]);
            calcSpdsBox.mvprintw(1,0,"BL % 7.4f % 7.4f BR", calcSpds[Rover5::BL], calcSpds[Rover5::BR]);

            double calcxpos, calcypos, calcang;
            virtualBot.GetPos(&calcxpos, &calcypos, &calcang);
            static PrintBox calcPosBox(3, 20, "Calc'd Pos:");
            calcPosBox.mvprintw(0,2,  "x: % 8.7f", calcxpos);
            calcPosBox.mvprintw(1,2,  "y: % 8.7f", calcypos);
            calcPosBox.mvprintw(2,0,"ang: % 8.7f", calcang);

            PrintBox::refreshAll();
        }
        else
        {
            printf(
                "%10u %4d %4d %4d %4d % 4d % 4d % 4d % 4d %4d %4d %5u %5d %5d %10u\n",
                stmicros,
                encdsts[Rover5::FL], encdsts[Rover5::FR],
                encdsts[Rover5::BL], encdsts[Rover5::BR],
                pows[Rover5::FL], pows[Rover5::FR],
                pows[Rover5::BL], pows[Rover5::BR],
                xpos, ypos, ang,
                xdist, ydist,
                endmicros
            );
        }
// }}}

        if (usecurses) {
            runOffKB();
        }
    }

    return 0;
}

void runOffKB() {
    //static PrintBox chBox("Most recent character:");
    int ch = getch();
    if (ch != ERR) { // If something was actually typed
        //chBox.mvprintw(0,0,"%4d", ch);
        static int xpwr = 0, ypwr = 0, zpwr = 0;
        switch (ch) {
        case 'w': ypwr = +255; break;
        case 'a': xpwr = -255; break;
        case 's': ypwr = -255; break;
        case 'd': xpwr = +255; break;
        case 'e': zpwr = +255; break;
        case 'q': zpwr = -255; break;
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
    if (system(sttycmd)) {
        cerr << "system(" << sttycmd << ") failed" << endl;
        exit(2);
    }
    return ardfd;
}

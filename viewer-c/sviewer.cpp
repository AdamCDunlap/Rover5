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

//#define USE_CURSES

#ifdef USE_CURSES
#include <curses.h>
bool usecurses = isatty(1); // stdout
#else
bool usecurses = false;
#endif //USE_CURSES

// stty the port
// read from the port
// parse data
// display data
// ?do math

bool checkForValue(int fd, char w);
int openPort(const char* ardpath);

namespace Rover5 {
    enum mtrNum_t { BR = 0, FR = 1, BL = 2, FL = 3 };
}
// {{{ WinClass
#ifdef USE_CURSES
class Win {
public:
    Win() {
        if (usecurses) {
            initscr();
            curs_set(0);

            int maxy, maxx;
            getmaxyx(stdscr, maxy, maxx);

            winds[Win::stmicros] = newwin(4, 15, 0, 0);
            mvwaddstr(winds[Win::stmicros], 1, 1, "Start Micros:");

            winds[Win::endmicros] = newwin(4, 15, 4, 0);
            mvwaddstr(winds[Win::endmicros], 1, 1, "End Micros:");

            winds[Win::encdsts] = newwin(7, 15, 0, 40);
            mvwaddstr(winds[Win::encdsts], 1, 1, "Enc Dists:");
            mvwaddstr(winds[Win::encdsts], 2, 1, "FR");
            mvwaddstr(winds[Win::encdsts], 3, 1, "FL");
            mvwaddstr(winds[Win::encdsts], 4, 1, "BR");
            mvwaddstr(winds[Win::encdsts], 5, 1, "BL");

            winds[Win::pows] = newwin(7, 15, 0, 60);
            mvwaddstr(winds[Win::pows], 1, 1, "Motor Powers:");
            mvwaddstr(winds[Win::pows], 2, 1, "FR");
            mvwaddstr(winds[Win::pows], 3, 1, "FL");
            mvwaddstr(winds[Win::pows], 4, 1, "BR");
            mvwaddstr(winds[Win::pows], 5, 1, "BL");

            winds[Win::pos] = newwin(7, 15, 10, 0);
            mvwaddstr(winds[Win::pos], 1, 1, "Reported Pos:");
            mvwaddstr(winds[Win::pos], 2, 1, "  X");
            mvwaddstr(winds[Win::pos], 3, 1, "  Y");
            mvwaddstr(winds[Win::pos], 4, 1, "ang");

            winds[Win::errs] = newwin(4, 10, maxy-4, maxx-10);
            mvwaddstr(winds[Win::errs], 1, 1, "Errors:");

            for (size_t i=0; i<n; i++) {
                box(winds[i], 0, 0);
                wrefresh(winds[i]);
            }
        }
    }

    ~Win() {
        if (usecurses) {
            for (size_t i=0; i<n; i++) {
                delwin(winds[i]);
            }
            endwin();
        }
    }

    enum names {stmicros, endmicros, encdsts, pows, pos, errs, n};
    WINDOW* operator[](size_t i) { return winds[i]; }
private:
    WINDOW* winds[n];
} win;
#endif // USE_CURSES
// }}}


//{{{ From Rover5.h
//#define TWO_PI 6.283185307179586476925286766559
///************************ TAKEN FROM ROVER5.H**************************/
//
///// Minimum power for a motor to move
//const int minPower = 25;
///// The radius of the wheels in mills
//const int wheelRadius = 2000;
//
///// Factor to multiple ticks by to get mills
//// 2 pi * radius = circumference
//// there are 3 rotations of the wheel per 1000 ticks [rover5 manual]
//// Ends up being 37.699111843077518861551720599354034610366032792501269
//const double ticksToMills = (TWO_PI * wheelRadius * 3.0)/1000.0;
////const double ticksToMills = 1;
//
///// Current number of encoder ticks for each motor
//long ticks[4];
//
///// Current speeds for each motor in encoder ticks/second
///// Maximum realistic value is about 25000
//int speeds[4];
//
///// Current powers for each motor from -255 to 255
//int powers[4];
//
///// Current position of the robot, obtained from encoder data, in ticks
/////  and milliradians for angle
////struct { long x; long y; unsigned int angle; } pos;
//struct Position { float x; long y; float angle; } pos;
//
//// Circular buffer class holding the last ten tick counts and times
//template <uint8_t bufsz> struct TickLogs {
//    long ticks[bufsz][4];
//    unsigned long times[bufsz];
//    uint8_t nextEntry;
//
//    TickLogs() : nextEntry(0) {}
//
//    void Put(long iticks[4], unsigned long time) {
//        memcpy(ticks[nextEntry], iticks, sizeof(ticks[0]));
//        times[nextEntry] = time;
//
//        nextEntry = (nextEntry >= bufsz-1)? 0 : nextEntry+1;
//    }
//};
//
//const uint8_t spdLogLen = 10;
////const uint8_t spdLogLen = 1;
//TickLogs<spdLogLen> tickLogs;
//
///************************ END TAKEN FROM ROVER5.H**************************/
//}}}

int main(int argc, char* argv[]) {

// Set up serial port
    const char* ardpath;
    if (argc >= 2) ardpath = argv[1];
    else ardpath = "/dev/ttyACM0";

    int ardfd = openPort(ardpath);

    while (true) {

        // Give these weird starting values so I'll know if they weren't
        //  changed
        uint32_t stmicros   = 1234567890;
        uint32_t endmicros  = 1234567890;
        int32_t  encdsts[4] = {1234567890, 1234567890, 1234567890, 1234567890};
        int16_t  pows[4]    = {12345,12345,12345,12345};
        int32_t  xpos = 1234567890, ypos = 1234567890;
        uint16_t ang = 12345;


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

        read(ardfd, &endmicros, 4);
        if (checkForValue(ardfd, 'D')) goto startreading;
        // }}}


// {{{ From Rover5.cpp
///************************ TAKEN FROM ROVER5.CPP**************************/
//
//    // Variables used for integrating/dervitizing
//    unsigned long curTime = stmicros;
//    static unsigned long lastTime = curTime;
//    unsigned int timeDiff = curTime - lastTime;
//    lastTime = curTime;
//    
//    // "n" function to get the current rotational velocity
//
//    // use K as defined in Ether's paper
//    // "Kinematic Analysis of Four-Wheel Mecanum Vehicle"
//    // http://www.chiefdelphi.com/media/papers/download/2722
//
//    // 6.75 is wheel base in inches, 8.5 is track witdth
//    // Factor of (1000/ticksToMills) on each one is to convert to ticks
//    // dividing by 1000 makes it into milliradians
//    // Units: ticks/milliradian
//    // Ends up being 0.8090376273838012901584924638102813403418365325139869
//    const double K = (((6.75*(1000/ticksToMills))/2 + (8.5*(1000/ticksToMills))/2) * 4)/1000;
//    
//    // no need to integrate the speeds when the distances are there
//    //
//    //long angVel = (+speeds[FL] -speeds[FR] +speeds[BL] -speeds[BR])/K;
//    // Integrate angVel to get the angle
//    // Devide by 1000000 to convert from microseconds to seconds
//    //pos.angle += (angVel * (currentTime - lastTime)) / 1000000;
//    
//    pos.angle = ticksToMills * (double)(+ticks[Rover5::FL] -ticks[Rover5::FR] +ticks[Rover5::BL] -ticks[Rover5::BR])/K;
//
//    
//    // "r" function to get the field relative velocity and rotational velocity
//    
//    // the postfix r means it's robot relative
//    // These are in ticks/second
//    int xvelr = (+(long)speeds[Rover5::FL] -(long)speeds[Rover5::FR] -(long)speeds[Rover5::BL] +(long)speeds[Rover5::BR])/4;
//    int yvelr = (+(long)speeds[Rover5::FL] +(long)speeds[Rover5::FR] +(long)speeds[Rover5::BL] +(long)speeds[Rover5::BR])/4;
//
//    // Now rotate the vector
//    float sinA = sin(pos.angle/1000.0);
//    float cosA = cos(pos.angle/1000.0);
//    float xvel = /*(int)*/(xvelr * cosA - yvelr * sinA);
//    float yvel = /*(int)*/(xvelr * sinA + yvelr * cosA);
//
//    // max val of xvel is 25000
//    // ((2^32)-1)/25000 = 172,000, which would mean if timeDiff is more than
//    // 172 milliseconds, it would overflow. So first divide timediff by 10
//    // (not much is lost since micros() only has a precision of 4) so that
//    // a full second and half can go by without overflow. More than that and
//    // the user deserves to get wrong answers.
//    //pos.x  += (((long)xvel * (long)(timeDiff/10))/100000);
//    //pos.y  += (((long)yvel * (long)(timeDiff/10))/100000);
//    pos.x  += (((float)xvel * (float)(timeDiff/10))/100000);
//    pos.y  += (((float)yvel * (float)(timeDiff/10))/100000);
//
//    //printf_P(PSTR("xvelr%5d yvelr%5d sinA %.3f xvel %.3f yvel %.3f pos.x % f pos.y % f pos.angle % f enc[FL] %ld enc[FR] %ld enc[BL] %ld enc[BR] %ld\r\n"),
//    //                  xvelr,   yvelr,     sinA,     xvel,     yvel,    pos.x,    pos.y,    pos.angle,  ticks[FL],  ticks[FR],  ticks[BL],  ticks[BR]);
//
//
///************************ TAKEN FROM ROVER5.CPP**************************/
// }}}


        #ifdef USE_CURSES
        if (usecurses) {
            mvwprintw(win[Win::stmicros], 2, 1, "%10u", stmicros);
            wrefresh(win[Win::stmicros]);

            mvwprintw(win[Win::endmicros], 2, 1, "%10u", endmicros);
            wrefresh(win[Win::endmicros]);

            mvwprintw(win[Win::encdsts], 2, 4, "% 5d", encdsts[Rover5::FL]);
            mvwprintw(win[Win::encdsts], 3, 4, "% 5d", encdsts[Rover5::FR]);
            mvwprintw(win[Win::encdsts], 4, 4, "% 5d", encdsts[Rover5::BL]);
            mvwprintw(win[Win::encdsts], 5, 4, "% 5d", encdsts[Rover5::BR]);
            wrefresh(win[Win::encdsts]);

            mvwprintw(win[Win::pows], 2, 4, "% 4d", pows[Rover5::FL]);
            mvwprintw(win[Win::pows], 3, 4, "% 4d", pows[Rover5::FR]);
            mvwprintw(win[Win::pows], 4, 4, "% 4d", pows[Rover5::BL]);
            mvwprintw(win[Win::pows], 5, 4, "% 4d", pows[Rover5::BR]);
            wrefresh(win[Win::pows]);

            mvwprintw(win[Win::pos], 2, 5, "% 5d", xpos);
            mvwprintw(win[Win::pos], 3, 5, "% 5d", ypos);
            mvwprintw(win[Win::pos], 4, 5, "% 5u", ang);
            wrefresh(win[Win::pos]);
            //enum names {stmicros, endmicros, encdsts, pows, pos, errs, n};
        }
        else
        #endif //USE_CURSES
        {
            printf(
                "%10u %4d %4d %4d %4d % 4d % 4d % 4d % 4d %4d %4d %5u %10u\n",
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
                endmicros
            );
        }
    }

    return 0;
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
    #ifdef USE_CURSES
    if (usecurses) {
        mvwprintw(win[Win::errs], 2, 1, "%8u", numerrs);
        wrefresh(win[Win::errs]);
    }
    #endif // USE_CURSES
    return false; // no error
}

int openPort(const char* ardpath) {
    int ardfd = open(ardpath, O_RDONLY);
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

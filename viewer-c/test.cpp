#include <iostream>
#include <fstream>
#include <stdint.h>
using namespace std;

int main(int argc, char* argv[]) {
    const char* ardpath;
    if (argc >= 2) ardpath = argv[1];
    else ardpath = "/dev/ttyACM0";

    ifstream ardport(ardpath, ifstream::in | ifstream::binary);
    ofstream binfile("binfile", ofstream::out | ofstream::binary | ofstream::trunc);
    ofstream logfile("logfile", ofstream::out | ofstream::trunc);

    while (ardport) {
        char c = 123;
        ardport.read(&c, 1);
        logfile << ardport.gcount();
        binfile << (char)c;
        
    }
}

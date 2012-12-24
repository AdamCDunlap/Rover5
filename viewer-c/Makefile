
CFLAGS = -Wall -Wextra
CPPFLAGS = $(CFLAGS) -std=gnu++11

viewer: sviewer.cpp Rover5.o Rover5.h Utility.o Utility.h PrintBox.o PrintBox.h 
	g++  $(CPPFLAGS)  sviewer.cpp -o viewer -lncurses Rover5.o Utility.o PrintBox.o

Rover5.o: Rover5.h Rover5.cpp
Utility.o: Utility.h Utility.cpp
PrintBox.o: PrintBox.h PrintBox.cpp

clean:
	rm -f Rover5.o Utility.o viewer

.PHONY: clean

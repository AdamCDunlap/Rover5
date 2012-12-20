
CFLAGS = -Wall -Wextra
CPPFLAGS = $(CFLAGS) -std=gnu++11

viewer: sviewer.cpp Rover5.o Rover5.h Utility.o Utility.h Win.o Win.h
	g++ $(CPPFLAGS)  sviewer.cpp -o viewer -lncurses Rover5.o Utility.o Win.o

clean:
	rm -f Rover5.o Utility.o Win.o viewer

.PHONY: clean

#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	initscr();

	hline(ACS_ULCORNER, 5);
	vline(ACS_LLCORNER, 5);

	getch();

	endwin();

	return 0;
}


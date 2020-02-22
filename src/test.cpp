#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	//Box root = { "Test F (Test 5++)" };
	initscr();

	for (auto i = 0; i < 30; ++i) {
		mvhline(i, 0, ACS_HLINE, i + 1);
	}
	refresh();

	getch();

	endwin();
	return 0;
}


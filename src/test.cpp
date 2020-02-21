#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	Box root = { "Test 4" };

	hline(ACS_ULCORNER, 5);
	vline(ACS_LLCORNER, 5);

	refresh();

	getch();

	return 0;
}


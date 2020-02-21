#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	Box root = { "Test 9" };

	mvhline(2, 1, 'C', root.width()  - 2);
	root.move(1, 1).horz_line(root.width() - 2, 'C');

	mvaddch(5, 5, ACS_HLINE);
	mvaddch(6, 6, ACS_VLINE);

	refresh();

	root.get_char();

	return 0;
}


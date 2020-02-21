#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	Box root = { "Test 9" };

	root.move(1, 1).horz_line(root.width()  - 2, ACS_HLINE);
	root.move(1, 2).vert_line(root.height() - 3, ACS_VLINE);

	mvaddch(5, 5, ACS_HLINE);
	mvaddch(6, 6, ACS_VLINE);

	root.refresh();

	root.get_char();

	return 0;
}


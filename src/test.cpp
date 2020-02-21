#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	Box root = { "Test 4" };

	root.move(1, 1).horz_line(5, ACS_ULCORNER);
	root.move(5, 5).vert_line(5, ACS_LLCORNER);

	root.refresh();

	root.get_char();

	return 0;
}


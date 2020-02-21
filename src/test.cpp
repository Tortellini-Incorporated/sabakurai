#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	Box root = { "Test 4" };

	root.horz_line(5, ACS_ULCORNER);
	root.vert_line(5, ACS_LLCORNER);

	root.refresh();

	root.get_char();

	return 0;
}


#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	Box root = { "Test F (Test 5++)" };

	root.draw().refresh();

	root.get_char();

	return 0;
}


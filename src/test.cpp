#include <ncurses.h>
#include <fstream>

#include "box.hpp"

std::ofstream file("debug-test.log");

int main() {
	Box root = { "" };

	root.draw().refresh();
	root.get_char();

	return 0;
}


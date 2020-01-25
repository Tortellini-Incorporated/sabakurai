#include <cstdint>
#include <ncurses.h>
#include <iostream>

#include "box.hpp"

int32_t main(int32_t argc, char ** argv) {
	Box root = {};
	root.block(true);

	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	root.draw().refresh();
	
	Box sub = { root, 1, 1, 32, 16 };
	sub.draw().refresh();

	root.move(1, 1).get_char();

	return 0;
}

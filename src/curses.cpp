#include <cstdint>
#include <ncurses.h>
#include <iostream>
#include <fstream>

#include "box.hpp"
#include "split.hpp"

std::ofstream file("debug.log");

int32_t main(int32_t argc, char ** argv) {
	Box root = { "Root Window" };
	root.block(true);
	file << root.width() << std::endl;

	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	root.draw().refresh();
	
	Split split = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return 0.5 * width;
		},
		Split::VERT
	};
	file << "yes" << std::endl;
	root.set_child(&split);
	file << "wat" << std::endl;

	Box left  = { root, "Left Split" };
	Box right = { root, "Right Split" };
	file << "left width: " << left.width() << std::endl;
	split.set_children(&left, &right);

	root.get_char();

	return 0;
}

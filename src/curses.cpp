#include <cstdint>
#include <ncurses.h>
#include <iostream>
#include <fstream>

#include "box.hpp"
#include "split.hpp"
#include "title.hpp"

int32_t main(int32_t argc, char ** argv) {
	Box root = { "" };

	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	root.draw().refresh();
	
	Split split = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return height < 5 ? height : 5;
		},
		Split::HORZ
	};
	root.set_child(&split);

	Title title = { root, true };
	Box main = { root, "" };
	split.set_children(&title, &main);

	root.get_char();

	return 0;
}

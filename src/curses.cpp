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
	
	Split split = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return height < 7 ? height : 7;
		},
		Split::HORZ
	};

	Box titleBox = { root, "" };
	Title title = { root, true };
	titleBox.set_child(&title);

	Box logBox = { root, "Log" };
	Box playerBox = { root, "Players" };
	Box commandBox = { root, "Command" };

	Split split2 = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return height * (3.0 / 4.0);
		},
		Split::HORZ
	};
	Split split3 = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return width * 0.8;
		},
		Split::VERT
	};
	split3.set_children(&logBox, &playerBox);
	split2.set_children(&split3, &commandBox);
	split.set_children(&titleBox, &split2);
	root.set_child(&split);

	root.draw().refresh();

	root.get_char();

	return 0;
}

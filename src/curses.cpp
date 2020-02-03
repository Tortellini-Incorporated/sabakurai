#include <cstdint>
#include <ncurses.h>
#include <iostream>
#include <fstream>

#include "box.hpp"
#include "split.hpp"
#include "title.hpp"
#include "player_list.hpp"
#include "log.hpp"

std::ofstream file("debug.log");

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

	Box playerBox = { root, "Players" };
	PlayerList players = { root, true };
	playerBox.set_child(&players);

	Box logBox = { root, "Log" };
	Log log = { root, true };
	logBox.set_child(&log);

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

	players.add_player(0, 0, "C1FR1");
	players.add_player(1, 0, "Gnarwhal");
	players.add_player(2, 0, "Garou");
	players.add_player(3, 0, "Hello World!");

	players.draw().refresh();

	root.get_char();

	players.remove_player(2);
	players.remove_player(0);

	players.draw().refresh();

	root.get_char();

	return 0;
}

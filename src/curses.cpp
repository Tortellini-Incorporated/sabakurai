#include <cstdint>
#include <ncurses.h>

#include "window.hpp"

int32_t main(int32_t argc, char ** argv) {
	auto screen = Window::init();

	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	refresh();

	Window window(0, 0, COLS, LINES);
	window.refresh();

	for (auto i = 0; i < 10; ++i) {
		getch();
		window.move(0, 1 + i)
		      .print("Hello World! %d", 32)
			  .add_char(ACS_RTEE)
			  .refresh();
	}

	getch();

	return 0;
}

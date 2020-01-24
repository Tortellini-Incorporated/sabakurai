#include <cstdint>
#include <ncurses.h>

#include "window.hpp"

int32_t main(int32_t argc, char ** argv) {
	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	refresh();

	Window window(0, 0, COLS, LINES);
	window.box("Hello World!");
	window.refresh();

	for (auto i = 0; i < 10; ++i) {
		getch();
		window.move(0, 1 + i)
		      .put(ACS_LTEE)
		      .line_horz(window.width() - 2, ACS_HLINE)
			  .move(window.width() - 1, 1 + i)
		      .put(ACS_RTEE)
			  .refresh();
	}

	getch();

	return 0;
}

#include <ncurses.h>

int main() {
	initscr();
	raw();
	noecho();
	keypad(stdscr, true);

	auto c = uint32_t{ 0 };
	while (c != 'q') {
		printw("0x%x\n", c = getch());
	}

	endwin();

	return 0;
}

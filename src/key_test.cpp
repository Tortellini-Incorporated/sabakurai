#include <ncurses.h>

int main() {
	initscr();
	raw();
	noecho();
	keypad(stdscr, true);

	auto c = uint32_t{ 0 };
	while (c != 'q') {
		c = getch();
		printw("Key Name: %s - 0x%x - %d\n", keyname(c), c, c);
	}

	endwin();

	return 0;
}

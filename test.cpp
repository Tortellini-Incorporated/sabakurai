#include <ncurses.h>

int main() {
	initscr();

	addch(ACS_VLINE);
	addch(ACS_HLINE);
	addch(ACS_LLCORNER);
	addch(ACS_ULCORNER);
	addch(ACS_LRCORNER);
	addch(ACS_URCORNER);

	getch();

	endwin();
}

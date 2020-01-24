#include "window.hpp"

Window Window::Internal::root = Window();

Window::Window() {
	initscr();
	internal.x = 0;
	internal.y = 0;
	internal.width  = COLS;
	internal.height = LINES;
	internal.window = stdscr;
}

auto Window::root() -> Window& {
	return Internal::root;
}

Window::Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	internal{
		x, y, width, height, ::newwin(height, width, y, x)
	} {}

Window::Window(Window && window) :
	internal{window.internal} {
	window.internal = { 0 };
}

auto Window::operator=(Window && window) -> Window& {
	internal = window.internal;
	window.internal = { 0 };
	return *this;
}

auto Window::box() -> Window& {
	box(0, 0, internal.width, internal.height);
	return *this;
}

auto Window::box(const std::string & name) -> Window& {
	box(0, 0, internal.width, internal.height);
	mvwprintw(internal.window, 0, 2, " %s ", name.c_str());
	return *this;
}

auto Window::box(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> Window& {
	mvwaddch(internal.window, y,              x,             ACS_ULCORNER);
	mvwaddch(internal.window, y + height - 1, x,             ACS_LLCORNER);
	mvwaddch(internal.window, y,              x + width - 1, ACS_URCORNER);
	mvwaddch(internal.window, y + height - 1, x + width - 1, ACS_LRCORNER);
	mvwhline(internal.window, y,              x + 1, ACS_HLINE, width  - 2);
	mvwhline(internal.window, y + height - 1, x + 1, ACS_HLINE, width  - 2);
	mvwvline(internal.window, y + 1, x,              ACS_VLINE, height - 2);
	mvwvline(internal.window, y + 1, x + width - 1,  ACS_VLINE, height - 2);
	return *this;
}

auto Window::box(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const std::string & name) -> Window& {
	box(x, y, width, height);
	mvwprintw(internal.window, y, x + 2, " %s ", name.c_str());
	return *this;
}

auto Window::unbox() -> Window& {
	unbox(0, 0, internal.width, internal.height);
	return *this;
}

auto Window::unbox(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> Window& {
	mvwaddch(internal.window, y,              x,             ' ');
	mvwaddch(internal.window, y + height - 1, x,             ' ');
	mvwaddch(internal.window, y,              x + width - 1, ' ');
	mvwaddch(internal.window, y + height - 1, x + width - 1, ' ');
	mvwhline(internal.window, y,              x + 1, ' ', width  - 2);
	mvwhline(internal.window, y + height - 1, x + 1, ' ', width  - 2);
	mvwvline(internal.window, y + 1, x,              ' ', height - 2);
	mvwvline(internal.window, y + 1, x + width - 1,  ' ', height - 2);
	return *this;
}

auto Window::clear() -> Window& {
	for (auto i = uint32_t{ 0 }; i < internal.width; ++i) {
		mvwvline(internal.window, 0, i, ' ', internal.height);
	}
	return *this;
}

auto Window::move(uint32_t x, uint32_t y) -> Window& {
	wmove(internal.window, y, x);
	return *this;
}

auto Window::put(uint32_t c) -> Window& {
	waddch(internal.window, c);
	return *this;
}

auto Window::line_horz(uint32_t width, uint32_t c) -> Window& {
	whline(internal.window, c, width);
	return *this;
}

auto Window::line_vert(uint32_t height, uint32_t c) -> Window& {
	wvline(internal.window, c, height);
	return *this;
}

auto Window::refresh() -> Window& {
	wrefresh(internal.window);
	return *this;
}

auto Window::x() -> uint32_t {
	return internal.x;
}

auto Window::y() -> uint32_t {
	return internal.y;
}

auto Window::width() -> uint32_t {
	return internal.width;
}

auto Window::height() -> uint32_t {
	return internal.height;
}

Window::~Window() {
	if (internal.window != stdscr) {
		clear();
		refresh();
		if (internal.window) {
			delwin(internal.window);
		}
	} else {
		endwin();
	}
}

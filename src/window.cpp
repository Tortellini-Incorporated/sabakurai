#include "window.hpp"

Window::Window() {
	internal.x      = 0;
	internal.y      = 0;
	internal.width  = COLS;
	internal.height = LINES;
	internal.window = stdscr;
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

auto Window::move(uint32_t x, uint32_t y) -> Window& {
	wmove(internal.window, y, x);
	return *this;
}

auto Window::add_char(uint32_t c) -> Window& {
	waddch(internal.window, c);
	return *this;
}

auto Window::print(const char * format, ...) -> Window& {
	std::va_list args;
	va_start(args, format);
	vwprintw(internal.window, format, args);
	va_end(args);
	return *this;
}

auto Window::clear() -> Window& {
	for (auto i = uint32_t{ 0 }; i < internal.width; ++i) {
		mvwvline(internal.window, 0, i, ' ', internal.height);
	}
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

auto Window::window() -> WINDOW* {
	return internal.window;
}

Window::~Window() {
	clear();
	refresh();
	if (internal.window) {
		delwin(internal.window);
	}
}

auto RootWindow::getch() -> void {
	auto ret = c;
	c = getch();
	if (c == KEY_RESIZE) {
		Window::internal.component.rezize();
		c = getch();
	}
	return ret;
}

RootWindow::RootWindow() {
	initscr();
	Window::internal.x      = 0;
	Window::internal.y      = 0;
	Window::internal.width  = COLS;
	Window::internal.height = LINES;
	Window::internal.window = stdscr;
}

void RootWindow::block(bool value) -> void {
	nedelay(Window::internal.window, !value);
}

RootWindow::~RootWindow() {
	endwin();
}

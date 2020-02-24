#include <fstream>

#include "window.hpp"

extern std::ofstream file;

auto Window::root_get_char(const Window & window) -> uint32_t {
	auto c = ::wgetch(window.internal.window);
	file << "Key Name: " << keyname(c) << ", " << c << std::endl;
	while (c == KEY_RESIZE) {
		internal = { 0, 0, uint32_t( COLS ), uint32_t( LINES ), stdscr, internal.root };
		component_resize();
		clear().draw().refresh();
		c = ::wgetch(window.internal.window);
	}
	return c;
}

auto Window::window_resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void {
	if (internal.window != 0) {
		::delwin(internal.window);
	}
	internal = {
		x, y, width, height, ::newwin(height, width, y, x), internal.root
	};
	keypad(internal.window, TRUE);
	component_resize();
}

auto Window::component_resize() -> void {}

Window::Window() {
	initscr();
	internal = {
		0, 0, uint32_t( COLS ), uint32_t( LINES ), stdscr, this
	};
	keypad(internal.window, TRUE);
}

Window::Window(Window & root, bool dummy) :
	internal{ 0, 0, 0, 0, 0, &root } {}

Window::Window(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	internal{
		x, y, width, height, ::newwin(height, width, y, x), &root
	} {		
	keypad(internal.window, TRUE);
}

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
	::waddch(internal.window, c);
	return *this;
}

auto Window::print(const char * format, ...) -> Window& {
	std::va_list args;
	va_start(args, format);
	::vwprintw(internal.window, format, args);
	va_end(args);
	return *this;
}

auto Window::horz_line(uint32_t width, uint32_t c) -> Window& {
	whline(internal.window, c, width);
	return *this;
}

auto Window::vert_line(uint32_t height, uint32_t c) -> Window& {
	wvline(internal.window, c, height);
	return *this;
}

auto Window::draw() -> Window& {
	return *this;
}

auto Window::get_char() -> uint32_t {
	return internal.root->root_get_char(*this);
}

auto Window::clear() -> Window& {
	for (auto i = uint32_t{ 0 }; i < internal.height; ++i) {
		mvwhline(internal.window, i, 0, ' ', internal.width);
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

auto Window::block(bool value) -> void {
	::nodelay(internal.window, !value);
}

Window::~Window() {
	if (internal.window != stdscr) {
		clear();
		refresh();
		delwin(internal.window);
	} else {
		endwin();
	}
}


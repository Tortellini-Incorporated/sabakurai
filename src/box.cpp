#include "box.hpp"

auto Box::component_resize() -> void {
	draw().refresh();
}

Box::Box() :
	Window() {}

Box::Box(Window & window, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(window, x, y, width, height) {}

auto Box::draw() -> Window& {
	move(0,                  0                  ).add_char(ACS_ULCORNER);
	move(internal.width - 1, 0                  ).add_char(ACS_URCORNER);
	move(0,                  internal.height - 1).add_char(ACS_LLCORNER);
	move(internal.width - 1, internal.height - 1).add_char(ACS_LRCORNER);
	move(1,                  0                  ).horz_line(internal.width  - 2, ACS_HLINE);
	move(1,                  internal.height - 1).horz_line(internal.width  - 2, ACS_HLINE);
	move(0,                  1                  ).vert_line(internal.height - 2, ACS_VLINE);
	move(internal.width - 1, 1                  ).vert_line(internal.height - 2, ACS_VLINE);
	return *this;
}

Box::~Box() {}

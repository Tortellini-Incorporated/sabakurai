#include "title.hpp"

const std::string Title::TITLE[] = {
	" ____________________          __ ____ _________________",
	"/ ___/ __ / __ / __ /   ___   / // / // / __ / __ /_  _/",
	"\\___ \\    \\ __ \\    \\  \\___\\  \\   <\\ \\\\ \\   <\\    \\_\\ \\_",
	"/____/_//_/____/_//_/         /_//_/____/_//_/_//_/____/"
};

Title::Title() :
	Window() {}

Title::Title(Window & root, bool dummy) :
	Window(root, true) {}

Title::Title(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, x, y, width, height) {}

auto Title::draw() -> Window& {
	auto min_height = uint32_t{ 4 };
	if (min_height > internal.height) {
		min_height = internal.height;
	}
	if (internal.width > 6) {
		move(0, internal.height >> 1).horz_line(4, '\\');
		if (TITLE[0].size() + 6 > internal.width) {
			for (auto i = 0; i < min_height; ++i) {
				move(6, i).print("%s", TITLE[i].substr(0, internal.width - 6).c_str());
			}
		} else {
			for (auto i = 0; i < min_height; ++i) {
				move(6, i).print("%s", TITLE[i].c_str());
			}
			if (internal.width > TITLE[0].size() + 8) {
				move(TITLE[0].size() + 8, internal.height >> 1).horz_line(internal.width - TITLE[0].size() - 8, '\\');
			}
		}
	} else {
		move(0, internal.height >> 1).horz_line(internal.width, '\\');
	}
	return *this;
}

Title::~Title() {}

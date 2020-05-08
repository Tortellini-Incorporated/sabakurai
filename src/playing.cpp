#include <fstream>

#include "playing.hpp"

extern std::ofstream file;

Playing::Playing() :
	Window(),
	players() {}

Playing::Playing(Window & root) :
	Window(root, true),
	players() {}

Playing::Playing(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, x, y, width, height),
	players() {}

auto Playing::set(const std::string & text, const std::string & name, uint32_t color) -> void {
	this->text  = text;
	this->name  = name;
	this->color = color;
	status    = PARTAKER;
	correct   = 0;
	incorrect = 0;
}

auto Playing::get_players() -> std::vector<Player>& {
	return players;
}

auto Playing::draw() -> Window& {
	move(0, 0).print("%s", text.c_str());
	move(0, 1).print("%s", name.c_str());
}

Playing::~Playing() {}

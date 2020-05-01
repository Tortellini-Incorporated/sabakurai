#include <fstream>

extern std::ofstream file;

Playing::Playing(PlayerList & players) :
	Window(),
	players(&players) {}

Playing::Playing(Window & root, PlayerList & players) :
	Window(root, true),
	offset(0),
	players(&players) {}

Playing::Playing(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height, PlayerList & players) :
	Window(root, x, y, width, height),
	players(&players) {}

auto Playing::update() -> bool {

}

auto Playing::draw() -> Window& {

}

Playing::~Playing() {}

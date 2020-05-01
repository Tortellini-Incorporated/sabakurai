#include <random>
#include <ctime>
#include <fstream>

#include "player_list.hpp"

extern std::ofstream file;

auto PlayerList::component_resize() -> void {
	if (players.size() - offset < internal.height) {
		if (players.size() <= internal.height) {
			offset = 0;
		} else {
			offset = players.size() - internal.height;
		}
	}
}

auto PlayerList::find_player(uint32_t id) -> uint32_t {
	auto index = uint32_t( -1 );
	while (players[++index].id != id);
	return index;
}

PlayerList::PlayerList() :
	Window(),
	offset(0),
	players(0) {

	auto random = std::default_random_engine{ (long unsigned int) time(0) };
	auto dist   = std::uniform_int_distribution<char>( 0, 9 );

	auto name = std::string("Entity ");
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));

	players.push_back({ 0, 0, false, false, name });
}

PlayerList::PlayerList(Window & root, bool dummy) :
	Window(root, true),
	offset(0),
	players(0) {
	
	auto random = std::default_random_engine{ (long unsigned int) time(0) };
	auto dist   = std::uniform_int_distribution<char>( 0, 9 );

	auto name = std::string("Entity ");
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));

	players.push_back({ 0, 0, false, false, name });
}

PlayerList::PlayerList(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, x, y, width, height),
	offset(0),
	players(0) {
	
	auto random = std::default_random_engine{ (long unsigned int) time(0) };
	auto dist   = std::uniform_int_distribution<char>( 0, 9 );

	auto name = std::string("Entity ");
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));
	name.append(1, '0' + dist(random));
	
	players.push_back({ 0, 0, false, false, name });
}

auto PlayerList::add_player(uint32_t id, uint32_t color, bool ready, bool spectator, std::string name) -> void {
	players.push_back({ id, color, ready, spectator, name });
}

auto PlayerList::get_player(uint32_t id) -> Player& {
	return players[find_player(id)];
}

auto PlayerList::get_player_index(uint32_t index) -> Player& {
	return players[index];
}

auto PlayerList::get_self() -> Player& {
	return players[0];
}

auto PlayerList::remove_player(uint32_t id) -> void {
	players.erase(players.begin() + find_player(id));
}

auto PlayerList::clear_list() -> void {
	players.erase(players.begin() + 1, players.end());
	players[0].ready = false;
}

auto PlayerList::length() -> size_t {
	return players.size();
}

auto PlayerList::draw() -> Window& {
	if (internal.width > 0) {
		for (auto i = 0; i < internal.height; ++i) {
			move(0, i).horz_line(internal.width, ' ');
		}
		auto max = players.size() - offset;
		if (max > internal.height) {
			max = internal.height;
		}
		for (auto i = 0; i < max; ++i) {
			auto & player = players[offset + i];
			auto print_name = std::string();
			if (player.spectate) {
				print_name.append("   ");
			} else if (player.ready) {
				print_name.append("[*]");
			} else {
				print_name.append("[ ]");
			}
			print_name.append(" ");
			if (i == 0) {
				print_name.append("> ");
			} else {
				print_name.append("  ");
			}
			print_name.append(player.name);
			if (internal.width == 3) {
				move(1, i).print(".");
			} else if (internal.width == 4) {
				move(1, i).print("..");
			} else if (print_name.size() > internal.width) {
				move(1, i).print("%s...", print_name.substr(0, internal.width - 4).c_str());
			} else if (internal.width >= 3) {
				move(1, i).print("%s", print_name.c_str());
			}
		}
	}
	return *this;
}

PlayerList::~PlayerList() {}

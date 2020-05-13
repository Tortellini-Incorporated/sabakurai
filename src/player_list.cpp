#include <random>
#include <ctime>

#include "player_list.hpp"

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
			auto print_prefix = std::string();
			if (player.spectate) {
				print_prefix.append("SPC");
			} else if (player.ready) {
				print_prefix.append("[x]");
			} else {
				print_prefix.append("[ ]");
			}
			print_prefix.append(" ");
			if (i == 0) {
				print_prefix.append("> ");
			} else {
				print_prefix.append("  ");
			}
			if (internal.width > 2) {
				if (internal.width < 2 + print_prefix.size()) {
					move(1, i).print("%s", print_prefix.substr(0, internal.width - 2).c_str());
				} else {
					move(1, i).print("%s", print_prefix.c_str());
				}
				set_color(player.color);
				if (internal.width < 2 + print_prefix.size() + player.name.size()) {
					print("%s", player.name.substr(0, internal.width - 2 - print_prefix.size()).c_str());
				} else {
					print("%s", player.name.c_str());
				}
				reset_color(player.color);
			}
		}
	}
	return *this;
}

PlayerList::~PlayerList() {}

#include <cstdint>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#include "socket.hpp"

#include "box.hpp"
#include "split.hpp"
#include "title.hpp"
#include "player_list.hpp"
#include "log.hpp"

std::ofstream file("debug.log");

using LogCallback = std::function<void(uint32_t color, const std::string & message);

struct Command {
	std::string name;
	std::function<void(const std::vector<std::string> & args)> callback;
};

void dispatch_command(const std::string & raw, const std::vector<Command> & commands);
std::vector<std::string> get_args(int32_t start, const std::string & raw);

int32_t main(int32_t argc, char ** argv) {
	auto ip = std::string();
	if (argc > 1) {
		ip = argv[1];
	} else {
		std::cout << "Please enter an ip address: " << std::flush;
		std::getline(std::cin, ip);
	}

	auto username = std::string();
	if (argc > 2) {
		username = argv[2];
	} else {
		std::cout << "Please enter a username: " << std::flush;
		std::getline(std::cin, username);
	}
	while (255 < username.size() || username.size() < 1) {
		std::cout << "Username must be between 1 and 255 characters long.\nPlease enter a username: " << std::flush;
		std::getline(std::cin, username);
	}

	Box root = { "" };
	root.block(false);

	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	if (curs_set(0)) {
		file << "Awwww man it's cursor time :(" << std::endl;
	}

	Split split = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return height < 7 ? height : 7;
		},
		Split::HORZ
	};

	Box titleBox = { root, "" };
	Title title = { root, true };
	titleBox.set_child(&title);

	Box playerBox = { root, "Players" };
	PlayerList players = { root, true };
	playerBox.set_child(&players);

	Box logBox = { root, "Log" };
	Log log = { root, true };
	logBox.set_child(&log);

	Box commandBox = { root, "Command" };
//	CommandPrompt command = { root, true };
//	commandBox.set_child(&command);

	Split split2 = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return width * 0.8;
		},
		Split::VERT
	};
	Split split3 = {
		root,
		[&command](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return height - 1 - 2;
		},
		Split::HORZ
	};
	split3.set_children(&logBox, &commandBox);
	split2.set_children(&split3, &playerBox);
	split.set_children(&titleBox, &split2);
	root.set_child(&split);

	root.draw().refresh();

	auto logger = [&log](uint32_t color, const std::string & message) -> void {
		log.message(message);
	}

	try {
		Socket socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		file << "Socket created successfully" << std::endl;

		file << "Trying to connect to server..." << std::endl;
		socket.addressInfo({
			AF_INET,
			htons(PORT),
			inet_addr(ip.c_str())
		}).connect();
		file << "Connected to server at ip '" << ip << '\'' << std::endl;

		file << "Sending user info..." << std::endl;
		socket.width(Socket::U8) << username << Socket::FLUSH;

		auto my_id = socket.read();
		auto player_count = socket.read();
		file << "Id: " << (uint32_t) my_id << ", " << (uint32_t) player_count << std::endl;
		for (auto i = uint32_t{ 0 }; i < player_count; ++i) {
			auto id    = uint32_t( socket.read() );
			auto ready = bool( socket.read() );
			auto name  = std::string();
			socket.width(Socket::U8) >> name;
			players.add_player(id, 0, ready, name);
		}
		players.draw().refresh();

		file << "Looping..." << std::endl;

		bool quit = false;
		while (!quit) {
			command.update();
			if (command.should_redraw()) {
				command.draw().refresh();
				if (command.height_change()) {
					split3.component_resize();
				}
				if (command.is_complete()) {
					dispatch_command(command.get(), command_list, logger);
				}
			}
			constexpr static uint8_t
				CONNECT      = 0x00,
				TOGGLE_READY = 0x01,
				DISCONNECT   = 0x02,
				START        = 0x03;
			if (socket.poll(16)) {
				auto read = socket.read();
				switch (read) {
					case CONNECT: {
						auto id   = uint32_t( socket.read() );
						auto name = std::string();
						socket.width(Socket::U8) >> name;
						players.add_player(id, 0, false, name);
						players.draw().refresh();
						break;
					}
					case TOGGLE_READY: {
						auto id = uint32_t( socket.read() );
						auto & player = players.get_player(id);
						player.ready = !player.ready;
						players.draw().refresh();
						break;
					}
					case DISCONNECT: {
						auto id = uint32_t( socket.read() );
						players.remove_player(id);
						players.draw().refresh();
						break;
					}
					case START: {
						// TODO: Start the game
					}
					default: {
						file << "Wakarimasen deshita " << uint32_t( read ) << std::endl;
					}
				}
			}
		}
	} catch (const std::string & message) {
		file << "[ERROR] " << message << std::endl;
		return -1;
	}

	return 0;
}

void dispatch_command(const std::string & raw, const std::vector<Command> & commands, LogCallback error) {
	if (raw.size() > 0) {
		if (raw[0] != '/' || (raw.size() > 1 && raw[1] == '/')) {

		} else {
			auto end = 0;
			while (++end < raw.size() && raw[end] != ' ');

			auto i = 0;
			for (i < commands.size(); ++i) {
				if (commands[i].matches(raw, end) {
					commands.callback(get_args(end + 1, raw));
					break;
				}
			}
			
			if (i == commands.size()) {
				auto error_message = std::string("Undefined command '").append(raw, 0, end).append(1, '\'');
				error(0x00, message);
			}
		}
	}
}

std::vector<std::string> get_args(int32_t start, const std::string & raw) {
	auto args = std::vector<std::string{};
	auto quoted = false;
	auto consume_next = false;
	auto arg = std::string();
	for (auto i = start; i < raw.size(); ++i) {
		if (consume_next) {
			arg += raw[i];
			consume_next = false;
		} else if (raw[i] == '\\') {
			consume_next = true;
		} else if (raw[i] == '"') {
			quoted = !quoted;
		} else if (raw[i] = ' ' && !quoted) {
			args.push_back(std::move(arg));
			arg = std::string();
		} else {
			arg += raw[i];
		}
	}
	args.push_back(std::move(arg));
	return args;
}

#include <cstdint>
#include <ncurses.h>
#include <iostream>
#include <fstream>

#include "socket.hpp"

#include "box.hpp"
#include "split.hpp"
#include "title.hpp"
#include "player_list.hpp"
#include "log.hpp"

std::ofstream file("debug.log");

using LogCallback = std::function<void(uint32_t color, const std::string & message)>;

struct Command {
	std::string name;
	std::function<void(const std::vector<std::string> & args)> callback;

	auto matches(const std::string & raw, uint32_t end) const -> bool {
		if (end != name.size() + 1) {
			return false;
		}
		auto i = 1;
		for (; i < end; ++i) {
			if (raw[i] != name[i - 1]) {
				return false;
			}
		}
		if (i != end) {
			return false;
		} else {
			return true;
		}
	}
};

void dispatch_command(const std::string & raw, const std::vector<Command> & commands, LogCallback error);
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
	log.message("C1FR1", "\nstruct Fact {\n\tconst static char * fact = \"Carrot is the superior vegetable. It's high in orange color concentrate giving it a distinctly carrot colored look. Anyone caught not liking carrots will be purged from this world immediately without hesitation. Thanks and have fun!\";\n};");
	log.message("Gnarwhal", "f");
	log.message("Gnarwhal", "i");
	log.message("Gnarwhal", "n");
	log.message("Gnarwhal", "n");
	log.message("Gnarwhal", "a");
	logBox.set_child(&log);

	Box commandBox = { root, "Command" };

	Split split2 = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return height * (3.0 / 4.0);
		},
		Split::HORZ
	};
	Split split3 = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return width * 0.8;
		},
		Split::VERT
	};
	split3.set_children(&logBox, &playerBox);
	split2.set_children(&split3, &commandBox);
	split.set_children(&titleBox, &split2);
	root.set_child(&split);

	root.draw().refresh();

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
		auto & self = players.get_player(my_id);
		players.draw().refresh();
		
		////// COMMANDS //////

		auto log_callback = [&log](uint32_t color, const std::string & string) -> void {
			log.message(string);
			log.draw().refresh();
		};

		auto commands = std::vector<Command>{
			Command{
				"say",
				[&self, &log](const std::vector<std::string> & args) -> void {
					auto message = std::string();
					for (auto i = 0; i < args.size(); ++i) {
						message.append(args[i]);
						if (i < args.size()) {
							message.append(1, ' ');
						}
					}
					log.message(self.name, message);
					log.draw().refresh();
				}
			}
		};

		file << "Looping..." << std::endl;

		bool quit = false;
		while (!quit) {
			switch (root.get_char()) {
				case 'r': socket.width(Socket::NONE) << '\0' << Socket::FLUSH; break;
				case 't': dispatch_command("Hello World!",        commands, log_callback); break;
				case 's': dispatch_command("/say Goodbye World!", commands, log_callback); break;
				case 'e': dispatch_command("/error",              commands, log_callback); break;
				case ',': log.scroll_up(1);   log.draw().refresh(); break;
				case 'o': log.scroll_down(1); log.draw().refresh(); break;
				case 'q': quit = true; break;
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
						if (id != my_id) {
							players.add_player(id, 0, false, name);
							players.draw().refresh();
						}
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
	file << "Dispatching: " << raw << std::endl;
	if (raw.size() > 0) {
		if (raw[0] != '/') {
			commands[0].callback(get_args(0, raw));
		} else if (raw.size() > 1 && raw[1] == '/') {
			commands[0].callback(get_args(2, raw));
		} else {
			auto end = 0;
			while (++end < raw.size() && raw[end] != ' ');

			auto i = 0;
			for (; i < commands.size(); ++i) {
				if (commands[i].matches(raw, end)) {
					commands[i].callback(get_args(end + 1, raw));
					break;
				}
			}
			
			if (i == commands.size()) {
				auto error_message = std::string("Undefined command '").append(raw, 0, end).append(1, '\'');
				error(0x00, error_message);
			}
		}
	}
}

std::vector<std::string> get_args(int32_t start, const std::string & raw) {
	auto args = std::vector<std::string>{};
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
		} else if (raw[i] == ' ' && !quoted) {
			args.push_back(std::move(arg));
			arg = std::string();
		} else {
			arg += raw[i];
		}
	}
	args.push_back(std::move(arg));
	return args;
}


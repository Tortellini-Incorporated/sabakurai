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
#include "command_prompt.hpp"

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

	file << "Wat" << std::endl;

	Split split = {
		root,
		[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
			return height < 7 ? height : 7;
		},
		Split::HORZ
	};
	file << "Wat2" << std::endl;

	Box titleBox = { root, "" };
	Title title = { root, true };
	titleBox.set_child(&title);

	file << "Wat3" << std::endl;
	Box playerBox = { root, "Players" };
	PlayerList players = { root, true };
	playerBox.set_child(&players);
	file << "Wat4" << std::endl;

	Box logBox = { root, "Log" };
	Log log = { root, true };
	log.message("Welcome to sabakurai! For assistance please enter /help");
	logBox.set_child(&log);

	file << "Wat5" << std::endl;
	Box commandBox = { root, "Command" };
	CommandPrompt command = { root, true };
	file << "Hello there" << std::endl;
	commandBox.set_child(&command);

	file << "Command prompt inited" << std::endl;

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
			if (command.line_count() + 2 > height - (height * (3.0 / 4.0))) {
				return height * (3.0 / 4.0);
			} else {
				return height - command.line_count() - 2;
			}
		},
		Split::HORZ
	};
	file << "Setting children :snatch:" << std::endl;
	split3.set_children(&logBox, &commandBox);
	split2.set_children(&split3, &playerBox);
	split.set_children(&titleBox, &split2);
	root.set_child(&split);
	
	file << "Tryna draw" << std::endl;
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
		socket
			<< uint8_t( username.size() )
			<< username
			<< Socket::FLUSH;

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
		
		bool quit = false;

		////// COMMANDS //////

		auto log_callback = [&log](uint32_t color, const std::string & string) -> void {
			log.message(string);
			log.draw().refresh();
		};

		auto commands = std::vector<Command>{
			Command{
				"say",
				[&socket](const std::vector<std::string> & args) -> void {
					auto message = std::string();
					for (auto i = 0; i < args.size(); ++i) {
						message.append(args[i]);
						if (i < args.size() - 1) {
							message.append(1, ' ');
						}
					}
					file << "Say: " << message << std::endl;
					socket
						<< uint8_t( 0x05 )
						<< uint16_t( message.size() )
						<< message
						<< Socket::FLUSH;
				}
			},
			Command{
				"quit",
				[&quit](const std::vector<std::string> & args) -> void {
					quit = true;
				}
			},
			Command{
				"help",
				[&log](const std::vector<std::string> & args) -> void {
					log.message(
						"Type a message and hit enter to send. Shift + Tab is new line. To enter a command prefix the message with a '/'. To send a message that starts with a '/' use a '//' at the start.\n"
						"Commands:\n"
						"- /help          - prints the help menu\n"
						"- /say <string>  - sends the given string. equivalent to just leaving the /say off\n"
						"- /quit          - quits the game\n"
						"- /ready         - toggle ready status\n"
						"- /name <string> - changes your name - UNIMPLEMENTED\n"
					);
					log.draw().refresh();
				}
			},
			Command{
				"ready",
				[&socket](const std::vector<std::string> & args) -> void {
					socket
						<< uint8_t( 0 )
						<< Socket::FLUSH;
				}
			},
			Command{
				"name",
				[&self, &socket, &log_callback, &log](const std::vector<std::string> & args) -> void {
					auto name = std::string();
					for (auto i = 0; i < args.size(); ++i) {
						name.append(args[i]);
						if (i < args.size() - 1) {
							name.append(1, ' ');
						}
					}
					if (name.size() > 0xFF) {
						log_callback(0x00, "Sorry. Max name length is 255.");
						log.draw().refresh();
					} else {
						socket
							<< uint8_t( 0x01 )
							<< uint8_t( name.size() )
							<< name
							<< Socket::FLUSH;
					}
				}
			}
		};

		file << "Looping..." << std::endl;

		while (!quit) {
			if (command.update()) {
				if (command.complete()) {
					dispatch_command(command.get(), commands, log_callback);
					command.clear_command();
				}
				if (command.height_change(command.height() + 1)) {
					split3.component_resize();
					logBox.draw();
					commandBox.draw();
					split3.refresh();
				} else {
					command.draw().refresh();
				}
			}
			split3.refresh();
			constexpr static uint8_t
				CONNECT       = 0x00,
				TOGGLE_READY  = 0x01,
				DISCONNECT    = 0x02,
				START         = 0x03,
				UPDATE_NAME   = 0x08,
				RELAY_MESSAGE = 0x09;
			if (socket.poll(16)) {
				auto read = socket.read();
				switch (read) {
					case CONNECT: {
						auto id   = uint32_t( socket.read() );
						auto name = std::string();
						socket.width(Socket::U8) >> name;
						if (id != my_id) {
							log.message(std::string("Player '").append(name).append("' connected"));
							players.add_player(id, 0, false, name);
							players.draw().refresh();
							log.draw().refresh();
						}
						break;
					}
					case TOGGLE_READY: {
						auto id = uint32_t( socket.read() );
						auto & player = players.get_player(id);
						player.ready = !player.ready;
						if (player.ready) {
							log.message(std::string("Player '").append(player.name).append("' is ready"));
						} else {
							log.message(std::string("Player '").append(player.name).append("' is no longer ready"));
						}
						players.draw().refresh();
						log.draw().refresh();
						break;
					}
					case DISCONNECT: {
						auto id = uint32_t( socket.read() );
						log.message(std::string("Player '").append(players.get_player(id).name).append("' disconnected"));
						players.remove_player(id);
						players.draw().refresh();
						log.draw().refresh();
						break;
					}
					case START: {
						// TODO: Start the game
						break;
					}
					case UPDATE_NAME: {
						auto id = uint32_t( socket.read() );
						auto & player = players.get_player(id);
						socket.width(Socket::U8) >> player.name;
						file << "New Name: " << player.name << std::endl;
						players.draw().refresh();
						break;
					}
					case RELAY_MESSAGE: {
						auto id = uint32_t( socket.read() );
						auto & player = players.get_player(id);
						auto message = std::string();
						socket.width(Socket::U16) >> message;
						log.message(player.name, message);
						log.draw().refresh();
						break;
					}
					default: {
						file << "Wakarimasen deshita " << uint32_t( read ) << std::endl;
					}
				}
			}
			command.move_cursor();
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
			commands[0].callback(get_args(1, raw));
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


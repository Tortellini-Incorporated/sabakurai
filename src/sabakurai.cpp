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

void dispatch_command(const std::string & raw, const std::vector<Command> & commands, Log & log);
std::vector<std::string> get_args(int32_t start, const std::string & raw);

struct LobbyState {
	Box root;
	Split split;
	Box title_box;
	Title title;
	Box player_box;
	PlayerList players;
	Box log_box;
	Log log;
	Box command_box;
	CommandPrompt command;
	Split split2;
	Split split3;

	Socket socket;
};

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

	LobbyState lobby = {
		{ "" },

		{
			lobby.root,
			[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
				return height < 7 ? height : 7;
			},
			Split::HORZ
		},

		{ lobby.root, "" },
		{ lobby.root, true },

		{ lobby.root, "Players" },
		{ lobby.root, true },

		{ lobby.root, "Log" },
		{ lobby.root, true },

		{ lobby.root, "Command" },
		{ lobby.root, true },
		{
			lobby.root,
			[](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
				return width * 0.8;
			},
			Split::VERT
		},
		{
			lobby.root,
			[&lobby](uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> uint32_t {
				if (lobby.command.line_count() + 2 > height - (height * (3.0 / 4.0))) {
					return height * (3.0 / 4.0);
				} else {
					return height - lobby.command.line_count() - 2;
				}
			},
			Split::HORZ
		},
		{ PF_INET, SOCK_STREAM, IPPROTO_TCP }
	};
	lobby.root.block(false);

	cbreak();
	noecho();

	lobby.title_box.set_child(&lobby.title);
	lobby.player_box.set_child(&lobby.players);
	lobby.log_box.set_child(&lobby.log);
	lobby.command_box.set_child(&lobby.command);
	lobby.split3.set_children(&lobby.log_box, &lobby.command_box);
	lobby.split2.set_children(&lobby.split3, &lobby.player_box);
	lobby.split.set_children(&lobby.title_box, &lobby.split2);
	lobby.root.set_child(&lobby.split);
	
	lobby.log.message("Welcome to sabakurai! For assistance please enter /help");

	lobby.root.draw().refresh();

	try {
		file << "Socket created successfully" << std::endl;

		file << "Trying to connect to server..." << std::endl;
		lobby.socket.addressInfo({
			AF_INET,
			htons(PORT),
			inet_addr(ip.c_str())
		}).connect();
		file << "Connected to server at ip '" << ip << '\'' << std::endl;

		file << "Sending user info..." << std::endl;
		lobby.socket
			<< uint8_t( username.size() )
			<< username
			<< Socket::FLUSH;

		auto my_id = lobby.socket.read();
		auto player_count = lobby.socket.read();
		file << "Id: " << (uint32_t) my_id << ", " << (uint32_t) player_count << std::endl;
		for (auto i = uint32_t{ 0 }; i < player_count; ++i) {
			auto id    = uint32_t( lobby.socket.read() );
			auto ready = bool( lobby.socket.read() );
			auto name  = std::string();
			lobby.socket.width(Socket::U8) >> name;
			lobby.players.add_player(id, 0, ready, name);
		}
		auto & self = lobby.players.get_player(my_id);
		lobby.players.draw().refresh();
		
		bool quit = false;

		////// COMMANDS //////

		auto commands = std::vector<Command>{
			Command{
				"say",
				[&lobby](const std::vector<std::string> & args) -> void {
					auto message = std::string();
					for (auto i = 0; i < args.size(); ++i) {
						message.append(args[i]);
						if (i < args.size() - 1) {
							message.append(1, ' ');
						}
					}
					file << "Say: " << message << std::endl;
					lobby.socket
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
				[&lobby](const std::vector<std::string> & args) -> void {
					lobby.log.message(
						"Type a message and hit enter to send. Shift + Tab is new line. To enter a command prefix the message with a '/'. To send a message that starts with a '/' use a '//' at the start.\n"
						"Commands:\n"
						"- /help          - prints the help menu\n"
						"- /say <string>  - sends the given string. equivalent to just leaving the /say off\n"
						"- /quit          - quits the game\n"
						"- /ready         - toggle ready status\n"
						"- /name <string> - changes your name - UNIMPLEMENTED\n"
					);
					lobby.log.draw().refresh();
				}
			},
			Command{
				"ready",
				[&lobby](const std::vector<std::string> & args) -> void {
					lobby.socket
						<< uint8_t( 0 )
						<< Socket::FLUSH;
				}
			},
			Command{
				"name",
				[&lobby](const std::vector<std::string> & args) -> void {
					auto name = std::string();
					for (auto i = 0; i < args.size(); ++i) {
						name.append(args[i]);
						if (i < args.size() - 1) {
							name.append(1, ' ');
						}
					}
					if (name.size() > 0xFF) {
						lobby.log.message(/*0x00, */"Sorry. Max name length is 255.");
						lobby.log.draw().refresh();
					} else {
						lobby.socket
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
			if (lobby.command.update()) {
				if (lobby.command.complete()) {
					dispatch_command(lobby.command.get(), commands, lobby.log);
					lobby.command.clear_command();
				}
				if (lobby.command.height_change(lobby.command.height() + 1)) {
					lobby.split3.component_resize();
					lobby.log_box.draw();
					lobby.command_box.draw();
					lobby.split3.refresh();
				} else {
					lobby.command.draw().refresh();
				}
			}
			lobby.split3.refresh();
			constexpr static uint8_t
				CONNECT       = 0x00,
				TOGGLE_READY  = 0x01,
				DISCONNECT    = 0x02,
				START         = 0x03,
				UPDATE_NAME   = 0x08,
				RELAY_MESSAGE = 0x09;
			if (lobby.socket.poll(16)) {
				auto read = lobby.socket.read();
				switch (read) {
					case CONNECT: {
						auto id   = uint32_t( lobby.socket.read() );
						auto name = std::string();
						lobby.socket.width(Socket::U8) >> name;
						if (id != my_id) {
							lobby.log.message(std::string("Player '").append(name).append("' connected"));
							lobby.players.add_player(id, 0, false, name);
							lobby.players.draw().refresh();
							lobby.log.draw().refresh();
						}
						break;
					}
					case TOGGLE_READY: {
						auto id = uint32_t( lobby.socket.read() );
						auto & player = lobby.players.get_player(id);
						player.ready = !player.ready;
						if (player.ready) {
							lobby.log.message(std::string("Player '").append(player.name).append("' is ready"));
						} else {
							lobby.log.message(std::string("Player '").append(player.name).append("' is no longer ready"));
						}
						lobby.players.draw().refresh();
						lobby.log.draw().refresh();
						break;
					}
					case DISCONNECT: {
						auto id = uint32_t( lobby.socket.read() );
						lobby.log.message(std::string("Player '").append(lobby.players.get_player(id).name).append("' disconnected"));
						lobby.players.remove_player(id);
						lobby.players.draw().refresh();
						lobby.log.draw().refresh();
						break;
					}
					case START: {
						// TODO: Start the game
						break;
					}
					case UPDATE_NAME: {
						auto id = uint32_t( lobby.socket.read() );
						auto & player = lobby.players.get_player(id);
						lobby.socket.width(Socket::U8) >> player.name;
						file << "New Name: " << player.name << std::endl;
						lobby.players.draw().refresh();
						break;
					}
					case RELAY_MESSAGE: {
						auto id = uint32_t( lobby.socket.read() );
						auto & player = lobby.players.get_player(id);
						auto message = std::string();
						lobby.socket.width(Socket::U16) >> message;
						lobby.log.message(player.name, message);
						lobby.log.draw().refresh();
						break;
					}
					default: {
						file << "Wakarimasen deshita " << uint32_t( read ) << std::endl;
					}
				}
			}
			lobby.command.move_cursor();
		}
	} catch (const std::string & message) {
		file << "[ERROR] " << message << std::endl;
		return -1;
	}

	return 0;
}

void dispatch_command(const std::string & raw, const std::vector<Command> & commands, Log & error) {
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
				error.message(/*0x00, */error_message);
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


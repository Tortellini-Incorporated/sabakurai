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

auto get_ip(const std::string & ip_string) -> in_addr_t;

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

auto dispatch_command(const std::string & raw, const std::vector<Command> & commands, Log & log) -> void;
auto get_args(int32_t start, const std::string & raw) -> std::vector<std::string>;

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
	uint32_t curr_addr;
};

auto disconnected(LobbyState & lobby) -> uint32_t;
auto connected(LobbyState & lobby) -> uint32_t;

const static auto
	DISCONNECTED = 0x00,
	CONNECTED    = 0x01,
	RECONNECT    = 0x02,
	QUIT         = 0x03;

const static auto
	COLOR_DEFAULT  = 0x00,
	COLOR_INVERTED = 0x01,
	COLOR_ERROR    = 0x02;

auto main(int32_t argc, char ** argv) -> int32_t {
	/*auto ip = std::string();
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
	}*/

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

	cbreak();
	noecho();

	if (!has_colors()) {
		return -1;
	}

	start_color();

	if (COLOR_PAIRS < 3) {
		return -2;
	}
	
	init_pair(COLOR_INVERTED, COLOR_BLACK, COLOR_WHITE);
	init_pair(COLOR_ERROR,    COLOR_RED,   COLOR_BLACK);

	lobby.title_box.set_child(&lobby.title);
	lobby.player_box.set_child(&lobby.players);
	lobby.log_box.set_child(&lobby.log);
	lobby.command_box.set_child(&lobby.command);
	lobby.split3.set_children(&lobby.log_box, &lobby.command_box);
	lobby.split2.set_children(&lobby.split3, &lobby.player_box);
	lobby.split.set_children(&lobby.title_box, &lobby.split2);
	lobby.root.set_child(&lobby.split);

	lobby.log.message("Welcome to sabakurai! For assistance please enter /help");

	lobby.command.block(false);
	lobby.root.draw().refresh();

	auto state = DISCONNECTED;
	while (state != QUIT) {
		switch (state) {
			case DISCONNECTED: state = disconnected(lobby); break;
			case CONNECTED:
			case RECONNECT:    state = connected(lobby);    break;
		}
	}

	return 0;
}

auto disconnected(LobbyState & lobby) -> uint32_t {
	auto quit = DISCONNECTED;

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
				lobby.log.message(lobby.players.get_self().name, message);
				lobby.log.draw().refresh();
			}
		},
		Command{
			"connect",
			[&quit, &lobby](const std::vector<std::string> & args) -> void {
				try {
					lobby.socket.addressInfo({
						AF_INET,
						htons(PORT),
						lobby.curr_addr = get_ip(args[0])
					}).connect();
				
					auto name = lobby.players.get_self().name;

					lobby.socket
						<< uint8_t( name.size() )
						<< name
						<< Socket::FLUSH;

					lobby.log.message(COLOR_DEFAULT, std::string("Connected to server '").append(args[0]).append(1, '\''));

					quit = CONNECTED;
				} catch (const std::string & error) {
					lobby.log.message(COLOR_ERROR, std::string("Failed to connect to server '").append(args[0]).append(1, '\''));
					lobby.curr_addr = 0;
				}
				lobby.log.draw().refresh();
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
					lobby.log.message(COLOR_ERROR, "Sorry. Max name length is 255.");
					lobby.log.draw().refresh();
				} else {
					lobby.players.get_self().name = name;
					lobby.players.draw().refresh();
				}
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
					"- /connect <ip>  - connects to the server at the ip\n"
					"- /name <string> - changes your name\n"
				);
				lobby.log.draw().refresh();
			}
		},
		Command{
			"quit",
			[&quit](const std::vector<std::string> & args) -> void {
				quit = QUIT;
			}
		},
		Command{
			"killme",
			[&lobby](const std::vector<std::string> & args) -> void {
				lobby.log.message("Medic", "Later");
				lobby.log.draw().refresh();
			}
		},
	};
	
	while (quit == DISCONNECTED) {
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
			lobby.split3.refresh();
		}
	}

	return quit;
}

auto connected(LobbyState & lobby) -> uint32_t {
	{
		auto my_id = lobby.socket.read();
		auto player_count = lobby.socket.read();
		for (auto i = uint32_t{ 0 }; i < player_count; ++i) {
			auto id    = uint32_t( lobby.socket.read() );
			auto ready = bool( lobby.socket.read() );
			auto name  = std::string();
			lobby.socket.width(Socket::U8) >> name;
			if (id == my_id) {
				lobby.players.get_self().id = my_id;
			} else {
				lobby.players.add_player(id, 0, ready, name);
			}
		}
		lobby.players.draw().refresh();
	}
		
	auto quit = CONNECTED;

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
				quit = QUIT;
			}
		},
		Command{
			"disconnect",
			[&quit, &lobby](const std::vector<std::string> & args) -> void {
				lobby.log.message(COLOR_DEFAULT, "Disconnected from server");
				lobby.log.draw().refresh();
				lobby.players.clear_list();
				lobby.players.draw().refresh();
				lobby.socket.close();
				quit = DISCONNECTED;
			}
		},
		Command{
			"connect",
			[&quit, &lobby](const std::vector<std::string> & args) -> void {
				lobby.players.clear_list();
				lobby.players.draw().refresh();
				lobby.socket.close();
				try {
					lobby.socket.addressInfo({
						AF_INET,
						htons(PORT),
						lobby.curr_addr = get_ip(args[0])
					}).connect();
				
					auto name = lobby.players.get_self().name;

					lobby.socket
						<< uint8_t( name.size() )
						<< name
						<< Socket::FLUSH;

					lobby.log.message(COLOR_DEFAULT, std::string("Connected to server '").append(args[0]).append(1, '\''));

					quit = RECONNECT;
				} catch (const std::string & error) {
					lobby.log.message(COLOR_ERROR, std::string("Failed to connect to server '").append(args[0]).append(1, '\''));

					quit = DISCONNECTED;
				}
				lobby.log.draw().refresh();
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
					"- /disconnect    - disconnects from the server\n"
					"- /connect <ip>  - connects to the server at the ip\n"
					"- /ready         - toggle ready status\n"
					"- /name <string> - changes your name\n"
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
					lobby.log.message(COLOR_ERROR, "Sorry. Max name length is 255.");
					lobby.log.draw().refresh();
				} else {
					lobby.socket
						<< uint8_t( 0x01 )
						<< uint8_t( name.size() )
						<< name
						<< Socket::FLUSH;
				}
			}
		},
		Command{
			"killme",
			[&lobby](const std::vector<std::string> & args) -> void {
				lobby.log.message("Medic", "Later");
				lobby.log.draw().refresh();
			}
		}
	};

	while (quit == CONNECTED) {
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
			lobby.split3.refresh();
		}
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
					if (id != lobby.players.get_self().id) {
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
	return quit;
}

auto dispatch_command(const std::string & raw, const std::vector<Command> & commands, Log & error) -> void {
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
				error.message(COLOR_ERROR, error_message);
				error.draw().refresh();
			}
		}
	}
}

auto get_args(int32_t start, const std::string & raw) -> std::vector<std::string> {
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

auto get_ip(const std::string & ip_string) -> in_addr_t {
	auto hints = addrinfo{ 0 };
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	auto * info = (addrinfo*) 0;
	getaddrinfo(ip_string.c_str(), NULL, &hints, &info);

	auto ret = ((sockaddr_in*) info->ai_addr)->sin_addr.s_addr;

	freeaddrinfo(info);

	return ret;
}

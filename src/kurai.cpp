#include <iostream>
#include <cstdint>
#include <string>

#include "socket.hpp"

auto main(int32_t argc, char ** argv) -> int32_t {
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

	try {
		Socket socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		std::cout << "Socket created successfully" << std::endl;

		std::cout << "Trying to connect to server..." << std::endl;
		socket.addressInfo({
			AF_INET,
			htons(PORT),
			inet_addr(ip.c_str())
		}).connect();
		socket.block(false, 16);
		std::cout << "Connected to server at ip '" << ip << '\'' << std::endl;

		std::cout << "Sending user info..." << std::endl;
		socket.width(Socket::U8) << username << Socket::FLUSH;

		bool quit = false;
		while (!quit) {
			fd_set input;
			FD_ZERO(&input);
			FD_SET(STDIN_FILENO, &input);
			if (FD_ISSET(STDIN_FILENO, &input)) {
				auto in = std::string();
				std::cin >> in;
				if (in.compare("ready") == 0) {
					socket.width(Socket::NONE) << 'r' << Socket::FLUSH;
				} else if (in.compare("unready") == 0) {
					socket.width(Socket::NONE) << 'u' << Socket::FLUSH;
				} else if (in.compare("quit") == 0) {
					quit = true;
				} else {
					std::cout << "No comprendo: " << in << std::endl;
				}
			}
			constexpr static uint8_t
				CONNECT      = 0x00,
				TOGGLE_READY = 0x01,
				DISCONNECT   = 0x02;
			char c;
			if (socket.read(c)) {
				auto id = uint32_t( 0 );
				auto name = std::string();
				char signal;
				socket.read(signal);
				switch (c) {
					case CONNECT:
						id = uint32_t( signal );
						socket.width(Socket::U8) >> name;
						std::cout << "Client connected -> id: " << id << ", username: " << name << std::endl;
						break;
					case TOGGLE_READY:
						id = uint32_t( signal );
						std::cout << "Client toggled ready -> id: " << id << std::endl;
						break;
					case DISCONNECT:
						id = uint32_t( signal );
						std::cout << "Client disconnected -> id: " << id << std::endl;
						break;
					default:
						std::cout << "Wakarimasen deshita" << std::endl;
				}
			}
		}
	} catch (const std::string & message) {
		std::cerr << message << std::endl;
		return -1;
	}
	return 0;
}

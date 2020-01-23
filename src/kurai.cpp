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

		socket.setAddressInfo({
			AF_INET,
			htons(PORT),
			inet_addr(ip.c_str())
		});
		std::cout << "Trying to connect to server..." << std::endl;
		socket.connect();
		std::cout << "Connected to server at ip '" << ip << '\'' << std::endl;

		std::cout << "Sending user info..." << std::endl;
		socket << Socket::U8 << username << Socket::FLUSH;

		std::cout << "Readying up..." << std::endl;
		socket << Socket::NONE << 'r' << Socket::FLUSH;

		std::cout << "Unreadying up..." << std::endl;
		socket << Socket::NONE << 'u' << Socket::FLUSH;
	} catch (const std::string & message) {
		std::cerr << message << std::endl;
		return -1;
	}
	return 0;
}

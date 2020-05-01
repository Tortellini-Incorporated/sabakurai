#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <error.h>

#include "socket.hpp"

#ifdef _WIN32
	int initSocket() {
		WSADATA wsaData;
		return WSAStartup(MAKEWORD(1, 1), &wasData);
	}

	int quitSocket() {
		return WSACleanup();
	}

	int closeSocket(CSocket socket) {
		int status = shutdown(socket, SD_BOTH);
		if (status == 0) {
			status = closesocket(socket);
		}
		return status;
	}
#else
	int initSocket() { return 0; }
	int quitSocket() { return 0; }

	int closeSocket(CSocket socket) {
		int status = shutdown(socket, SHUT_RDWR);
		if (status == 0) {
			status = close(socket);
		}
		return status;
	}
#endif

extern std::ofstream file;

Socket::Socket(int domain, int type, int protocol) :
	socket(::socket(domain, type, protocol)),
	socket_info({ domain, type, protocol }),
	mAddressInfo({}),
	connected(false),
	message(),
	mFlags(),
	mWidth(NONE) {
	if (socket == -1) {
		throw std::string("Failed to create socket");
	}
	pollFd.fd     = socket;
	pollFd.events = POLLIN | POLLOUT;
}

auto Socket::addressInfo(const sockaddr_in & addressInfo) -> Socket& {
	mAddressInfo = addressInfo;
	return *this;
}

auto Socket::flags(Flags flags) -> Socket& {
	mFlags = flags;
	return *this;
}

auto Socket::width(Width width) -> Socket& {
	mWidth = width;
	return *this;
}

auto Socket::connect(uint32_t timeout) -> bool {
	::fcntl(socket, F_SETFL, O_NONBLOCK);
	if (::connect(socket, (sockaddr *) &mAddressInfo, sizeof(sockaddr_in)) == -1 && errno != EINPROGRESS) {
		file << "Connection error: " << strerror(errno) << std::endl;
		close();
		return connected = false;
	}
	::fcntl(socket, F_SETFL, ~O_NONBLOCK);
	
	if (::poll(&pollFd, 1, timeout) < 1) {
		file << "Connection timed out" << std::endl;
		close();
		return connected = false;
	}

	auto option   = int32_t();
	auto int_size = sizeof(int32_t);
	if (getsockopt(socket, SOL_SOCKET, SO_ERROR, (void*) &option, (socklen_t *) &int_size)) {
		file << "Connection error: " << errno << ", " << strerror(errno) << std::endl;
		return connected = false;
	}
	if (option) {
		file << "Connection error: " << option << ", " << strerror(option) << std::endl;
		return connected = false;
	}
	return connected = true;
}

auto Socket::close() -> void {
	::close(socket);
	socket = ::socket(socket_info.domain, socket_info.type, socket_info.protocol);
	connected = false;
}

auto Socket::poll(uint32_t timeout) -> bool {
	auto pollResult = ::poll(&pollFd, 1, timeout);
	if (pollResult > 0 && pollFd.revents & POLLIN) {
		return true;
	} else {
		return false;
	}
}

auto Socket::read() -> uint8_t {
	char c;
	recv(socket, &c, 1, mFlags.flags);
	return c;
}

auto Socket::operator<<(const char * str) -> Socket& {
	--str;
	while (*++str != '\0') {
		message.push_back(*str);
	}
	return *this;
}

auto Socket::operator<<(const std::string & str) -> Socket& {
	for (auto i = 0; i < str.size(); ++i) {
		message.push_back(str[i]);
	}
	return *this;
}

auto Socket::operator<<(char data) -> Socket& {
	message.push_back(data);
	return *this;
}

auto Socket::operator<<(int8_t data) -> Socket& {
	message.push_back(data);
	return *this;
}

auto Socket::operator<<(uint8_t data) -> Socket& {
	message.push_back(data);
	return *this;
}

auto Socket::operator<<(int16_t data) -> Socket& {
	message.push_back((data >> 8) & 0xFF);
	message.push_back((data     ) & 0xFF);
	return *this;
}

auto Socket::operator<<(uint16_t data) -> Socket& {
	message.push_back((data >> 8) & 0xFF);
	message.push_back((data     ) & 0xFF);
	return *this;
}

auto Socket::operator<<(int32_t data) -> Socket& {
	message.push_back((data >> 24) & 0xFF);
	message.push_back((data >> 16) & 0xFF);
	message.push_back((data >>  8) & 0xFF);
	message.push_back((data      ) & 0xFF);
	return *this;
}

auto Socket::operator<<(uint32_t data) -> Socket& {
	message.push_back((data >> 24) & 0xFF);
	message.push_back((data >> 16) & 0xFF);
	message.push_back((data >>  8) & 0xFF);
	message.push_back((data      ) & 0xFF);
	return *this;
}

auto Socket::operator<<(Flush flush) -> Socket& {
	if (flush == FLUSH_LINE) {
		message.push_back('\n');
	}
	for (auto i : message) {
		file << uint32_t( i ) << " ";
	}
	file << std::endl;
	send(socket, &message[0], message.size(), mFlags.flags);
	message.clear();
	return *this;
}

auto Socket::operator>>(std::string & string) -> Socket& {
	auto vec = std::vector<char>();
	*this >> vec;
	vec.push_back('\0');
	string = &vec[0];
	return *this;
}

auto Socket::operator>>(std::vector<char> & vector) -> Socket& {
	char buffer[4];
	recv(socket, buffer, mWidth + 1, mFlags.flags);
	auto length = uint32_t( buffer[0] );
	for (auto i = 1; i <= mWidth; ++i) {
		length <<= 8;
		length |= buffer[i];
	}
	vector.resize(length);
	recv(socket, &vector[0], length, mFlags.flags);
	return *this;
}

Socket::~Socket() {
	close();
}

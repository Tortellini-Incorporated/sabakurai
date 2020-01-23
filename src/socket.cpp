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

Socket::Socket(int domain, int type, int protocol) :
	socket(::socket(domain, type, protocol)),
	addressInfo({}),
	connected(false),
	message() {
	if (socket == -1) {
		throw std::string("Failed to create socket");
	}
}

auto Socket::setAddressInfo(const sockaddr_in & addressInfo) -> void {
	this->addressInfo = addressInfo;
}

auto Socket::connect() -> void {
	if (::connect(socket, (sockaddr *) &addressInfo, sizeof(sockaddr_in)) == -1) {
		throw std::string("Failed to connect to server");
	}
	connected = true;
}

auto Socket::close() -> void {
	::close(socket);
	connected = false;
}

Socket::~Socket() {
	close();
}

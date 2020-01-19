#ifndef SABAKURAI_SOCKET
#define SABAKURAI_SOCKET

#ifdef _WIN32
	#ifdef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501
	#endif

	typedef SOCKET Socket;

	int sockInit() {
		WSADATA wsaData;
		return WSAStartup(MAKEWORD(1, 1), &wasData);
	}

	int sockQuit() {
		return WSACleanup();
	}

	int closeSocket(Socket socket) {
		int status = shutdown(socket, SD_BOTH);
		if (status == 0) {
			status = closesocket(socket);
		}
		return status;
	}
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>

	typedef int Socket;

	#define INVALID_SOCKET -1

	int sockInit() { return 0; }
	int sockQuit() { return 0; }

	int closeSocket(Socket socket) {
		int status = shutdown(socket, SHUT_RDWR);
		if (status == 0) {
			status = close(socket);
		}
		return status;
	}
#endif

#endif // SABAKURAI_SOCKET

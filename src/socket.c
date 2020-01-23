#include "socket.h"

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

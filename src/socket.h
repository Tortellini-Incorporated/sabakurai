#ifndef SABAKURAI_SOCKET
#define SABAKURAI_SOCKET

#ifdef _WIN32
	#ifdef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501
	#endif

	#include <winsock2.h>

	typedef SOCKET CSocket;

	extern int initSocket();
	extern int quitSocket();

	extern int closeSocket(CSocket socket);
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>

	typedef int CSocket;

	#define INVALID_SOCKET -1

	extern int initSocket();
	extern int quitSocket();

	extern int closeSocket(CSocket socket);
#endif

#endif // SABAKURAI_SOCKET

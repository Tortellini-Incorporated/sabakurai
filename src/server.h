#ifndef SABA_SERVER
#define SABA_SERVER

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

//server

typedef struct ServerSession {
	fd_set fdSet;
	int server;
	int* clients;
	int maxClients;
	void* sessionData;
} ServerSession;

typedef enum ServerState {
	SSTATE_NORMAL		= 0,
	SSTATE_CALM_EXIT	= 1,
	SSTATE_ERROR_UNKNOWN	= -1,
	SSTATE_INVALID_SOCKET	= -2,
	SSTATE_FAILED_BIND	= -3,
	SSTATE_FAILED_LISTEN	= -4,
	SSTATE_FAILED_SELECT	= -5,
	SSTATE_FAILED_ACCEPT	= -6,
} ServerState;

typedef ServerState (*PacketRecievedCallback)(ServerSession*, int, void*, int);
/*
 * arg 1 is the session from which the packet was recieved
 * arg 2 is the index of the client which send the packet
 * arg 3 is the data from the client
 * arg 4 is the length of the packet
 */

typedef ServerState (*NewConnectionCallback)(ServerSession*, int, struct sockaddr_in);

/*
 * arg 1 is the session which the new client connected to
 * arg 2 is the index of the new client
 * arg 3 is the address of the client
 */

typedef ServerState (*DisconnectCallback)(ServerSession*, int);

/*
 * arg 1 is the session which the client disconnected from
 * arg 2 is the index of the client which disconnected
 */

ServerState createServer(ServerSession*, int capacity, int port);

ServerState updateFDSet(ServerSession*);

ServerState updateFDSetTimed(ServerSession*, struct timeval*);

ServerState checkRequests(ServerSession*, NewConnectionCallback);

ServerState checkMessages(ServerSession*, PacketRecievedCallback, DisconnectCallback);

ServerState processActivity(ServerSession*, NewConnectionCallback, PacketRecievedCallback, DisconnectCallback);

ServerState processActivityTimed(ServerSession*, int seconds, int microseconds, NewConnectionCallback, PacketRecievedCallback, DisconnectCallback);

void sendString(int socket, char* message);

#endif //SABA_SERVER

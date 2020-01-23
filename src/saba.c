#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

//server

#define MAX_CLIENTS 10

typedef struct serverSession {
	fd_set fdSet;
	int server;
	int* clients;
	int maxClients;
	void* sessionData;
} serverSession;

typedef enum serverState {
	SSTATE_NORMAL		= 0,
	SSTATE_CALM_EXIT	= 1,
	SSTATE_ERROR_UNKNOWN	= -1,
	SSTATE_INVALID_SOCKET	= -2,
	SSTATE_FAILED_BIND	= -3,
	SSTATE_FAILED_LISTEN	= -4,
	SSTATE_FAILED_SELECT	= -5,
	SSTATE_FAILED_ACCEPT	= -6,
} serverState;

typedef serverState (*packetRecievedCallback)(serverSession*, int, void*, int);
/* 
 * arg 1 is the session from which the packet was recieved 
 * arg 2 is the index of the client which send the packet
 * arg 3 is the data from the client
 * arg 4 is the length of the packet
 */

typedef serverState (*newConnectionCallback)(serverSession*, int, struct sockaddr_in);

/*
 * arg 1 is the session which the new client connected to
 * arg 2 is the index of the new client
 * arg 3 is the address of the client
 */

typedef serverState (*disconnectCallback)(serverSession*, int);

/*
 * arg 1 is the session which the client disconnected from
 * arg 2 is the index of the client which disconnected
 */

serverState createServer(serverSession* server, int capacity, int port) {
	server->server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server->server < 0) {
		return SSTATE_INVALID_SOCKET;
	}

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_port = htons(port);
	sad.sin_addr.s_addr = INADDR_ANY;
	
	int optval = 1;	
	setsockopt(server->server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

	if (bind(server->server, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
		return SSTATE_FAILED_BIND;
	}

	server->clients = calloc(capacity, sizeof(int));
	server->maxClients = capacity;

	if (listen(server->server, 15) < 0) {
		return SSTATE_FAILED_LISTEN;
	}
	return SSTATE_NORMAL;
}

serverState updateFDSet(serverSession* server) {
	FD_ZERO(&server->fdSet);

	FD_SET(server->server, &server->fdSet);

	int maxSocket = server->server;
		
	for (int i = 0; i < server->maxClients; ++i) {
		if (server->clients[i] > 0) {
			FD_SET(server->clients[i], &server->fdSet);
		}
		if (server->clients[i] > maxSocket) {
			maxSocket = server->clients[i];
		}
	}

	struct timeval timeoutPeriod;
	memset(&timeoutPeriod, 0, sizeof(timeoutPeriod));
	timeoutPeriod.tv_sec = 3;

	if (select(maxSocket + 1, &server->fdSet, NULL, NULL, &timeoutPeriod) < 0) {
		return SSTATE_FAILED_SELECT;
	}

	return SSTATE_NORMAL;
}

serverState updateFDSetTimed(serverSession* server, struct timeval* timeoutPeriod) {
	FD_ZERO(&server->fdSet);

	FD_SET(server->server, &server->fdSet);

	int maxSocket = server->server;
		
	for (int i = 0; i < server->maxClients; ++i) {
		if (server->clients[i] > 0) {
			FD_SET(server->clients[i], &server->fdSet);
		}
		if (server->clients[i] > maxSocket) {
			maxSocket = server->clients[i];
		}
	}

	if (select(maxSocket + 1, &server->fdSet, NULL, NULL, timeoutPeriod) < 0) {
		return SSTATE_FAILED_SELECT;
	}

	return SSTATE_NORMAL;
}

serverState checkRequests(serverSession* server, newConnectionCallback callback) {
	if (FD_ISSET(server->server, &server->fdSet)) {
		struct sockaddr_in socketAddress;
		socklen_t addressSize = sizeof(socketAddress);

		int newSocket = accept(server->server, (struct sockaddr*)&socketAddress, &addressSize);
		if (newSocket < 0) {
			fprintf(stderr, "failed to accept request");
			return SSTATE_FAILED_ACCEPT;
		}
		int i;
		for (i = 0; i < server->maxClients; ++i) {
			if (server->clients[i] == 0) {
				server->clients[i] = newSocket;
				break;
			}
		}

		/*handle the connection*/
		if (callback) {
			callback(server, i, socketAddress);
		}
		//fprintf(stdout, "incoming connection from %s (id %d)\n", inet_ntoa(socketAddress.sin_addr), newSocket);
	}
	return SSTATE_NORMAL;
}

serverState checkMessages(serverSession* server, packetRecievedCallback recCallback, disconnectCallback dcCallback) {
	static char buffer[513];
	for (int i = 0; i < server->maxClients; ++i) {
		if (FD_ISSET(server->clients[i], &server->fdSet)) {
			int bytesRead = read(server->clients[i], buffer, 512);
			if (!bytesRead) {
				if (dcCallback) {
					dcCallback(server, i);
				}
				close(server->clients[i]);
				server->clients[i] = 0;
			} else {
				buffer[bytesRead] = '\0';
				if (recCallback) {
					recCallback(server, i, buffer, bytesRead);
				}
			}
		}
	}
	return SSTATE_NORMAL;
}

serverState processActivity(serverSession* server, newConnectionCallback ncCallback, packetRecievedCallback npCallback, disconnectCallback dcCallback) {
	serverState ret;
	ret = updateFDSet(server);
	if (ret) {
		return ret;
	}
	ret = checkRequests(server, ncCallback);
	if (ret) {
		return ret;
	}
	ret = checkMessages(server, npCallback, dcCallback);
	return ret;
}

serverState processActivityTimed(serverSession* server, int seconds, int microseconds, newConnectionCallback ncCallback, packetRecievedCallback npCallback, disconnectCallback dcCallback) {
	struct timeval period;
	serverState ret;
	period.tv_sec = seconds;
	period.tv_usec = microseconds;
	
	while (period.tv_sec + period.tv_usec > 0) {
		ret = updateFDSetTimed(server, &period);
		if (ret) {
			return ret;
		}
		ret = checkRequests(server, ncCallback);
		if (ret) {
			return ret;
		}
		ret = checkMessages(server, npCallback, dcCallback);
		if (ret) {
			return ret;
		}
	}
	return SSTATE_NORMAL;
}

void sendString(int socket, char* message) {
	int bytesSent = send(socket, message, strlen(message), 0);
	if (bytesSent != strlen(message)) {
		fprintf(stderr, "error: sent %d/%d bytes", bytesSent, strlen(message));
	}
}

typedef struct player {
	char* name;
	int progress;
} player;

typedef struct gameData {
	player players[MAX_CLIENTS];
	int numPlayers;
	int numReady;
	int currentState;// 0 = waiting for players to be ready, 1 = ingame
} gameData;

serverState packetRecievedCB(serverSession* server, int client, void* data, int length) {
	gameData* session = (gameData*) server->sessionData;
	while (length > 0) {
		if (session->players[client].name == 0) {//player announcing name
			int nameLength = *((int*) data) & 0xFF;
			session->players[client].name = malloc(sizeof(char) * (nameLength + 1));
			memcpy(session->players[client].name, ((char*)data) + 1, nameLength * sizeof(char));
			session->players[client].name[nameLength] = 0;
			printf("debug: %s (index %d) announced name (message length: %d, itta %d)\n", session->players[client].name, client, length, nameLength);
			length -= nameLength + 1;
			++session->numPlayers;
			continue;
		}
		if (session->currentState) {
			
		} else {
			char msg = *(char*) data;
			if (msg == 'r') {
				printf("debug: %s is now ready\n", session->players[client].name);
				session->players[client].progress = 0;
				++session->numReady;
			} else if (msg = 'u') {
				printf("debug: %s is no longer ready\n", session->players[client].name);
				session->players[client].progress = -1;
				--session->numReady;
			}
			--length;
		}
	}
	return SSTATE_NORMAL;
}

serverState newConnectionCB(serverSession* server, int client, struct sockaddr_in adr) {
	printf("debug: new connection %d\n", client);
	gameData* session = (gameData*) server->sessionData;
	session->players[client].name = 0;
	session->players[client].progress = -1;
}

serverState disconnectCB(serverSession* server, int client) {
	gameData* session = (gameData*) server->sessionData;
	if (session->currentState) {
		
	} else {
		if (session->players[client].name) {
			--session->numPlayers;
			if (!session->players[client].progress) {
				--session->numReady;
			}
		}
	}
}

int main(int argc, char** argv) {
	serverSession server;

	server.sessionData = calloc(MAX_CLIENTS, sizeof(player));

	createServer(&server, MAX_CLIENTS, PORT);

	printf("server starting\n");

	int interval = 5;

	while (1) {
		if (processActivityTimed(&server, 3, 0, newConnectionCB, packetRecievedCB, disconnectCB)) {
			fprintf(stderr, "error: exiting\n");
			return 1;
		}
		--interval;
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (server.clients[i]) {
				fprintf(stdout, "debug: sending to %d interval %d\n", server.clients[i], interval);
				if (interval > 0) {
					sendString(server.clients[i], "yo!");
				} else {
					sendString(server.clients[i], "quit");
				}
			}
		}
		if (interval == 0) {
			interval = 5;
		}
	}
	
	return 0;
}

/* TODO
 *
 * callbacks
 * better error handling
 */

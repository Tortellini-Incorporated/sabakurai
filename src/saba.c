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

int createServer(serverSession* server, int capacity, int port) {
	server->server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server->server < 0) {
		return 1;
	}

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_port = htons(port);
	sad.sin_addr.s_addr = INADDR_ANY;
	
	int optval = 1;	
	setsockopt(server->server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

	if (bind(server->server, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
		return 2;
	}

	server->clients = calloc(capacity, sizeof(int));
	server->maxClients = capacity;

	if (listen(server->server, 15) < 0) {
		return 3;
	}
	return 0;
}

int updateFDSet(serverSession* server) {
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
		fprintf(stderr, "error: failed activity?\n");
		return 1;
	}

	printf("debug: %d seconds and %d microseconds\n", timeoutPeriod.tv_sec, timeoutPeriod.tv_usec);
	printf("debug: about %f seconds total\n", (float) timeoutPeriod.tv_sec + ((float)(timeoutPeriod.tv_usec) / 1000000.0));

	return 0;
}

int updateFDSetTimed(serverSession* server, struct timeval* timeoutPeriod) {
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
		fprintf(stderr, "error: failed activity?\n");
		return 1;
	}

	return 0;
}

int checkRequests(serverSession* server) {
	if (FD_ISSET(server->server, &server->fdSet)) {
		struct sockaddr_in socketAddress;
		socklen_t addressSize = sizeof(socketAddress);

		int newSocket = accept(server->server, (struct sockaddr*)&socketAddress, &addressSize);
		if (newSocket < 0) {
			fprintf(stderr, "failed to accept request");
			return -1;
		}

		for (int i = 0; i < server->maxClients; ++i) {
			if (server->clients[i] == 0) {
				server->clients[i] = newSocket;
				break;
			}
		}

		/*handle the connection*/
		fprintf(stdout, "incoming connection from %s (id %d)\n", inet_ntoa(socketAddress.sin_addr), newSocket);
	}
	return 0;
}

int checkMessages(serverSession* server) {
	static char buffer[513];
	for (int i = 0; i < server->maxClients; ++i) {
		if (FD_ISSET(server->clients[i], &server->fdSet)) {
			int bytesRead = read(server->clients[i], buffer, 512);
			if (!bytesRead) {
				fprintf(stdout, "client id %d disconnected\n", server->clients[i]);
				close(server->clients[i]);
				server->clients[i] = 0;
			} else {
				buffer[bytesRead] = '\0';
				fprintf(stdout, "client id %d sent message of length %d: %s\n", server->clients[i], bytesRead, buffer);
			}
		}
	}
	return 0;
}

int processActivity(serverSession* server) {
	if (updateFDSet(server)) {
		fprintf(stderr, "error: idk man, but you fucked up something");
		return -1;
	}
	checkRequests(server);
	checkMessages(server);
}

int processActivityTimed(serverSession* server, int seconds, int microseconds) {
	struct timeval period;
	period.tv_sec = seconds;
	period.tv_usec = microseconds;
	
	while (period.tv_sec + period.tv_usec > 0) {
		if (updateFDSetTimed(server, &period)) {
			fprintf(stderr, "error: idk man, but you fucked up something");
			return -1;
		}
		checkRequests(server);
		checkMessages(server);
	}
		
}

void sendString(int socket, char* message) {
	int bytesSent = send(socket, message, strlen(message), 0);
	if (bytesSent != strlen(message)) {
		fprintf(stderr, "error: sent %d/%d bytes", bytesSent, strlen(message));
	}
}

int main(int argc, char** argv) {
	serverSession server;

	createServer(&server, MAX_CLIENTS, PORT);

	printf("server starting\n");

	int interval = 5;

	while (1) {
		processActivityTimed(&server, 3, 0);
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

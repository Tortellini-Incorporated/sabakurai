#include "server.h"

//server

ServerState createServer(ServerSession* server, int capacity, int port) {
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

ServerState updateFDSet(ServerSession* server) {
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

ServerState updateFDSetTimed(ServerSession* server, struct timeval* timeoutPeriod) {
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

ServerState checkRequests(ServerSession* server, NewConnectionCallback callback) {
	if (FD_ISSET(server->server, &server->fdSet)) {
		struct sockaddr_in socketAddress;
		socklen_t addressSize = sizeof(socketAddress);

		int newSocket = accept(server->server, (struct sockaddr*)&socketAddress, &addressSize);
		if (newSocket < 0) {
			fprintf(stderr, "failed to accept request\n");
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

ServerState checkMessages(ServerSession* server, PacketRecievedCallback recCallback, DisconnectCallback dcCallback) {
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

ServerState processActivity(ServerSession* server, NewConnectionCallback ncCallback, PacketRecievedCallback npCallback, DisconnectCallback dcCallback) {
	ServerState ret;
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

ServerState processActivityTimed(ServerSession* server, int seconds, int microseconds, NewConnectionCallback ncCallback, PacketRecievedCallback npCallback, DisconnectCallback dcCallback) {
	struct timeval period;
	ServerState ret;
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
		fprintf(stderr, "error: sent %d/%d bytes\n", bytesSent, (int) strlen(message));
	}
}

void sendPacket(int socket, void* message, int size) { 
	int bytesSent = send(socket, message, size, 0);
	if (bytesSent != size) {
		fprintf(stderr, "error: sent %d/%d bytes\n", bytesSent, size);
	}
}

void broadcastPacket(ServerSession* server, void* message, int size) {
	for (int i = 0; i < server->maxClients; ++i) {
		if (server->clients[i]) {
			sendPacket(server->clients[i], message, size);
		}
	}
}

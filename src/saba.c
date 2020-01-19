#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

//server

#define MAX_CLIENTS 10

int manageNewConnection(int);

int updateFDSet(fd_set* fdSet, int serverFD, int* clientSockets, int maxClients) {
	FD_ZERO(fdSet);

	FD_SET(serverFD, fdSet);

	int maxSocket = serverFD;
		
	for (int i = 0; i < maxClients; ++i) {
		if (clientSockets[i] > 0) {
			FD_SET(clientSockets[i], fdSet);
		}
		if (clientSockets[i] > maxSocket) {
			maxSocket = clientSockets[i];
		}
	}

	struct timeval timeoutPeriod;
	memset(&timeoutPeriod, 0, sizeof(timeoutPeriod));
	timeoutPeriod.tv_sec = 3;

	if (select(maxSocket + 1, fdSet, NULL, NULL, &timeoutPeriod) < 0) {
		fprintf(stderr, "error: failed activity?\n");
		return 1;
	}
	return 0;
}

int checkRequests(fd_set* fdSet, int serverFD, int* clientSockets, int maxClients) {
	if (FD_ISSET(serverFD, fdSet)) {
		struct sockaddr_in socketAddress;
		socklen_t addressSize = sizeof(socketAddress);

		static char* greetingMessage = "d.nnr cbkae.p\n\r";

		int newSocket = accept(serverFD, (struct sockaddr*)&socketAddress, &addressSize);
		if (newSocket < 0) {
			fprintf(stderr, "failed to accept request");
			return -1;
		}

		for (int i = 0; i < maxClients; ++i) {
			if (clientSockets[i] == 0) {
				clientSockets[i] = newSocket;
				break;
			}
		}

		/*handle the connection*/
		fprintf(stdout, "incoming connection from %s (id %d)\n", inet_ntoa(socketAddress.sin_addr), newSocket);
		int sendval = send(newSocket, greetingMessage, strlen(greetingMessage), 0);
		if (sendval != strlen(greetingMessage)) {
			fprintf(stdout, "partial or failed message send, error %d", sendval);
			return -2;
		}
	}
	return 0;
}

int checkMessages(fd_set* fdSet, int serverFD, int* clientSockets, int maxClients) {
	static char buffer[513];
	for (int i = 0; i < maxClients; ++i) {
		if (FD_ISSET(clientSockets[i], fdSet)) {
			int bytesRead = read(clientSockets[i], buffer, 512);
			if (!bytesRead) {
				fprintf(stdout, "client id %d disconnected\n", clientSockets[i]);
				close(clientSockets[i]);
				clientSockets[i] = 0;
			} else {
				buffer[bytesRead] = '\0';
				fprintf(stdout, "client id %d sent message of length %d: %s\n", clientSockets[i], bytesRead, buffer);
			}
		}
	}
	return 0;
}

void sendString(int socket, char* message) {
	int bytesSent = send(socket, message, strlen(message), 0);
	if (bytesSent != strlen(message)) {
		fprintf(stderr, "error: sent %d/%d bytes", bytesSent, strlen(message));
	}
}

int main(int argc, char** argv) {

	char buffer[513];

	int serverFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverFD < 0) {
		fprintf(stderr, "error: failed to create socket\n");
		return 1;
	}
	printf("socket created!\n");

	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_port = htons(PORT);
	sad.sin_addr.s_addr = INADDR_ANY;
	
	int optval = 1;	
	setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

	if (bind(serverFD, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
		fprintf(stderr, "error: socket failed to bind\n");
		return 1;
	}

	fd_set readFD;
	int clientSockets[MAX_CLIENTS];
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		clientSockets[i] = 0;
	}
	if (listen(serverFD, 15) < 0) {
		fprintf(stderr, "error: failed listen, get ears m8\n");
		return 1;
	}
	printf("server starting\n");

	int interval = 5;
	
	char* messageA = "yo!";

	char* messageB = "quit";

	while (1) {
		if (updateFDSet(&readFD, serverFD, clientSockets, MAX_CLIENTS)) {
			fprintf(stderr, "error: idk man, but you fucked up something");
			break;
		}
		checkRequests(&readFD, serverFD, clientSockets, MAX_CLIENTS);
		checkMessages(&readFD, serverFD, clientSockets, MAX_CLIENTS);
		--interval;
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (clientSockets[i]) {
				fprintf(stdout, "debug: sending to %d\n", clientSockets[i]);
				if (interval > 0) {
					sendString(clientSockets[i], "yo!");
				} else {
					sendString(clientSockets[i], "quit");
				}
			}
		}
		if (interval == 0) {
			interval = 5;
		}
	}
	
	return 0;
}

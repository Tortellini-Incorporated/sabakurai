#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

//server

#define MAX_CLIENTS 10

int manageConnection(int);

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
	int activity;

	socklen_t addressSize = sizeof(sad);
	while (1) {
		FD_ZERO(&readFD);

		FD_SET(serverFD, &readFD);

		int maxFD = serverFD;
		
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (clientSockets[i] > 0) {
				FD_SET(clientSockets[i], &readFD);
			}
			if (clientSockets[i] > maxFD) {
				maxFD = clientSockets[i];
			}
		}

		activity = select(maxFD + 1, &readFD, NULL, NULL, NULL);

		if (activity < 0) {
			fprintf(stderr, "error: failed activity?\n");
			//cleanexit
		}

		if (FD_ISSET(serverFD, &readFD)) {
			int newSocket = accept(serverFD, (struct sockaddr*)&sad, &addressSize);
			if (newSocket < 0) {
				fprintf(stderr, "failed to accept request");
			}

			char* greetingMessage = "d.nnr cbkae.p\n\r";

			fprintf(stdout, "incoming connection from %s (id %d)\n", inet_ntoa(sad.sin_addr), newSocket);
			int sendval = send(newSocket, greetingMessage, strlen(greetingMessage), 0);
			if (sendval != strlen(greetingMessage)) {
				fprintf(stdout, "partial or failed message send, error %d", sendval);
			}
			for (int i = 0; i < MAX_CLIENTS; ++i) {
				if (clientSockets[i] == 0) {
					clientSockets[i] = newSocket;
					break;
				}
			}
		}

		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (FD_ISSET(clientSockets[i], &readFD)) {
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

	}
		
	/*struct sockaddr client;
	int clientlen = sizeof(client);
	int clientConnection = accept(serverFD, &client, &clientlen);
	if (clientConnection < 0) {
		fprintf(stderr, "failed to accept client connection\n");
		return 1;
	}
	printf("connected to a client\n");
	manageConnection(clientConnection);
	close(clientConnection);
	shutdown(serverFD, SHUT_RDWR);
	close(serverFD);*/
	
	return 0;
}

int manageConnection(int fileDescriptor) {
	const char* message = "ANK ME HARDER";
	if (send(fileDescriptor, message, strlen(message) + 1, 0) < 0) {
		perror("Erro");
	}
}

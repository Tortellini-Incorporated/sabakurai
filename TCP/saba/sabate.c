#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

//server

int manageConnection(int);

int main(int argc, char** argv) {
	int serverFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverFD >= 0) {
		printf("socket created!\n");

		struct sockaddr_in sad;
		memset(&sad, 0, sizeof(sad));
		sad.sin_family = AF_INET;
		sad.sin_port = htons(20000);
		sad.sin_addr.s_addr = INADDR_ANY;
		int optval = 1;	
		setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

		if (bind(serverFD, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
			fprintf(stderr, "socket failed to bind\n");
			return 1;
		}

		listen(serverFD, 15);
		struct sockaddr client;
		int clientlen = sizeof(client);
		int clientConnection = accept(serverFD, &client, &clientlen);
		if (clientConnection < 0) {
			fprintf(stderr, "failed to accept client connection\n");
			return 1;
		}
		printf("connected to a client\n");
		manageConnection(serverFD);
		close(clientConnection);
		shutdown(serverFD, SHUT_RDWR);
		close(serverFD);
	} else {
		fprintf(stderr, "failed to create socket\n");
		return 1;
	}
	
	return 0;
}

int manageConnection(int fileDescriptor) {
	const char* message = "ANK ME HARDER";
	write(fileDescriptor, message, strlen(message) + 1);
}

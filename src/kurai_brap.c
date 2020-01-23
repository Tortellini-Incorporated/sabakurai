#include <stdio.h>
#include <string.h>
#include <time.h>

#include "socket.h"

#ifdef _WIN32
	#include <windows.h>
	
	void csleep(unsigned millis) {
		Sleep(millis);
	}
#else
	#include <unistd.h>

	void csleep(unsigned millis) {
		usleep(millis * 1000);
	}
#endif

void timer(Socket socket, double millis);
int tryRecieving(Socket socket);

//client
int main(int argc, char** argv) {
	/*if (argc != 2) {
		fprintf(stderr, "please specify an ip to connect to\n");
		return 1;
	}
	int socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFD >= 0) {
		printf("socket created!\n");
		struct sockaddr_in sad = {
			AF_INET, 
			htons(PORT), 
			inet_addr(argv[1])};
		int connection = connect(socketFD, (struct sockaddr*) &sad, sizeof(sad));
		if (connection < 0) {
			fprintf(stderr, "there was an error with message %d w/ connection\n", connection);
			return 1;
		}
		printf("connected");
	char buffer[64];
		int numBytes = read(socketFD, buffer, 64);
		printf("message of length %d recieved: %s\n", numBytes, buffer);
		char* line = 0;
		size_t length = 0;
		do {
			getline(&line, &length, stdin);
			write(socketFD, line, strlen(line) - 1);
		} while (line[0] != 'q');
	} else {
		printf("failed to create socket\n");
	}
	close(socketFD);*/
	if (argc < 2) {
		fprintf(stderr, "Please provide a server ip\n");
		return -1;
	}
	Socket sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		fprintf(stderr, "Failed to create socket");
		return -1;
	}
	printf("Socket created successfully\n");
	struct sockaddr_in serverAddress = {
		AF_INET,
		htons(PORT),
		inet_addr(argv[1])
	};
	printf("Trying to connect\n");
	if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
		fprintf(stderr, "Failed to connect\n");
		return -1;
	}
	printf("Connected to server\n");
	timer(sock, 3000);
	close(sock);
	return 0;
}

void timer(Socket socket, double millis) {
	clock_t lastTime = clock();
	int looping = 1;
	csleep((unsigned) (millis * 0.95));
	while (looping) {
		if ((float) (clock() - lastTime) / CLOCKS_PER_SEC * 1000 > millis) {
			looping = tryRecieving(socket);
			if (looping != 2) {
				csleep((unsigned) (millis * 0.95));
				lastTime = clock();
			}
		}
	}
}

int tryRecieving(Socket socket) {
	printf("Recieving...\n");
	fd_set input;
	FD_ZERO(&input);
	FD_SET(socket, &input);
	if (FD_ISSET(socket, &input)) {
		int read = 0;
		char buffer[256];
		if ((read = recv(socket, buffer, 255, 0)) == -1) {
			fprintf(stderr, "Error reading from server\n");
		}
		buffer[read] = 0;
		printf("%s\n", buffer);
		if (strcmp(buffer, "quit") == 0) {
			return 0;
		}
		return 1;
	} else {
		return 2;
	}
}


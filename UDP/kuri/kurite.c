#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

//client

#define MAX_MESSAGE_SIZE 128

char* getMessage(int, struct sockaddr_in*);

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "error: please specify an ip to connect to and a message\n");
		return 1;
	}
	int socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketFD >= 0) {
		printf("debugger: socket created!\n");
		struct sockaddr_in sad = {
			AF_INET, 
			htons(20000), 
			inet_addr(argv[1])};
		sendto(socketFD, argv[2], strlen(argv[2]), MSG_CONFIRM, (struct sockaddr*)&sad, sizeof(sad));
		char* message = getMessage(socketFD, &sad);
		printf(" - %s\n", message);
		free(message);
	} else {
		fprintf(stderr, "error: failed to create socket\n");
	}
	close(socketFD);
	return 0;
}

char* getMessage(int fd, struct sockaddr_in* serverAddr) {
	int messageLength;
	char* ret = malloc(sizeof(char) * MAX_MESSAGE_SIZE);
	printf("debugger: appeasing slab\n");
	int size = recvfrom(fd, ret, MAX_MESSAGE_SIZE, MSG_WAITALL, (struct sockaddr*)serverAddr, &messageLength);
	ret[size] = '\0';
	printf("debugger: length: %d, size: %d\n", messageLength, size);
	return ret;
}

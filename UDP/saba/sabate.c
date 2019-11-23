// UDP Sabate Test
// Author: Gnarwhal

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int main() {
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("Faiwed to cweate socket xwx\n");
		return -1;
	}
	printf("Socket cweated successfuwwy\n");

	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;

	memset(&serverAddress, 0, sizeof(struct sockaddr_in));
	memset(&clientAddress, 0, sizeof(struct sockaddr_in));
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(20000);

	if (bind(sock, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) < 0) {
		printf("Faiwed to bind socky wocky\n");
		return -1;
	}
	printf("Yay! Socket was bouwnd!\n");

	char buffer[512];
	int addressLength = sizeof(struct sockaddr_in);
	int bytesRecieved;

	bytesRecieved = recvfrom(sock, buffer, 511, MSG_WAITALL, (struct sockaddr *) &clientAddress, &addressLength); 
	buffer[bytesRecieved] = '\0';

	printf("Recieved message from client '%s'\n", inet_ntoa(clientAddress.sin_addr));
	printf("Client be like: %s\n", buffer);

	const char * MESSAGE = "Hewwo C1FW1 uwu";
	if (sendto(sock, MESSAGE, strlen(MESSAGE), MSG_CONFIRM, (struct sockaddr *) &clientAddress, addressLength) < 0) {
		printf("Oh no! Message lost in twansit!\n");
		return -1;
	}

	printf("Message swent to cwient uwu\n");

	close(sock);

	return 0;
}


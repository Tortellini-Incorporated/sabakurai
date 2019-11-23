#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

//client
int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "please specify an ip to connect to\n");
		return 1;
	}
	int socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFD >= 0) {
		printf("socket created!\n");
		struct sockaddr_in sad = {
			AF_INET, 
			htons(20000), 
			inet_addr(argv[1])};
		int connection = connect(socketFD, (struct sockaddr*) &sad, sizeof(sad));
		if (connection < 0) {
			fprintf(stderr, "there was an error with message %d w/ connection\n", connection);
			return 1;
		}
		char buffer[64];
		int numBytes = read(socketFD, buffer, 64);
		printf("message of length %d recieved: %s\n", numBytes, buffer);
	} else {
		printf("failed to create socket\n");
	}
	close(socketFD);
	printf("ank me daddy\n");
	return 0;
}

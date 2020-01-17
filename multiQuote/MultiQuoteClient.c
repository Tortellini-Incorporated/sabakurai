//
//  MultiQuoteClient
//
//  Created by <insert name> 
//  Username: <inser username>
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>

#define kQUOTEPORT      1717
#define kMULTIQUOTEPORT 1818

void readMessage(int socket_fd, char* buffer) {
	int num_bytes = read(socket_fd, buffer, 512);
	int i;
	int found_end = 0;

	while (!found_end) {
		for (i = 0; i < num_bytes; ++i) {
			if (buffer[i] == 13 || buffer[i] == 10) {
				buffer[i] = '\0';
				printf("%s\n", buffer);
				found_end = 1;
				break;
			}
		}
		if (!found_end) {
			buffer[i] = '\0';

			num_bytes = read(socket_fd, buffer, 512);
		}
	}
}

int main(int argc, const char * argv[])
{
	int socket_fd;
	struct sockaddr_in sad;
	struct hostent* saba_addr;
	int connection;

	long numQuotes;
	char* garbage;

	char buffer[513];

	const char* message_another = "ANOTHER\r\n";
	const char* message_close = "CLOSE\r\n";
	if (argc != 3) {
		return -1;
	}

	numQuotes = strtol(argv[2], &garbage, 10);

	if (numQuotes <= 0) {
		return 0;
	}

	socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (socket_fd < 0) {
		return -2;
	}
	
	sad = (struct sockaddr_in) {
		AF_INET,
		htons(kMULTIQUOTEPORT),
		
	};
	sad.sin_family = AF_INET;
	sad.sin_port = htons(kMULTIQUOTEPORT);
	saba_addr = gethostbyname(argv[1]);
	memcpy(&sad.sin_addr, saba_addr->h_addr, saba_addr->h_length);
	connection = connect(socket_fd, (struct sockaddr*) &sad, sizeof(sad));
	
	if (connection < 0) {
		return -3;
	}
	
	readMessage(socket_fd, buffer);
	--numQuotes;
	while (numQuotes) {
		write(socket_fd, message_another, 10);
		readMessage(socket_fd, buffer);
		--numQuotes;
	}
	write(socket_fd, message_close, 8);

	return 0;
}

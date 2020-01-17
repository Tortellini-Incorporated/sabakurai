//
//  MultiQuoteServer
//
//  Created by my cats
//  Username: f/a;leokm;oZma'owp/z;fompaewfs
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

int index_of(char* string, int length, char character) {
	int i;
	for (i = 0; i < length; ++i) {
		if (string[i] == character) {
			return i;
		}
	}
	return -1;
}

int parse_quotes(const char* path, char*** target) {
	int ret = -1;
	FILE* fp;
	char buffer[512];
	int i;
	int j;
	
	fp = fopen(path, "r");
	while (!feof(fp)) {
		if (fread(buffer, sizeof(char), 1, fp)) {
			if (*buffer == '\n') {
				++ret;
			}
		}
	}
	fseek(fp, 0, SEEK_SET);
	*target = malloc(sizeof(char*) * ret);
	for (i = 0; i < ret; ++i) {
		int total_read = 0;
		while (1) {
			int num_read = fread(buffer, sizeof(char), 512, fp);
			int ind_of_end = index_of(buffer, num_read, '\n');
			if (ind_of_end != -1) {
				if (total_read == 0) {
					(*target)[i] = malloc(sizeof(char) * (ind_of_end + 2));
					memcpy((*target)[i], buffer,
							sizeof(char) * (ind_of_end + 1));
					(*target)[i][ind_of_end] = '\r';
					(*target)[i][ind_of_end + 1] = '\n';
				} else {
					char* temp = (*target)[i];
					(*target)[i] = malloc(
							sizeof(char) * (ind_of_end + 2 + total_read));
					memcpy((*target)[i], temp, sizeof(char) * total_read);
					memcpy((*target)[i] + total_read, buffer,
							sizeof(char) * (ind_of_end + 1));
					(*target)[i][ind_of_end] = '\r';
					(*target)[i][ind_of_end + 1] = '\n';
					free(temp);
				}
				fseek(fp, ind_of_end - num_read + 1, SEEK_CUR);
				break;
			} else {
				if (total_read == 0) {
					(*target)[i] = malloc(sizeof(char) * num_read);
					memcpy((*target)[i], buffer,
							sizeof(char) * num_read);
					total_read = num_read;
				} else {
					char* temp = (*target)[i];
					(*target)[i] = malloc(
							sizeof(char) * (ind_of_end + 2 + total_read));
					memcpy((*target)[i], temp, sizeof(char) * total_read);
					memcpy((*target)[i] + total_read, buffer,
							sizeof(char) * (ind_of_end + 1));
					total_read += num_read;
					free(temp);
				}
			}
		}
	}
	fclose(fp);
	return ret;
}

int get_message_type(char* message, int length) {
	int i = 0;
	if (length >= 7) {
		if (
				message[0] == 'A' &&
				message[1] == 'N' &&
				message[2] == 'O' &&
				message[3] == 'T' &&
				message[4] == 'H' &&
				message[5] == 'E' &&
				message[6] == 'R'
		   ) {
			return 1;
		} else if (
				message[0] == 'C' &&
				message[1] == 'L' &&
				message[2] == 'O' &&
				message[3] == 'S' &&
				message[4] == 'E'
				) {
			return 0;
		}
	} else if (length >= 5) {
		if (
				message[0] == 'C' &&
				message[1] == 'L' &&
				message[2] == 'O' &&
				message[3] == 'S' &&
				message[4] == 'E'
				) {
			return 0;
		}
	}
	return -1;
}

void sendMessage(int fd, int index, char** quotes) {
	int remaining = index_of(quotes[index], RAND_MAX, '\n');
	int passed = 0;
	while (remaining > 512) {
		write(fd, quotes[index] + passed, 512 * sizeof(char));
		passed += 512;
	}
	write(fd, quotes[index] + passed, (remaining + 1) * sizeof(char));
}

int main(int argc, const char * argv[])
{
    int socket_fd;
	struct sockaddr_in sad;
	struct sockaddr_in cad;
	struct hostent* saba_addr;
	char** quotes;
	int quote_num;

	int current_quote = 0; 

	const char* message_error = "ERROR\r\n";
	const char* message_heck_off = "BYE\r\n";

	if (argc != 2) {
		return -1;
	}

	quote_num = parse_quotes(argv[1], &quotes);

	if (quote_num <= 0) {
		return 0;
	}

	socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (socket_fd < 0) {
		return -2;
	}
	
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_port = htons(kMULTIQUOTEPORT);
	sad.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_fd, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		return -3;
	}

	if (listen(socket_fd, 8) < 0) {
		return -4;
	}
	
	while (1) {
		int client_structure_size = sizeof(cad);
		int client_fd = accept(socket_fd,
				(struct sockaddr*) &cad,
				&client_structure_size);
		if (client_fd < 0) {
			return -5;
		}
		sendMessage(client_fd, current_quote, quotes);
		++current_quote;
		while (1) {
			char buffer[512];
			int cml = read(client_fd, buffer, 512);
			int type = get_message_type(buffer, cml);
			if (type == -1) {
				write(client_fd, message_error, 7);
			} else if (type == 1) {
				sendMessage(client_fd, current_quote, quotes);
			} else if (type == 0) {
				write(client_fd, message_heck_off, 5); 
				break;
			}
			++current_quote;
			if (current_quote >= quote_num) {
				current_quote = 0;
			}
		}
		close(client_fd);
	}
    return 0;
}

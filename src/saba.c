#include "server.h"

//server

#define MAX_CLIENTS 10
#define NUM_STRINGS 856
#define MAX_FILE_SIZE 2048

typedef struct Player {
	char* name;
	int progress;
	unsigned int finishTime;
} Player;

typedef struct GameData {
	Player players[MAX_CLIENTS];
	int numPlayers;
	int numReady;
	int currentState;// 0 = waiting for Players to be ready, 1 = ingame
} GameData;

int getRandomStringMesg(char* buffer) {
	int numToRead = rand() % NUM_STRINGS;
	char titleBuffer[32];
	sprintf(titleBuffer, "texts/%d\0", numToRead);
	
	FILE* file = fopen(titleBuffer, "r");
	if (!file) {
		printf("invalid file %d\n", numToRead);
		return -1;
	}

	printf("reading file %d\n", numToRead);

	int size = fread(buffer + 3, 1, MAX_FILE_SIZE, file);
	buffer[0] = 3;
	buffer[1] = size >> 8;
	buffer[2] = size & 0x00FF;
	return size + 3;
}

void startGame(ServerSession* server) {
	GameData* session = (GameData*) server->sessionData;
	session->currentState = 1;
	for (int i = 0; i < session->numPlayers; ++i) {
		session->players[i].progress = 0;
	}
	char buffer[MAX_FILE_SIZE];
	int size = getRandomStringMesg(buffer);
	broadcastPacket(server, buffer, size);
}

void updatePlayers(ServerSession* server) {
	GameData* session = (GameData*) server->sessionData;
	char* buffer = malloc(session->numPlayers * 6 + 1);
	buffer[0] = 4;
	int offset = 1;
	for (int i = 0; i < server->maxClients; ++i) {
		if (server->clients[i]) {
			buffer[offset] = i;
			if (session->players[i].finishTime > 0) {
				buffer[offset + 1] = 1;
				buffer[offset + 2] = session->players[i].finishTime >> 24;
				buffer[offset + 3] = session->players[i].finishTime >> 16;
				buffer[offset + 4] = session->players[i].finishTime >> 8;
				buffer[offset + 5] = session->players[i].finishTime & 0xFF;
				offset += 6;
			} else if (session->players[i].progress >= 0) {
				buffer[offset + 1] = 0;
				buffer[offset + 2] = session->players[i].progress >> 8;
				buffer[offset + 3] = session->players[i].progress & 0xFF;
				offset += 4;
			} else if (session->players[i].progress = -1) {
				buffer[offset + 1] = 2;
				offset += 2;
			}
		}
	}
	broadcastPacket(server, buffer, offset);
	free(buffer);
}

ServerState packetRecievedCB(ServerSession* server, int client, void* data, int length) {
	GameData* session = (GameData*) server->sessionData;
	printf("debug: parsing message from [%s], message [%s]\n", session->players[client].name, data);
	while (length > 0) {
		if (session->players[client].name == 0) {//Player announcing name
			int nameLength = *((int*) data) & 0xFF;
			session->players[client].name = malloc(sizeof(char) * (nameLength + 1));
			memcpy(session->players[client].name, ((char*)data) + 1, nameLength * sizeof(char));
			session->players[client].name[nameLength] = 0;
			printf("debug: [%s] (index %d) announced name\n", session->players[client].name, client);

			char* msgData = malloc(2 + nameLength);
			msgData[0] = 0;
			msgData[1] = client;
			msgData[2] = (char) nameLength;
			memcpy(msgData + 3, session->players[client].name, nameLength);
			broadcastPacket(server, msgData, 3 + nameLength);
			free(msgData);

			length -= nameLength + 1;
			data = ((char*) data) - (nameLength + 1);
			++session->numPlayers;
			continue;
		}
		if (session->currentState) {
			char type = *(char*) data;
			if (type == 0) {
				session->players[client].progress = ((char*) data)[1] << 8 + ((char*) data)[2];
			} else if (type == 1) {
				session->players[client].finishTime = ((char*) data)[1] << 24 + ((char*) data)[2] << 16 + ((char*) data)[3] + ((char*) data)[4] << 8 + ((char*) data)[5];
			} else {
				data = (char*) data - 1;
				--length;
			}
		} else {
			char msg = *(char*) data;
			if (msg == 'r' && !session->players[client].progress) {
				printf("debug: %s is now ready\n", session->players[client].name);
				char sdata[2];
				sdata[0] = 1;
				sdata[1] = client;
				broadcastPacket(server, sdata, 2);
				session->players[client].progress = 0;
				++session->numReady;
				if (session->numReady == session->numPlayers && session->numPlayers > 0) {
					startGame(server);
				}

			} else if (msg == 'u' && session->players[client].progress) {
				printf("debug: %s is no longer ready\n", session->players[client].name);
				char sdata[2];
				sdata[0] = 1;
				sdata[1] = client;
				broadcastPacket(server, sdata, 2);
				session->players[client].progress = -1;
				--session->numReady;
			}
			data = ((char*) data) - 1;
			--length;
		}
	}
	return SSTATE_NORMAL;
}

ServerState newConnectionCB(ServerSession* server, int client, struct sockaddr_in adr) {
	printf("debug: new connection %d\n", client);
	GameData* session = (GameData*) server->sessionData;
	session->players[client].name = 0;
	session->players[client].progress = -1;
}

ServerState disconnectCB(ServerSession* server, int client) {
	GameData* session = (GameData*) server->sessionData;
	printf("debug: [%s] disconnected\n", session->players[client]);
	if (session->currentState) {
		
	} else {
		if (session->players[client].name) {
			--session->numPlayers;
			if (!session->players[client].progress) {
				--session->numReady;
			}
		}
	}
}

int main(int argc, char** argv) {
	ServerSession server;
	GameData sessionData;

	server.sessionData = &sessionData;
	memset(&sessionData, 0, sizeof(sessionData));

	createServer(&server, MAX_CLIENTS, PORT);
	
	time_t t;
	srand((unsigned) time(&t));

	printf("server starting\n");

	int interval = 5;

	while (1) {
		if (processActivityTimed(&server, 0, 16666, newConnectionCB, packetRecievedCB, disconnectCB)) {
			fprintf(stderr, "error: exiting\n");
			return 1;
		}
		if (sessionData.currentState) {
			updatePlayers(&server);
		}
	}

	return 0;
}

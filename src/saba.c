#include "server.h"
#include "packetHandlers.h"
#include "gameSession.h"
#include "sabaSettings.h"

//server

int getRandomStringMesg(char* buffer) {
	int numToRead = rand() % NUM_STRINGS;
	char titleBuffer[32];
	sprintf(titleBuffer, "texts/%d", numToRead);
	
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
	buffer[0] = 3;
	int size = getRandomStringMesg(buffer + 3);
	buffer[1] = (size >> 8) & 0xFF;
	buffer[2] = size & 0xFF;
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
				buffer[offset + 2] = (session->players[i].finishTime >> 24) & 0xFF;
				buffer[offset + 3] = (session->players[i].finishTime >> 16) & 0xFF;
				buffer[offset + 4] = (session->players[i].finishTime >> 8) & 0xFF;
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
	printf("debug: parsing message from [%s], message [%s] length %d\n", session->players[client].name, (char*) data, length); 
	int bytesRead = 0;
	while (length > 0) {
		if (session->players[client].name == 0) {//Player announcing name
			bytesRead = phOnConnect(server, session, client, data);
		} else if (session->currentState) {// in game
			char type = *(char*) data;
			printf("debug: in game, checking message type, type [%d]\n", type);
			if (type == 2) {//SEND_PROGRESS
				bytesRead = phSendProgress(server, session, client, data);
			} else if (type == 3) {//COMPLETED_TEXT
				bytesRead = phCompletedText(server, session, client, data);
			} else if (type == 4) {//EXIT_TO_LOBBY
				bytesRead = phExitToLobby(server, session, client, data);
			}
		} else {// waiting
			char msg = *(char*) data;
			printf("debug: waiting, checking message type, type [%d]\n", msg);
			if (msg == 0) {//TOGGLE_READY
				bytesRead = phToggleReady(server, session, client, data);
			} else if (msg == 1) {//CHANGE_NAME
				bytesRead = phChangeName(server, session, client, data);
			} else if (msg == 5) {//SEND_MESSAGE
				bytesRead = phSendMessage(server, session, client, data);
			}
		}
		if (bytesRead < 1) {
			printf("error: failed to parse message\n");
			return SSTATE_NORMAL;
		} else {
			data = ((char*) data) + bytesRead;
			length -= length;
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
	printf("debug: [%s] disconnected\n", session->players[client].name);
	if (session->currentState) {
		//was I going to do something here?
	} else {
		if (session->players[client].name) {
			--session->numPlayers;
			if (!session->players[client].progress) {
				--session->numReady;
			}
			char packet_data[2];
			packet_data[0] = 2;
			packet_data[1] = client;
			broadcastPacket(server, packet_data, 2);
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
	sessionData.startTimer = -1;

	while (1) {
		if (processActivityTimed(&server, 0, 16666, newConnectionCB, packetRecievedCB, disconnectCB)) {
			fprintf(stderr, "error: exiting\n");
			return 1;
		}
		if (sessionData.currentState) {
			updatePlayers(&server);
		}
		if (sessionData.numReady == sessionData.numPlayers && sessionData.numPlayers > 0) {
			--sessionData.startTimer;
		} else {
			sessionData.startTimer = -1;
		}
		if (sessionData.startTimer == 0) {
			startGame(&server);
		}
	}

	return 0;
}

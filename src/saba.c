#include "server.h"
#include "packetHandlers.h"
#include "gameSession.h"
#include "sabaSettings.h"

//server

int getRandomStringMesg(char* buffer) {
	int numToRead = rand() % NUM_STRINGS;
	static char titleBuffer[32];
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

void startGame(ServerSession* server, GameData* session) {
	session->currentState = 1;
	for (int i = 0; i < server->maxClients; ++i) {
		session->players[i].progress = 0;
		session->players[i].finishTime = -1;
	}
	session->startTimer = -1;
	static char buffer[MAX_FILE_SIZE];
	buffer[0] = 3;
	int size = getRandomStringMesg(buffer + 3);
	buffer[1] = (size >> 8) & 0xFF;
	buffer[2] = size & 0xFF;
	broadcastPacket(server, buffer, size);
}

void endGame(ServerSession* server, GameData* session) {
	char* packet;
	int totalMsgLen = 2;

	session->numPlayers = 0;
	session->currentState = 0;
	session->startTimer = -1;
	
	for (int i = 0; i < server->maxClients; ++i) {
		session->players[i].progress = -1;
		session->players[i].finishTime = -1;
		if (server->clients[i] && session->players[i].name) {
			++session->numPlayers;
			totalMsgLen += 3 + strlen(session->players[i].name);
		}
	}
	packet = malloc(totalMsgLen);
	packet[0] = 7;
	packet[1] = session->numPlayers;
	
	int index = 2;
	
	for (int i = 0; i < server->maxClients; ++i) {
		if (server->clients[i] && session->players[i].name) {
			int nameLen = strlen(session->players[i].name);
			packet[index++] = i;
			packet[index++] = 0;//nobody should be ready
			packet[index++] = nameLen;
			memcpy(packet + index, session->players[i].name, nameLen);
			index += nameLen;
		}
	}
	broadcastPacket(server, packet, totalMsgLen);
	free(packet);
}

ServerState packetRecievedCB(ServerSession* server, int client, void* data, int length) {
	GameData* session = (GameData*) server->sessionData;
	int bytesRead = 0;
	while (length > 0) {
		if (session->players[client].name == 0) {//Player announcing name
			bytesRead = phOnConnect(server, session, client, data);
		} else if (session->currentState) {// in game
			char msgType = *(char*) data;
			if        (msgType == 2) {//SEND_PROGRESS
				bytesRead = phSendProgress(server, session, client, data);
			} else if (msgType == 3) {//COMPLETED_TEXT
				bytesRead = phCompletedText(server, session, client, data);
			} else if (msgType == 4) {//EXIT_TO_LOBBY
				bytesRead = phExitToLobby(server, session, client, data);
			} else if (msgType == 0) {//TOGGLE_READY
				bytesRead = phToggleReady(server, session, client, data);
			} else if (msgType == 1) {//CHANGE_NAME
				bytesRead = phChangeName(server, session, client, data);
			} else if (msgType == 5) {//SEND_MESSAGE
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
	printf("debug: new connection given id [%d]\n", client);
	GameData* session = (GameData*) server->sessionData;
	session->players[client].name = 0;
	session->players[client].progress = -1;
	session->players[client].finishTime = -1;
}

ServerState disconnectCB(ServerSession* server, int client) {
	GameData* session = (GameData*) server->sessionData;
	printf("debug: [%s] disconnected\n", session->players[client].name);
	if (session->currentState) {
		char packet[2];
		packet[1] = client;
		if (session->players[client].progress >= 0) {
			packet[0] = 6;
			broadcastPacket(server, packet, 2);
		}
		packet[0] = 2;
		broadcastPacket(server, packet, 2);
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
		
		if (sessionData.startTimer > 0) {
			--sessionData.startTimer;
		}

		if (sessionData.currentState) {
			if (sessionData.startTimer == 0) {
				endGame(&server, &sessionData);
			}
		} else {
			if (sessionData.numReady != sessionData.numPlayers) {
				sessionData.startTimer = -1;
			}
			if (sessionData.startTimer == 0) {
				startGame(&server, &sessionData);
			}
		}
	}

	return 0;
}

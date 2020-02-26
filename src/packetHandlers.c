#include "packetHandlers.h"
// LOBBY

int phOnConnect(ServerSession* server, GameData* session, int client, char* data) {	// NO VALUE
	int nameLength = data[0];
	session->players[client].name = malloc(sizeof(char) * (nameLength + 1));
	memcpy(session->players[client].name, data + 1, nameLength * sizeof(char));
	session->players[client].name[nameLength] = 0;
	printf("debug: [%s] (index %d) announced name\n", session->players[client].name, client);

	char* msgData;

	int total_message_len = 2;
	for (int i = 0; i < server->maxClients; ++i) {
		if ((server->clients[i] || i == client) && session->players[i].name) {
			total_message_len += 3 + strlen(session->players[i].name);
		}
	}
	msgData = malloc(total_message_len * sizeof(char));
	msgData[0] = client;
	msgData[1] = session->numPlayers + 1;
	int index = 2;
	for (int i = 0; i < server->maxClients; ++i) {
		if ((server->clients[i] || i == client) && session->players[i].name) {
			size_t name_len = strlen(session->players[i].name) * sizeof(char);
			msgData[index++] = i;
			msgData[index++] = session->players[i].progress + 1;
			msgData[index++] = name_len;
			memcpy(msgData + index, session->players[i].name, name_len);
			index += name_len;
		}
	}
	sendPacket(server->clients[client], msgData, total_message_len * sizeof(char));

	msgData = realloc(msgData, 3 + nameLength);
	msgData[0] = 0;
	msgData[1] = client;
	msgData[2] = (char) nameLength;
	memcpy(msgData + 3, session->players[client].name, nameLength);
	broadcastPacket(server, msgData, 3 + nameLength);
	free(msgData);

	++session->numPlayers;
	return nameLength + 1;
}

int phToggleReady(ServerSession* server, GameData* session, int client, char* data) {	// 0
	if (session->players[client].progress) {
		session->players[client].progress = 0;
		++session->numReady;
	} else {
		session->players[client].progress = -1;
		--session->numReady;
	}
	printf("debug: %s toggled ready, now %d\n", session->players[client].name, session->players[client].progress + 1);
	char sdata[2];
	sdata[0] = 1;
	sdata[1] = client;
	broadcastPacket(server, sdata, 2);
	return 1;
}

int phChangeName(ServerSession* server, GameData* session, int client, char* data) {	// 1
	int nameLen = data[1];
	printf("debug: [%s] changing their name to ", session->players[client].name);
	session->players[client].name = realloc(session->players[client].name, nameLen * sizeof(char));
	memcpy(session->players[client].name, data + 2, nameLen);
	printf("[%s]\n", session->players[client].name);

	char* scratch = malloc(sizeof(char) * (nameLen + 3));
	scratch[0] = 8;
	scratch[1] = client;
	scratch[2] = nameLen;
	memcpy(scratch + 3, session->players[client].name, nameLen);
	broadcastPacket(server, scratch, (3 + nameLen) * sizeof(char));
	free(scratch);
	return nameLen + 2;
}

int phSendMessage(ServerSession* server, GameData* session, int client, char* data) {	// 5
	int msgLen = data[1] << 8;
	printf("debug: relaying message, length [%d]\n", msgLen);
	msgLen += data[2];
	char* scratch = malloc(sizeof(char) * (msgLen + 5));
	scratch[0] = 9;
	scratch[1] = client;
	scratch[2] = msgLen >> 8;
	scratch[3] = msgLen & 0xFF;
	memcpy(scratch + 4, data + 3, msgLen * sizeof(char));
	broadcastPacket(server, scratch, (4 + msgLen) * sizeof(char));
	scratch[msgLen + 4] = 0;
	printf("debug: [%s]:[%s]\n", session->players[client].name, scratch + 4);
	free(scratch);
	return 3 + msgLen;
}

// INGAME

int phSendProgress(ServerSession* server, GameData* session, int client, char* data) {	// 2
	session->players[client].progress = data[1] << 8 + data[2];
	char packet[4];
	packet[0] = 4;
	packet[1] = client;
	packet[2] = data[1];
	packet[3] = data[2];
	broadcastPacket(server, packet, 4);
	return 3;
}

int phCompletedText(ServerSession* server, GameData* session, int client, char* data) {	// 3
	session->players[client].finishTime =	data[1] << 24 +
											data[2] << 16 +
											data[3] << 8 +
											data[4];
	char packet[6];
	packet[0] = 5;
	packet[1] = client;
	packet[2] = data[1];
	packet[3] = data[2];
	packet[4] = data[3];
	packet[5] = data[4];
	broadcastPacket(server, packet, 6);
	if (session->startTimer < 0) {
		session->startTimer = 3600;//60 seconds
	}
	return 5;
}

int phExitToLobby(ServerSession* server, GameData* session, int client, char* data) {	// 4
	session->players[client].progress = -1;
	char packet[2];
	packet[0] = 6;
	packet[1] = client;
	broadcastPacket(server, packet, 2);
	return 1;
}

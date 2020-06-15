#include "packetHandlers.h"
// LOBBY

int phOnConnect(ServerSession* server, GameData* session, int client, char* data) {	// NO VALUE
	session->players[client].color = data[0];
	int nameLength = data[1];
	session->players[client].name = malloc(sizeof(char) * (nameLength + 1));
	memcpy(session->players[client].name, data + 2, nameLength * sizeof(char));
	session->players[client].name[nameLength] = 0;
	printf("debug: [%s] (index %d) announced name\n", session->players[client].name, client);
	if (session->currentState) {
		session->players[client].spectator = 1;
		++session->numSpectators;
	}

	char* msgData;

	int total_message_len = 3 + (7 + session->text_size) * session->currentState;
	for (int i = 0; i < server->maxClients; ++i) {
		if ((server->clients[i] || i == client) && session->players[i].name) {
			total_message_len += 5 + 5 * session->currentState + strlen(session->players[i].name);
		}
	}
	msgData = malloc(total_message_len * sizeof(char));
	msgData[0] = session->currentState;
	msgData[1] = client;
	if (session->currentState) {
		msgData[2] = session->winner;
		msgData[3] = (session->winnerTime >> 24) & 0xFF;
		msgData[4] = (session->winnerTime >> 16) & 0xFF;
		msgData[5] = (session->winnerTime >>  8) & 0xFF;
		msgData[6] = (session->winnerTime >>  0) & 0xFF;
		msgData[7] = (session->text_size  >>  8) & 0xFF;
		msgData[8] = (session->text_size  >>  0) & 0xFF;
		memcpy(msgData + 9, session->text, session->text_size);
		msgData[9 + session->text_size] = session->numPlayers + 1;
	} else {
		msgData[2] = session->numPlayers + 1;
	}
	int index = 3 + (7 + session->text_size) * session->currentState;
	for (int i = 0; i < server->maxClients; ++i) {
		if ((server->clients[i] || i == client) && session->players[i].name) {
			size_t name_len = strlen(session->players[i].name) * sizeof(char);
			msgData[index++] = i;
			msgData[index++] = session->players[i].spectator;
			msgData[index++] = session->players[i].color;
			msgData[index++] = name_len;
			memcpy(msgData + index, session->players[i].name, name_len);
			index += name_len;
			if (session->currentState) {
				msgData[index++] = (session->players[i].progress   >>  8) & 0xFF;
				msgData[index++] = (session->players[i].progress   >>  0) & 0xFF;
				msgData[index++] = (session->players[i].finishTime >> 24) & 0xFF;
				msgData[index++] = (session->players[i].finishTime >> 16) & 0xFF;
				msgData[index++] = (session->players[i].finishTime >>  8) & 0xFF;
				msgData[index++] = (session->players[i].finishTime >>  0) & 0xFF;
			} else {
				msgData[index++] = session->players[i].progress + 1;
			}
		}
	}
	sendPacket(server->clients[client], msgData, total_message_len * sizeof(char));

	msgData = realloc(msgData, 4 + nameLength);
	msgData[0] = 0;
	msgData[1] = client;
	msgData[2] = data[0];
	msgData[3] = (char) nameLength;
	memcpy(msgData + 4, session->players[client].name, nameLength);
	broadcastPacket(server, msgData, 4 + nameLength);
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

int phToggleSpectate(ServerSession* server, GameData* session, int client, char* data) {
	session->numSpectators += -2 * session->players[client].spectator + 1;
	session->players[client].spectator = !session->players[client].spectator;
	printf("debug: %s toggled spectator, now %d\n", session->players[client].name, session->players[client].spectator);
	char sdata[2];
	sdata[0] = 10;
	sdata[1] = client;
	broadcastPacket(server, sdata, 2);
	return 1;
}

int phChangeName(ServerSession* server, GameData* session, int client, char* data) {	// 1
	int nameLen = data[1];
	printf("debug: [%s] changing their name to ", session->players[client].name);
	session->players[client].name = realloc(session->players[client].name, nameLen * sizeof(char) + 1);
	memcpy(session->players[client].name, data + 2, nameLen);
	session->players[client].name[nameLen] = '\0';
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
	int msgLen  = data[1] << 8;
	    msgLen += data[2];
	printf("debug: relaying message, length [%d]\n", msgLen);
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

int phDisconnect(ServerSession* server, GameData* session, int client, char* data) { // 7
	printf("debug: [%s] disconnected\n", session->players[client].name);
	if (session->players[client].name) {
		--session->numPlayers;
		if (!session->currentState && !session->players[client].progress) {
			--session->numReady;
		} else if (session->currentState && session->winner == client) {
			session->winner     = -1;
			session->winnerTime = -1;
		}
		if (session->players[client].spectator) {
			--session->numSpectators;
		}
		free(session->players[client].name);
		char packet_data[2];
		packet_data[0] = 2;
		packet_data[1] = client;
		broadcastPacket(server, packet_data, 2);
		close(server->clients[client]);
		server->clients[client] = 0;
	}
	return 1;
}

int phTextSet(ServerSession* server, GameData* session, int client, char* data) { // 8
	unsigned char * udata = data;
	data[0] = 13;
	int data_count = 0;
	if (data[1] == 0) {
		data_count = 3;
	} else if (data[1] == 1) {
		session->text_size = udata[2] << 8 | udata[3];
		data_count = 4;
	}
	if (data_count) {
		broadcastPacket(server, data, data_count);
	}
	data[0] = 8;
	return 1;
}

int phTimeout(ServerSession* server, GameData* session, int client, char* data) {
	unsigned char * udata = data;
	session->timeout = udata[1] << 8 | udata[2];
	udata[0] = 14;
	broadcastPacket(server, data, 3);
	udata[0] = 9;
	return 2;
}

int phColor(ServerSession* server, GameData* session, int client, char* data) {
	session->players[client].color = data[1];
	char packet[3];
	packet[0] = 15;
	packet[1] = client;
	packet[2] = data[1];
	broadcastPacket(server, packet, 3);
	return 1;
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
	int time = data[1] << 24 +
	           data[2] << 16 +
	           data[3] <<  8 +
	           data[4];
	session->players[client].finishTime = time;

	char packet[6];
	packet[0] = 5;
	packet[1] = client;
	packet[2] = data[1];
	packet[3] = data[2];
	packet[4] = data[3];
	packet[5] = data[4];
	broadcastPacket(server, packet, 6);
	if (session->startTimer < 0) {
		session->startTimer = (int) (session->timeout * 1000000.0 / 16666.0);
	}
	if (session->winner == -1 || time < session->winnerTime) {
		session->winner     = client;
		session->winnerTime = time;
	}
	++session->numCompleted;
	return 5;
}

/* int phExitToLobby(ServerSession* server, GameData* session, int client, char* data) {	// 4
	session->players[client].progress = -1;
	char packet[2];
	packet[0] = 6;
	packet[1] = client;
	broadcastPacket(server, packet, 2);
	return 1;
} */

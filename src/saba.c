#include "server.h"

//server

#define MAX_CLIENTS 10

typedef struct Player {
	char* name;
	int progress;
} Player;

typedef struct GameData {
	Player players[MAX_CLIENTS];
	int numPlayers;
	int numReady;
	int currentState;// 0 = waiting for Players to be ready, 1 = ingame
} GameData;

ServerState packetRecievedCB(ServerSession* server, int client, void* data, int length) {
	GameData* session = (GameData*) server->sessionData;
	printf("debug: parsing message from [%s], message [%s]", session->players[client].name, data);
	while (length > 0) {
		if (session->players[client].name == 0) {//Player announcing name
			int nameLength = *((int*) data) & 0xFF;
			session->players[client].name = malloc(sizeof(char) * (nameLength + 1));
			memcpy(session->players[client].name, ((char*)data) + 1, nameLength * sizeof(char));
			session->players[client].name[nameLength] = 0;
			printf("debug: [%s] (index %d) announced name (message length: %d, itta %d)\n", session->players[client].name, client, length, nameLength);
			length -= nameLength + 1;
			data = ((char*) data) - (nameLength + 1);
			++session->numPlayers;
			continue;
		}
		if (session->currentState) {
			fprintf(stderr, "error: invalid game state\n");
			data = ((char*) data) - 1;
			--length;
		} else {
			char msg = *(char*) data;
			if (msg == 'r') {
				printf("debug: %s is now ready\n", session->players[client].name);
				session->players[client].progress = 0;
				++session->numReady;
			} else if (msg = 'u') {
				printf("debug: %s is no longer ready\n", session->players[client].name);
				session->players[client].progress = -1;
				--session->numReady;
			}
			data = ((char*) data) - 1;
			--length;
		}
	}
	printf("debug: done parsing message\n");
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

	printf("server starting\n");

	int interval = 5;

	while (1) {
		if (processActivityTimed(&server, 3, 0, newConnectionCB, packetRecievedCB, disconnectCB)) {
			fprintf(stderr, "error: exiting\n");
			return 1;
		}
		--interval;
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (server.clients[i]) {
				fprintf(stdout, "debug: sending to %d interval %d\n", server.clients[i], interval);
				if (interval > 0) {
					sendString(server.clients[i], "yo!");
				} else {
					sendString(server.clients[i], "quit");
				}
			}
		}
		if (interval == 0) {
			interval = 5;
		}
	}

	return 0;
}

/* TODO
 *
 * callbacks
 * better error handling
 */

#ifndef SABA_PACKET_HANDLERS
#define SABA_PACKET_HANDLERS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "gameSession.h"

/*
packet function:
int phPacketName(ServerSession*, GameData*, int, char*);
return value is bytes read;
further arguments are special cases.
*/

// LOBBY

int phOnConnect(ServerSession* server, GameData* session, int client, char* data);	// NO VALUE

int phToggleReady(ServerSession* server, GameData* session, int client, char* data); // 0

int phChangeName(ServerSession* server, GameData* session, int client, char* data);	// 1

int phSendMessage(ServerSession* server, GameData* session, int client, char* data);	// 5

// INGAME

int phSendProgress(ServerSession* server, GameData* session, int client, char* data);	// 2

int phCompletedText(ServerSession* server, GameData* session, int client, char* data);	// 3

int phExitToLobby(ServerSession* server, GameData* session, int client, char* data);	// 4

#endif

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

int phOnConnect(ServerSession* server, GameData* session, int client, char* data);	    // NO VALUE

int phToggleReady(ServerSession* server, GameData* session, int client, char* data);    // 0

int phToggleSpectate(ServerSession* server, GameData* session, int client, char* data); // 6

int phChangeName(ServerSession* server, GameData* session, int client, char* data);	    // 1

int phSendMessage(ServerSession* server, GameData* session, int client, char* data);	// 5

int phDisconnect(ServerSession* server, GameData* session, int client, char* data);     // 7

int phTextSet(ServerSession* server, GameData* session, int client, char* data);        // 8

int phTimeout(ServerSession* server, GameData* session, int client, char* data);        // 9

int phColor(ServerSession* server, GameData* session, int client, char* data);          // 10

// INGAME

int phSendProgress(ServerSession* server, GameData* session, int client, char* data);	// 2

int phCompletedText(ServerSession* server, GameData* session, int client, char* data);	// 3

// int phExitToLobby(ServerSession* server, GameData* session, int client, char* data);	// 4

#endif

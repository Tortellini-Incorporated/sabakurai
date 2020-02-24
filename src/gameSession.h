#ifndef SABA_GAME_SESSION
#define SABA_GAME_SESSION

#include "sabaSettings.h"

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
	int startTimer;
} GameData;

#endif

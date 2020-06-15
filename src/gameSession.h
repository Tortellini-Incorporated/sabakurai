#ifndef SABA_GAME_SESSION
#define SABA_GAME_SESSION

#include "sabaSettings.h"

typedef struct Player {
	char* name;
	int progress;
	int spectator;
	int color;
	unsigned int finishTime;
} Player;

typedef struct GameData {
	Player players[MAX_CLIENTS];
	int numPlayers;
	int numReady;
	int numSpectators;
	int numCompleted;
	int currentState;// 0 = waiting for Players to be ready, 1 = ingame
	int startTimer;
	int timeout;
	int text_size;
	char text[MAX_FILE_SIZE];
	int winner;
	unsigned int winnerTime;
} GameData;

#endif

# Sabakurai

The race for bass (and bagel-sama)

## Network Communication Documentation

Packet data is represented as a struct. The structs are not used in code. They serve only to show the layout of the data in a packet. All integers are sent in big endian.

## Network Communication Documentation - Server -> Client

These are the messages sent to the client by the server

### ON_CONNECT

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct OnConnect {
	uint8_t client_id;             // The id assigned to the client that just connected
	uint8_t player_count;          // The number of players already connected to the server
	Player  players[player_count]; // The list of information for already connected players excluding the player who just connected
};//server should tell the client if there is a game currently going on desho

struct Player {
	uint8_t id;                // The id of the player
	uint8_t ready;             // Whether the player is ready or not
	uint8_t name_length;       // The length of the player's name in bytes
	uint8_t name[name_length]; // The name of the player
};
```

### Waiting

The first byte of every message indicates the type of message it is

| TYPE                            | VALUE |
|---------------------------------|-------|
| [CONNECT](#CONNECT)             | 0     |
| [TOGGLE_READY](#TOGGLE_READY)   | 1     |
| [DISCONNECT](#DISCONNECT)       | 2     |
| [START](#START)                 | 3     |
| [UPDATE_NAME](#UPDATE_NAME)     | 8     |
| [RELAY_MESSAGE](#RELAY_MESSAGE) | 9     |

#### CONNECT

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct Connect {
	const MessageType type = CONNECT;	// 0
	uint8_t id;							// The id of the player connecting
	uint8_t name_length;				// The length of the player's name in bytes
	uint8_t name[name_length];			// The player's name
};
```

#### TOGGLE_READY

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct ToggleReady {
	const MessageType type = TOGGLE_READY;	// 1
	uint8_t id;								// The id of the player that changed their ready state
};
```

#### DISCONNECT

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct Disconnect {
	const MessageType type = DISCONNECT;	// 2
	uint8_t id;								// The id of the player that disconnected
};
```

#### START

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct Start {
	const MessageType type = START;	// 3
	uint16_t messageLength;
	char message[messageLength];
}
```

#### UPDATE_NAME

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct UpdateName {
	const MessageType type = UPDATE_NAME;	// 8
	uint8_t id;								// id of the player who changed their name
	uint8_t length;							// length of the players new name
	char newName[length];					// new name of the player
}
```

#### RELAY_MESSAGE

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct RelayMessage { 
	const MessageType type = RELAY_MESSAGE;	// 9
	uint8_t id;								// id of the player who sent the message
	uint16_t length;						// length of the data
	char message[length];					// message
}
```

### Playing

| TYPE                                  | VALUE |
|---------------------------------------|-------|
| [UPDATE_PROGRESS](#UPDATE_PROGRESS)   | 4     |
| [PLAYER_COMPLETED](#PLAYER_COMPLETED) | 5     |
| [PLAYER_EXIT_GAME](#PLAYER_EXIT_GAME) | 6     |
| [GAME_OVER](#GAME_OVER)               | 7     |

#### UPDATE_PROGRESS

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct UpdateProgress { //CURRENT IMPLEMENTATION ONLY, TO BE DEPRECATED
	const MessageType type = UPDATE_PROGRESS;
	uint8_t id;				//user who made progress
	uint8_t state;			//0 = in game, 1 = completed, 2 = waiting
	uint16_t chracterIndex; //character they are typing
}

struct UpdateProgress {	//not implemented yet
	const MessageType type = UPDATE_PROGRESS;	// 4
	uint8_t id;									// player who made progress
	uint16_t progress;							// new index they are at
}
```

#### PLAYER_COMPLETED

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct PlayerCompleted {
	const MessageType type = PLAYER_COMPLETED;	// 5
	uint8_t id;									//
	uint32_t time;								//
}
```

#### PLAYER_EXIT_GAME

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct PlayerExitGame {
	const MessageType type = PLAYER_EXIT_GAME;	// 6
	uint8_t id;									//
}
```

#### GAME_OVER

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct GameOver {
	const MessageType type = GAME_OVER;	// 7
	uint8_t numPlayers;					// number of players in the lobby
	Player players[numPlayers];			// this array includes _all_ players, including the one recieving the message
}
```

for more details on the Player struct see [ON_CONNECT](#ON_CONNECT)


## Network Communication Documentation - Client -> Server

These are the messages sent to the server by the client

### On Connect

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct OnConnect {
	uint8_t name_length;       // The length of the client's name in bytes
	uint8_t name[name_length]; // The client's name
};
```

### Waiting

The first byte of every message indicates the type of message it is

| TYPE                          | VALUE |
|-------------------------------|-------|
| [TOGGLE_READY](#TOGGLE_READY) | 0     |
| [CHANGE_NAME](#CHANGE_NAME)   | 1     |
| [SEND_MESSAGE](#SEND_MESSAGE) | 5     |

#### TOGGLE_READY

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct ToggleReady {
	const MessageType type = TOGGLE_READY;	// 0
};
```

#### CHANGE_NAME

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct ChangeName {
	const MessageType type = CHANGE_NAME;	// 1
	uint8_t name_length;					// The length of the client's name in bytes
	uint8_t name[name_length];				// The name of the client
};
```

#### SEND_MESSAGE

 - [x] saba implemented
 - [ ] kurai imeplemented

```cpp
struct SendMessage {
	const MessageType type = SEND_MESSAGE;	// 5
	uint16_t length;						//
	char message[length];					//
}
```

### Playing

| TYPE                              | VALUE |
|-----------------------------------|-------|
| [SEND_PROGRESS](#SEND_PROGRESS)   | 2     |
| [COMPLETED_TEXT](#COMPLETED_TEXT) | 3     |
| [EXIT_TO_LOBBY](#EXIT_TO_LOBBY)   | 4     |

#### SEND_PROGRESS

- [ ] saba implemented
- [ ] kurai impelemented

```cpp
struct SendProgress {
	const MessageType = SEND_PROGRESS;	// 2
	uint16_t characterIndex;			// 
};
```

#### COMPLETED_TEXT

- [ ] saba implemented
- [ ] kurai implemented

```cpp
struct CompletedText {
	const MessageType = COMPLETED_TEXT;	// 3
	uint32_t time;						// 
}
```

#### EXIT_TO_LOBBY

- [ ] saba implemented
- [ ] kurai implemented

```cpp
struct ExitToLobby {
	const MessageType = EXIT_TO_LOBBY;	// 4
}
```


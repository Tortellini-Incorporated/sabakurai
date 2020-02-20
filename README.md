###### Sabakurai

The race for bass (and bagel-sama)

## Network Communication Documentation

Packet data is represented as a struct. The structs are not used in code. They serve only to show the layout of the data in a packet. All integers are sent in big endian.

## Network Communication Documentation - Server -> Client

These are the messages sent to the client by the server

### ON_CONNECT

 - [x] saba implemented
 - [ ] kurai implemented

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

| TYPE                          | VALUE |
|-------------------------------|-------|
| [CONNECT](#CONNECT)           | 0     |
| [TOGGLE_READY](#TOGGLE_READY) | 1     |
| [DISCONNECT](#DISCONNECT)     | 2     |
| [START](#START)               | 3     |

#### CONNECT

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct Connect {
	const MessageType type = CONNECT;
	uint8_t id;                // The id of the player connecting
	uint8_t name_length;       // The length of the player's name in bytes
	uint8_t name[name_length]; // The player's name
};
```

#### TOGGLE_READY

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct ToggleReady {
	const MessageType type = TOGGLE_READY;
	uint8_t id; // The id of the player that changed their ready state
};
```

#### DISCONNECT

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct Disconnect {
	const MessageType type = DISCONNECT;
	uint8_t id; // The id of the player that disconnected
};
```

#### START

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct Start {
	const MessageType type = START;
	uint16_t messageLength;
	char message[messageLength];
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

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct UpdateProgress { //CURRENT IMPLEMENTATION ONLY, TO BE DEPRECATED
	const MessageType type = UPDATE_PROGRESS;
	uint8_t id;				//user who made progress
	uint8_t state;			//0 = in game, 1 = completed, 2 = waiting
	uint16_t chracterIndex; //character they are typing
}
```

#### PLAYER_COMPLETED

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct PlayerCompleted {
	const MessageType type = PLAYER_COMPLETED;
	uint8_t id;
	uint32_t time;
}
```

#### PLAYER_EXIT_GAME

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct PlayerExitGame {
	const MessageType type = PLAYER_EXIT_GAME;
	uint8_t id;
}
```

#### GAME_OVER

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct GameOver {
	const MessageType type = GAME_OVER;
	uint8_t numPlayers;					// number of players in the lobby
	Player players[numPlayers];			// this array includes _all_ players, including the one recieving the message
}
```

for more details on the Player struct see [ON_CONNECT](#ON_CONNECT)


## Network Communication Documentation - Client -> Server

These are the messages sent to the server by the client

### On Connect

 - [x] saba implemented
 - [ ] kurai implemented

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

#### TOGGLE_READY

 - [x] saba implemented
 - [ ] kurai implemented

```cpp
struct ToggleReady {
	const MessageType type = TOGGLE_READY;
};
```

#### CHANGE_NAME

 - [ ] saba implemented
 - [ ] kurai implemented

```cpp
struct ChangeName {
	const MessageType type = CHANGE_NAME;
	uint8_t name_length;					// The length of the client's name in bytes
	uint8_t name[name_length];				// The name of the client
};
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
	const MessageType = SEND_PROGRESS;
	uint16_t characterIndex;
};
```

#### COMPLETED_TEXT

- [ ] saba implemented
- [ ] kurai implemented

```cpp
struct CompletedText {
	const MessageType = COMPLETED_TEXT;
	uint32_t time;
}
```

#### EXIT_TO_LOBBY

- [ ] saba implemented
- [ ] kurai implemented

```cpp
struct ExitToLobby {
	const MessageType = EXIT_TO_LOBBY;
}
```


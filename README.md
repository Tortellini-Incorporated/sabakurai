###### Sabakurai

The race for bass (and bagel-sama)

## Network Communication Documentation

Packet data is represented as a struct. The structs are not used in code. They serve only to show the layout of the data in a packet. 

## Network Communication Documentation - Server -> Client

These are the messages sent to the client by the server

### On Connect

```cpp
struct OnConnect {
	uint8_t client_id;             // The id assigned to the client that just connected
	uint8_t player_count;          // The number of players already connected to the server
	Player  players[player_count]; // The list of information for already connected players
};

struct Player {
	uint8_t id;                // The id of the player
	uint8_t ready;             // Whether the player is ready or not
	uint8_t name_length;       // The length of the player's name in bytes
	uint8_t name[name_length]; // The name of the player
};
```

### Waiting

The first byte of every message indicates the type of message it is

| TEXT                          | VALUE |
|:-----------------------------:|:-----:|
| [CONNECT](#CONNECT)           | 0     |
| [TOGGLE_READY](#TOGGLE_READY) | 1     |
| [DISCONNECT](#DISCONNECT)     | 2     |
| [START](#START)               | 3     |

```cpp
enum MessageType : uint8_t {
	CONNECT      = 0, // Sent when a new player connects to the server
	TOGGLE_READY = 1, // Sent when a player changes their ready status
	DISCONNECT   = 2, // Sent when a player disconnects from the server
	START        = 3, // Sent when the game is starting
};
```

#### CONNECT

```cpp
struct Connect {
	const MessageType type = CONNECT;
	uint8_t id;                // The id of the player connecting
	uint8_t name_length;       // The length of the player's name in bytes
	uint8_t name[name_length]; // The player's name
};
```

#### TOGGLE_READY

```cpp
struct ToggleReady {
	const MessageType type = TOGGLE_READY;
	uint8_t id; // The id of the player that changed their ready state
};
```

#### DISCONNECT

```cpp
struct Disconnect {
	const MessageType type = DISCONNECT;
	uint8_t id; // The id of the player that disconnected
};
```

#### START

```cpp
struct Start {
	const MessageType type = START;
}
```

### Playing

| TEXT                                  | VALUE |
|:-------------------------------------:|       |
| [UPDATE_PROGRESS](#UPDATE_PROGRESS)   | 4     |
| [PLAYER_COMPLETED](#PLAYER_COMPLETED) | 5     |
| [PLAYER_EXIT_GAME](#PLAYER_EXIT_GAME) | 6     |
| [GAME_OVER](#GAME_OVER)               | 7     |


```cpp
enum MessageType {
	UPDATE_PROGRESS		= 4	// updating a players progress in the text
	PLAYER_COMPLETED	= 5	// a player finished the text
	PLAYER_EXIT_GAME	= 6	// a player left the session to the waiting room
	GAME_OVER			= 7	// all players done or quit
}
```

#### UPDATE_PROGRESS

```cpp
struct UpdateProgress { //CURRENT IMPLEMENTATION ONLY, TO BE DEPRECATED
	uint8_t id;				//USER THAT MADE PROGRESS
	uint8_t state;			//0 = in game, 1 = completed, 2 = waiting
	uint16_t chracterIndex; //character they are typing
	uint32_t timeStamp;		//how many ms have passed since the client recieved the start message
}
```

#### PLAYER_COMPLETED

```cpp
struct PlayerCompleted {
	/*TODO*/
}
```

#### PLAYER_EXIT_GAME

```cpp
struct PlayerExitGame {
	/*TODO*/
}
```

#### GAME_OVER

```cpp
struct GameOver {
	/*TODO*/
}
```

## Network Communication Documentation - Client -> Server

These are the messages sent to the server by the client

### On Connect

```cpp
struct OnConnect {
	uint8_t name_length;       // The length of the client's name in bytes
	uint8_t name[name_length]; // The client's name
};
```

### Waiting

The first byte of every message indicates the type of message it is

| TEXT                         | VALUE |
|:----------------------------:|       |
|[TOGGLE_READY](#TOGGLE_READY) | 0     |
|[CHANGE_NAME](#CHANGE_NAME)   | 1     |

```cpp
enum MessageType : uint8_t {
	TOGGLE_READY = 0, // Change the ready status of the client
	CHANGE_NAME  = 1, // Changes the name of the client
};
```

#### TOGGLE_READY

```cpp
struct ToggleReady {
	const MessageType type = TOGGLE_READY;
};
```

#### CHANGE_NAME

```cpp
struct ChangeName {
	const MessageType type = CHANGE_NAME;
	uint8_t name_length;       // The length of the client's name in bytes
	uint8_t name[name_length]; // The name of the client
};
```

### Playing

/*TODO*/

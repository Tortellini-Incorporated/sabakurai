# Sabakurai

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

TBD...

### Playing

TBD...

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

```cpp
enum MessageType {
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


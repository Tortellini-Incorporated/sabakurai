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
	uint8_t in_progress;           // Whether or not a game is in progress on the server
	uint8_t client_id;             // The id assigned to the client that just connected
	// ONLY PRESENT IF CURRENTLY IN GAME //
	uint8_t winner;                // The id of the player that has won (if any -1 indicates no winner yet)
	uint32_t winner_time;          // The finish time of the current winner
	uint16_t message_length;       // The length of the message to be typed
	char message[message_length];  // The message to be typed
	///////////////////////////////////////
	uint8_t player_count;          // The number of players already connected to the server
	Player  players[player_count]; // The list of information for already connected players including the player who just connected
};//server should tell the client if there is a game currently going on desho

/* IF CONNECTING TO A SERVER CURRENTLY IN THE LOBBY STATE */

struct Player {
	uint8_t id;                // The id of the player
	uint8_t spectator;         // Whether the player is a spectator or not
	uint8_t name_length;       // The length of the player's name in bytes
	uint8_t name[name_length]; // The name of the player
	uint8_t ready;             // Whether the player is ready or not
};

/* IF CONNECTING TO A SERVER IN THE PLAYING STATE */

struct Player {
	uint8_t  id;                // The if of the player
	uint8_t  spectator;         // Whether the player is a spectator or not
	uint8_t  name_length;       // The length of the player's name in bytes
	uint8_t  name[name_length]; // The name of the player
	uint16_t progress;          // The progress of the player
	uint32_t finish_time;       // The time in which the player finished in microseconds (or undefined if incomplete)
};

```

### Waiting

The first byte of every message indicates the type of message it is

| TYPE                              | VALUE |
|-----------------------------------|-------|
| [CONNECT](#CONNECT)               | 0     |
| [TOGGLE_READY](#TOGGLE_READY)     | 1     |
| [DISCONNECT](#DISCONNECT)         | 2     |
| [START](#START)                   | 3     |
| [UPDATE_NAME](#UPDATE_NAME)       | 8     |
| [RELAY_MESSAGE](#RELAY_MESSAGE)   | 9     |
| [RELAY_SPECTATE](#RELAY_SPECTATE) | 10    |
| [BEGIN_TIMER](#BEGIN_TIMER)       | 11    |
| [CANCEL_TIMER](#CANCEL_TIMER)     | 12    |
| [REDACTED]                        | 13    |
| [UPDATE_TIMEOUT](#UPDATE_TIMEOUT) | 14    |
| [UPDATE_COLOR](!UPDATE_COLOR)     | 15    |

#### CONNECT

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct Connect {
	const MessageType type = CONNECT; // 0
	uint8_t id;                       // The id of the player connecting
	uint8_t name_length;              // The length of the player's name in bytes
	uint8_t name[name_length];        // The player's name
};
```

#### TOGGLE_READY

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct ToggleReady {
	const MessageType type = TOGGLE_READY; // 1
	uint8_t id;                            // The id of the player that changed their ready state
};
```

#### DISCONNECT

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct Disconnect {
	const MessageType type = DISCONNECT; // 2
	uint8_t id;                          // The id of the player that disconnected
};
```

#### START

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct Start {
	const MessageType type = START; // 3
	uint16_t message_length;        // The length of the message to be typed
	char message[message_length];   // The message to be typed
};
```

#### UPDATE_NAME

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct UpdateName {
	const MessageType type = UPDATE_NAME; // 8
	uint8_t id;                           // The id of the player who changed their name
	uint8_t name_length;                  // The length of the players new name
	char new_name[name_length];           // The new name of the player
};
```

#### RELAY_MESSAGE

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct RelayMessage { 
	const MessageType type = RELAY_MESSAGE; // 9
	uint8_t id;                             // The id of the player who sent the message
	uint16_t message_length;                // The length of the message
	char message[message_length];           // The message
};
```

#### RELAY_SPECTATE

 - [x] saba implemented
 - [x] kurai implemented
 
```cpp
struct RelaySpectate {
	const MessageType type = RELAY_SPECTATE; // 10
	uint8_t id;                              // The id of the player changing their spectate status
};
```

#### BEGIN_TIMER

 - [x] saba implemented
 - [x] kurai implemented
 
```cpp
struct BeginTimer {
	const MessageType type = BEGIN_TIMER; // 11
	uint8_t seconds;                      // The number of seconds until the game starts
};
```

#### CANCEL_TIMER

 - [x] saba implemented
 - [x] kurai implemented
 
```cpp
struct CancelTimer {
 	const MessageType type = CANCEL_TIMER; // 12
};
```

#### UPDATE_TIMEOUT

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct UpdateTimeout {
	const MessageType type = UPDATE_TIMEOUT; // 14
	uint16_t timeout;                        // The timeout duration in seconds
};
```

#### UPDATE_COLOR

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct UpdateColor {
	const MessageType type = UPDATE_COLOR; // 15
	uint8_t id;                            // The id of the client updating their color
	uint8_t color;                         // The color they have set to
};
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
 - [x] kurai implemented

```cpp
struct UpdateProgress {
	const MessageType type = UPDATE_PROGRESS; // 4
	uint8_t id;                               // The id of the player who made progress
	uint16_t progress;                        // The new index they are at
};
```

#### PLAYER_COMPLETED

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct PlayerCompleted {
	const MessageType type = PLAYER_COMPLETED; // 5
	uint8_t id;                                // The id of the player who won
	uint32_t time;                             // The time at which they won
};
```

#### GAME_OVER

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct GameOver {
	const MessageType type = GAME_OVER; // 7
};
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
| [SPECTATE](#SPECTATE)         | 6     |
| [DISCONNECT](#DISCONNECT)     | 7     |
| [REDACTED]                    | 8     |
| [TIMEOUT](#TIMEOUT)           | 9     |
| [COLOR](#COLOR)               | 10    |

#### TOGGLE_READY

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct ToggleReady {
	const MessageType type = TOGGLE_READY; // 0
};
```

#### CHANGE_NAME

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct ChangeName {
	const MessageType type = CHANGE_NAME; // 1
	uint8_t name_length;                  // The length of the client's name in bytes
	uint8_t name[name_length];            // The name of the client
};
```

#### SEND_MESSAGE

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct SendMessage {
	const MessageType type = SEND_MESSAGE; // 5
	uint16_t message_length;               // The length of the message
	char message[message_length];          // The message
};
```

#### SPECTATE

 - [x] saba implemented
 - [x] kurai implemented
 
```cpp
struct Spectate {
	const MessageType type = SPECTATE; // 6
};
```

#### DISCONNECT

 - [x] saba implemented
 - [x] kurai implemented
 
```cpp
struct Disconnect {
	const MessageType type = DISCONNECT; // 7
	uint8_t id;                          // The id of the disconnected player
};
```

#### TIMEOUT

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct Timeout {
	const MessageType type = TIMEOUT; // 9
	uint16_t timeout;                 // The duration of the timeout in seconds
};
```

#### COLOR

 - [x] saba implemented
 - [x] kurai implemented

```cpp
struct Color {
	const MessageType type = COLOR; // 10
	uint8_t color;                  // The color being selected
};
```

### Playing

| TYPE                              | VALUE |
|-----------------------------------|-------|
| [SEND_PROGRESS](#SEND_PROGRESS)   | 2     |
| [COMPLETED_TEXT](#COMPLETED_TEXT) | 3     |

#### SEND_PROGRESS

- [x] saba implemented
- [x] kurai implemented

```cpp
struct SendProgress {
	const MessageType = SEND_PROGRESS; // 2
	uint16_t character_index;          // The index in the text the player is at
};
```

#### COMPLETED_TEXT

- [x] saba implemented
- [x] kurai implemented

```cpp
struct CompletedText {
	const MessageType = COMPLETED_TEXT; // 3
	uint32_t time;                      // The time at which the text was finished being typed
};
```


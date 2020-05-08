#ifndef SABAKURAI_SOCKET
#define SABAKURAI_SOCKET

#ifdef _WIN32
	#ifdef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501
	#endif

	#include <winsock2.h>

	using CSocket = SOCKET;

	extern int initSocket();
	extern int quitSocket();

	extern int closeSocket(CSocket socket);
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <poll.h>
	#include <unistd.h>

	using CSocket = int;

	#define INVALID_SOCKET -1

	extern int initSocket();
	extern int quitSocket();

	extern int closeSocket(CSocket socket);
#endif

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>

extern std::ofstream file;

class Socket {
	public:
		struct Flags {
			int flags;
		};

		enum Width { NONE = -1, U8 = 0, U16 = 1, U32 = 3 };
		enum Flush { FLUSH, FLUSH_LINE };

	private:
		struct SocketInfo {
			int domain;
			int type;
			int protocol;
		};

		int socket;
		SocketInfo socket_info;
		sockaddr_in mAddressInfo;
		bool connected;
		std::vector<uint8_t> message;
		Flags mFlags;
		Width mWidth;
		pollfd pollFd;

	public:
		Socket(int domain, int type, int protocol);
		Socket(const Socket & socket) = delete;

		auto operator=(const Socket & socket) -> Socket& = delete;

		auto addressInfo(const sockaddr_in & addressInfo) -> Socket&;
		auto flags(Flags flags) -> Socket&;
		auto width(Width width) -> Socket&;
		auto connect(uint32_t timeout) -> bool;
		auto close() -> void;

		auto poll(uint32_t timeout) -> bool;
		auto read()   -> uint8_t;
		auto read16() -> uint16_t;
		auto read32() -> uint32_t;

		auto operator<<(const char * str) -> Socket&;
		auto operator<<(const std::string & str) -> Socket&;
		auto operator<<(char     data) -> Socket&;
		auto operator<<( int8_t  data) -> Socket&;
		auto operator<<(uint8_t  data) -> Socket&;
		auto operator<<( int16_t data) -> Socket&;
		auto operator<<(uint16_t data) -> Socket&;	
		auto operator<<( int32_t data) -> Socket&;
		auto operator<<(uint32_t data) -> Socket&;
		auto operator<<(Flush flush)   -> Socket&;

		auto operator>>(std::string & string) -> Socket&;
		auto operator>>(std::vector<char> & vector) -> Socket&;
		auto operator>>(char & c) -> Socket&;

		~Socket();
};

#define SABAKURAI_SOCKET_FORWARD
#endif // SABAKURAI_SOCKET


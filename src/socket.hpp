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

class Socket {
	public:
		struct Flags {
			int flags;
		};

		enum Width { NONE = -1, U8 = 0, U16 = 1, U32 = 3 };
		enum Flush { FLUSH, FLUSH_LINE };

	private:
		int socket;
		sockaddr_in mAddressInfo;
		bool connected;
		std::stringstream message;
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
		auto connect() -> void;
		auto close() -> void;

		auto poll(uint32_t timeout) -> bool;
		auto read() -> uint8_t;
		template <typename T>
		auto operator<<(const T & data) -> Socket& {
			message << data;
			return *this;
		}

		auto operator<<(Flush flush) -> Socket& {
			if (flush == FLUSH_LINE) {
				message << '\n';
			}
			auto messageString = message.str();
			auto index  = 0;
			auto length = messageString.size();
			switch (mWidth) {
				case U32: messageString.insert(index, 1, uint8_t((length >> ((mWidth - index) * 8)) & 0xFF)); ++index;
				          messageString.insert(index, 1, uint8_t((length >> ((mWidth - index) * 8)) & 0xFF)); ++index;
				case U16: messageString.insert(index, 1, uint8_t((length >> ((mWidth - index) * 8)) & 0xFF)); ++index;
				case U8:  messageString.insert(index, 1, uint8_t((length >>  (mWidth - index)     ) & 0xFF));
			}
			send(socket, messageString.c_str(), messageString.size(), mFlags.flags);
			message.str(std::string());
			return *this;
		}

		auto operator>>(std::string & string) -> Socket&;
		auto operator>>(std::vector<char> & vector) -> Socket&;
		auto operator>>(char & c) -> Socket&;

		~Socket();
};

#define SABAKURAI_SOCKET_FORWARD
#endif // SABAKURAI_SOCKET


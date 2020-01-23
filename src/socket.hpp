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
	#include <unistd.h>

	using CSocket = int;

	#define INVALID_SOCKET -1

	extern int initSocket();
	extern int quitSocket();

	extern int closeSocket(CSocket socket);
#endif

#include <cstdint>
#include <string>
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
		sockaddr_in addressInfo;
		bool connected;
		std::stringstream message;
		Flags flags;
		Width width;

	public:
		Socket(int domain, int type, int protocol);
		Socket(const Socket & socket) = delete;

		auto operator=(const Socket & socket) -> Socket& = delete;

		auto setAddressInfo(const sockaddr_in & addressInfo) -> void;
		auto connect() -> void;
		auto close() -> void;

		template <typename T>
		auto operator<<(const T & data) -> Socket& {
			message << data;
			return *this;
		}

		auto operator<<(Width width) -> Socket& {
			this->width = width;
			return *this;
		}

		auto operator<<(Flags flags) -> Socket& {
			this->flags = flags;
			return *this;
		}

		auto operator<<(Flush flush) -> Socket& {
			if (flush == FLUSH_LINE) {
				message << '\n';
			}
			auto messageString = message.str();
			auto index  = -1;
			auto length = messageString.size();
			switch (width) {
				case U32: messageString.insert(index, 1, uint8_t((length >> ((width - index) * 8)) & 0xFF));
				          messageString.insert(index, 1, uint8_t((length >> ((width - index) * 8)) & 0xFF));
				case U16: messageString.insert(index, 1, uint8_t((length >> ((width - index) * 8)) & 0xFF));
				case U8:  messageString.insert(index, 1, uint8_t((length >>  (width - index)     ) & 0xFF));
			}
			send(socket, messageString.c_str(), messageString.size(), flags.flags);
			message.str(std::string());
			return *this;
		}

		~Socket();
};

#define SABAKURAI_SOCKET_FORWARD
#endif // SABAKURAI_SOCKET


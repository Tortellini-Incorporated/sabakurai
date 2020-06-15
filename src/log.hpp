#ifndef SABAKURAI_LOG
#define SABAKURAI_LOG

#include <vector>
#include <string>

#include "window.hpp"

class Log : public Window {
	protected:
		struct Message {
			uint32_t    color;
			uint32_t    author_length;
			bool        authored;
			std::string message;
		};

		std::vector<Message> log;

		auto message_height(uint32_t message_index) -> uint32_t;
		auto get_line(uint32_t message_index, std::string & write, uint32_t line) -> bool;

	public:
		uint32_t offset;

		Log();
		Log(Window & root, bool dummy);
		Log(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		auto scroll_up(uint32_t amount) -> void;
		auto scroll_down(uint32_t amount) -> void;
		auto message(const std::string & message) -> void;
		auto message(uint32_t color, const std::string & message) -> void;
		auto message(const std::string & author, const std::string & message) -> void;
		auto message(uint32_t color, const std::string & author, const std::string & message) -> void;
		virtual auto draw() -> Window&;
		~Log();
};

#define SABAKURAI_LOG_FORWARD
#endif // SABAKURAI_LOG


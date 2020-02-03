#ifndef SABAKURAI_LOG
#define SABAKURAI_LOG

#include <mutex>
#include <vector>
#include <string>

#include "window.hpp"

class Log : public Window {
	protected:
		struct Message {
			bool authored;
			std::string author;
			std::string message;
		};

		std::mutex mutex;
		std::vector<Message> log;

	public:
		Log();
		Log(Window & root, bool dummy);
		Log(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		auto message(const std::string & message) -> void;
		auto message(const std::string & author, const std::string & message) -> void;
		virtual auto draw() -> Window&;
		~Log();
};

#define SABAKURAI_LOG_FORWARD
#endif // SABAKURAI_LOG


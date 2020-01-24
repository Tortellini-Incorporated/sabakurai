#ifndef SABAKURAI_WINDOW
#define SABAKURAI_WINDOW

#include <cstdint>
#include <string>
#include <ncurses.h>
#include <sstream>

class Window {
	private:
		struct Internal {
			uint32_t x;
			uint32_t y;
			uint32_t width;
			uint32_t height;

			WINDOW * window;

			static Window root;
		} internal;

		friend class Internal;

		Window();

	public:
		static auto root() -> Window&;

		enum StreamRefresh { Refresh };

		Window(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		Window(const Window & window) = delete;
		Window(Window && window);
	
		auto operator=(const Window & window) -> Window& = delete;
		auto operator=(Window && window) -> Window&;

		auto box() -> Window&;
		auto box(const std::string & name) -> Window&;
		auto box(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> Window&;
		auto box(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const std::string & name) -> Window&;
		auto unbox() -> Window&;
		auto unbox(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> Window&;
		auto clear() -> Window&;
		
		auto move(uint32_t x, uint32_t y) -> Window&;
		
		auto put(uint32_t c) -> Window&;
		auto line_horz(uint32_t width,  uint32_t c) -> Window&;
		auto line_vert(uint32_t height, uint32_t c) -> Window&;

		template <typename T>
		auto operator<<(const T & data) -> Window& {
			auto stream = std::stringstream();
			stream << data;
			wprintw(internal.window, "%s", stream.str().c_str());
			return *this;
		}

		auto operator<<(StreamRefresh refresh) -> Window& {
			return this->refresh();
		}

		auto refresh() -> Window&;

		auto x() -> uint32_t;
		auto y() -> uint32_t;
		auto width() -> uint32_t;
		auto height() -> uint32_t;

		~Window();	
};

#define SABAKURAI_WINDOW_FORWARD
#endif // SABAKURAI_WINDOW


#ifndef SABAKURAI_WINDOW
#define SABAKURAI_WINDOW

#include <cstdint>
#include <ncurses.h>
#include <cstdarg>

class Window {
	protected:
		struct Internal {
			uint32_t x;
			uint32_t y;
			uint32_t width;
			uint32_t height;

			WINDOW * window;
			Window * root;
		} internal;

		auto root_get_char(WINDOW * window) -> uint32_t;
		virtual auto component_resize() -> void;

	public:
		auto window_resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void;

		Window();
		Window(Window & root, bool dummy);
		Window(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		Window(const Window & window) = delete;
		Window(Window && window);

		auto operator=(const Window & window) -> Window& = delete;
		auto operator=(Window && window) -> Window&;

		auto move(uint32_t x, uint32_t y) -> Window&;

		auto add_char(uint32_t c) -> Window&;
		auto print(const char * format, ...) -> Window&;
		auto horz_line(uint32_t width,  uint32_t c) -> Window&;
		auto vert_line(uint32_t height, uint32_t c) -> Window&;

		virtual auto draw() -> Window&;

		auto get_char() -> uint32_t;

		virtual auto clear() -> Window&;
		virtual auto refresh() -> Window&;

		auto x() -> uint32_t;
		auto y() -> uint32_t;
		auto width() -> uint32_t;
		auto height() -> uint32_t;
		auto window() -> WINDOW*;

		auto block(bool value = true) -> void;
		
		~Window();
};

class RootWindow : public Window {
	private:
		struct Internal {
			Window * child;
		} root_internal;

	public:
		RootWindow();
		~RootWindow();

	friend class Window;
};

#define SABAKURAI_WINDOW_FORWARD
#endif // SABAKURAI_WINDOW


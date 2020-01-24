#ifndef SABAKURAI_WINDOW
#define SABAKURAI_WINDOW

#include <cstdint>
#include <ncurses.h>
#include <cstdarg>

class Window : public Resizable {
	private:
		struct Internal {
			uint32_t x;
			uint32_t y;
			uint32_t width;
			uint32_t height;

			WINDOW * window;
		} internal;

	protected:
		Window();

	public:
		Window(RootWindow & window, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		Window(const Window & window) = delete;
		Window(Window && window);

		auto operator=(const Window & window) -> Window& = delete;
		auto operator=(Window && window) -> Window&;

		auto move(uint32_t x, uint32_t y) -> Window&;

		auto add_char(uint32_t c) -> Window&;
		auto print(const char * format, ...) -> Window&;

		auto get_char() -> uint32_t;

		auto clear() -> Window&;
		auto refresh() -> Window&;

		auto resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void;

		auto x() -> uint32_t;
		auto y() -> uint32_t;
		auto width() -> uint32_t;
		auto height() -> uint32_t;
		auto window() -> WINDOW*;

		~Window();	
};

class RootWindow : public Window {
	private:
		uint32_t c;

		auto getch() -> void;
	public:
		RootWindow();
		auto block(bool value = true) -> void;
		~RootWindow();

	friend class Window;
};		

#define SABAKURAI_WINDOW_FORWARD
#endif // SABAKURAI_WINDOW


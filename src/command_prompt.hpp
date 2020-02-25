#ifndef SABAKURAI_COMMAND_PROMPT
#define SABAKURAI_COMMAND_PROMPT

#include <string>

#include "window.hpp"

class CommandPrompt : public Window {
	protected:
		std::string command;
	
		uint32_t offset;
		uint32_t cursor_pos;
		uint32_t last_count;
		bool     is_complete;

		struct Position {
			uint32_t x;
			uint32_t y;
		};
		auto cursor_position() -> Position;

	public:
		CommandPrompt();
		CommandPrompt(Window & root, bool dummy);
		CommandPrompt(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		auto line_count() -> uint32_t;
		auto update() -> bool;
		auto complete() -> bool;
		auto height_change(uint32_t max) -> bool;
		auto get() -> std::string;
		auto clear_command() -> void;
		auto move_cursor() -> void;
		virtual auto draw() -> Window&;
		~CommandPrompt();
};

#define SABAKURAI_COMMAND_PROMPT_FORWARD
#endif // SABAKURAI_COMMAND_PROMPT


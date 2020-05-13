#include <stack>

#include "command_prompt.hpp"

auto CommandPrompt::cursor_position() -> Position {
	if (internal.width == 0) {
		return {};
	}
	auto pos   = Position{};
	auto start = uint32_t{};
	auto end   = uint32_t{};
	while (end < cursor_pos) {
		if (command[end] == '\n') {
			pos.x = 0;
			++pos.y;
			start = end + 1;
		} else if (end - start > internal.width) {
			pos.x = 0;
			++pos.y;
			start = end;
		} else {
			++pos.x;
		}
		++end;
	}
	pos.y = line_count() - pos.y - 1;
	return pos;
}

auto CommandPrompt::component_resize() -> void {
	block(false);
}

CommandPrompt::CommandPrompt() :
	Window(),
	command(),
	offset(0),
	cursor_pos(0),
	last_count(1),
	is_complete(false) {}

CommandPrompt::CommandPrompt(Window & root, bool dummy) :
	Window(root, true),
	command(),
	offset(0),
	cursor_pos(0),
	last_count(1),
	is_complete(false) {}

CommandPrompt::CommandPrompt(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, x, y, width, height),
	command(),
	offset(0),
	cursor_pos(0),
	last_count(1),
	is_complete(false) {}

auto CommandPrompt::line_count() -> uint32_t {
	auto lines = uint32_t{};
	auto start = uint32_t{};
	auto end   = uint32_t{};
	while (end <= command.size()) {
		if (end == command.size()) {
			++lines;
		} else if (command[end] == '\n') {
			++lines;
			start = end + 1;
		} else if (end - start >= internal.width) {
			++lines;
			start = end;
		}
		++end;
	}
	return lines;
}

auto CommandPrompt::update() -> bool {
	auto c = int32_t( '\0' );
	if ((c = get_char()) != ERR) {
		if (' ' <= c && c <= '~') {                                 // Printable ascii character
			command.insert(cursor_pos, 1, c);
			++cursor_pos;
		} else if (c == 0x9) {                                      // Tab
			command.insert(cursor_pos, 4, ' ');
			cursor_pos += 4;
		} else if (c == KEY_BTAB) {                                 // New line -- Shift + Tab
			command.insert(cursor_pos, 1, '\n');
			++cursor_pos;
		} else if (c == 0xA) {                                      // Enter
			is_complete = true;
		} else if (c == KEY_DC && cursor_pos < command.size()) {    // Delet
			command.erase(cursor_pos, 1);
		} else if ((c == KEY_BACKSPACE || c == 0x08) && cursor_pos > 0) {          // Backspace
			command.erase(cursor_pos - 1, 1);
			--cursor_pos;
		} else if (c == KEY_LEFT && cursor_pos > 0) {               // Move backward
			--cursor_pos;
		} else if (c == KEY_RIGHT && cursor_pos < command.size()) { // Move forward
			++cursor_pos;
		} else if (c == KEY_UP) {                                   // Move up
			if (cursor_pos > 0) {
				auto x = 1;
				auto local_cursor_pos = int32_t( cursor_pos ) - 1;
				while (local_cursor_pos > 0 && command[local_cursor_pos] != '\n') {
					--local_cursor_pos;
					++x;
				}
				if (local_cursor_pos > 0) {
					--x;
					--local_cursor_pos;
					while (local_cursor_pos > 0 && command[local_cursor_pos] != '\n') {
						--local_cursor_pos;
					}
					if (local_cursor_pos != 0) {
						++local_cursor_pos;
					}
					while (x > 0 && command[local_cursor_pos] != '\n') {
						--x;
						++local_cursor_pos;
					}
				}
				cursor_pos = local_cursor_pos;
			}
		} else if (c == KEY_DOWN) {                                 // Move down
			auto x = 1;
			auto local_cursor_pos = int32_t( cursor_pos );
			while (local_cursor_pos - x >= 0 && command[local_cursor_pos - x] != '\n') {
				++x;
			}
			--x;
			while (local_cursor_pos < command.size() && command[local_cursor_pos] != '\n') {
				++local_cursor_pos;
			}
			if (local_cursor_pos < command.size()) {
				++local_cursor_pos;
				while (x > 0 && local_cursor_pos < command.size() && command[local_cursor_pos] != '\n') {
					--x;
					++local_cursor_pos;
				}
			}
			cursor_pos = local_cursor_pos;
		} else {
			return false;
		}
		auto pos = cursor_position();
		if (pos.y < offset) {
			offset = pos.y;
		} else if (pos.y >= offset + internal.height) {
			offset = pos.y - internal.height + 1;
		}
		return true;
	}
	return false;
}

auto CommandPrompt::complete() -> bool {
	return is_complete;
}

auto CommandPrompt::height_change(uint32_t max) -> bool {
	auto lines = line_count();
	if (lines > max) {
		lines = max;
	}
	if (last_count == lines) {
		return false;
	} else {
		last_count = lines;
		return true;
	}
}

auto CommandPrompt::get() -> std::string {
	return command;
}

auto CommandPrompt::get_and_clear() -> std::string {
	auto ret = command;
	clear_command();
	return ret;
}

auto CommandPrompt::clear_command() -> void {
	is_complete = false;
	command = std::string();
	cursor_pos = 0;
	offset = 0;
}

auto CommandPrompt::move_cursor() -> void {
	auto pos = cursor_position();
	move(pos.x, internal.height - 1 - pos.y + offset);
}

auto CommandPrompt::draw() -> Window& {
	if (internal.width > 0 && internal.height > 0) {
		struct Line {
			uint32_t begin;
			uint32_t length;
		};

		auto lines = std::stack<Line>();
		auto start = uint32_t{};
		auto end   = uint32_t{};
		while (end <= command.size()) {
			if (command[end] == '\n') {
				lines.push({ start, end - start });
				start = end + 1;
			} else if (end - start >= internal.width) {
				lines.push({ start, end - start });
				start = end;
			} else if (end == command.size()) {
				lines.push({ start, end - start });
			}
			++end;
		}
		for (auto i = 0; i < offset; ++i) {
			lines.pop();
		}

		for (auto i = 0; i < internal.height; ++i) {
			move(0, i).horz_line(internal.width, ' ');
		}
		auto i = int32_t( lines.size() ) - 1;
		if (i > int32_t( internal.height ) - 1) {
			i = internal.height - 1;
		}
		for (; i >= 0; --i) {
			auto & line = lines.top();
			if (line.length > 0) {
				move(0, i).print("%s", command.substr(line.begin, line.length).c_str());
			}
			lines.pop();
		}
	}
	return *this;
}

CommandPrompt::~CommandPrompt() {}

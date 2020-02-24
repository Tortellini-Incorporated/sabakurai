#include <fstream>
#include <stack>

#include "command_prompt.hpp"

extern std::ofstream file;

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
		if (c == 0x9 || (' ' <= c && c <= '~')) {                   // Tab or a printable ascii character
			file << c << std::endl;
			command.insert(cursor_pos, 1, c);
			++cursor_pos;
		} else if (c == 0xA) {                                      // Enter
			is_complete = true;
		} else if (c == KEY_BTAB) {                                 // New line -- Shift + Tab
			command.insert(cursor_pos, 1, '\n');
			++cursor_pos;
		} else if (c == KEY_DC && cursor_pos < command.size()) {    // Delet
			command.erase(cursor_pos, 1);
		} else if (c == KEY_BACKSPACE && cursor_pos > 0) {          // Backspace
			command.erase(cursor_pos - 1, 1);
			--cursor_pos;
		} else if (c == KEY_LEFT && cursor_pos > 0) {               // Move backward
			--cursor_pos;
		} else if (c == KEY_RIGHT && cursor_pos < command.size()) { // Move forward
			++cursor_pos;
		} else {
			return false;
		}
		auto pos = cursor_position();
		if (pos.y < offset) {
			offset = pos.y;
		} else if (pos.y >= offset + internal.height) {
			offset = pos.y - internal.height + 1;
		}
		file << "Update Offset: " << offset << std::endl;
		return true;
	}
	return false;
}

auto CommandPrompt::complete() -> bool {
	return is_complete;
}

auto CommandPrompt::height_change() -> bool {
	auto lines = line_count();
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
		file << "Offset: " << offset << std::endl;
		file << "Linounoteuhont: " << i << ", " << lines.size() << std::endl;
		if (i > int32_t( internal.height ) - 1) {
			i = internal.height - 1;
		}
		file << "Linounoteuhont: " << i << ", " << lines.size() << std::endl;
		for (; i >= 0; --i) {
			auto & line = lines.top();
			if (line.length > 0) {
				file << line.begin << ", " << line.length << ": " << command.substr(line.begin, line.length) << std::endl;
				move(0, i).print("%s", command.substr(line.begin, line.length).c_str());
			}
			lines.pop();
		}
	}
	return *this;
}

CommandPrompt::~CommandPrompt() {}

#include <stack>
#include <algorithm>
#include <fstream>

#include "log.hpp"

Log::Log() :
	Window(),
	offset(0),
	log() {}

Log::Log(Window & root, bool dummy) :
	Window(root, true),
	offset(0),
	log() {}

Log::Log(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, true),
	offset(0),
	log() {}

extern std::ofstream file;

auto Log::scroll_up(uint32_t amount) -> void {
	auto lines = 0;
	for (auto i = uint32_t{ 0 }; i < log.size(); ++i) {
		auto & message = log[i];
		auto start = uint32_t{};
		auto end   = uint32_t{};
		while (end <= message.message.size()) {
			if (end == message.message.size()) {
				++lines;
			} else if (message.message[end] == '\n') {
				++lines;
				start = end + 1;
			} else if (end - start >= internal.width - 4 * (start > message.author_length)) {
				++lines;
				start = end;
			}
			++end;
		}
	}
	if (lines > internal.height) {
		offset += amount;
		if (offset > lines - internal.height) {
			offset = lines - internal.height;
		}
	}
}

auto Log::scroll_down(uint32_t amount) -> void {
	if (offset < amount) {
		offset = 0;
	} else {
		offset -= amount;
	}
}

auto Log::message(const std::string & message) -> void {
	log.push_back({ 0, uint32_t( message.size() ), false, message });
	if (offset > 0) {
		++offset;
	}
}

auto Log::message(uint32_t color, const std::string & message) -> void {
	log.push_back({ color, uint32_t( message.size() ), false, message });
	if (offset > 0) {
		++offset;
	}
}

auto Log::message(const std::string & author, const std::string & message) -> void {
	auto expanded = std::string("[").append(author).append("]: ");
	for (auto i = 0; i < message.size(); ++i) {
		auto c = message[i];
		if (c == '\t') {
			expanded.append("    ");
		} else {
			expanded.push_back(c);
		}
	}
	log.push_back({ 0, uint32_t( author.size() ), true, std::move(expanded) });
	if (offset > 0) {
		++offset;
	}
}

auto Log::message(uint32_t color, const std::string & author, const std::string & message) -> void {
	auto expanded = std::string("[").append(author).append("]: ");
	for (auto i = 0; i < message.size(); ++i) {
		auto c = message[i];
		if (c == '\t') {
			expanded.append("    ");
		} else {
			expanded.push_back(c);
		}
	}
	log.push_back({ color, uint32_t( author.size() ), true, std::move(expanded) });
	if (offset > 0) {
		++offset;
	}
}

auto Log::draw() -> Window& {
	if (internal.width > 0 && internal.height > 0) {
		struct Line {
			uint32_t message_index;
			uint32_t begin;
			uint32_t length;
			bool     indent;
		};

		auto lines = std::stack<Line>();
		for (auto i = uint32_t{ 0 }; i < log.size(); ++i) {
			auto & message = log[i];
			auto start = uint32_t{};
			auto end   = uint32_t{};
			while (++end <= message.message.size()) {
				if (message.message[end] == '\n') {
					lines.push({ i, start, end - start, start > message.author_length });
					start = ++end;
				} else if (end - start >= internal.width - 4 * (start > message.author_length)) {
					lines.push({ i, start, end - start, start > message.author_length });
					start = end;
				} else if (end == message.message.size()) {
					lines.push({ i, start, end - start, start > message.author_length });
				}
			}
		}
		for (auto i = 0; i < offset; ++i) {
			lines.pop();
		}

		for (auto i = uint32_t{ 0 }; i < internal.height; ++i) {
			move(0, i).horz_line(internal.width, ' ');
		}
		auto i = int32_t( lines.size() ) - 1;
		if (i > int32_t( internal.height ) - 1) {
			i = internal.height - 1;
		}
		auto author_length = uint32_t( 0 );
		for (; i >= 0; --i) {
			auto & line = lines.top();
			auto & message = log[line.message_index];
			if (line.begin == 0) {
				author_length = message.author_length;
				auto line_length = line.length;
				move(0, i);
				if (message.authored) {
					print("[");
					--line_length;
				}
				set_color(message.color);
				print("%s", message.message.substr(message.authored, std::min(line_length, author_length)).c_str());
				reset_color(message.color);
				if (line_length > author_length) {
					print("%s", message.message.substr(message.authored + author_length, line_length - author_length).c_str());
					author_length = 0;
				} else {
					author_length -= line_length;
				}	
			} else if (author_length > 0) {	
				auto out = std::string();
				if (line.indent) {
					out.append("    ");
				}
				out.append(message.message.substr(line.begin, line.length));
				set_color(message.color);
				move(0, i).print("%s", out.substr(line.begin, std::min(line.length, author_length)).c_str());
				reset_color(message.color);
				if (line.length > author_length) {
					print("%s", out.substr(line.begin + author_length, line.length - author_length).c_str());
					author_length = 0;
				} else {
					author_length -= line.length;
				}	
			} else {
				auto out = std::string();
				if (line.indent) {
					out.append("    ");
				}
				out.append(message.message.substr(line.begin, line.length));
				move(0, i).print("%s", out.c_str());
			}
			lines.pop();
		}
	}
	return *this;
}

Log::~Log() {}

#include <stack>
#include <fstream>

#include "log.hpp"

Log::Log() :
	Window(),
	log() {}

Log::Log(Window & root, bool dummy) :
	Window(root, true),
	log() {}

Log::Log(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, true),
	log() {}

auto Log::message(const std::string & message) -> void {
	log.push_back({ uint32_t( message.size() ), message });
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
	log.push_back({ uint32_t( author.size() + 3 ), std::move(expanded) });
}

extern std::ofstream file;

auto Log::draw() -> Window& {
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

	for (auto i = uint32_t{ 0 }; i < internal.height; ++i) {
		move(0, i).horz_line(internal.width, ' ');
	}
	auto i = int32_t( lines.size() ) - 1;
	if (i > internal.height) {
		i = internal.height - 1;
	}
	for (; i >= 0; --i) {
		auto & line = lines.top();
		if (line.length > 0) {
			auto & message = log[line.message_index];
			auto out = std::string();
			if (line.indent) {
				out.append("    ");
			}
			out.append(message.message.substr(line.begin, line.length));
			move(0, i).print("%s", out.c_str());
		}
		lines.pop();
	}
	return *this;
}

Log::~Log() {}

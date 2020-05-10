#include <fstream>

#include "playing.hpp"

extern std::ofstream file;

Playing::Playing() :
	Window(),
	players(),
	start_time(std::chrono::steady_clock::now()) {}

Playing::Playing(Window & root) :
	Window(root, true),
	players(),
	start_time(std::chrono::steady_clock::now()) {}

Playing::Playing(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, x, y, width, height),
	players(),
	start_time(std::chrono::steady_clock::now()) {}

auto Playing::set(const std::string & text, const std::string & name, uint32_t color) -> void {
	this->text  = text;
	this->name  = name;
	this->color = color;
	status    = PARTAKER;
	correct   = 0;
	incorrect = 0;
}

auto Playing::get_players() -> std::vector<Player>& {
	return players;
}

auto Playing::feed_char(uint32_t c) -> void {
	if (c == KEY_BACKSPACE || c == 0x08) {
		if (incorrect > 0) {
			--incorrect;
		} else if (correct > 0) {
			--correct;
		}
	} else if (correct + incorrect < text.size()) {
		if (incorrect == 0 && text[correct] == c) {
			++correct;
		} else {
			++incorrect;
		}
	}
}

auto Playing::draw() -> Window& {
	// Bounding box
	move(0,                  0                  ).add_char(ACS_ULCORNER);
	move(internal.width - 1, 0                  ).add_char(ACS_URCORNER);
	move(0,                  internal.height - 1).add_char(ACS_LLCORNER);
	move(internal.width - 1, internal.height - 1).add_char(ACS_LRCORNER);
	move(1,                  0                  ).horz_line(internal.width  - 2, ACS_HLINE);
	move(1,                  internal.height - 1).horz_line(internal.width  - 2, ACS_HLINE);
	move(0,                  1                  ).vert_line(internal.height - 2, ACS_VLINE);
	move(internal.width - 1, 1                  ).vert_line(internal.height - 2, ACS_VLINE);

	// Opponents
	auto line_count = internal.height - 2;
	for (auto i = uint32_t( 0 ); i < players.size() && line_count > 0; ++i) {

	}

	// Self
	move(0,                  internal.height - 7).add_char(ACS_LTEE);
	move(internal.width - 1, internal.height - 7).add_char(ACS_RTEE);
	move(1,                  internal.height - 7).horz_line(internal.width - 2, ACS_HLINE);
	
	auto name_header = std::string("////  ").append(name).append("  ").append(internal.width - 12, '/');
	move(2,                  internal.height - 6).print("%s", name_header.substr(0, internal.width - 4).c_str());

	auto start = (internal.width / 2) - correct - incorrect;
	if (start < 2) {
		start = 2;
	}
	auto text_width = internal.width - 4;

	move(1, internal.height - 4).horz_line(internal.width - 2, ' ');
	set_color(0);
	move(start, internal.height - 4);
	if (correct > 0 && incorrect < text_width / 2) {
		if (correct + incorrect > text_width / 2) {
			print("%s", text.substr(correct + incorrect - text_width / 2, text_width / 2 - incorrect).c_str());
		} else {
			print("%s", text.substr(0, correct).c_str());
		}
	}
	reset_color(0);
	set_color(2);
	if (incorrect > 0) {
		if (incorrect < text_width / 2) {
			print("%s", text.substr(correct, incorrect).c_str());
		} else {
			print("%s", text.substr(correct + incorrect - text_width / 2, text_width / 2).c_str());
		}
	}
	reset_color(2);
	set_color(1);
	if (text.size() - correct - incorrect > text_width - text_width / 2) {
		print("%s", text.substr(correct + incorrect, text_width - text_width / 2).c_str());
	} else {
		print("%s", text.substr(correct + incorrect, text.size() - correct - incorrect).c_str());
	}
	reset_color(1);
	
	move(2,                  internal.height - 3).horz_line(internal.width - 4, ACS_HLINE);

	auto time = std::chrono::duration<float>(std::chrono::steady_clock::now() - start_time).count();

	auto cpm = uint32_t(float(correct) / (time / 60.0f));
	auto info_text = std::string("Progress [ ")
	.append(std::to_string(uint32_t(100.0f * float(correct) / float(text.size()))))
	.append("% ]  /  CPM [ " ).append(std::to_string(cpm    ))
	.append( " ]  /  WPM [ " ).append(std::to_string(cpm / 5))
	.append( " ]  /  Time [ ").append(std::to_string(uint32_t(time))).append("s ]");
	if (info_text.size() > internal.width - 6) {
		info_text = info_text.substr(0, internal.width - 6);
	}
	move(3, internal.height - 2).print("%s", info_text.c_str());

	return *this;
}

Playing::~Playing() {}

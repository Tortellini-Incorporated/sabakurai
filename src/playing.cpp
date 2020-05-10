#include <fstream>

#include "playing.hpp"

extern std::ofstream file;

auto Playing::component_resize() -> void {
	block(false);
}

Playing::Playing() :
	Window(),
	players(),
	start_time(std::chrono::steady_clock::now()),
	end_time(start_time) {}

Playing::Playing(Window & root) :
	Window(root, true),
	players(),
	start_time(std::chrono::steady_clock::now()),
	end_time(start_time) {}

Playing::Playing(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, x, y, width, height),
	players(),
	start_time(std::chrono::steady_clock::now()),
	end_time(start_time) {}

auto Playing::set(const std::string & text, const std::string & name, uint32_t color) -> void {
	this->text  = text;
	self.name      = name;
	self.color     = color;
	self.status    = PARTAKER;
	self.progress  = 0;
	self.incorrect = 0;
}

auto Playing::get_player(uint32_t id) -> Player& {
	if (id == self.id) {
		return self;
	} else {
		for (auto i = uint32_t( 0 ); i < players.size(); ++i) {
			if (players[i].id == id) {
				return players[i];
			}
		}
		return self;
	}
}

auto Playing::get_players() -> std::vector<Player>& {
	return players;
}

auto Playing::feed_char(uint32_t c) -> void {
	if (c == KEY_BACKSPACE || c == 0x08) {
		if (self.incorrect > 0) {
			--self.incorrect;
		} else if (self.progress > 0) {
			--self.progress;
		}
	} else if (self.progress + self.incorrect < text.size()) {
		if (self.incorrect == 0 && text[self.progress] == c) {
			++self.progress;
			if (self.progress == text.size()) {
				end_time = std::chrono::steady_clock::now();
			}
		} else {
			++self.incorrect;
		}
	}
}

auto Playing::get_progress() -> uint32_t {
	return self.progress;
}

auto Playing::completed() -> bool {
	return self.progress  == text.size();
}

auto Playing::get_time() -> uint32_t {
	return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
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
	
	auto name_header = std::string("////  ").append(self.name).append("  ").append(internal.width - 12, '/');
	move(2,                  internal.height - 6).print("%s", name_header.substr(0, internal.width - 4).c_str());

	auto start = (internal.width / 2) - self.progress - self.incorrect;
	if (start < 2) {
		start = 2;
	}
	auto text_width = internal.width - 4;

	move(1, internal.height - 4).horz_line(internal.width - 2, ' ');
	set_color(0);
	move(start, internal.height - 4);
	if (self.progress > 0 && self.incorrect < text_width / 2) {
		if (self.progress + self.incorrect > text_width / 2) {
			print("%s", text.substr(self.progress + self.incorrect - text_width / 2, text_width / 2 - self.incorrect).c_str());
		} else {
			print("%s", text.substr(0, self.progress).c_str());
		}
	}
	reset_color(0);
	set_color(2);
	if (self.incorrect > 0) {
		if (self.incorrect < text_width / 2) {
			print("%s", text.substr(self.progress, self.incorrect).c_str());
		} else {
			print("%s", text.substr(self.progress + self.incorrect - text_width / 2, text_width / 2).c_str());
		}
	}
	reset_color(2);
	set_color(1);
	if (text.size() - self.progress - self.incorrect > text_width - text_width / 2) {
		print("%s", text.substr(self.progress + self.incorrect, text_width - text_width / 2).c_str());
	} else {
		print("%s", text.substr(self.progress + self.incorrect, text.size() - self.progress - self.incorrect).c_str());
	}
	reset_color(1);
	
	move(2,                  internal.height - 3).horz_line(internal.width - 4, ACS_HLINE);
	
	auto time = std::chrono::duration<float>(std::chrono::steady_clock::now() - start_time).count();
	if (completed()) {
		time = std::chrono::duration<float>(end_time - start_time).count();
	}

	auto cpm = uint32_t(float(self.progress) / (time / 60.0f));
	auto info_text = std::string("Progress [ ")
	.append(std::to_string(uint32_t(100.0f * float(self.progress) / float(text.size()))))
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

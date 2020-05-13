#include <queue>
#include <algorithm>

#include "playing.hpp"

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

auto Playing::set(const std::string & text, uint32_t id, const std::string & name, uint32_t color, bool spectate) -> void {
	this->text     = text;
	self.id        = id;
	self.name      = name;
	self.color     = color;
	self.status    = spectate;
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

auto Playing::get_self() -> Self& {
	return self;
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
				finalize();
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
	return self.progress == text.size();
}

auto Playing::finalize() -> void {
	end_time = std::chrono::steady_clock::now();
}

auto Playing::get_time() -> uint32_t {
	return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
}

auto Playing::get_cpm() -> uint32_t {
	auto progress = self.progress;
	if (self.status == COMPLETED) {
		progress = text.size();
	}
	return uint32_t(progress / (float(get_time()) / 60000000.0f));
}

auto Playing::get_wpm() -> uint32_t {
	return get_cpm() / 5;
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

	// Clear the internals
	for (auto i = 0; i < internal.height - 2; ++i) {
		move(1, 1 + i).horz_line(internal.width - 2, ' ');
	}

	// Opponents
	struct Entry {
		std::string text;
		uint32_t color;
	};
	auto lines = std::queue<std::vector<Entry>>();
	for (auto i = uint32_t( 0 ); i < players.size(); ++i) {
		auto & player = players[i];
		if (player.status == DISCONNECTED && player.progress == 0) {
			players.erase(players.begin() + i);
		} else {
			if (player.status == COMPLETED) {
				auto completed_text = std::string("| Completed ")
					.append(std::to_string(player.progress / 1000000))
					.append("s ");

				auto width = internal.width - 6 - completed_text.size();
				lines.push(std::vector<Entry>{
					{ std::string(1, '['),           0            },
					{ std::string(width, '/'),       player.color },
					{ completed_text.append(1, ']'), 0            }
				});

				auto cpm = uint32_t(text.size() / (float(player.progress) / 60000000.0f));
				lines.push(std::vector<Entry>{
					{
						std::string(1, ' ')
							.append(player.name)
							.append("  /  CPM [ ")
							.append(std::to_string(cpm    ))
							.append("  /  WPM [ ")
							.append(std::to_string(cpm / 5))
							.append(" ]"),
						0
					}
				});
			} else if (player.status == DISCONNECTED) {
				auto percent = float(player.progress) / float(text.size());
				lines.push(std::vector<Entry>{
					{ std::string(1, '['),                                                                            0            },
					{ std::string(                     uint32_t((internal.width - 6) * percent), 'x'),                player.color },
					{ std::string(internal.width - 6 - uint32_t((internal.width - 6) * percent), ' ').append(1, ']'), 0            }
				});
				lines.push({{ std::string(1, ' ').append(player.name).append("  /  DISCONNECTED"), 0 }});
			} else if (player.status == SPECTATOR) {
				lines.push({{ std::string(1, '[').append(internal.width - 6, ' ').append(1, ']'), 0 }});
				lines.push({{ std::string(player.name).append("  /  SPECTATOR"), 0 }});
			} else {
				auto width = internal.width - 6 - 15;
				auto percent = float(player.progress) / float(text.size());
				auto line = std::vector<Entry>();
				line.push_back({ std::string(1, '['), 0 });
				line.push_back({ std::string(uint32_t(width * percent), '/'), player.color });
				auto entry = std::string(width - uint32_t(width * percent), ' ')
					.append("| Progress ");
				if (percent < 10) {
					entry.append(1, ' ');
				}
				entry.append(std::to_string(uint32_t(percent * 100.0f)))
				     .append("% ]");
				line.push_back({ std::move(entry), 0 });
				lines.push(std::move(line));

				auto time = std::chrono::duration<float>(std::chrono::steady_clock::now() - start_time).count() / 60.0f;
				auto cpm = uint32_t(player.progress / time);
				lines.push({{
					std::string(1, ' ')
						.append(player.name)
						.append(  "  /  CPM [ ")
						.append(std::to_string(cpm    ))
						.append(" ]  /  WPM [ ")
						.append(std::to_string(cpm / 5))
						.append(" ]"),
					0
				}});
			}
		}
	}
	auto offset = 0; // Temporary
	auto count  = -1;
	while (++count < offset) {
		if (count % 3 != 2) {
			lines.pop();
		}
	}
	count = 2 - count % 3;
	auto line_num = -1;
	while (lines.size() > 0 && ++line_num < internal.height - 8) {
		move(2, 1 + line_num);
		if (line_num % 3 == count) {
			horz_line(internal.width - 4, ACS_HLINE);
		} else {
			for (auto & entry : lines.front()) {
				set_color(entry.color);
				print("%s", entry.text.c_str());
				reset_color(entry.color);
			}
			lines.pop();
		}
	}
	if (++line_num > -1 && line_num < internal.height - 8) {
		move(2, 1 + line_num).horz_line(internal.width - 4, ACS_HLINE);
	}

	// Self
	move(0,                  internal.height - 7).add_char(ACS_LTEE);
	move(internal.width - 1, internal.height - 7).add_char(ACS_RTEE);
	move(1,                  internal.height - 7).horz_line(internal.width - 2, ACS_HLINE);
	
	move(2, internal.height - 6).print("%s", std::string("////  ").substr(0, std::clamp(int32_t(internal.width) - 2, 0, 6)).c_str());
	set_color(self.color);
	auto name = self.name;
	name.append(2, ' ');
	print("%s", name.substr(0, std::clamp(int32_t(internal.width) - 8, 0, int32_t(name.size()))).c_str());
	reset_color(self.color);
	print("%s", std::string(std::max(int32_t(internal.width) - 8 - int32_t(name.size()), 0), '/').c_str());

	if (self.status != SPECTATOR) {
		auto local_progress = self.progress;
		if (self.status == COMPLETED) {
			local_progress = text.size();
		}
		auto start = (internal.width / 2) - local_progress - self.incorrect;
		if (local_progress + self.incorrect > internal.width / 2 - 2) {
			start = 2;
		}
		auto text_width = internal.width - 4;

		move(start, internal.height - 4);
		if (local_progress > 0 && self.incorrect < text_width / 2) {
			if (local_progress + self.incorrect > text_width / 2) {
				print("%s", text.substr(local_progress + self.incorrect - text_width / 2, text_width / 2 - self.incorrect).c_str());
			} else {
				print("%s", text.substr(0, local_progress).c_str());
			}
		}
		set_color(2);
		if (self.incorrect > 0) {
			if (self.incorrect < text_width / 2) {
				print("%s", text.substr(local_progress, self.incorrect).c_str());
			} else {
				print("%s", text.substr(local_progress + self.incorrect - text_width / 2, text_width / 2).c_str());
			}
		}
		reset_color(2);
		set_color(1);
		if (text.size() - local_progress - self.incorrect > text_width - text_width / 2) {
			print("%s", text.substr(local_progress + self.incorrect, text_width - text_width / 2).c_str());
		} else {
			print("%s", text.substr(local_progress + self.incorrect, text.size() - local_progress - self.incorrect).c_str());
		}
		reset_color(1);
	
		auto time = self.status == COMPLETED ? 
			std::chrono::duration<float>(end_time - start_time).count() :
			std::chrono::duration<float>(std::chrono::steady_clock::now() - start_time).count();

		auto cpm = uint32_t(float(local_progress) / (time / 60.0f));
		auto info_text = std::string("Progress [ ")
		.append(std::to_string(uint32_t(100.0f * float(local_progress) / float(text.size()))))
		.append("% ]  /  CPM [ " ).append(std::to_string(cpm    ))
		.append( " ]  /  WPM [ " ).append(std::to_string(cpm / 5))
		.append( " ]  /  Time [ ").append(std::to_string(uint32_t(time))).append("s ]");
		if (info_text.size() > internal.width - 6) {
			info_text = info_text.substr(0, internal.width - 6);
		}
		move(3, internal.height - 2).print("%s", info_text.c_str());
	} else {
		move(3, internal.height - 2).print("SPECTATOR");
	}

	move(2,                  internal.height - 3).horz_line(internal.width - 4, ACS_HLINE);

	return *this;
}

Playing::~Playing() {}

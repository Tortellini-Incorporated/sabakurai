#ifndef SABAKURAI_PLAYING
#define SABAKURAI_PLAYING

#include <vector>
#include <chrono>

#include "window.hpp"

class Playing : public Window {
	public:
		constexpr static uint32_t
			PARTAKER     = 0x00,
			SPECTATOR    = 0x01,
			DISCONNECTED = 0x02,
			COMPLETED    = 0x03;

		struct Player {
			uint32_t id;
			std::string name;
			uint32_t color;
			uint32_t status;
			uint32_t progress;
		};

	protected:
		std::string text;
		std::vector<Player> players;

		struct Self : public Player {
			uint32_t incorrect;
		} self;
		
		using Time = decltype(std::chrono::steady_clock::now());

		Time start_time;
		Time end_time;
	
	public:
		virtual auto component_resize() -> void;

		Playing();
		Playing(Window & root);
		Playing(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		auto set(const std::string & text, uint32_t id, const std::string & name, uint32_t color, bool spectate) -> void;
		auto get_player(uint32_t id) -> Player&;
		auto get_self() -> Self&;
		auto get_players() -> std::vector<Player>&;
		auto feed_char(uint32_t c) -> void;
		auto get_progress() -> uint32_t;
		auto completed() -> bool;
		auto get_time() -> uint32_t;
		auto get_cpm() -> uint32_t;
		auto get_wpm() -> uint32_t;
		virtual auto draw() -> Window&;
		~Playing();
};

#define SABAKURAI_PLAYING_FORWARD
#endif // SABAKURAI_PLAYING


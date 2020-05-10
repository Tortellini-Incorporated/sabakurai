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
			DISCONNECTED = 0x02;

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

		std::string name;
		uint32_t color;
		uint32_t status;
		uint32_t correct;
		uint32_t incorrect;
		
		decltype(std::chrono::steady_clock::now()) start_time;
	
	public:
		Playing();
		Playing(Window & root);
		Playing(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		auto set(const std::string & text, const std::string & name, uint32_t color) -> void;
		auto get_players() -> std::vector<Player>&;
		auto feed_char(uint32_t c) -> void;
		virtual auto draw() -> Window&;
		~Playing();
};

#define SABAKURAI_PLAYING_FORWARD
#endif // SABAKURAI_PLAYING


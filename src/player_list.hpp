#ifndef SABAKURAI_PLAYER_LIST
#define SABAKURAI_PLAYER_LIST

#include <vector>
#include <string>

#include "window.hpp"

class PlayerList : public Window {
	public:
		struct Player {
			uint32_t id;
			uint32_t color;
			bool ready;
			std::string name;
		};

	protected:
		uint32_t offset;
		std::vector<Player> players;

		auto find_player(uint32_t id) -> uint32_t;
	public:
		virtual auto component_resize() -> void;

		PlayerList();
		PlayerList(Window & root, bool dummy);
		PlayerList(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		auto add_player(uint32_t id, uint32_t color, bool ready, std::string name) -> void;
		auto get_player(uint32_t id) -> Player&;
		auto remove_player(uint32_t id) -> void;
		virtual auto draw() -> Window&;
		~PlayerList();	
};

#define SABAKURAI_PLAYER_LIST_FORWARD
#endif // SABAKURAI_PLAYER_LIST


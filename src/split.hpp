#ifndef SABAKURAI_SPLIT
#define SABAKURAI_SPLIT

#include <functional>

#include "window.hpp"

class Split : public Window {
	public:
		enum SplitDirection { HORZ, VERT };

	protected:

		using LocationCallback = std::function<uint32_t(uint32_t, uint32_t, uint32_t, uint32_t)>;
		
		LocationCallback split_location;
		SplitDirection split_direction;
		Window * first_child;
		Window * second_child;

	public:
		virtual auto component_resize() -> void;
		
		Split(LocationCallback split_location, SplitDirection direction);
		Split(Window & root, LocationCallback split_location, SplitDirection direction);
		Split(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height, LocationCallback split_location, SplitDirection direction);
		auto set_first_child(Window * window) -> void;
		auto set_second_child(Window * window) -> void;
		auto set_children(Window * first, Window * second) -> void;
		auto set_location(LocationCallback location) -> void;
		auto set_direction(LocationCallback location, SplitDirection direction) -> void;
		virtual auto draw() -> Window&;
		virtual auto clear() -> Window&;
		virtual auto refresh() -> Window&;
		~Split();
};

#define SABAKURAI_SPLIT_FORWARD
#endif // SABAKURAI_SPLIT


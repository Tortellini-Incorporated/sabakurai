#ifndef SABAKURAI_BOX
#define SABAKURAI_BOX

#include <string>

#include "window.hpp"

class Box : public Window {
	protected:
		std::string name;
		Window * child;

	public:
		virtual auto component_resize() -> void;

		Box(const std::string & name);
		Box(Window & root, const std::string & name);
		Box(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const std::string & name);
		auto set_child(Window * window) -> void;
		auto set_name(const std::string & name) -> void;
		virtual auto draw() -> Window&;
		virtual auto clear() -> Window&;
		virtual auto refresh() -> Window&;
		~Box();
};

#define SABAKURAI_BOX_FORWARD
#endif // SABAKURAI_BOX


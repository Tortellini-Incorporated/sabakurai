#ifndef SABAKURAI_BOX
#define SABAKURAI_BOX

#include "window.hpp"

class Box : public Window {
	protected:
		virtual auto component_resize() -> void;
	public:
		Box();
		Box(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		virtual auto draw() -> Window&;
		~Box();
};

#define SABAKURAI_BOX_FORWARD
#endif // SABAKURAI_BOX


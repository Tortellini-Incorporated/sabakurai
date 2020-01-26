#ifndef SABAKURAI_TITLE
#define SABAKURAI_TITLE

#include <string>

#include "window.hpp"

class Title : public Window {
	protected:
		const static std::string TITLE[];

	public:
		Title();
		Title(Window & root, bool dummy);
		Title(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		virtual auto draw() -> Window&;
		~Title();
};

#define SABAKURAI_TITLE_FORWARD
#endif // SABAKURAI_TITLE


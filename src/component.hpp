#ifndef SABAKURAI_COMPONENT
#define SABAKURAI_COMPONENT

class Resizeable {
	public:
		virtual auto resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void = 0;
};

class Component : public Resizeable {
	protected:
		Window * window;

	public:
		Component(Window & window);

		

#define SABAKURAI_COMPONENT_FORWARD
#endif // SABAKURAI_COMPONENT


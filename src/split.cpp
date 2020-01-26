#include "split.hpp"

auto Split::component_resize() -> void {
	auto split_location = this->split_location(internal.x, internal.y, internal.width, internal.height);
	if (split_direction == HORZ) {
		if (first_child != 0) {
			first_child->window_resize(internal.x, internal.y, internal.width, split_location);
		}
		if (second_child != 0) {
			second_child->window_resize(internal.x, internal.y + split_location, internal.width, internal.height - split_location);
		}
	} else {
		if (first_child != 0) {
			first_child->window_resize(internal.x, internal.y, split_location, internal.height);
		}
		if (second_child != 0) {
			second_child->window_resize(internal.x + split_location, internal.y, internal.width - split_location, internal.height);
		}
	}
}

Split::Split(LocationCallback split_location, SplitDirection direction) :
	Window(),
	first_child(0),
	second_child(0),
	split_location(split_location),
	split_direction(direction) {}


Split::Split(Window & root, LocationCallback split_location, SplitDirection direction) :
	Window(root, true),
	first_child(0),
	second_child(0),
	split_location(split_location),
	split_direction(direction) {}

Split::Split(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height, LocationCallback split_location, SplitDirection direction) :
	Window(root, x, y, width, height),
	first_child(0),
	second_child(0),
	split_location(split_location),
	split_direction(direction) {}

auto Split::set_first_child(Window * child) -> void {
	this->first_child = child;
	if (first_child != 0) {
		auto split_location = this->split_location(internal.x, internal.y, internal.width, internal.height);
		if (split_direction == HORZ) {
			first_child->window_resize(internal.x, internal.y, internal.width, split_location);
		} else {
			first_child->window_resize(internal.x, internal.y, split_location, internal.height);
		}
	}
}

auto Split::set_second_child(Window * child) -> void {
	this->second_child = child;
	if (second_child != 0) {
		auto split_location = this->split_location(internal.x, internal.y, internal.width, internal.height);
		if (split_direction == HORZ) {
			second_child->window_resize(internal.x, split_location, internal.width, internal.height - split_location);
		} else {
			second_child->window_resize(split_location, internal.y, internal.width - split_location, internal.height);
		}
	}
}

auto Split::set_children(Window * first, Window * second) -> void {
	this->first_child  = first;
	this->second_child = second;
	component_resize();
}

auto Split::set_location(LocationCallback location) -> void {
	this->split_location = location;
	component_resize();
}

auto Split::set_direction(LocationCallback location, SplitDirection direction) -> void {
	this->split_location = location;
	this->split_direction = direction;
	component_resize();
}

Split::~Split() {}

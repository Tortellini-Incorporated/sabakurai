#include "log.hpp"

Log::Log() :
	Window(),
	mutex(),
	log() {}

Log::Log(Window & root, bool dummy) :
	Window(root, true),
	mutex(),
	log() {}

Log::Log(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
	Window(root, true),
	mutex(),
	log() {}

auto Log::message(const std::string & message) -> void {
	std::lock_guard<std::mutex> lock(mutex);
	log.push_back({ false, "", message });
}

auto Log::message(const std::string & author, const std::string & message) -> void {
	std::lock_guard<std::mutex> lock(mutex);
	log.push_back({ true, author, message });
}

auto Log::draw() -> Window& {

	return *this;
}

Log::~Log() {}

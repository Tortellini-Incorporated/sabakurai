/*******************************************************************************
 *
 * Copyright (c) 2019 Gnarwhal
 *
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/

#ifndef SABAKURAI_PLAYING
#define SABAKURAI_PLAYING

#include <vector>

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
	
	public:
		Playing();
		Playing(Window & root);
		Playing(Window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		auto set(const std::string & text, const std::string & name, uint32_t color) -> void;
		auto get_players() -> std::vector<Player>&;
		virtual auto draw() -> Window&;
		~Playing();
};

#define SABAKURAI_PLAYING_FORWARD
#endif // SABAKURAI_PLAYING


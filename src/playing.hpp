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

#ifndef SABAKURAI_PLAYER_LIST_FORWARD
#define SABAKURAI_PLAYER_LIST_FORWARD

class PlayerList;

#endif // SABAKURAI_PLAYER_LIST_FORWARD

#ifndef SABAKURAI_PLAYING
#define SABAKURAI_PLAYING

#include "window.hpp"

#include "player_list.hpp"

class Playing : public Window {
	protected:
		PlayerList * players;
	
	public:
		Playing(PlayerList & players);
		Playing(Window & root, PlayerList & players);
		Playing(window & root, uint32_t x, uint32_t y, uint32_t width, uint32_t height, PlayerList & players);
		auto update() -> bool;
		virtual auto draw() -> Window&;
};

#define SABAKURAI_PLAYING_FORWARD
#endif // SABAKURAI_PLAYING


// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once
#include <Flash++.h>
#include <Renderable.h>
#include <MathUtils.h>

using namespace AS3::ui;

namespace SpaceTriangle
{
	/*
	* Base class for all things within the game.
	*/
	class Actor : public Renderable
	{
		public:

			vec3 pos,speed;

			Actor(flash::display3D::Context3D _ctx3d) : Renderable(_ctx3d) {
			}

			virtual ~Actor() { }

			virtual void tick() = 0;

			virtual void render() { }

			void gravity(Actor *a) {
				if(a == this)
					return;

				/*vec3 dir = a->pos - pos;
				float s = 1.0 / dir.length();
				dir.normalize();
				speed += s * dir;*/
			}
	};
}
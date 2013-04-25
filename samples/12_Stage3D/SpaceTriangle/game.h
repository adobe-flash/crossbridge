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
#include <vector>
#include <Flash++.h>

#include <Actor.h>

using namespace std;
using namespace AS3::ui;

namespace SpaceTriangle
{
	class Ship;
	class Missile;
	
	class Game
	{
		friend class Ship;
		friend class Missile;
		friend class Asteroid;

		flash::display3D::Context3D ctx3d;
		flash::display::Stage3D s3d;
		flash::display::Stage stage;

		vector<Actor*> actors;

		Ship *player;

		float width,height;

	public:
		Game();

		void tick();

		void render(flash::display3D::Context3D _ctx3d);

		void init(flash::display::Stage _stage, flash::display::Stage3D _s3d);

		void handleKeyUp(int keyCode);
		void handleKeyDown(int keyCode);

		void spawnMissile(vec3 pos, vec3 speed, float rads);
		void spawnAsteroid(vec3 pos, vec3 speed);
	};
}
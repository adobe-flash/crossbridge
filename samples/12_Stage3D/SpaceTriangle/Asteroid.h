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
#include <Actor.h>

namespace SpaceTriangle
{
	class Game;

	/*
	* One of the projectiles flying through space
	*/
	class Asteroid : public Actor
	{
		var colorVector;
		flash::display3D::IndexBuffer3D i3dbuffer;
		flash::display3D::VertexBuffer3D v3dbuffer;
		flash::geom::Matrix3D objectToWorld;

		Game *game;

	public:
		Asteroid(flash::display3D::Context3D ctx3d, Game *_game, vec3 _pos, vec3 _speed);

		virtual ~Asteroid() {}

		virtual void tick();

		virtual void render();
	};
}
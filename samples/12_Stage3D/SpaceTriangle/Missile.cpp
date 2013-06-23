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

#include "Missile.h"
#include "Game.h"
#include "Ship.h"
#include <MathUtils.h>

namespace SpaceTriangle
{
	Missile::Missile(flash::display3D::Context3D _ctx3d, Game *_game, vec3 _pos, vec3 _speed, float _rads) :
		Actor(_ctx3d),
		game(_game),
		rads(_rads)
	{
		speed = _speed;
		pos = _pos;

		vec3 x(0,2,0);
		x.rotate2D(rads);
		speed = x;

		i3dbuffer = ctx3d->createIndexBuffer(6);
	    uint16_t indicies[] = {
	    	2,1,0,2,0,3
	    };
	    i3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&indicies[0], 0, 6, (void*)&indicies[0]);

	    v3dbuffer = ctx3d->createVertexBuffer(4, 3);
	    float verticies[] = {
	    	-2.5, -2.5, 1.0,
	        -2.5, 2.5, 1.0,
	        2.5, 2.5, 1.0,
	        2.5, -2.5, 1.0,
	    };
	    v3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&verticies[0], 0, 4, (void*)&verticies[0]);

	    colorVector = internal::new_Vector_Number();
	    colorVector[0] = internal::new_Number(1.0);
	    colorVector[1] = internal::new_Number(0.5);
	    colorVector[2] = internal::new_Number(0.5);
	    colorVector[3] = internal::new_Number(0.5);

	    objectToWorld = flash::geom::Matrix3D::_new();
	}

	void Missile::tick()
	{
		pos += speed;

		if(pos.x < -game->width / 2.0)
			pos.x = game->width / 2.0;
		else if(pos.x > game->width / 2.0)
			pos.x = -game->width / 2.0;
		
		if(pos.y < -game->height / 2.0)
			pos.y = game->height / 2.0;
		else if(pos.y > game->height / 2.0)
			pos.y = -game->height / 2.0;
	}

	void Missile::render()
	{
		objectToWorld->identity();
		objectToWorld->appendRotation(rads * (180.0/M_PI), flash::geom::Vector3D::Z_AXIS, internal::_null);
		objectToWorld->appendTranslation(pos.x, pos.y, pos.z);

		ctx3d->setProgramConstantsFromMatrix(flash::display3D::Context3DProgramType::VERTEX, 4, objectToWorld, true);
		ctx3d->setProgramConstantsFromVector(flash::display3D::Context3DProgramType::FRAGMENT, 0, colorVector, -1);
		ctx3d->setVertexBufferAt(0, v3dbuffer, 0, flash::display3D::Context3DVertexBufferFormat::FLOAT_3);
		ctx3d->drawTriangles(i3dbuffer, 0, -1);
	}
}
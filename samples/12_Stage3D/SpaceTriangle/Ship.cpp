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

#include "Ship.h"
#include "Game.h"
#include <MathUtils.h>

namespace SpaceTriangle
{
	Ship::Ship(flash::display3D::Context3D _ctx3d, Game *_game) :
		Actor(_ctx3d),
		game(_game),
		thrusting(false),
		direction(0,1,0),
		rotating(false),
		rs(0.0),
		ts(0.0)
	{
		i3dbuffer = ctx3d->createIndexBuffer(3);
	    uint16_t indicies[] = {
	    	2,1,0
	    };
	    i3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&indicies[0], 0, 3, (void*)&indicies[0]);

	    v3dbuffer = ctx3d->createVertexBuffer(3, 3);
	    float verticies[] = {
	    	-15, -20, 1.0,
	        0, 20, 1.0,
	        15, -20, 1.0,
	    };
	    v3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&verticies[0], 0, 3, (void*)&verticies[0]);

	    colorVector = internal::new_Vector_Number();
	    colorVector[0] = internal::new_Number(0.5);
	    colorVector[1] = internal::new_Number(0.5);
	    colorVector[2] = internal::new_Number(0.5);
	    colorVector[3] = internal::new_Number(0.5);

	    objectToWorld = flash::geom::Matrix3D::_new();
	}

	void Ship::handleKeyUp(int keyCode) {
		switch(keyCode) {
			case 37: // left
				rotating = false;
				break;
			case 38: // up
				thrusting = false;
				break;
			case 39: // right
				rotating = false;
				break;
			case 40: // down
				break;
		}
	}
	
	void Ship::handleKeyDown(int keyCode) {
		switch(keyCode) {
			case 32: // space
				game->spawnMissile(pos, speed, rads);
				break;
			case 37: // left
				rs = 0.05;
				rotating = true;
				break;
			case 38: // up
				thrusting = true;
				ts = 1.0;
				break;
			case 39: // right
				rs = -0.05;
				rotating = true;
				break;
			case 40: // down
				thrusting = true;
				ts = -1.0;
				break;
		}
	}

	void Ship::tick()
	{
		if(rotating) {
			rads += rs;
			direction.rotate2D(rs);
			direction.normalize();
		}

		if(thrusting) {
			speed += (ts * 0.25 * direction);
		}

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

	void Ship::render()
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
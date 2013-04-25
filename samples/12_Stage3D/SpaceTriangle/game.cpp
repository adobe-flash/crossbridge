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

#include <game.h>
#include <Ship.h>
#include <Missile.h>
#include <Asteroid.h>
#include <Flash++.h>
#include <AGAL.h>
#include <MathUtils.h>

namespace SpaceTriangle
{
	Game::Game() {

	}

	void Game::tick() {
		int n = actors.size();
		for(vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
			Actor *a = *i;

			for(int j=0;j<3;j++) {
				a->gravity(actors[rand() % n]);
			}

			a->tick();
		}
	}

	void Game::render(flash::display3D::Context3D _ctx3d) {
		ctx3d = _ctx3d;
		static int tc = 0;
        ctx3d->clear(0.1, 0.2, 0.3, 1, 1, 0, 0xffffffff);

		for(vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
			(*i)->render();
		}

        ctx3d->present();
	}

	void Game::handleKeyUp(int keyCode) {
		switch(keyCode) {
			case 32:
			case 37:
			case 38:
			case 39:
			case 40:
				player->handleKeyUp(keyCode);
				break;
		}
	}
	
	void Game::handleKeyDown(int keyCode) {
		switch(keyCode) {
			case 70: // 'f'
				stage->displayState = flash::display::StageDisplayState::FULL_SCREEN;
				break;
			case 32:
			case 37:
			case 38:
			case 39:
			case 40:
				player->handleKeyDown(keyCode);
				break;
		}
	}

	void Game::spawnMissile(vec3 pos, vec3 speed, float rads)
	{
		Missile *m = new Missile(ctx3d, this, pos, speed, rads);
		actors.push_back(m);
	}

	void Game::spawnAsteroid(vec3 pos, vec3 speed)
	{
		Asteroid *a = new Asteroid(ctx3d, this, pos, speed);
		actors.push_back(a);
	}

	void Game::init(flash::display::Stage _stage, flash::display::Stage3D _s3d)
	{
		stage = _stage;
		s3d = _s3d;

		// We have a context, we just need to configure the backbuffer to fill the
	    // stage.
	    ctx3d = s3d->context3D;
	    ctx3d->enableErrorChecking = false;
	    ctx3d->configureBackBuffer(stage->stageWidth, stage->stageHeight, 2,
	                               true, false);

	    com::adobe::utils::AGALMiniAssembler vasm =
	        com::adobe::utils::AGALMiniAssembler::_new(false);
	    vasm->assemble(flash::display3D::Context3DProgramType::VERTEX,
	                   "m44 vt0, va0, vc4\n"
	                   "m44 vt1, vt0, vc0\n"
	                   "mov op, vt1\n");

	    com::adobe::utils::AGALMiniAssembler fasm =
	        com::adobe::utils::AGALMiniAssembler::_new(false);
	    fasm->assemble(flash::display3D::Context3DProgramType::FRAGMENT,
	                   "mov oc, fc0\n");

	    flash::display3D::Program3D program = ctx3d->createProgram();
	    program->upload(vasm->agalcode, fasm->agalcode);
	    ctx3d->setProgram(program);

	    width = stage->stageWidth;
	    height = stage->stageHeight;

		com::adobe::utils::PerspectiveMatrix3D projection =
			com::adobe::utils::PerspectiveMatrix3D::_new(internal::_null);
		projection->orthoLH(stage->stageWidth, stage->stageHeight, 0.1, 10.0);
		ctx3d->setProgramConstantsFromMatrix(flash::display3D::Context3DProgramType::VERTEX, 0, projection, true);

		flash::geom::Matrix3D identity = flash::geom::Matrix3D::_new();
		ctx3d->setProgramConstantsFromMatrix(flash::display3D::Context3DProgramType::VERTEX, 4, identity, true);

	    player = new Ship(ctx3d, this);

	    for(int i=0; i<5; i++) {
	    	vec3 pos(rand() % (int)width, rand() % (int)height, 1.0);
	    	vec3 speed((rand() % 8) - 4, (rand() % 8) - 4, 0.0);
	    	spawnAsteroid(pos, speed);
	    }

		actors.push_back(player);
	}
}
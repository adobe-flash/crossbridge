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

#include <vector>
#include <AS3/AS3.h>
#include <Flash++.h>

// Flash 11.4 introduced a worker-based concurrency model in which each worker
// has access to almost all of the Flash APIs with the one restriction that
// objects on one worker cannot be passed to other workers, all communication
// must happen via special communication APIs (MessageChannel) or via the 
// shared ByteArray support (Flash 11.5)
//
// Flash++ contains two sets of bindings for the flash APIs, one that refers to
// objects on the UI worker (the main flash instance that has access to the 
// primary display) and another that refers to objects local to a given worker.
//
// When interacting with "ui" references property access and method calls will
// block until they can be serviced by the main worker when it calls
// CModule.serviceUIRequests() (Which it should be doing on EnterFrame, or on a
// timer).
//
// Because we will just be using UI references in this example we can avoid the
// need to type the additional "AS3::ui" prefix with a normal C++ using
// declaration.

using namespace AS3::ui;
// using namespace AS3::local; // if we wanted everything to be worker-local

int main()
{
	// Get a reference to the current stage
	flash::display::Stage stage = internal::get_Stage();

	// Just to prove we can lets make a C++ vector containing sprite references
	std::vector<flash::display::Sprite> sprites;
	int numCircles = 10;

	for(int i=0; i<numCircles; i++) {
		// Construct a new Sprite object
		flash::display::Sprite mySprite = flash::display::Sprite::_new();

		// Access its "graphics" property to get the canvas we can draw to
		flash::display::Graphics graphics = mySprite->graphics;

		// Draw simple filled circle
		graphics->beginFill(0xff00ff, 0.5);
		graphics->drawCircle(0.0, 0.0, 30.0);
		graphics->endFill();

		// Add the sprite into our C++ vector
		sprites.push_back(mySprite);
	}

	// Add the circles to the stage
	for(int i=0; i<numCircles; i++) {
		// Take one of the sprites and position it along the diagonal of
		// the screen
		flash::display::Sprite s = sprites[i];
		s->x = i * 25;
		s->y = i * 25;

		// Add it to the stage so we can see it
		stage->addChild(s);
	}
}
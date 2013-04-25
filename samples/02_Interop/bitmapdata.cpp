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

static const int COORDBITS = 7; // 7 bits => dim of 128
static const int DIM = 1 << COORDBITS;

// return a "swizzled" (Morton order -- x/y bits interleaved) offset
// for a given input offset and coordinate bit count
static int swizzleOffset(int offs)
{
  int result = 0;
  int coordBits = COORDBITS;
  int offsHi = offs >> COORDBITS; // take the higher order bits
  int coordBits2 = COORDBITS * 2;
  while(coordBits--)
  {
    // put the lowest bit in the "lo" offset bits
    // into the highest bit of the result...
    // it'll get shifted into a lower position
    // as the loop executes
    result |= ((offs & 1) << coordBits2);
    offs >>= 1;
    result >>= 1;
    // same for the "hi" offset bits
    result |= ((offsHi & 1) << coordBits2);
    offsHi >>= 1;
    result >>= 1;
  }
  return result;
}

// swizzle a whole image worth of pixels
static void swizzlePixels(unsigned *dst, unsigned *src)
{
	int offs = DIM * DIM;

	while(offs--)
	{
		int swiz = swizzleOffset(offs);
		dst[swiz] = src[offs];
	}
}

// handle a mouse click and swizzle our BitmapData's pixels
static var mouseDownHandler(void *arg, var args)
{
	flash::events::MouseEvent event = flash::events::MouseEvent(args[0]);
	flash::display::Sprite sprite = flash::display::Sprite(event->target); // the container Sprite
	flash::display::Bitmap bitmap = flash::display::Bitmap(sprite->getChildAt(0)); // Bitmap is the only child
	flash::display::BitmapData bitmapData = bitmap->bitmapData;
	// ByteArray corresponding to our ram!
	// C ptrs are equivalent to offsets into this ByteArray
	flash::utils::ByteArray ram = internal::get_ram();

	// allocate space for the pixels
	unsigned *src = new unsigned[DIM * DIM];
	unsigned *dst = new unsigned[DIM * DIM];
	// copy current pixels directly to ram
	bitmapData->copyPixelsToByteArray(bitmapData->rect, ram, src);
	// swizzle them!
	swizzlePixels(dst, src);
	// write new pixels directly from ram
	bitmapData->setPixels(bitmapData->rect, ram, dst);
	// clean up
	delete dst;
	delete src;
	return internal::_undefined;
}

int main()
{
	// Get a reference to the current stage
	flash::display::Stage stage = internal::get_Stage();

	// Create a Shape
	flash::display::Shape myShape = flash::display::Shape::_new();
	// Access its "graphics" property to get the canvas we can draw to
	flash::display::Graphics graphics = myShape->graphics;

	// Draw simple shape
	graphics->beginFill(0xff00ff, 0.5);
	graphics->drawCircle(64.0, 64.0, 64.0);
	graphics->endFill();
	graphics->beginFill(0xffff00, 0.5);
	graphics->drawCircle(64.0, 64.0, 40.0);
	graphics->endFill();
	graphics->beginFill(0x00ffff, 0.5);
	graphics->drawCircle(64.0, 64.0, 16.0);
	graphics->endFill();

	// Create a BitmapData
	flash::display::BitmapData myBitmapData = flash::display::BitmapData::_new(DIM, DIM);
	// Draw the Shape into it
	myBitmapData->draw(myShape);

	// Create a Bitmap
	flash::display::Bitmap myBitmap = flash::display::Bitmap::_new(myBitmapData);

	// wrap it in a Sprite
	flash::display::Sprite mySprite = flash::display::Sprite::_new();

	mySprite->addChild(myBitmap);
	// Add a click handler
	mySprite->addEventListener("mouseDown", Function::_new(&mouseDownHandler, NULL));
	
	// Add it to the stage so we can see it
	stage->addChild(mySprite);

	// Go async so we can handle the mouseDowns!
	AS3_GoAsync();

	// Not reached!
	return 0;
}

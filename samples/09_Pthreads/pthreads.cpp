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

#include <AS3/AS3++.h> // using AS3 var wrapper class
#include <Flash++.h> // using AVM2 sync primitives
#include <pthread.h>

// use "ui" AS3 var wrappers which marshall var manipulations to the ui Worker
using namespace AS3::ui;

static char mouseMoveCond; // just use the address as a condition

// mouseDown listener to that makes the Sprite follow the mouse -- associated with a background thread
static var mouseMoveProc(void *arg, var as3Args)
{
  flash::events::MouseEvent event = var(as3Args[0]);
  flash::display::Sprite mySprite = *(var *)arg;

  // set the sprite's coords to the mouse's
  mySprite->x = event->stageX;
  mySprite->y = event->stageY;
  // signal threadProc's avm2_self_msleep to wake up
  avm2_wake(&mouseMoveCond);
  return internal::_undefined;
}

// thread proc to watch mouseMove
static void *threadProc(void *arg)
{
  // mySprite was passed to us!
  // it's a "ui" var so any operations are marshalled to the ui Worker...
  // so we can use it from any thread!
  flash::display::Sprite mySprite = *(var *)arg;
  flash::display::Stage stage = internal::get_Stage();

  // add a moveMove event listener!
  stage->addEventListener(flash::events::MouseEvent::MOUSE_MOVE, Function::_new(mouseMoveProc, (void*)&mySprite), false, 0, false);

  int count = 0;

  for(;;)
  {
    // sleep, allowing the ui thread to call mouseMoveProc on our behalf
    avm2_self_msleep(&mouseMoveCond, 0);
    printf("mouseMove! (%d)\n", count++);
  }
  return NULL;
}

// mouseDown listener that changes the Sprite's opacity -- associated with main thread
static var mouseDownProc(void *arg, var as3Args)
{
  flash::display::Sprite mySprite = *(var *)arg;
  mySprite->alpha = mySprite->alpha * 0.9;

  return internal::_undefined;
}

static char mainCond; // address used to wake the avm2_self_msleep in main (though no one signals it right now!)

int main()
{
  // get a reference to the Stage
  flash::display::Stage stage = internal::get_Stage();
  flash::display::Sprite mySprite = flash::display::Sprite::_new();

  // get mySprite.graphics
  flash::display::Graphics graphics = mySprite->graphics;

  // draw  simple filled circle
  graphics->beginFill(0xff00ff, 1.0);
  graphics->drawCircle(0.0, 0.0, 50.0);
  graphics->endFill();

  // add it to the stage!
  stage->addChild(mySprite);
  
  // create a Function to watch mouseDown
  stage->addEventListener(flash::events::MouseEvent::MOUSE_DOWN, Function::_new(mouseDownProc, (void*)&mySprite), false, 0, false);

  pthread_t thread;

  // create a new thread to watch mouse moves!
  pthread_create(&thread, NULL, threadProc, &mySprite);
  // wait for exit to be signaled
  avm2_self_msleep(&mainCond, 0);
  // wait for it to finish
  pthread_join(thread, NULL);
  printf("done!\n");
  return 0;
}

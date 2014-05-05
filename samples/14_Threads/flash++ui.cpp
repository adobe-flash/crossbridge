// Example of the functionality in <Flash++.h>

#include <pthread.h>
#include <AS3/AVM2.h>
#include <Flash++.h>

// use namespace AS3::ui to give us access to the Flash API via C++
// the "ui" part of the namespace gives us the version of the Flash API
// that is automatically delegated to the ui Worker -- all operations
// on objects manipulated through AS3::ui are delegated to the 
// ui Worker automatically!
using namespace AS3::ui;

// handle mouseDown for our Sprites
static var mouseDownEventHandler(void *, var args) {
  flash::events::MouseEvent event = flash::events::MouseEvent(args[0]);
  flash::display::Sprite sprite = flash::display::Sprite(event->target);
  flash::display::Stage stage = internal::get_Stage();
  // remove clicked Sprites from Stage
  stage->removeChild(sprite);
  return internal::new_undefined();
}

// create a few circle Sprites and add them to the Stage
static void makeSomeSprites(unsigned color) {
  // get access to the current Stage
  flash::display::Stage stage = internal::get_Stage();
  for(int i = 0; i < 15; i++) {
    // create a new Sprite
    flash::display::Sprite sprite = flash::display::Sprite::_new();
    // draw into it
    flash::display::Graphics g = sprite->graphics;
    g->beginFill(color);
    g->drawCircle(rand() % 500, rand() % 500, 50);
    g->endFill();
    // make partially transparent
    sprite->alpha = 0.5;
    // listen for clicks
    sprite->addEventListener(flash::events::MouseEvent::MOUSE_DOWN,
      Function::_new(mouseDownEventHandler, NULL));
    // add it to the stage
    stage->addChild(sprite);
    // this Worker will sleep for 1s but while sleeping it will allow the UI
    // Worker to impersonate this pthread to call our mouseDownEventHandler;
    // mouseDownEventHandler will be called on the UI Worker but will impersonate
    // this pthread from C's perspective (in terms of thread id, pthread_self(),
    // pthread_get/setspecific, lock ownership, etc.)
    avm2_self_msleep(NULL, 1000);
  }
  avm2_self_msleep(NULL, 0); // sleep forever, allowing impersonation
}

// synchronize at a barrier to make all 3 threads add their
// Sprites at the same time
static pthread_barrier_t sBarrier;

// thread proc that just calls makeSomeSprites
static void *threadProc(void *colorArg) {
  // synchronize everyone
  pthread_barrier_wait(&sBarrier);
  makeSomeSprites((unsigned)colorArg);
  return NULL;
}

int main() {
  pthread_barrier_init(&sBarrier, NULL, 3);

  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, threadProc, (void *)0xff0000);
  pthread_create(&thread2, NULL, threadProc, (void *)0x00ff00);
  // synchronize everyone
  pthread_barrier_wait(&sBarrier);
  makeSomeSprites(0x0000ff);
  // unreached!
  return 0;
}

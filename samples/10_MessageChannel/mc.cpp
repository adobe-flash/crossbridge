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

#include <pthread.h>
extern "C" {
#include "sys/thr.h"
}
#include <AS3/AS3.h>
#include <Flash++.h>
#include <sys/time.h>

static volatile long tid = 0;
static pthread_barrier_t barrier;

static timeval tvStart; // initialized in main

// return # of ms since start
static int getTimestamp()
{
  timeval tvNow;
  gettimeofday(&tvNow, NULL);
  return 1000 * (tvNow.tv_sec - tvStart.tv_sec) + (tvNow.tv_usec - tvStart.tv_usec) / 1000;
}

using namespace AS3::local;

static var channelMessageEventListenerProc(void *arg, var as3Args)
{
  int now = getTimestamp();
  flash::events::Event event = flash::events::Event(as3Args[0]); // event
  flash::system::MessageChannel msgChan = flash::system::MessageChannel(event->target); // event.target
  var msg = msgChan->receive();
  int timestamp = _int(msg)->valueOf();

  printf("Message! Latency %dms\n", now - timestamp);
  return internal::_undefined;
}

static void *threadProc(void *arg)
{
  long id;

  thr_self(&id);
  tid = id;

  pthread_barrier_wait(&barrier); // wait for main to pick up tid
  pthread_barrier_wait(&barrier); // wait for main to create messageChannel

  flash::system::Worker worker = internal::get_Worker();
  flash::system::MessageChannel msgChan = worker->getSharedProperty("com.mycompany.messageChannel");

  msgChan->addEventListener("channelMessage", Function::_new(channelMessageEventListenerProc, NULL));
  AS3_GoAsync(); // return to event loop to allow channelMessage Events to get serviced
  return NULL; // not actually reached
}

int main()
{
  // get initial time for timestamping
  gettimeofday(&tvStart, NULL);

  // set up barrier
  pthread_barrier_init(&barrier, NULL, 2);

  pthread_t thread;

  pthread_create(&thread, NULL, threadProc, NULL);

  pthread_barrier_wait(&barrier); // wait for tid to get initialized

  flash::system::Worker selfWorker = internal::get_Worker();
  flash::system::Worker otherWorker = internal::get_Worker(tid);
  flash::system::MessageChannel msgChan = selfWorker->createMessageChannel(otherWorker);
  otherWorker->setSharedProperty("com.mycompany.messageChannel", msgChan);

  pthread_barrier_wait(&barrier); // thread can now hook up to message channel

  for(;;)
  {
    sleep(1);
    msgChan->send(_int::_new(getTimestamp())); // send current timestamp
  }
  return 0;
}

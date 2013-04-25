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

#include <stdio.h>
#include <pthread.h>
#include "AS3/AS3.h"

static char threadBuf[256];

static void *threadProc(void *arg)
{
  static int counter = 0;
  printf("Thread proc!\n");
  sprintf(threadBuf, "threadProc invoked %d times", ++counter);
  return NULL;
}

static pthread_t thread;

void spawnThread() __attribute__((used, annotate("as3sig:public function spawnThread():void")));

void spawnThread()
{
  printf("spawnThread run\n");
  pthread_create(&thread, NULL, threadProc, NULL);
  pthread_join(thread, NULL);
  printf("spawnThread joined (%s)\n", threadBuf);
}

int main()
{
  printf("main run\n");
  pthread_create(&thread, NULL, threadProc, NULL);
  pthread_join(thread, NULL);
  printf("main joined (%s)\n", threadBuf);

  AS3_GoAsync();
}

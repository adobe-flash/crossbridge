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

static volatile int count = 0;

static void cleanupProc(void *arg)
{
  printf("cleaning up! [%s]\n", (char *)arg);
}

static void *threadProc(void *arg)
{
  printf("thread started\n");
  pthread_cleanup_push(cleanupProc, arg);
  for(;;)
  {
    pthread_testcancel();
    __sync_fetch_and_add(&count, 1);
  }
  pthread_cleanup_pop(0);
  printf("error - didn't cancel!\n");
  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t thread;

  pthread_create(&thread, NULL, &threadProc, "string arg");
  while(count < 2*1000*1000);
  printf("trying to cancel\n");
  pthread_cancel(thread);
  pthread_join(thread, NULL);
  
  return 0;
}

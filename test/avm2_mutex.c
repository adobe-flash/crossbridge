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
#include <stdlib.h>

#define MAXTHREADS 10
#define MAXMUTEXES 10

static unsigned sMutexes[MAXMUTEXES] = { 0 };
static volatile unsigned sCounter = 0;

static void *threadProc(void *arg)
{
  static int sS = 0;
  int n = (int)arg;
  int iter = 500 * n;
  unsigned seed;

//printf("- %d\n", seed);
  seed = __sync_fetch_and_add(&sS, 1);
  while(iter--)
  {
    int ln = rand_r(&seed) % n;
    avm2_lock(sMutexes + ln);
//printf("%d\n", iter);
    avm2_unlock(sMutexes + ln);
  }
//printf("done\n");
  __sync_fetch_and_add(&sCounter, 1);
  return NULL;
}

int main()
{
  pthread_t threads[MAXTHREADS];
  int i, j, n;

  for(i = 1; i <= 10; i++) // # of threads
    for(j = 1; j <= 10; j++) // # of mutexes
    {
      unsigned counterDone = sCounter + i;

      printf("%d threads %d mutexes\n", i, j);
      for(n = 0; n < i; n++)
        pthread_create(threads + n, NULL, threadProc, (void *)j);
      while(sCounter != counterDone);
      // wait for the last thread to finish exiting...
      {
        unsigned dummy = 1;

        avm2_msleep(&dummy, &dummy, 1000);
      }
      for(n = 0; n < i; n++)
        pthread_join(threads[n], NULL);
    }
  return 0;
}

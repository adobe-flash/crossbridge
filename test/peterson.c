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
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#ifdef __AVM2__
#include <AS3/AVM2.h>

#endif

// fence test
// Peterson locks on x86 require fencing!

// http://en.wikipedia.org/wiki/Peterson_lock
// http://bartoszmilewski.com/2008/11/05/who-ordered-memory-fences-on-an-x86/

// 0 => no ordering -- insufficient
// 1 => asm "memory" ordering -- insufficient
// 2 => __sync_synchronize fence -- sufficient!
#ifndef ORDER_STRENGTH
#define ORDER_STRENGTH 2
#endif

#ifdef __AVM2__
#define CRASH_ADDR ((void *)-1)
#else
#define CRASH_ADDR ((void *)0)
#endif

static long long sCount[2] = { 0 } ;

typedef struct
{
  volatile bool *selfWants;
  volatile bool *otherWants;
  volatile int *victim;
  volatile int * volatile *selfIncr;
  volatile int * volatile *otherIncr;
  int id;
} ThreadArgs;

static void *threadProc(void *args)
{
  ThreadArgs *targs = (ThreadArgs *)args;
  volatile bool *selfWants = targs->selfWants;
  volatile bool *otherWants = targs->otherWants;
  volatile int *victim = targs->victim;
  volatile int * volatile *selfIncr = targs->selfIncr;
  volatile int * volatile *otherIncr = targs->otherIncr;
  int id = targs->id;

  for(;;)
  {
    volatile int *otherIncrPrev;
    // enter cs
    *selfWants = true;
    *victim = id;
#if ORDER_STRENGTH == 0
    // no ordering -- not sufficient!
#elif ORDER_STRENGTH == 1
    __asm("" ::: "memory"); // not sufficient!
#elif ORDER_STRENGTH == 2
#ifdef __AVM2__
    __sync_synchronize(); // sufficient (does nothing on Darwin?!)
#elif __x86_64__ || __i386__
    __asm __volatile__("mfence" ::: "memory"); // sufficient (x86 fence)
#else
#error Need a fence for your system!
#endif
#else
#error Please select an order strength
#endif
    while(*otherWants && *victim == id);
    // in cs
    otherIncrPrev = *otherIncr;
    *otherIncr = CRASH_ADDR;
    ++**selfIncr;
    ++sCount[id]; // not atomic, so (highly -- it's 2 words) approximate...
    *otherIncr = otherIncrPrev;
    // exit cs
    *selfWants = false;
  }
  return NULL;
}

#define CHUNK 256 // > cache line
#define THREADPAIRS 4

int main(int argc, char **argv)
{
  int s = (argc > 1) ? atoi(argv[1]) : 15;
  time_t end = time(NULL) + s;
  int i;
  long long stored_0 = 0;
  long long stored_1 = 0;

  for(i = 0; i < THREADPAIRS; i++)
  {
    bool *zeroWants = calloc(1, CHUNK);
    bool *oneWants = calloc(1, CHUNK);
    int *victim = calloc(1, CHUNK);
    int **zeroIncr = malloc(CHUNK);
    int **oneIncr = malloc(CHUNK);
    ThreadArgs *zeroArgs = calloc(1, sizeof(ThreadArgs));
    ThreadArgs *oneArgs = calloc(1, sizeof(ThreadArgs));
    pthread_t zeroThread, oneThread;
    
    *zeroIncr = calloc(1, CHUNK);
    *oneIncr = calloc(1, CHUNK);

    zeroArgs->selfWants = zeroWants;
    zeroArgs->otherWants = oneWants;
    zeroArgs->victim = victim;
    zeroArgs->selfIncr = zeroIncr;
    zeroArgs->otherIncr = oneIncr;

    oneArgs->selfWants = oneWants;
    oneArgs->otherWants = zeroWants;
    oneArgs->victim = victim;
    oneArgs->selfIncr = oneIncr;
    oneArgs->otherIncr = zeroIncr;
    oneArgs->id = 1;

    pthread_create(&zeroThread, NULL, threadProc, zeroArgs);
    pthread_create(&oneThread, NULL, threadProc, oneArgs);
  }
  while(time(NULL) < end)
  {
#ifdef __AVM2__
    unsigned dummy = 1;
    avm2_msleep(&dummy, &dummy, 1000);
#else
    sleep(1);
#endif
    if(sCount[0] > stored_0 && sCount[1] > stored_1 ){
      //printf("stored_0=%llu sCount[0]=%llu stored_1=%llu sCount[1]=%llu\n", stored_0, sCount[0], stored_1, sCount[1]);
      stored_0 = sCount[0];
      stored_1 = sCount[1];
    }else{
      printf("RESULT=FAIL\n");
      return -1;
    }
  }
	
  printf("RESULT=PASS\n");

  return 0;
}














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
#include <stdio.h>

#define MAXTHREAD 10
#define MAXMUTEX 10

static pthread_mutex_t sMutexes[MAXMUTEX];

int successes = 0;
int EXPECTED_SUCCESSES = 100;

static void *threadProc(void *arg)
{
  static int sS = 0;
  int n = (int)arg;
  int iter = 4000;
  unsigned seed;

  //printf("- %d\n", seed);
  seed = __sync_fetch_and_add(&sS, 1);
  while(iter--)
  {
    int ln = rand_r(&seed) % n;
    pthread_mutex_lock(sMutexes + ln);
    //printf("%d\n", iter);
    pthread_mutex_unlock(sMutexes + ln);
  }
  return NULL;
}

int main()
{
  pthread_mutex_t minit = PTHREAD_MUTEX_INITIALIZER;
  pthread_t threads[MAXTHREAD];
  int i, j, n;
  size_t mem_start = 0;

#ifdef __AVM2__
	__asm("%0 = ram.length" : "=r"(mem_start));
#endif	
	
	for(i = 1; i <= MAXTHREAD; i++){ // # of threads
		for(j = 1; j <= MAXMUTEX; j++) // # of mutexes
		{
		  size_t mem = 0;
	#ifdef __AVM2__
		  __asm("%0 = ram.length" : "=r"(mem));
	#endif
		  printf("%d threads, %d mutexes, memory %s\n", i, j, mem == mem_start ? "ok" : "not ok");
		  if(mem != mem_start){
			break;
		  }else{
			++successes;
		  }
		  for(n = 0; n < j; n++)
			sMutexes[n] = minit;
		  for(n = 0; n < i; n++)
			pthread_create(threads + n, NULL, threadProc, (void *)j);
		  for(n = 0; n < i; n++)
			pthread_join(threads[n], NULL);
		  for(n = 0; n < j; n++)
			pthread_mutex_destroy(sMutexes + n);
		}
		if(successes != i*MAXMUTEX){
			break;
		}
	}
	if(successes == EXPECTED_SUCCESSES){
		printf("RESULT=PASS");
	}else{
		printf("RESULT=FAIL");
	}
  return 0;
}

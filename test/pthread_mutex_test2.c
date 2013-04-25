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

#define MAXTHREAD 15
#define MAXMUTEX 20

static pthread_cond_t sCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t sCondMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t sMutexes[MAXMUTEX];
volatile static int sTotal[MAXMUTEX] = { 0 };
volatile static int sThreadCount = 0;
volatile static int sMutexCount;
volatile static int sCycle = 0;

static int total()
{
  int result = 0, i;

  for(i = 0; i < MAXMUTEX; i++)
    result += sTotal[i];
  return result;
}

static void *threadProc(void *arg)
{
  int id = (int)arg;
  unsigned seed = id;
  pthread_mutex_lock(&sCondMutex);
  for(;;)
  {
    __sync_fetch_and_add(&sCycle, 1);
    pthread_cond_wait(&sCond, &sCondMutex);
    if(sThreadCount < 0)
      break;
    if(id < sThreadCount)
    {
      int i;
      for(i = 0; i < 4000; i++)
      {
        int ln = rand_r(&seed) % sMutexCount;

        pthread_mutex_lock(sMutexes + ln);
        sTotal[ln]++;
        pthread_mutex_unlock(sMutexes + ln);
      }
    }
  }
  pthread_mutex_unlock(&sCondMutex);
  return NULL;
}

static void *dummyThreadProc(void *arg)
{
  return NULL;
}

int main()
{
  pthread_mutex_t minit = PTHREAD_MUTEX_INITIALIZER;
  pthread_t threads[MAXTHREAD];
  int i, j, n;

  for(j = 0; j < MAXMUTEX; j++)
    sMutexes[j] = minit;
  for(i = 0; i < MAXTHREAD; i++)
    pthread_create(threads + i, NULL, threadProc, (void *)i);
  while(sCycle < MAXTHREAD); // everyone waiting?
  pthread_mutex_lock(&sCondMutex);
  pthread_mutex_unlock(&sCondMutex);
  for(i = 1; i <= MAXTHREAD; i++) // # of threads
    for(j = 1; j <= MAXMUTEX; j++) // # of mutexes
    {
      int nextCycle = sCycle + MAXTHREAD;

      printf("%d threads %d mutexes %d total\n", i, j, total());
      sThreadCount = i;
      sMutexCount = j;
      pthread_cond_broadcast(&sCond);
      while(sCycle != nextCycle)
      {
#if 0
        pthread_t dummyThread;
        pthread_create(&dummyThread, NULL, dummyThreadProc, NULL);
        pthread_join(dummyThread, NULL);
#endif
      }
      pthread_mutex_lock(&sCondMutex);
      pthread_mutex_unlock(&sCondMutex);
    }
  sThreadCount = -1;
  pthread_cond_broadcast(&sCond);
  {
    int k;
    for(k = 0; k < MAXTHREAD; k++)
      pthread_join(threads[k], NULL);
  }
  printf("%d total\n", total());

  if(i - 1 == MAXTHREAD && j - 1 == MAXMUTEX)
    printf("RESULT=PASS");
  else
    printf("RESULT=FAIL");
	
  return 0;
}

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
#ifdef __AVM2__
#include <AS3/AVM2.h>
#endif

static void delaySome()
{
#ifdef __AVM2__
  unsigned dummy;

  avm2_self_msleep(&dummy, 300);
#else
  sleep(1);
#endif
}

static pthread_barrier_t sBarrier;
static pthread_key_t sKey;
static long sTid = -1;

static void *threadProc(void *arg)
{
  int i;
#ifdef __AVM2__
  thr_self(&sTid);
#endif
  pthread_setspecific(sKey, arg);
  pthread_barrier_wait(&sBarrier);
  for(i = 0; i < 5; i++)
  {
    printf("thread specific: %d\n", (int)pthread_getspecific(sKey));
    delaySome();
  }
  return NULL;
}

static void cleanupProc(void *val)
{
  printf("cleanup: %d\n", (int)val);
}

static void *getspecThunk(void *key)
{
  return pthread_getspecific(*(pthread_key_t *)key);
}

int main()
{
  int i;
  pthread_t thread;

  pthread_barrier_init(&sBarrier, NULL, 2);
  pthread_key_create(&sKey, cleanupProc);
  pthread_create(&thread, NULL, threadProc, (void *)78);
  pthread_setspecific(sKey, (void *)42);
  pthread_barrier_wait(&sBarrier);
#ifdef __AVM2__
  {
    unsigned dummy;

    avm2_self_msleep(&dummy, 100);
  }
#endif
  for(i = 0; i < 5; i++)
  {
    printf("main specific: %d\n", (int)pthread_getspecific(sKey));
#ifdef __AVM2__
    if(i < 3)
      printf("impersonate specific: %d\n", (int)avm2_thr_impersonate(sTid, getspecThunk, &sKey));
#endif
    
    delaySome();
  }
  pthread_join(thread, NULL);
  pthread_key_delete(sKey);
  return 0;
}

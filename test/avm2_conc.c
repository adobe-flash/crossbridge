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
#include <AS3/AVM2.h>

static volatile int done = 0;
static int dummy;
static long tid1;

static void delaySome()
{
  avm2_self_msleep(&dummy, 300); // 300ms sleep (keeps us under 15s timeout in player)
}

static void *lockThread(void *arg)
{
  long id;

  thr_self(&id);
  avm2_lock((unsigned *)arg);
  printf("locked: %ld\n", id);
  delaySome();
  avm2_unlock((unsigned *)arg);
  printf("unlocked: %ld => %d\n", id, avm2_locked_id((unsigned *)arg));
  __sync_fetch_and_add(&done, 1);
  return NULL;
}

static unsigned msleepThreadMutex = 0;

static void *msleepThread(void *arg)
{
  long id;

  thr_self(&id);
  avm2_lock(&msleepThreadMutex);
  printf("about to msleep: %ld (%p)\n", id, arg);
  avm2_msleep(arg, &msleepThreadMutex, 0);
  printf("awake!: %ld\n", id);
  delaySome();
  avm2_unlock(&msleepThreadMutex);
  __sync_fetch_and_add(&done, 1);
  return NULL;
}

static void *printTid(void *arg)
{
  long tid;

  thr_self(&tid);
  tid1 = tid;
  printf("thread tid: %ld [%s]\n", tid, (char *)arg);
}

static void *printTidThread(void *arg)
{
  printTid(arg);
  delaySome();
  delaySome();
  return NULL;
}

#define THREADS 4

int main(int argc, char **argv) 
{ 
  int i;
  unsigned lock = 0;

  // contend some locks
  avm2_lock(&lock);
  for(i = 0; i < THREADS; i++)
  {
    pthread_t thread;

    pthread_create(&thread, NULL, lockThread, &lock);
    delaySome();
  }
  printf("about to unlock...\n");
  avm2_unlock(&lock);
  while(done != THREADS);
  delaySome();
  done = 0;
  // wake a bunch of sleepers
  for(i = 0; i < THREADS; i++)
  {
    pthread_t thread;

    pthread_create(&thread, NULL, msleepThread, &dummy);
    delaySome();
  }
  avm2_lock(&msleepThreadMutex);
  printf("about to wake...\n");
  avm2_wake(&dummy); // wake them all
  delaySome();
  avm2_unlock(&msleepThreadMutex);
  while(done != THREADS);
  delaySome();
  done = 0;
  // wake one at a time
  for(i = 0; i < THREADS; i++)
  {
    pthread_t thread;

    pthread_create(&thread, NULL, msleepThread, &dummy);
    delaySome();
  }
  for(i = 0; i < THREADS; i++)
  {
    avm2_lock(&msleepThreadMutex);
    printf("about to wake one...\n");
    avm2_wake_one(&dummy);
    delaySome();
    avm2_unlock(&msleepThreadMutex);
  }
  while(done != THREADS);
  done = 0;
  {
    long tid;
    pthread_t thread;

    thr_self(&tid);
    printf("main tid: %ld\n", tid);

    pthread_create(&thread, NULL, printTidThread, "real thread");

    delaySome();
    printf("about to impersonate...\n");
    avm2_thr_impersonate(tid1, printTid, "impersonator!");
  }
  printf("done\n");
  printf("RESULT=PASS\n");
  return 0;
} 


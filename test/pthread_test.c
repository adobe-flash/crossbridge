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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#define THREADS 4
#define BIG_NUMBER_SYNC 1000000
#define BIG_NUMBER_MUTEX 60000
#define BIG_NUMBER_SPECIFIC 10000000

int sync_pass = 0;
int mutex_pass = 0;
int specific_pass = 0;
int cond_signal_pass = 0;
int cond_broadcast_pass = 0;
int rwlock_pass = 0;
int rwlock_acquired_0 = 0;
int rwlock_acquired_1 = 0;
int rwlock_acquired_2 = 0;
int rwlock_acquired_3 = 0;
int rwlock_released_0 = 0;
int rwlock_released_1 = 0;
int rwlock_released_2 = 0;
int rwlock_released_3 = 0;
int rwkill_pass = 0;
int azr_error = 0;
int final_pass = 0;

int specific_counter = 0;
pthread_mutex_t specific_counter_lock;

#define AZR(X) \
  do { if((X) != 0){ \
         err("[" #X "] had non-zero return"); \
         azr_error = 1; \
       } \
  } while(0)

static void err(const char *str)
{
  fprintf(stderr, "error: %s\n", str);
  exit(-1);
}

static void *test_sync_tp(void *arg)
{
  int i;

  for(i = 0; i < BIG_NUMBER_SYNC; i++)
    __sync_fetch_and_add((int *)arg, 1);
  return NULL;
}

static void test_sync()
{
  int counter = 0;
  int i;
  pthread_t threads[THREADS];

  for(i = 0; i < THREADS; i++)
    AZR(pthread_create(threads + i, NULL, test_sync_tp, &counter));
  for(i = 0; i < THREADS; i++)
    AZR(pthread_join(threads[i], NULL));
  printf("\tcounter: %d\n", counter);

  if(counter == BIG_NUMBER_SYNC * THREADS){
    sync_pass = 1;
  }	
}

struct tm_args
{
  int counter;
  pthread_mutex_t mutex;
};

static void *test_mutex_tp(void *arg)
{
  int i;
  volatile struct tm_args *args = (struct tm_args *)arg;

  for(i = 0; i < BIG_NUMBER_MUTEX; i++)
  {
    AZR(pthread_mutex_lock(&args->mutex));
    args->counter++;
    AZR(pthread_mutex_unlock(&args->mutex));
  }
  return NULL;
}

static void test_mutex()
{
  int i;
  pthread_t threads[THREADS];
  volatile struct tm_args args = { 0, PTHREAD_MUTEX_INITIALIZER };

  for(i = 0; i < THREADS; i++)
    AZR(pthread_create(threads + i, NULL, test_mutex_tp, &args));
  for(i = 0; i < THREADS; i++)
    AZR(pthread_join(threads[i], NULL));
  AZR(pthread_mutex_destroy(&args.mutex));
  printf("\tcounter: %d\n", args.counter);
  
  if(args.counter == BIG_NUMBER_MUTEX * THREADS){
    mutex_pass = 1;
  }	
	
}

static void test_specific_kd(void *val)
{
  printf("\tspecific: %d\n", (int)val);

  pthread_mutex_lock(&specific_counter_lock);

  // Every thread must report BIG_NUMBER_SPECIFIC, and
  // we must get that THREADS times.
  if((int)val == BIG_NUMBER_SPECIFIC){
    ++specific_counter;		
  }

  if(specific_counter == THREADS){
    specific_pass = 1;
  }else{
    specific_pass = 0;
  }

  pthread_mutex_unlock(&specific_counter_lock);
}

static void *test_specific_tp(void *arg)
{
  pthread_key_t *key = (pthread_key_t *)arg;
  int i;

  for(i = 0; i < BIG_NUMBER_SPECIFIC; i++)
    AZR(pthread_setspecific(*key, (void *)(1 + (int)pthread_getspecific(*key))));
  return NULL;
}

static void test_specific()
{
  int i;
  pthread_t threads[THREADS];
  pthread_key_t key;

  AZR(pthread_key_create(&key, &test_specific_kd));
  for(i = 0; i < THREADS; i++)
    AZR(pthread_create(threads + i, NULL, test_specific_tp, &key));
  for(i = 0; i < THREADS; i++)
    AZR(pthread_join(threads[i], NULL));
  AZR(pthread_key_delete(key));
}

struct tcs_args
{
  int tid;
  int counter;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
};

static void *test_cond_tp(void *arg)
{
  volatile struct tcs_args *args = (struct tcs_args *)arg;
  int tid = __sync_fetch_and_add(&args->tid, 1);

  AZR(pthread_mutex_lock(&args->mutex));
  __sync_fetch_and_add(&args->counter, 1);
  AZR(pthread_cond_wait(&args->cond, &args->mutex));
  __sync_fetch_and_sub(&args->counter, 1);
  AZR(pthread_mutex_unlock(&args->mutex));
}

static void test_cond_signal()
{
  int i;
  pthread_t threads[THREADS];
  volatile struct tcs_args args = { 0, 0, PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER };

  for(i = 0; i < THREADS; i++)
    AZR(pthread_create(threads + i, NULL, test_cond_tp, &args));
  while(args.counter != THREADS);
  printf("\tall threads started\n");
  // make sure they're all waiting
  AZR(pthread_mutex_lock(&args.mutex));
  AZR(pthread_mutex_unlock(&args.mutex));
  printf("\tall threads waiting\n");
  // they're all waiting on the condition now, signal one at a time
  for(i = THREADS-1; i >= 0; i--)
  {
    if(args.counter != (i+1))
      err("unexpected counter value in test_cond_signal");
    AZR(pthread_mutex_lock(&args.mutex));
    AZR(pthread_cond_signal(&args.cond));
    AZR(pthread_mutex_unlock(&args.mutex));
    while(args.counter != i);
  }
  printf("\tall threads signaled\n");
  cond_signal_pass = 1;
  for(i = 0; i < THREADS; i++)
    AZR(pthread_join(threads[i], NULL));
  AZR(pthread_cond_destroy(&args.cond));
  AZR(pthread_mutex_destroy(&args.mutex));
}

static void test_cond_broadcast()
{
  int i;
  pthread_t threads[THREADS];
  volatile struct tcs_args args = { 0, 0, PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER };

  for(i = 0; i < THREADS; i++)
    AZR(pthread_create(threads + i, NULL, test_cond_tp, &args));
  while(args.counter != THREADS); // wait for them all to start up
  printf("\tall threads started\n");
  // make sure they're all waiting
  AZR(pthread_mutex_lock(&args.mutex));
  AZR(pthread_mutex_unlock(&args.mutex));
  printf("\tall threads waiting\n");
  // they're all waiting on the condition now, signal all at once!
  AZR(pthread_mutex_lock(&args.mutex));
  AZR(pthread_cond_broadcast(&args.cond));
  AZR(pthread_mutex_unlock(&args.mutex));
  while(args.counter != 0);
  printf("\tall threads signaled\n");
  cond_broadcast_pass = 1;
  for(i = 0; i < THREADS; i++)
    AZR(pthread_join(threads[i], NULL));
  AZR(pthread_cond_destroy(&args.cond));
  AZR(pthread_mutex_destroy(&args.mutex));
}

static void *test_kill_tp(void *arg)
{
  for(;;)
    __sync_fetch_and_add((int *)arg, 1);
  retun NULL;
}

static void test_kill()
{
  pthread_t thread;
  volatile int counter = 0;

  AZR(pthread_create(&thread, NULL, test_kill_tp, &counter));
  printf("\tthread created\n");
  while(counter < 100);
  AZR(pthread_kill(thread, SIGUSR1));
  AZR(pthread_join(thread, NULL));
  printf("\tthread killed\n");
  rwkill_pass = 1;
}

struct trw_args
{
  int tid;
  int readers;
  pthread_rwlock_t rwlock;
};

static void *test_rwlock_tp(void *arg)
{
  volatile struct trw_args *args = (struct trw_args *)arg;
  int tid = __sync_fetch_and_add(&args->tid, 1);

  while(tid != args->readers);
  AZR(pthread_rwlock_rdlock(&args->rwlock));
  printf("\trwlock acquired %d\n", tid);
	
  if(tid == 0)
    rwlock_acquired_0 = 1;
  else if(tid == 1)
    rwlock_acquired_1 = 1;
  else if(tid == 2)
    rwlock_acquired_2 = 1;
  else if(tid == 3)
    rwlock_acquired_3 = 1;
	
  args->readers++;
  while(args->readers != tid);
  AZR(pthread_rwlock_unlock(&args->rwlock));
  printf("\trwlock released %d\n", tid);

  if(tid == 0)
    rwlock_released_0 = 1;
  else if(tid == 1)
    rwlock_released_1 = 1;
  else if(tid == 2)
    rwlock_released_2 = 1;
  else if(tid == 3)
    rwlock_released_3 = 1;

  args->readers--;

  return NULL;
}

static void test_rwlock()
{
  int i;
  pthread_t threads[THREADS];
  volatile struct trw_args args = { 0, 0, PTHREAD_RWLOCK_INITIALIZER };

  AZR(pthread_rwlock_wrlock(&args.rwlock));
  for(i = 0; i < THREADS; i++)
    AZR(pthread_create(threads + i, NULL, test_rwlock_tp, &args));
  AZR(pthread_rwlock_unlock(&args.rwlock));
  while(args.readers != THREADS);
  printf("\tall readers acquired locks\n");
  if(pthread_rwlock_trywrlock(&args.rwlock) != EBUSY)
    err("pthread_rwlock_trywrlock didn't return EBUSY in test_rwlock");
  printf("\twr lock not acquired\n");
  args.readers--;
  AZR(pthread_rwlock_wrlock(&args.rwlock));
  printf("\twr lock acquired\n");
  AZR(pthread_rwlock_unlock(&args.rwlock));
  for(i = 0; i < THREADS; i++)
    AZR(pthread_join(threads[i], NULL));
  AZR(pthread_rwlock_destroy(&args.rwlock));
  rwlock_pass = rwlock_acquired_0 && rwlock_acquired_1 && rwlock_acquired_2 && rwlock_acquired_3 &&
	rwlock_released_0 && rwlock_released_1 && rwlock_released_2 && rwlock_released_3; 
}

void sigusr1(int dummy)
{
  pthread_exit(NULL); // just exit this thread (emulate behavior we'll have in player...)
}

int main()
{
	
  if (pthread_mutex_init(&specific_counter_lock, NULL) != 0)
  {
    printf("\nspecific_counter_lock init failed\n");
    return 1;
  }	

  signal(SIGUSR1, sigusr1);
  printf("testing __sync\n");
  test_sync(); // test __sync stuff
  printf("testing mutex\n");
  test_mutex(); // test mutex stuff
  printf("testing specific\n");
  test_specific(); // test get/setspecific stuff
  printf("testing cond_signal\n");
  test_cond_signal();
  printf("testing cond_broadcast\n");
  test_cond_broadcast();
  printf("testing rwlock\n");
  test_rwlock();
  printf("testing kill\n");
  test_kill();
	
  final_pass = !azr_error && sync_pass && mutex_pass && specific_pass && 
               cond_signal_pass && cond_broadcast_pass &&
               rwlock_pass && rwkill_pass;

  if( final_pass ){
    printf("RESULT=PASS\n");
  }else{
    printf("RESULT=FAIL\n");
  }
	
  return 0;
}

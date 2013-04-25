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

// trivial test for "pthread_create"
// uses trace to not require posix
#include <pthread.h>
#include <stdio.h>

static volatile int done = 0;
static int bar = 0;
int successfulDoneOnes = 0;
int successfulDoneTwos = 1;
int pass = 0;
int EXPECTED_SUCCESSFUL_DONES = 4;
pthread_mutex_t lock1;

static void *foo(void *arg)
{
  int n;
  printf("in foo -- bar: %d done: %d\n", *(int *)arg, done);

  pthread_mutex_lock(&lock1);
  if( done == successfulDoneOnes ){
    ++successfulDoneOnes;
  }else{
    printf("PROBLEM 1 done=%d, successfulDoneOnes=%d\n", done, successfulDoneOnes);
  }

  n = __sync_add_and_fetch(&done, 1);
  printf("(after atomic incr) done: %d\n", done);

  if( done == successfulDoneTwos ){
    ++successfulDoneTwos;
  }else{
    printf("PROBLEM 2 done=%d, successfulDoneTwos=%d\n", done, successfulDoneTwos);
  }

  pthread_mutex_unlock(&lock1);
	
  return NULL;
}

#define THREADS 4

int main(int argc, char **argv) 
{ 
  int i;

  if(pthread_mutex_init(&lock1, NULL) != 0){
    printf("mutex_init failed\n");
    return 1;
  }

  if(pthread_mutex_init(&lock1, NULL) != 0){
    printf("mutex_init failed\n");
    return 1;
  }
	
  for(i = 0, bar = 123; i < THREADS; i++, bar++)
  {
    pthread_t tid;
    printf("creating #%d\n", i);
    int x = pthread_create(&tid, NULL, foo, &bar);
    printf("creating #%d\n", x);
  }
	
  while(done != THREADS);
  printf("done\n");

  if(successfulDoneOnes == EXPECTED_SUCCESSFUL_DONES && successfulDoneTwos == EXPECTED_SUCCESSFUL_DONES + 1){
    printf("RESULT=PASS\n");
  }else{
    printf("RESULT=FAIL\n");
  }
	
  pthread_mutex_destroy(&lock1);

  return 0;
} 


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

#ifdef __AVM2__
#include <AS3/AVM2.h>
#else
#define pthread_suspend_np pthread_suspend
#define pthread_resume_np pthread_continue
#endif

int successes = 0;
int EXPECTED_SUCCESSES = 6;

static void delaySome()
{
#ifdef __AVM2__
  unsigned dummyLock = 1;

  avm2_msleep(&dummyLock, &dummyLock, 300);
#else
  sleep(1);
#endif
}

static volatile int counter = 0;
int counter_saved = 0;

void *threadProc(void *arg)
{
  for(;;)
  {
    __sync_fetch_and_add(&counter, 1);
    pthread_testcancel();
  }
  return 0;
}

int main(int argc, char **argv) 
{ 
  pthread_t thread;
  
  if( counter == 0 ){
    ++successes;
  }else{
    printf("FAIL: pre-create counter problem\n");
    return 0;
  }
  
  pthread_create(&thread, NULL, threadProc, NULL);
  
  if( counter == 0 ){
    ++successes;
  }else{
    printf("FAIL: post-create counter problem\n");
    return 0;
  }
  
  delaySome();
  delaySome();

  if( counter > 0 ){
    ++successes;
  }else{
    printf("FAIL: counter problem after initial delays\n");
    return 0;
  }  
  pthread_suspend_np(thread);
  counter_saved = counter;
  delaySome();
  delaySome();

  if( counter == counter_saved ){
    ++successes;
  }else{
    printf("FAIL: counter != counter_saved when suspended\n");
    return 0;
  }    
  pthread_resume_np(thread);

  if( counter == counter_saved ){
    ++successes;
  }else{
    printf("FAIL: counter != counter_saved when resuming\n");
    return 0;
  }      
  delaySome();
  delaySome();

  if( counter > counter_saved ){
    ++successes;
  }else{
    printf("FAIL: counter problem after resuming and waiting\n");
    return 0;
  }
  
  if( successes == EXPECTED_SUCCESSES ){
    printf("RESULT=PASS\n");
  }else{
    printf("RESULT=FAIL\n");
  }
  return 0;
}
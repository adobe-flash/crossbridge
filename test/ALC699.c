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

// regressiong test for ALC699
// compile with:
// alchemy/sdk/usr/bin/gcc -pthread -emit-swf ALC699.c -o ALC699.swf

#include <AS3/AVM2.h>
#include <pthread.h>
#include <stdio.h>

static void *threadProc(void *arg)
{
  sleep(90);
  *(int *)arg = 1;
  return NULL;
}

static void *nopProc(void *arg)
{
  return arg;
}

int main()
{
  volatile int stop = 0;
  pthread_t thread;
  pthread_create(&thread, NULL, threadProc, (int *)&stop);
  printf("start (test duration ~91s -- ensure this amount of time will trip the script timeout in your player AND that you're running a debugger player [so you'll see the exception!])\n");
  sleep(1);
  while(!stop)
    avm2_ui_thunk(nopProc, NULL);
  pthread_join(thread, NULL);
  printf("done\n");
  return 0;
}

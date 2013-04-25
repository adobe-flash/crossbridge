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
extern "C" {
#include <sys/thr.h>
}
#include <AS3/AVM2.h>
#include <stdio.h>

static char cond;

extern "C" int tprintf(const char *tmpl, ...);

static void *wakeThunk(void *arg)
{
  return (void *)avm2_wake_one(arg);
}

static void *threadProc(void *arg)
{
  avm2_thr_impersonate((long)arg, wakeThunk, &cond);
  return NULL;
}

int main()
{
  pthread_t thread;
  long id;

  thr_self(&id);
  pthread_create(&thread, NULL, threadProc, (void *)id);
  tprintf("sleeping!\n");
  avm2_self_msleep(&cond, 0);
  tprintf("ok!\n");
  pthread_join(thread, NULL);
  return 0;
}

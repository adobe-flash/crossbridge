/*
** Copyright (c) 2013 Adobe Systems Inc

** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:

** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
*/

// tests global write ordering

#include <stdio.h>
#include <pthread.h>
#include <string.h>

typedef struct {
  volatile int valid;
  char pad[4100];
  volatile int * volatile ptr;
} Data;

#define THREADS 4

Data data[THREADS];

int foo = 1;

void *threadProc(void *arg)
{
  int tid = (int)arg;
  int counter = 0;
  Data *mydata = data + tid;
  Data *peerdata = data + ((tid + 1) % THREADS);

  for(int i = 0; i < 1000*1000*200; i++)
  {
    if(peerdata->valid)
    {
      counter += *(peerdata->ptr);
      peerdata->valid = 0;
    }
    if(mydata->valid == 0)
    {
      mydata->ptr = (int *)-1;
      mydata->ptr = &foo;
      mydata->valid = 1;
    }
  }
  printf("%d %d\n", tid, counter);
  return NULL;
}

int main()
{
  memset((void *)data, 0, sizeof(data));

  pthread_t threads[THREADS];
  for(int i = 0; i < THREADS; i++)
    pthread_create(threads + i, NULL, threadProc, (void *)i);
  for(int i = 0; i < THREADS; i++)
    pthread_join(threads[i], NULL);
  printf("SUCCESS!\n");
  return 0;
}

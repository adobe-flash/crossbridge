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

static void *threadProc(void *arg)
{
  return arg;
}

int main()
{
  #define THREADS 10
  pthread_t threads[THREADS];
  int n;

  for(n = 0; n < THREADS; n++)
    pthread_create(threads + n, NULL, threadProc, NULL);
  for(n = 0; n < THREADS * 1000; n++)
  {
    if(!(n % 100))
      printf("%d\n", n);
    pthread_join(threads[n % THREADS], NULL);
    pthread_create(threads + (n % THREADS), NULL, threadProc, NULL);
  }
  for(n = 0; n < THREADS; n++)
    pthread_join(threads[n], NULL);
  return 0;
}

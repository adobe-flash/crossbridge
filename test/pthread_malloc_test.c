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
#include <string.h>

#define THREADS 4
#define SLOTS 10
#define MAX_SIZE (1024 * 1024) 

void *threadProc(void *arg)
{
  void *mem[SLOTS];
  int i, j;
  int thread_id = *( (int*)arg );
  char* current_output;

  for(i = 0; i < SLOTS; i++){
    mem[i] = malloc(rand() % MAX_SIZE + 1);
    if(mem[i] == NULL){
	  printf("RESULT=FAIL");
	  return 0;
    }
  }

  for(j = 0; j < 2000; j++)
  {
    for(i = 0; i < SLOTS; i++)
    {
      free(mem[i]);
      mem[i] = malloc(rand() % MAX_SIZE + 1);
      if(mem[i] == NULL){
	    printf("RESULT=FAIL");
	    return 0;
      }
    }
  }

  for(i = 0; i < SLOTS; i++)
    free(mem[i]);

  return NULL;
}

int main()
{

    pthread_t threads[THREADS];
    int i, j;
	for(i = 0; i < THREADS; i++){
		pthread_create(threads + i, NULL, threadProc, NULL);
	}

	for(j = 0; j < THREADS; j++)
		pthread_join(threads[j], NULL);

	// If we got this far, all malloc() calls succeeeded.
	printf("RESULT=PASS\n");
	return 0;
}

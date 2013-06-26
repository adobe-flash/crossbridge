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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define BUFFER_SIZE 4096
int buffer[BUFFER_SIZE];
int loop_count = 4096;
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MAX_THREADS 4
pthread_t threads[MAX_THREADS];

void* runTest(void* ptr) {
	(void)ptr;
	int i=0,j=0;
	for(j=0; j<loop_count; j++) {
		for(i=0; i<BUFFER_SIZE; i++) {
			pthread_mutex_lock(&buffer_mutex);
			buffer[i] = i;
			pthread_mutex_unlock(&buffer_mutex);
		}
	}

	return NULL;
}

int main()
{
	clock_t start, end;
	double s=0;
	int t=0,tc=0;

	fprintf(stderr, "Uncontended lock test...\n");
	loop_count = 4096;
	start = clock();
		runTest(NULL);
	end = clock();
	s = ((double) (end - start)) / CLOCKS_PER_SEC;
	fprintf(stderr, "time: %fs\n", s);

	for(t=2; t<=MAX_THREADS; t++) {
		fprintf(stderr, "Contended lock test (%d threads)...\n", t);
		loop_count = 256;
		if(pthread_mutex_trylock(&buffer_mutex) != 0) {
			fprintf(stderr, "TEST FAILED");
			abort();
		}
		for(tc=0; tc<t; tc++)
			pthread_create(&threads[tc], NULL, runTest, NULL);
		sleep(2);
		start = clock();
			pthread_mutex_unlock(&buffer_mutex);
			for(tc=0; tc<t; tc++)
				pthread_join(threads[tc], NULL);
		end = clock();
		s = ((double) (end - start)) / CLOCKS_PER_SEC;
		fprintf(stderr, "time: %fs\n", s);
	}
}
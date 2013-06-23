#include <stdio.h>
#include <stdlib.h>

double loop(float *x, float *y, long length) {
  long i;
  double accumulator = 0.0;
  for (i=0; i<length; ++i) {
    accumulator += (double)x[i] * (double)y[i];
  }
  return accumulator;
}

#ifdef SMALL_PROBLEM_SIZE
#define COUNT 100000
#else
#define COUNT 500000
#endif

int main(int argc, char *argv[]) {
  int i, j;
  float x[2048];
  float y[2048];
  double total = 0.0;
  float a = 0.0f;
  float b = 1.0f;
    
  for (i=0; i<COUNT; ++i) {
    if (i % 10) {
      a = 0.0f;
      b = 1.0f;
    } else {
      a += 0.1f;
      b += 0.2f;
    }
    for (j=0; j<2048; ++j) {
      x[j] = a + (float)j;
      y[j] = b + (float)j;
    }
    total += loop(x, y, 2048);
  }
  
  printf("Total is %g\n", total);
    
  return 0;
}

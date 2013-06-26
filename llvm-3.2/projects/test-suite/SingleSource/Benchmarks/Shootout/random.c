/* -*- mode: c -*-
 * $Id: random.c 36673 2007-05-03 16:55:46Z laurov $
 * http://www.bagley.org/~doug/shootout/
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define inline static

#define IM 139968
#define IA 3877
#define IC 29573

inline double gen_random(double max) {
  static long last = 42;
    
  last = (last * IA + IC) % IM;
  return( max * last / IM );
}

int main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 40000000
#else
#define LENGTH 400000000
#endif
  int N = ((argc == 2) ? atoi(argv[1]) : LENGTH) - 1;
    
  while (N--) {
    gen_random(100.0);
  }
  printf("%.9f\n", gen_random(100.0));
  return(0);
}

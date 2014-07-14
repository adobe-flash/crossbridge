/* -*- mode: c -*-
 * $Id: fib2.c 36673 2007-05-03 16:55:46Z laurov $
 * http://www.bagley.org/~doug/shootout/
 */

int atoi(char *);
#include <stdio.h>

unsigned long
fib(unsigned long n) {
    if (n < 2)
	return(1);
    else
	return(fib(n-2) + fib(n-1));
}

int
main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 40
#else
#define LENGTH 43
#endif
    int N = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    printf("%ld\n", fib(N));
    return(0);
}

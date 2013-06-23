// -*- mode: c++ -*-
// $Id: fibo.cpp 36673 2007-05-03 16:55:46Z laurov $
// http://www.bagley.org/~doug/shootout/

#include <iostream>
#include <stdlib.h>

using namespace std;

unsigned long fib(unsigned long n) {
    if (n < 2)
	return(1);
    else
	return(fib(n-2) + fib(n-1));
}

int main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 40
#else
#define LENGTH 43
#endif
    int n = ((argc == 2) ? atoi(argv[1]) : LENGTH);

    cout << fib(n) << endl;
    return(0);
}

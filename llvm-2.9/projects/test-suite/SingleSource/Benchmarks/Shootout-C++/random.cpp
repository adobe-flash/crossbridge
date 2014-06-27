// -*- mode: c++ -*-
// $Id: random.cpp 36673 2007-05-03 16:55:46Z laurov $
// http://www.bagley.org/~doug/shootout/

#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

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
#define LENGTH 4000000
#else
#define LENGTH 400000000
#endif
    int N = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    double result = 0;
    
    while (N--) {
	result = gen_random(100.0);
    }
    cout.precision(9);
    cout.setf(ios::fixed);
    cout << result << endl;
    return(0);
}


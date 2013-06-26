// -*- mode: c++ -*-
// $Id: ary3.cpp 53813 2008-07-21 02:52:34Z nicholas $
// http://www.bagley.org/~doug/shootout/

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 150000
#else
#define LENGTH 1500000
#endif
    int i, k, n = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    typedef vector<int> ARY;
    ARY x(n);
    ARY y(n);

    for (i=0; i<n; i++) {
	x[i] = i + 1;
    }
    for (k=0; k<1000; k++) {
	for (int i = n - 1; i >= 0; --i) {
	    y[i] += x[i];
	}
    }

    cout << y[0] << " " << y.back() << endl;
}

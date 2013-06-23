// -*- mode: c++ -*-
// $Id: ary.cpp 53813 2008-07-21 02:52:34Z nicholas $
// http://www.bagley.org/~doug/shootout/

#include <cstdlib>
#include <iostream>
#include <vector>

int
main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 900000
#else
#define LENGTH 9000000
#endif
    int i, n = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    typedef std::vector<int> ARY;
    ARY x(n);
    ARY y(n);

    for (i=0; i<n; i++) {
	x[i] = i;
    }
    for (int i = n - 1; i >= 0; --i) {
        y[i] = x[i];
    }

    std::cout << y.back() << std::endl;
}

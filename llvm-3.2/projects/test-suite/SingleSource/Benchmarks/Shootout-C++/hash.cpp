// -*- mode: c++ -*-
// $Id: hash.cpp 59315 2008-11-14 20:29:00Z edwin $
// http://www.bagley.org/~doug/shootout/

#include <stdio.h>
#include <iostream>
#include <ext/hash_map>
#include <cstring>

using namespace std;
using namespace __gnu_cxx;

struct eqstr {
    bool operator()(const char* s1, const char* s2) const {
	return strcmp(s1, s2) == 0;
    }
};

int
main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 50000
#else
#define LENGTH 500000
#endif
    int n = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    char buf[16];
    typedef hash_map<const char*, int, hash<const char*>, eqstr> HM;
    HM X;

    for (int i=1; i<=n; i++) {
	sprintf(buf, "%x", i);
	X[strdup(buf)] = i;
    }

    int c = 0;
    for (int i=n; i>0; i--) {
	sprintf(buf, "%d", i);
	if (X[strdup(buf)]) c++;
    }

    cout << c << endl;
}

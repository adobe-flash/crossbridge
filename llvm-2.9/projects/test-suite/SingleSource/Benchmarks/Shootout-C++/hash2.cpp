// -*- mode: c++ -*-
// $Id: hash2.cpp 59315 2008-11-14 20:29:00Z edwin $
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
#define LENGTH 200
#else
#define LENGTH 2000
#endif
    int n = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    char buf[16];
    typedef hash_map<const char*, int, hash<const char*>, eqstr> HM;
    HM hash1, hash2;

    for (int i=0; i<10000; i++) {
	sprintf(buf, "foo_%d", i);
	hash1[strdup(buf)] = i;
    }
    for (int i=0; i<n; i++) {
	for (HM::iterator k = hash1.begin(); k != hash1.end(); ++k) {
	    hash2[(*k).first] += hash1[(*k).first];
	}
    }
    cout << hash1["foo_1"] << " " << hash1["foo_9999"] << " "
	 << hash2["foo_1"] << " " << hash2["foo_9999"] << endl;
}

// -*- mode: c++ -*-
// $Id: ackermann.cpp 36673 2007-05-03 16:55:46Z laurov $
// http://www.bagley.org/~doug/shootout/

#include <iostream>
#include <stdlib.h>

using namespace std;

int Ack(int M, int N) { return(M ? (Ack(M-1,N ? Ack(M,(N-1)) : 1)) : N+1); }

int main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 11
#else
#define LENGTH 12
#endif
    int n = ((argc == 2) ? atoi(argv[1]) : LENGTH);

    cout << "Ack(3," << n << "): " << Ack(3, n) << endl;
    return(0);
}

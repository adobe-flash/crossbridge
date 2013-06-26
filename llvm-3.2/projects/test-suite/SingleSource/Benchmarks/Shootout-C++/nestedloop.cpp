// -*- mode: c++ -*-
// $Id: nestedloop.cpp 36673 2007-05-03 16:55:46Z laurov $
// http://www.bagley.org/~doug/shootout/

#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 30
#else
#define LENGTH 46
#endif
    int n = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    int a, b, c, d, e, f, x=0;
	
    for (a=0; a<n; a++)
	for (b=0; b<n; b++)
	    for (c=0; c<n; c++)
		for (d=0; d<n; d++)
		    for (e=0; e<n; e++)
			for (f=0; f<n; f++)
			    x++;

    cout << x << endl;
    return(0);
}

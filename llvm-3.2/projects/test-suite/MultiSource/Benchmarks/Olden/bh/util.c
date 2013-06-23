/* For copyright information, see olden_v1.0/COPYRIGHT */

/*
 * UTIL: various useful routines and functions.
 * Copyright (c) 1991, Joshua E. Barnes, Honolulu, HI.
 * 	    It's free because it's yours.
 */

#include "stdinc.h"
#include <errno.h>

#define A	16807.0
#define M	2147483647.0

void exit(int);

/* random:
 * return a random number, which is also the next seed.
 */
double my_rand(double seed)
{
    double	t = A*seed  + 1; 
    double floor();


    seed = t - (M * floor(t / M));
    return seed;

} /* end of random */

/*
 * XRAND: generate floating-point random number.
 */

double xrand(double xl, double xh, double r)
{   double res;

    res = xl + (xh-xl)*r/2147483647.0;
    return (res);
}




/*
 * ERROR: scream and die quickly.
 */

error(char *msg)
{
    fprintf(stderr, msg);
    if (errno != 0)
        perror("Error");
    exit(0);
}




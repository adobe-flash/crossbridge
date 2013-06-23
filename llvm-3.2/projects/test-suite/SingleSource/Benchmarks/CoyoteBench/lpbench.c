/*
    lpbench
    Standard C version
    17 April 2003

    Written by Scott Robert Ladd (scott@coyotegulch.com)
    No rights reserved. This is public domain software, for use by anyone.

    A number-crunching benchmark that can be used as a fitness test for
    evolving optimal compiler options via genetic algorithm.

    This program is a modernized version of the classic Linpack 
    benchmark. My implementation is based on Bonnie Toy's translation
    of the original Fortran source code, including modifications by Jack
    Dongarra and Roy Longbottom.
    
    I've updated the code as follows:
    
      1) ANSI C code
      2) A timing mechanism based on the POSIX clock_gettime() function
      3) Replaces C's rather weak random number generator
      4) Compute results for a 2500x2500 (6.25 million element) matrix

    Note that the code herein is design for the purpose of testing 
    computational performance; error handling and other such "niceties"
    is virtually non-existent.

    Actual benchmark results can be found at:
            http://www.coyotegulch.com

    Please do not use this information or algorithm in any way that might
    upset the balance of the universe or otherwise cause a disturbance in
    the space-time continuum.
*/

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// embedded random number generator; ala Park and Miller
static       long seed = 1325;
static const long IA   = 16807;
static const long IM   = 2147483647;
static const double AM = 4.65661287525E-10;
static const long IQ   = 127773;
static const long IR   = 2836;
static const long MASK = 123459876;

static double random_double()
{
    long k;
    double result;
    
    seed ^= MASK;
    k = seed / IQ;
    seed = IA * (seed - k * IQ) - IR * k;
    
    if (seed < 0)
        seed += IM;
    
    result = AM * seed;
    seed ^= MASK;
    
    return result;
}

#if defined(ACOVEA)
#if defined(arch_pentium4)
static const int N   = 1000;
static const int NM1 =  999; // N - 1
static const int NP1 = 1001; // N + 1
#else
static const int N   =  600;
static const int NM1 =  599; // N - 1
static const int NP1 =  601; // N + 1
#endif
#else
#ifdef SMALL_PROBLEM_SIZE
static const int N   = 400;
static const int NM1 = 399; // N - 1
static const int NP1 = 401; // N + 1
#else
static const int N   = 2000;
static const int NM1 = 1999; // N - 1
static const int NP1 = 2001; // N + 1
#endif
#endif

// benchmark code
void matgen (double ** a, double * b)
{
    // fill arrays
    int i, j;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            a[j][i] = random_double();

    for (i = 0; i < N; i++)
        b[i] = 0.0;

    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++)
            b[i] += a[j][i];
}

// finds the index of element having max. absolute value.
int idamax(int n, double * dx, int dx_off, int incx)
{
    double dmax, dtemp;
    int i, ix, itemp = 0;

    if (n < 1) 
        itemp = -1;
    else
    {
        if (n ==1)
            itemp = 0;
        else
        {
            if (incx != 1)
            {
                // code for increment not equal to 1
                dmax = fabs(dx[dx_off]);
                ix = 1 + incx;

                for (i = 1; i < n; i++)
                {
	                dtemp = fabs(dx[ix + dx_off]);

                    if (dtemp > dmax) 
                    {
	                    itemp = i;
	                    dmax = dtemp;
                    }

                    ix += incx;
                }
            }
            else
            {
                // code for increment equal to 1
                itemp = 0;
                dmax = fabs(dx[dx_off]);

                for (i = 1; i < n; i++)
                {
	                dtemp = fabs(dx[i + dx_off]);

	                if (dtemp > dmax)
                    {
                        itemp = i;
                        dmax = dtemp;
                    }
                }
            }
        }
    }

    return itemp;
}
    
// forms the dot product of two vectors.
static double ddot(int n, double * dx, int dx_off, int incx, double * dy, int dy_off, int incy)
{
    int i;
    double dtemp = 0.0;

    if (n > 0)
    {
        if (incx != 1 || incy != 1)
        {
            // code for unequal increments or equal increments not equal to 1
            int ix = 0;
	        int iy = 0;

            if (incx < 0)
                ix = (-n + 1) * incx;

	        if (incy < 0)
                iy = (-n + 1) * incy;

            for (i = 0;i < n; i++)
            {
                dtemp += dx[ix + dx_off] * dy[iy + dy_off];
                ix    += incx;
                iy    += incy;
            }
        }
        else
        {
            // code for both increments equal to 1
            for (i=0; i < n; i++)
                dtemp += dx[i + dx_off] * dy[i + dy_off];
        }
    }

    return(dtemp);
}

// scales a vector by a constant.
void dscal(int n, double da, double * dx, int dx_off, int incx)
{
    int i;

    if (n > 0)
    {
        if (incx != 1)
        {
            // code for increment not equal to 1
	        int nincx = n * incx;

            for (i = 0; i < nincx; i += incx)
                dx[i + dx_off] *= da;
        }
        else
        {
            // code for increment equal to 1
            for (i = 0; i < n; i++)
                dx[i + dx_off] *= da;
        }
    }
}

//  constant times a vector plus a vector.
void daxpy(int n, double da, double * dx, int dx_off, int incx, double * dy, int dy_off, int incy)
{
    int i;

    if ((n > 0) && (da != 0))
    {
        if (incx != 1 || incy != 1)
        {
            // code for unequal increments or equal increments not equal to 1
            int ix = 0;
            int iy = 0;

            if (incx < 0)
                ix = (1 - n) * incx;

            if (incy < 0)
                iy = (1 - n) * incy;

            for (i = 0; i < n; i++)
            {
                dy[iy + dy_off] += da * dx[ix + dx_off];
                ix += incx;
                iy += incy;
	        }

	        return;
        }
        else
        {
            // code for both increments equal to 1
            for (i = 0; i < n; i++)
                dy[i + dy_off] += da * dx[i + dx_off];
        }
    }
}

// Factors a double precision matrix by gaussian elimination.
void dgefa(double ** a, int * ipvt)
{
    double temp;
    int k, j;

    for (k = 0; k < NM1; k++)
    {
        double * col_k = a[k];
        int kp1 = k + 1;

        // find l = pivot index
        int l = idamax(N - k, col_k, k, 1) + k;
	    ipvt[k] = l;

    	// zero pivot implies this column already triangularized
        if (col_k[l] != 0)
        {
	        // interchange if necessary
    	    if (l != k)
            {
    	        temp     = col_k[l];
	            col_k[l] = col_k[k];
	            col_k[k] = temp;
            }

            // compute multipliers
            temp = -1.0 / col_k[k];
            dscal(N - kp1, temp, col_k, kp1, 1);

    	    // row elimination with column indexing
            for (j = kp1; j < N; j++)
            {
	            double * col_j = a[j];
	            temp = col_j[l];

    	        if (l != k)
                {
                    col_j[l] = col_j[k];
                    col_j[k] = temp;
                }

                daxpy(N - kp1, temp, col_k, kp1, 1, col_j, kp1, 1);
            }
        }
    }

    ipvt[N - 1] = N - 1;
}

//  Solves the double precision system a * x = b  or trans(a) * x = b
//  using the factors computed by dgeco or dgefa.
void dgesl(double ** a, int * ipvt, double * b)
{
    double t;
    int k, kb;

    // solve  a * x = b.  first solve  l*y = b
    for (k = 0; k < NM1; k++)
    {
        int l = ipvt[k];
        t = b[l];

        if (l != k)
        {
            b[l] = b[k];
            b[k] = t;
        }

        int kp1 = k + 1;
        daxpy(N - kp1, t, a[k], kp1, 1, b, kp1, 1);
    }

    // now solve  u*x = y
    for (kb = 0; kb < N; kb++)
    {
        k     = N - (kb + 1);
        b[k] /= a[k][k];
        t     = -b[k];
        daxpy(k, t, a[k], 0, 1, b, 0, 1);
    }
}

int main(int argc, char ** argv)
{
    int i;
    
    // do we have verbose output?
    bool ga_testing = false;
    
    if (argc > 1)
    {
        for (i = 1; i < argc; ++i)
        {
            if (!strcmp(argv[1],"-ga"))
            {
                ga_testing = true;
                break;
            }
        }
    }
    
    double ** a = (double **)malloc(sizeof(double) * N);
    
    for (i = 0; i < N; ++i)
        a[i] = (double *)malloc(sizeof(double) * NP1);
    
    double * b = (double *)malloc(sizeof(double) * N);
    double * x = (double *)malloc(sizeof(double) * N);
    int * ipvt = (int *)malloc(sizeof(int)    * N);

    // calculate operations per timeInSeconds
    double ops = (2.0 * ((double)N * N * N)) / 3.0 + 2.0 * ((double)N * N);

    // generate matrix    
    matgen(a,b);
    
    // get starting time    
    //struct timespec start, stop;
    //clock_gettime(CLOCK_REALTIME,&start);

    // what we're timing
    dgefa(a,ipvt);
    dgesl(a,ipvt,b);
    
    // calculate run time
    //clock_gettime(CLOCK_REALTIME,&stop);        
    double run_time = 0;//(stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    // clean up
    free(ipvt);
    free(x);
    free(b);
    
    for (i = 0; i < N; ++i)
        free(a[i]);
    
    free(a);

    // report runtime
    if (ga_testing)
        fprintf(stdout,"%f",run_time);
    else        
        fprintf(stdout,"\nlpbench (Std. C) run time: %f\n\n",run_time);
    
    fflush(stdout);
    
    // done
    return 0;
}

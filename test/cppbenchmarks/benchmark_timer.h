/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
    
    Shared source file for timing, used by all the benchmarks
*/

/******************************************************************************/

#include <time.h>

/******************************************************************************/

/* Yes, this would be easier with a class or vector
 but it needs to work for both C and C++ code
*/

/*  simple timer functions */
clock_t start_time, end_time;

void start_timer() { start_time = clock(); }

double timer() {
  end_time = clock();
  return (end_time - start_time)/ (double)(CLOCKS_PER_SEC);
}

/******************************************************************************/

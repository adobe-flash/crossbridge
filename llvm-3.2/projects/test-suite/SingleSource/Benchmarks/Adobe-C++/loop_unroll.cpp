/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)


Goal:  Test compiler optimizations related to loop unrolling

Assumptions:

	1) the compiler will unroll loops to hide instruction latency
		for() {}
		while() {}
		do {} while()
		goto

	2) if the compiler unrolls the loop, it should not be slower than the original loop without unrolling

	3) the compiler should unroll a multi-calculation loop as well as a single calculation loop 
		up to the limit of performance gain for unrolling that loop
		in other words: no penalty for manually unrolling,
						as long as the manual unroll is less than or equal to the optimum unroll factor

	4) The compiler should recognize and unroll all loop styles with the same efficiency
		in other words: do, while, for, and goto should have identical performance

*/

#include "benchmark_stdint.hpp"
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "benchmark_results.h"
#include "benchmark_timer.h"

/******************************************************************************/

// this constant may need to be adjusted to give reasonable minimum times
// For best results, times should be about 1.0 seconds for the minimum test run
#ifdef SMALL_PROBLEM_SIZE
int iterations = 50;
#else
int iterations = 1000;
#endif

// 8000 items, or between 8k and 64k of data
// this is intended to remain within the L2 cache of most common CPUs
#define SIZE 	8000

// initial value for filling our arrays, may be changed from the command line
double init_value = 1.0;

/******************************************************************************/

#include "benchmark_shared_tests.h"

/******************************************************************************/
/******************************************************************************/

template <typename T>
T hash_func2(T seed) {
	return (914237 * (seed + 12345)) - 13;
}

template <typename T>
T complete_hash_func(T seed) {
	return hash_func2( hash_func2( hash_func2( seed ) ) );
}

/******************************************************************************/

template <typename T>
inline void check_sum(T result) {
  T temp = (T)SIZE * complete_hash_func( (T)init_value );
  if (!tolerance_equal<T>(result,temp)) printf("test %i failed\n", current_test);
}

/******************************************************************************/

// this is the heart of our loop unrolling - a class that unrolls itself to generate the inner loop code
// at least as long as we keep F < 50 (or some compilers won't compile it)
template< int F, typename T >
struct loop_inner_body {
	inline static void do_work(T &result, const T *first, int n) {
		loop_inner_body<F-1,T>::do_work(result, first, n);
		T temp = first[ n + (F-1) ];
		temp = complete_hash_func( temp );
		result += temp;
	}
};

template< typename T >
struct loop_inner_body<0,T> {
	inline static void do_work(T &, const T *, int) {
	}
};

/******************************************************************************/
/******************************************************************************/

// F is the unrolling factor
template <int F, typename T >
void test_for_loop_unroll_factor(const T* first, int count, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
	int n = 0;
	
    for (; n < (count - (F-1)); n += F) {
		loop_inner_body<F,T>::do_work(result,first, n);
	}
	
    for (; n < count; ++n) {
		result += complete_hash_func( first[n] );
	}
	
    check_sum<T>(result);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/

// F is the unrolling factor
template <int F, typename T >
void test_while_loop_unroll_factor(const T* first, int count, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
	int n = 0;
	
    while ( n < (count - (F-1)) ) {
		loop_inner_body<F,T>::do_work(result,first, n);
		n += F;
	}
	
    while ( n < count ) {
		result += complete_hash_func( first[n] );
		++n;
	}
	
    check_sum<T>(result);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/

// F is the unrolling factor
template <int F, typename T >
void test_do_loop_unroll_factor(const T* first, int count, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
	int n = 0;
	
	if ((count - n) >= F)
		do {
			loop_inner_body<F,T>::do_work(result,first, n);
			n += F;
		} while (n < (count - (F-1)));
	
	if (n < count)
		do {
			result += complete_hash_func( first[n] );
			++n;
		} while (n != count);
	
    check_sum<T>(result);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/

// F is the unrolling factor
template <int F, typename T >
void test_goto_loop_unroll_factor(const T* first, int count, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
	int n = 0;
	
	if ((count - n) >= F) {
loop2_start:
		loop_inner_body<F,T>::do_work(result,first, n);
		n += F;
		
		if (n < (count - (F-1)))
			goto loop2_start;
	}

	if (n < count) {
loop_start:
		result += complete_hash_func( first[n] );
		++n;
		
		if (n != count)
			goto loop_start;
	}
	
    check_sum<T>(result);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/
/******************************************************************************/

// our global arrays of numbers to be operated upon

double dataDouble[SIZE];

int32_t data32[SIZE];

// not elegant, but I need strings to hang around until we print the results
// and I don't want to pull in STL
const int UnrollLimit = 32;
char temp_string[UnrollLimit][100];

/******************************************************************************/
/******************************************************************************/

// another unrolled loop to create all of our tests
template< int N, typename T >
struct for_loop_tests {
	static void do_test( const T *data, const char *label_base ) {
		for_loop_tests<N-1, T>::do_test(data, label_base);
		test_for_loop_unroll_factor<N>( data, SIZE, temp_string[N-1] );
	}
};

template<typename T>
struct for_loop_tests<0,T> {
	static void do_test( const T *, const char * ) {
	}
};

/******************************************************************************/

template< int N, typename T >
struct while_loop_tests {
	static void do_test( const T *data, const char *label_base ) {
		while_loop_tests<N-1, T>::do_test(data, label_base);
		test_while_loop_unroll_factor<N>( data, SIZE, temp_string[N-1] );
	}
};

template<typename T>
struct while_loop_tests<0,T> {
	static void do_test( const T *, const char * ) {
	}
};

/******************************************************************************/

template< int N, typename T >
struct do_loop_tests {
	static void do_test( const T *data, const char *label_base ) {
		do_loop_tests<N-1, T>::do_test(data, label_base);
		test_do_loop_unroll_factor<N>( data, SIZE, temp_string[N-1] );
	}
};

template<typename T>
struct do_loop_tests<0,T> {
	static void do_test( const T *, const char * ) {
	}
};

/******************************************************************************/

template< int N, typename T >
struct goto_loop_tests {
	static void do_test( const T *data, const char *label_base ) {
		goto_loop_tests<N-1, T>::do_test(data, label_base);
		test_goto_loop_unroll_factor<N>( data, SIZE, temp_string[N-1] );
	}
};

template<typename T>
struct goto_loop_tests<0,T> {
	static void do_test( const T *, const char * ) {
	}
};

/******************************************************************************/
/******************************************************************************/

int main(int argc, char** argv) {
	if (argc > 1) iterations = atoi(argv[1]);
	if (argc > 2) init_value = (double) atof(argv[2]);


// int32_t
	::fill(data32, data32+SIZE, int32_t(init_value));
	
	for_loop_tests<UnrollLimit, int32_t>::do_test( data32, "int32_t for loop unroll" );
	
	while_loop_tests<UnrollLimit, int32_t>::do_test( data32, "int32_t while loop unroll" );

	do_loop_tests<UnrollLimit, int32_t>::do_test( data32, "int32_t do loop unroll" );

	goto_loop_tests<UnrollLimit, int32_t>::do_test( data32, "int32_t goto loop unroll" );	


// double
	iterations /= 4;

	::fill(dataDouble, dataDouble+SIZE, double(init_value));
	
	for_loop_tests<UnrollLimit, double>::do_test( dataDouble, "double for loop unroll" );
	
	while_loop_tests<UnrollLimit, double>::do_test( dataDouble, "double while loop unroll" );

	do_loop_tests<UnrollLimit, double>::do_test( dataDouble, "double do loop unroll" );

	goto_loop_tests<UnrollLimit, double>::do_test( dataDouble, "double goto loop unroll" );	


	return 0;
}

// the end
/******************************************************************************/
/******************************************************************************/

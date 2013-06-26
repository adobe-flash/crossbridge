/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html )


Goal:  Test compiler optimizations related to simple language defined types

Assumptions:

	1) the compiler will move loop invariant calculations on simple types out of a loop
		aka: loop invariant code motion
		
		for (i = 0; i < N; ++i)						temp = A + B + C + D;
			result = input[i] + A+B+C+D;	==>		for (i = 0; i < N; ++i)
														result = input[i] + temp;

*/

/******************************************************************************/

#include "benchmark_stdint.hpp"
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cmath>
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
const int SIZE = 8000;


// initial value for filling our arrays, may be changed from the command line
double init_value = 1.0;

/******************************************************************************/

// our global arrays of numbers to be operated upon

double dataDouble[SIZE];
float dataFloat[SIZE];

uint64_t data64unsigned[SIZE];
int64_t data64[SIZE];

uint32_t data32unsigned[SIZE];
int32_t data32[SIZE];

uint16_t data16unsigned[SIZE];
int16_t data16[SIZE];

uint8_t data8unsigned[SIZE];
int8_t data8[SIZE];

/******************************************************************************/

#include "benchmark_shared_tests.h"

/******************************************************************************/

// v1 is constant in the function, so we can move the addition or subtraction of it outside the loop entirely
// converting it to a multiply and a summation of the input array
// Note that this is always legal for integers
// it can only be applied to floating point if using inexact math (relaxed IEEE rules)
template <typename T, typename Shifter>
void test_hoisted_variable1(T* first, int count, T v1, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
    for (int n = 0; n < count; ++n) {
		result += first[n];
	}
	result += count * v1;
    check_shifted_variable_sum<T, Shifter>(result, v1);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/


int main(int argc, char** argv) {
	double temp = 1.0;

	if (argc > 1) iterations = atoi(argv[1]);
	if (argc > 2) init_value = (double) atof(argv[2]);
	if (argc > 3) temp = (double)atof(argv[3]);


// int8_t
	::fill(data8, data8+SIZE, int8_t(init_value));
	int8_t var1int8_1, var1int8_2, var1int8_3, var1int8_4;
	var1int8_1 = int8_t(temp);
	var1int8_2 = var1int8_1 * int8_t(2);
	var1int8_3 = var1int8_1 + int8_t(2);
	var1int8_4 = var1int8_1 + var1int8_2 / var1int8_3;
	
	// test moving redundant calcs out of loop
	test_variable1< int8_t, custom_add_variable<int8_t> > (data8, SIZE, var1int8_1, "int8_t variable add");
	test_hoisted_variable1< int8_t, custom_add_variable<int8_t> > (data8, SIZE, var1int8_1, "int8_t variable add hoisted");
	test_variable4< int8_t, custom_add_multiple_variable<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable adds");

	test_variable1< int8_t, custom_sub_variable<int8_t> > (data8, SIZE, var1int8_1, "int8_t variable subtract");
	test_variable4< int8_t, custom_sub_multiple_variable<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable subtracts");
	
	test_variable1< int8_t, custom_multiply_variable<int8_t> > (data8, SIZE, var1int8_1, "int8_t variable multiply");
	test_variable4< int8_t, custom_multiply_multiple_variable<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable multiplies");
	test_variable4< int8_t, custom_multiply_multiple_variable2<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable multiplies2");

	test_variable1< int8_t, custom_divide_variable<int8_t> > (data8, SIZE, var1int8_1, "int8_t variable divide");
	test_variable4< int8_t, custom_divide_multiple_variable<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable divides");
	test_variable4< int8_t, custom_divide_multiple_variable2<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable divides2");
	
	test_variable4< int8_t, custom_mixed_multiple_variable<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable mixed");

	test_variable1< int8_t, custom_variable_and<int8_t> > (data8, SIZE, var1int8_1, "int8_t variable and");
	test_variable4< int8_t, custom_multiple_variable_and<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable and");

	test_variable1< int8_t, custom_variable_or<int8_t> > (data8, SIZE, var1int8_1, "int8_t variable or");
	test_variable4< int8_t, custom_multiple_variable_or<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable or");

	test_variable1< int8_t, custom_variable_xor<int8_t> > (data8, SIZE, var1int8_1, "int8_t variable xor");
	test_variable4< int8_t, custom_multiple_variable_xor<int8_t> > (data8, SIZE, var1int8_1, var1int8_2, var1int8_3, var1int8_4, "int8_t multiple variable xor");

// unsigned8
	::fill(data8unsigned, data8unsigned+SIZE, uint8_t(init_value));
	uint8_t var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4;
	var1uint8_1 = uint8_t(temp);
	var1uint8_2 = var1uint8_1 * uint8_t(2);
	var1uint8_3 = var1uint8_1 + uint8_t(2);
	var1uint8_4 = var1uint8_1 + var1uint8_2 / var1uint8_3;
	
	// test moving redundant calcs out of loop
	test_variable1< uint8_t, custom_add_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, "uint8_t variable add");
	test_hoisted_variable1< uint8_t, custom_add_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, "uint8_t variable add hoisted");
	test_variable4< uint8_t, custom_add_multiple_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable adds");

	test_variable1< uint8_t, custom_sub_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, "uint8_t variable subtract");
	test_variable4< uint8_t, custom_sub_multiple_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable subtracts");
	
	test_variable1< uint8_t, custom_multiply_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, "uint8_t variable multiply");
	test_variable4< uint8_t, custom_multiply_multiple_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable multiplies");
	test_variable4< uint8_t, custom_multiply_multiple_variable2<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable multiplies2");

	test_variable1< uint8_t, custom_divide_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, "uint8_t variable divide");
	test_variable4< uint8_t, custom_divide_multiple_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable divides");
	test_variable4< uint8_t, custom_divide_multiple_variable2<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable divides2");
	
	test_variable4< uint8_t, custom_mixed_multiple_variable<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable mixed");

	test_variable1< uint8_t, custom_variable_and<uint8_t> > (data8unsigned, SIZE, var1uint8_1, "uint8_t variable and");
	test_variable4< uint8_t, custom_multiple_variable_and<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable and");

	test_variable1< uint8_t, custom_variable_or<uint8_t> > (data8unsigned, SIZE, var1uint8_1, "uint8_t variable or");
	test_variable4< uint8_t, custom_multiple_variable_or<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable or");

	test_variable1< uint8_t, custom_variable_xor<uint8_t> > (data8unsigned, SIZE, var1uint8_1, "uint8_t variable xor");
	test_variable4< uint8_t, custom_multiple_variable_xor<uint8_t> > (data8unsigned, SIZE, var1uint8_1, var1uint8_2, var1uint8_3, var1uint8_4, "uint8_t multiple variable xor");


// int16_t
	::fill(data16, data16+SIZE, int16_t(init_value));
	int16_t var1int16_1, var1int16_2, var1int16_3, var1int16_4;
	var1int16_1 = int16_t(temp);
	var1int16_2 = var1int16_1 * int16_t(2);
	var1int16_3 = var1int16_1 + int16_t(2);
	var1int16_4 = var1int16_1 + var1int16_2 / var1int16_3;

	// test moving redundant calcs out of loop
	test_variable1< int16_t, custom_add_variable<int16_t> > (data16, SIZE, var1int16_1, "int16_t variable add");
	test_hoisted_variable1< int16_t, custom_add_variable<int16_t> > (data16, SIZE, var1int16_1, "int16_t variable add hoisted");
	test_variable4< int16_t, custom_add_multiple_variable<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable adds");

	test_variable1< int16_t, custom_sub_variable<int16_t> > (data16, SIZE, var1int16_1, "int16_t variable subtract");
	test_variable4< int16_t, custom_sub_multiple_variable<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable subtracts");
	
	test_variable1< int16_t, custom_multiply_variable<int16_t> > (data16, SIZE, var1int16_1, "int16_t variable multiply");
	test_variable4< int16_t, custom_multiply_multiple_variable<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable multiplies");
	test_variable4< int16_t, custom_multiply_multiple_variable2<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable multiplies2");

	test_variable1< int16_t, custom_divide_variable<int16_t> > (data16, SIZE, var1int16_1, "int16_t variable divide");
	test_variable4< int16_t, custom_divide_multiple_variable<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable divides");
	test_variable4< int16_t, custom_divide_multiple_variable2<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable divides2");
	
	test_variable4< int16_t, custom_mixed_multiple_variable<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable mixed");

	test_variable1< int16_t, custom_variable_and<int16_t> > (data16, SIZE, var1int16_1, "int16_t variable and");
	test_variable4< int16_t, custom_multiple_variable_and<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable and");

	test_variable1< int16_t, custom_variable_or<int16_t> > (data16, SIZE, var1int16_1, "int16_t variable or");
	test_variable4< int16_t, custom_multiple_variable_or<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable or");

	test_variable1< int16_t, custom_variable_xor<int16_t> > (data16, SIZE, var1int16_1, "int16_t variable xor");
	test_variable4< int16_t, custom_multiple_variable_xor<int16_t> > (data16, SIZE, var1int16_1, var1int16_2, var1int16_3, var1int16_4, "int16_t multiple variable xor");


// unsigned16
	::fill(data16unsigned, data16unsigned+SIZE, uint16_t(init_value));
	uint16_t var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4;
	var1uint16_1 = uint16_t(temp);
	var1uint16_2 = var1uint16_1 * uint16_t(2);
	var1uint16_3 = var1uint16_1 + uint16_t(2);
	var1uint16_4 = var1uint16_1 + var1uint16_2 / var1uint16_3;

	// test moving redundant calcs out of loop
	test_variable1< uint16_t, custom_add_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, "uint16_t variable add");
	test_hoisted_variable1< uint16_t, custom_add_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, "uint16_t variable add hoisted");
	test_variable4< uint16_t, custom_add_multiple_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable adds");

	test_variable1< uint16_t, custom_sub_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, "uint16_t variable subtract");
	test_variable4< uint16_t, custom_sub_multiple_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable subtracts");
	
	test_variable1< uint16_t, custom_multiply_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, "uint16_t variable multiply");
	test_variable4< uint16_t, custom_multiply_multiple_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable multiplies");
	test_variable4< uint16_t, custom_multiply_multiple_variable2<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable multiplies2");

	test_variable1< uint16_t, custom_divide_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, "uint16_t variable divide");
	test_variable4< uint16_t, custom_divide_multiple_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable divides");
	test_variable4< uint16_t, custom_divide_multiple_variable2<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable divides2");
	
	test_variable4< uint16_t, custom_mixed_multiple_variable<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable mixed");

	test_variable1< uint16_t, custom_variable_and<uint16_t> > (data16unsigned, SIZE, var1uint16_1, "uint16_t variable and");
	test_variable4< uint16_t, custom_multiple_variable_and<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable and");

	test_variable1< uint16_t, custom_variable_or<uint16_t> > (data16unsigned, SIZE, var1uint16_1, "uint16_t variable or");
	test_variable4< uint16_t, custom_multiple_variable_or<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable or");

	test_variable1< uint16_t, custom_variable_xor<uint16_t> > (data16unsigned, SIZE, var1uint16_1, "uint16_t variable xor");
	test_variable4< uint16_t, custom_multiple_variable_xor<uint16_t> > (data16unsigned, SIZE, var1uint16_1, var1uint16_2, var1uint16_3, var1uint16_4, "uint16_t multiple variable xor");


// int32_t
	::fill(data32, data32+SIZE, int32_t(init_value));
	int32_t var1int32_1, var1int32_2, var1int32_3, var1int32_4;
	var1int32_1 = int32_t(temp);
	var1int32_2 = var1int32_1 * int32_t(2);
	var1int32_3 = var1int32_1 + int32_t(2);
	var1int32_4 = var1int32_1 + var1int32_2 / var1int32_3;

	// test moving redundant calcs out of loop
	test_variable1< int32_t, custom_add_variable<int32_t> > (data32, SIZE, var1int32_1, "int32_t variable add");
	test_hoisted_variable1< int32_t, custom_add_variable<int32_t> > (data32, SIZE, var1int32_1, "int32_t variable add hoisted");
	test_variable4< int32_t, custom_add_multiple_variable<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable adds");

	test_variable1< int32_t, custom_sub_variable<int32_t> > (data32, SIZE, var1int32_1, "int32_t variable subtract");
	test_variable4< int32_t, custom_sub_multiple_variable<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable subtracts");
	
	test_variable1< int32_t, custom_multiply_variable<int32_t> > (data32, SIZE, var1int32_1, "int32_t variable multiply");
	test_variable4< int32_t, custom_multiply_multiple_variable<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable multiplies");
	test_variable4< int32_t, custom_multiply_multiple_variable2<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable multiplies2");

	test_variable1< int32_t, custom_divide_variable<int32_t> > (data32, SIZE, var1int32_1, "int32_t variable divide");
	test_variable4< int32_t, custom_divide_multiple_variable<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable divides");
	test_variable4< int32_t, custom_divide_multiple_variable2<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable divides2");
	
	test_variable4< int32_t, custom_mixed_multiple_variable<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable mixed");

	test_variable1< int32_t, custom_variable_and<int32_t> > (data32, SIZE, var1int32_1, "int32_t variable and");
	test_variable4< int32_t, custom_multiple_variable_and<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable and");

	test_variable1< int32_t, custom_variable_or<int32_t> > (data32, SIZE, var1int32_1, "int32_t variable or");
	test_variable4< int32_t, custom_multiple_variable_or<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable or");

	test_variable1< int32_t, custom_variable_xor<int32_t> > (data32, SIZE, var1int32_1, "int32_t variable xor");
	test_variable4< int32_t, custom_multiple_variable_xor<int32_t> > (data32, SIZE, var1int32_1, var1int32_2, var1int32_3, var1int32_4, "int32_t multiple variable xor");


// unsigned32
	::fill(data32unsigned, data32unsigned+SIZE, uint32_t(init_value));
	uint32_t var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4;
	var1uint32_1 = uint32_t(temp);
	var1uint32_2 = var1uint32_1 * uint32_t(2);
	var1uint32_3 = var1uint32_1 + uint32_t(2);
	var1uint32_4 = var1uint32_1 + var1uint32_2 / var1uint32_3;
	
	// test moving redundant calcs out of loop
	test_variable1< uint32_t, custom_add_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, "uint32_t variable add");
	test_hoisted_variable1< uint32_t, custom_add_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, "uint32_t variable add hoisted");
	test_variable4< uint32_t, custom_add_multiple_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable adds");

	test_variable1< uint32_t, custom_sub_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, "uint32_t variable subtract");
	test_variable4< uint32_t, custom_sub_multiple_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable subtracts");
	
	test_variable1< uint32_t, custom_multiply_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, "uint32_t variable multiply");
	test_variable4< uint32_t, custom_multiply_multiple_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable multiplies");
	test_variable4< uint32_t, custom_multiply_multiple_variable2<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable multiplies2");

	test_variable1< uint32_t, custom_divide_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, "uint32_t variable divide");
	test_variable4< uint32_t, custom_divide_multiple_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable divides");
	test_variable4< uint32_t, custom_divide_multiple_variable2<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable divides2");
	
	test_variable4< uint32_t, custom_mixed_multiple_variable<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable mixed");

	test_variable1< uint32_t, custom_variable_and<uint32_t> > (data32unsigned, SIZE, var1uint32_1, "uint32_t variable and");
	test_variable4< uint32_t, custom_multiple_variable_and<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable and");

	test_variable1< uint32_t, custom_variable_or<uint32_t> > (data32unsigned, SIZE, var1uint32_1, "uint32_t variable or");
	test_variable4< uint32_t, custom_multiple_variable_or<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable or");

	test_variable1< uint32_t, custom_variable_xor<uint32_t> > (data32unsigned, SIZE, var1uint32_1, "uint32_t variable xor");
	test_variable4< uint32_t, custom_multiple_variable_xor<uint32_t> > (data32unsigned, SIZE, var1uint32_1, var1uint32_2, var1uint32_3, var1uint32_4, "uint32_t multiple variable xor");
	

// int64_t
	::fill(data64, data64+SIZE, int64_t(init_value));
	int64_t var1int64_1, var1int64_2, var1int64_3, var1int64_4;
	var1int64_1 = int64_t(temp);
	var1int64_2 = var1int64_1 * int64_t(2);
	var1int64_3 = var1int64_1 + int64_t(2);
	var1int64_4 = var1int64_1 + var1int64_2 / var1int64_3;

	// test moving redundant calcs out of loop
	test_variable1< int64_t, custom_add_variable<int64_t> > (data64, SIZE, var1int64_1, "int64_t variable add");
	test_hoisted_variable1< int64_t, custom_add_variable<int64_t> > (data64, SIZE, var1int64_1, "int64_t variable add hoisted");
	test_variable4< int64_t, custom_add_multiple_variable<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable adds");

	test_variable1< int64_t, custom_sub_variable<int64_t> > (data64, SIZE, var1int64_1, "int64_t variable subtract");
	test_variable4< int64_t, custom_sub_multiple_variable<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable subtracts");
	
	test_variable1< int64_t, custom_multiply_variable<int64_t> > (data64, SIZE, var1int64_1, "int64_t variable multiply");
	test_variable4< int64_t, custom_multiply_multiple_variable<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable multiplies");
	test_variable4< int64_t, custom_multiply_multiple_variable2<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable multiplies2");

	test_variable1< int64_t, custom_divide_variable<int64_t> > (data64, SIZE, var1int64_1, "int64_t variable divide");
	test_variable4< int64_t, custom_divide_multiple_variable<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable divides");
	test_variable4< int64_t, custom_divide_multiple_variable2<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable divides2");
	
	test_variable4< int64_t, custom_mixed_multiple_variable<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable mixed");

	test_variable1< int64_t, custom_variable_and<int64_t> > (data64, SIZE, var1int64_1, "int64_t variable and");
	test_variable4< int64_t, custom_multiple_variable_and<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable and");

	test_variable1< int64_t, custom_variable_or<int64_t> > (data64, SIZE, var1int64_1, "int64_t variable or");
	test_variable4< int64_t, custom_multiple_variable_or<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable or");

	test_variable1< int64_t, custom_variable_xor<int64_t> > (data64, SIZE, var1int64_1, "int64_t variable xor");
	test_variable4< int64_t, custom_multiple_variable_xor<int64_t> > (data64, SIZE, var1int64_1, var1int64_2, var1int64_3, var1int64_4, "int64_t multiple variable xor");


// unsigned64
	::fill(data64unsigned, data64unsigned+SIZE, uint64_t(init_value));
	uint64_t var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4;
	var1uint64_1 = uint64_t(temp);
	var1uint64_2 = var1uint64_1 * uint64_t(2);
	var1uint64_3 = var1uint64_1 + uint64_t(2);
	var1uint64_4 = var1uint64_1 + var1uint64_2 / var1uint64_3;

	// test moving redundant calcs out of loop
	test_variable1< uint64_t, custom_add_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, "uint64_t variable add");
	test_hoisted_variable1< uint64_t, custom_add_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, "uint64_t variable add hoisted");
	test_variable4< uint64_t, custom_add_multiple_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable adds");

	test_variable1< uint64_t, custom_sub_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, "uint64_t variable subtract");
	test_variable4< uint64_t, custom_sub_multiple_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable subtracts");
	
	test_variable1< uint64_t, custom_multiply_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, "uint64_t variable multiply");
	test_variable4< uint64_t, custom_multiply_multiple_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable multiplies");
	test_variable4< uint64_t, custom_multiply_multiple_variable2<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable multiplies2");

	test_variable1< uint64_t, custom_divide_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, "uint64_t variable divide");
	test_variable4< uint64_t, custom_divide_multiple_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable divides");
	test_variable4< uint64_t, custom_divide_multiple_variable2<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable divides2");
	
	test_variable4< uint64_t, custom_mixed_multiple_variable<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable mixed");

	test_variable1< uint64_t, custom_variable_and<uint64_t> > (data64unsigned, SIZE, var1uint64_1, "uint64_t variable and");
	test_variable4< uint64_t, custom_multiple_variable_and<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable and");

	test_variable1< uint64_t, custom_variable_or<uint64_t> > (data64unsigned, SIZE, var1uint64_1, "uint64_t variable or");
	test_variable4< uint64_t, custom_multiple_variable_or<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable or");

	test_variable1< uint64_t, custom_variable_xor<uint64_t> > (data64unsigned, SIZE, var1uint64_1, "uint64_t variable xor");
	test_variable4< uint64_t, custom_multiple_variable_xor<uint64_t> > (data64unsigned, SIZE, var1uint64_1, var1uint64_2, var1uint64_3, var1uint64_4, "uint64_t multiple variable xor");



// float
	::fill(dataFloat, dataFloat+SIZE, float(init_value));
	float var1Float_1, var1Float_2, var1Float_3, var1Float_4;
	var1Float_1 = float(temp);
	var1Float_2 = var1Float_1 * float(2.0);
	var1Float_3 = var1Float_1 + float(2.0);
	var1Float_4 = var1Float_1 + var1Float_2 / var1Float_3;

	// test moving redundant calcs out of loop
	test_variable1< float, custom_add_variable<float> > (dataFloat, SIZE, var1Float_1, "float variable add");
	test_hoisted_variable1< float, custom_add_variable<float> > (dataFloat, SIZE, var1Float_1, "float variable add hoisted");
	test_variable4< float, custom_add_multiple_variable<float> > (dataFloat, SIZE, var1Float_1, var1Float_2, var1Float_3, var1Float_4, "float multiple variable adds");

	test_variable1< float, custom_sub_variable<float> > (dataFloat, SIZE, var1Float_1, "float variable subtract");
	test_variable4< float, custom_sub_multiple_variable<float> > (dataFloat, SIZE, var1Float_1, var1Float_2, var1Float_3, var1Float_4, "float multiple variable subtracts");
	
	test_variable1< float, custom_multiply_variable<float> > (dataFloat, SIZE, var1Float_1, "float variable multiply");
	test_variable4< float, custom_multiply_multiple_variable<float> > (dataFloat, SIZE, var1Float_1, var1Float_2, var1Float_3, var1Float_4, "float multiple variable multiplies");
	test_variable4< float, custom_multiply_multiple_variable2<float> > (dataFloat, SIZE, var1Float_1, var1Float_2, var1Float_3, var1Float_4, "float multiple variable multiplies2");

	test_variable1< float, custom_divide_variable<float> > (dataFloat, SIZE, var1Float_1, "float variable divide");
	test_variable4< float, custom_divide_multiple_variable<float> > (dataFloat, SIZE, var1Float_1, var1Float_2, var1Float_3, var1Float_4, "float multiple variable divides");
	test_variable4< float, custom_divide_multiple_variable2<float> > (dataFloat, SIZE, var1Float_1, var1Float_2, var1Float_3, var1Float_4, "float multiple variable divides2");
	
	test_variable4< float, custom_mixed_multiple_variable<float> > (dataFloat, SIZE, var1Float_1, var1Float_2, var1Float_3, var1Float_4, "float multiple variable mixed");


// double
	::fill(dataDouble, dataDouble+SIZE, double(init_value));
	double var1Double_1, var1Double_2, var1Double_3, var1Double_4;
	var1Double_1 = double(temp);
	var1Double_2 = var1Double_1 * double(2.0);
	var1Double_3 = var1Double_1 + double(2.0);
	var1Double_4 = var1Double_1 + var1Double_2 / var1Double_3;

	// test moving redundant calcs out of loop
	test_variable1< double, custom_add_variable<double> > (dataDouble, SIZE, var1Double_1, "double variable add");
	test_hoisted_variable1< double, custom_add_variable<double> > (dataDouble, SIZE, var1Double_1, "double variable add hoisted");
	test_variable4< double, custom_add_multiple_variable<double> > (dataDouble, SIZE, var1Double_1, var1Double_2, var1Double_3, var1Double_4, "double multiple variable adds");
	
	test_variable1< double, custom_sub_variable<double> > (dataDouble, SIZE, var1Double_1, "double variable subtract");
	test_variable4< double, custom_sub_multiple_variable<double> > (dataDouble, SIZE, var1Double_1, var1Double_2, var1Double_3, var1Double_4, "double multiple variable subtracts");
	
	test_variable1< double, custom_multiply_variable<double> > (dataDouble, SIZE, var1Double_1, "double variable multiply");
	test_variable4< double, custom_multiply_multiple_variable<double> > (dataDouble, SIZE, var1Double_1, var1Double_2, var1Double_3, var1Double_4, "double multiple variable multiplies");
	test_variable4< double, custom_multiply_multiple_variable2<double> > (dataDouble, SIZE, var1Double_1, var1Double_2, var1Double_3, var1Double_4, "double multiple variable multiplies2");

	test_variable1< double, custom_divide_variable<double> > (dataDouble, SIZE, var1Double_1, "double variable divide");
	test_variable4< double, custom_divide_multiple_variable<double> > (dataDouble, SIZE, var1Double_1, var1Double_2, var1Double_3, var1Double_4, "double multiple variable divides");
	test_variable4< double, custom_divide_multiple_variable2<double> > (dataDouble, SIZE, var1Double_1, var1Double_2, var1Double_3, var1Double_4, "double multiple variable divides2");
	
	test_variable4< double, custom_mixed_multiple_variable<double> > (dataDouble, SIZE, var1Double_1, var1Double_2, var1Double_3, var1Double_4, "double multiple variable mixed");  

	
	return 0;
}

// the end
/******************************************************************************/
/******************************************************************************/

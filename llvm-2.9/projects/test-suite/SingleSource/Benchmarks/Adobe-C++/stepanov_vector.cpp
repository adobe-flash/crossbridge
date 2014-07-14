/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html )


Goal:  examine any change in performance when moving from pointers to vector iterators


Assumptions:
	1) Vector iterators should not perform worse than raw pointers.
	
		Programmers should never be tempted to write
			std::sort( &*vec.begin(), &*( vec.begin() + vec.size() ) )
		instead of
			std::sort( vec.begin(), vec.end() )

HIstory:
	This is an extension to Alex Stepanov's original abstraction penalty benchmark
	to test the compiler vendor implementation of vector iterators.

*/

#include <cstddef>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "benchmark_results.h"
#include "benchmark_timer.h"
#include "benchmark_algorithms.h"

/******************************************************************************/
/******************************************************************************/

// this constant may need to be adjusted to give reasonable minimum times
// For best results, times should be about 1.0 seconds for the minimum test run
#ifdef SMALL_PROBLEM_SIZE
int iterations = 600;
#else
int iterations = 60000;
#endif

// 2000 items, or about 16k of data
// this is intended to remain within the L2 cache of most common CPUs
const int SIZE = 2000;

// initial value for filling our arrays, may be changed from the command line
double init_value = 3.0;

/******************************************************************************/
/******************************************************************************/

inline void check_sum(double result) {
  if (result != SIZE * init_value) printf("test %i failed\n", current_test);
}

/******************************************************************************/

template <typename Iterator>
void verify_sorted(Iterator first, Iterator last) {
	if (!is_sorted(first,last))
		printf("sort test %i failed\n", current_test);
}

/******************************************************************************/

// a template using the accumulate template and iterators

template <typename Iterator, typename T>
void test_accumulate(Iterator first, Iterator last, T zero, const char *label) {
	int i;


	for(i = 0; i < iterations; ++i)
		check_sum( double( accumulate(first, last, zero) ) );

}

/******************************************************************************/

template <typename Iterator, typename T>
void test_insertion_sort(Iterator firstSource, Iterator lastSource, Iterator firstDest,
						Iterator lastDest, T zero, const char *label) {
	int i;


	for(i = 0; i < iterations; ++i) {
		::copy(firstSource, lastSource, firstDest);
		insertionSort< Iterator, T>( firstDest, lastDest );
		verify_sorted( firstDest, lastDest );
	}
	
}

/******************************************************************************/

template <typename Iterator, typename T>
void test_quicksort(Iterator firstSource, Iterator lastSource, Iterator firstDest,
					Iterator lastDest, T zero, const char *label) {
	int i;


	for(i = 0; i < iterations; ++i) {
		::copy(firstSource, lastSource, firstDest);
		quicksort< Iterator, T>( firstDest, lastDest );
		verify_sorted( firstDest, lastDest );
	}
	
}

/******************************************************************************/

template <typename Iterator, typename T>
void test_heap_sort(Iterator firstSource, Iterator lastSource, Iterator firstDest,
					Iterator lastDest, T zero, const char *label) {
	int i;


	for(i = 0; i < iterations; ++i) {
		::copy(firstSource, lastSource, firstDest);
		heapsort< Iterator, T>( firstDest, lastDest );
		verify_sorted( firstDest, lastDest );
	}
	
}

/******************************************************************************/
/******************************************************************************/

// our global arrays of numbers to be summed

double data[SIZE];
double dataMaster[SIZE];

/******************************************************************************/

// declaration of our iterator types and begin/end pairs
typedef double* dp;
dp dpb = data;
dp dpe = data + SIZE;
dp dMpb = dataMaster;
dp dMpe = dataMaster + SIZE;

typedef std::reverse_iterator<dp> rdp;
rdp rdpb(dpe);
rdp rdpe(dpb);
rdp rdMpb(dMpe);
rdp rdMpe(dMpb);

typedef std::reverse_iterator<rdp> rrdp;
rrdp rrdpb(rdpe);
rrdp rrdpe(rdpb);
rrdp rrdMpb(rdMpe);
rrdp rrdMpe(rdMpb);

typedef std::vector<double>::iterator vdp;

typedef std::vector<double>::reverse_iterator rvdp;
typedef std::reverse_iterator< vdp > rtvdp;

typedef std::reverse_iterator<rvdp> rtrvdp;
typedef std::reverse_iterator< rtvdp > rtrtvdp;


/******************************************************************************/
/******************************************************************************/


int main(int argc, char** argv) {

	double dZero = 0.0;

	if (argc > 1) iterations = atoi(argv[1]);
	if (argc > 2) init_value = (double) atof(argv[2]);
	
	// seed the random number generator so we get repeatable results
	srand( (int)init_value + 123 );
	

	::fill(dpb, dpe, double(init_value));
	
	std::vector<double>   vec_data;
	vec_data.resize(SIZE);

	::fill(vec_data.begin(), vec_data.end(), double(init_value));
	
	rtvdp rtvdpb(vec_data.end());
	rtvdp rtvdpe(vec_data.begin());
	
	rtrvdp rtrvdpb(vec_data.rend());
	rtrvdp rtrvdpe(vec_data.rbegin());
	
	rtrtvdp rtrtvdpb(rtvdpe);
	rtrtvdp rtrtvdpe(rtvdpb);

	test_accumulate(dpb, dpe, dZero, "double pointer verify2");
	test_accumulate(vec_data.begin(), vec_data.end(), dZero, "double vector iterator");
	test_accumulate(rdpb, rdpe, dZero, "double pointer reverse");
	test_accumulate(vec_data.rbegin(), vec_data.rend(), dZero, "double vector reverse_iterator");
	test_accumulate(rtvdpb, rtvdpe, dZero, "double vector iterator reverse");
	test_accumulate(rrdpb, rrdpe, dZero, "double pointer reverse reverse");
	test_accumulate(rtrvdpb, rtrvdpe, dZero, "double vector reverse_iterator reverse");
	test_accumulate(rtrtvdpb, rtrtvdpe, dZero, "double vector iterator reverse reverse");




	// the sorting tests are much slower than the accumulation tests - O(N^2)
	iterations = iterations / 1000;
	
	std::vector<double>   vec_dataMaster;
	vec_dataMaster.resize(SIZE);
	
	// fill one set of random numbers
	fill_random<double *, double>( dMpb, dMpe );
	
	// copy to the other sets, so we have the same numbers
	::copy( dMpb, dMpe, vec_dataMaster.begin() );
	
	rtvdp rtvdMpb(vec_dataMaster.end());
	rtvdp rtvdMpe(vec_dataMaster.begin());
	
	rtrvdp rtrvdMpb(vec_dataMaster.rend());
	rtrvdp rtrvdMpe(vec_dataMaster.rbegin());
	
	rtrtvdp rtrtvdMpb(rtvdMpe);
	rtrtvdp rtrtvdMpe(rtvdMpb);

	test_insertion_sort(dMpb, dMpe, dpb, dpe, dZero, "insertion_sort double pointer verify2");
	test_insertion_sort(vec_dataMaster.begin(), vec_dataMaster.end(), vec_data.begin(), vec_data.end(), dZero, "insertion_sort double vector iterator");
	test_insertion_sort(rdMpb, rdMpe, rdpb, rdpe, dZero, "insertion_sort double pointer reverse");
	test_insertion_sort(vec_dataMaster.rbegin(), vec_dataMaster.rend(), vec_data.rbegin(), vec_data.rend(), dZero, "insertion_sort double vector reverse_iterator");
	test_insertion_sort(rtvdMpb, rtvdMpe, rtvdpb, rtvdpe, dZero, "insertion_sort double vector iterator reverse");
	test_insertion_sort(rrdMpb, rrdMpe, rrdpb, rrdpe, dZero, "insertion_sort double pointer reverse reverse");
	test_insertion_sort(rtrvdMpb, rtrvdMpe, rtrvdpb, rtrvdpe, dZero, "insertion_sort double vector reverse_iterator reverse");
	test_insertion_sort(rtrtvdMpb, rtrtvdMpe, rtrtvdpb, rtrtvdpe, dZero, "insertion_sort double vector iterator reverse reverse");

	
	// these are slightly faster - O(NLog2(N))
	iterations = iterations * 8;
	
	test_quicksort(dMpb, dMpe, dpb, dpe, dZero, "quicksort double pointer verify2");
	test_quicksort(vec_dataMaster.begin(), vec_dataMaster.end(), vec_data.begin(), vec_data.end(), dZero, "quicksort double vector iterator");
	test_quicksort(rdMpb, rdMpe, rdpb, rdpe, dZero, "quicksort double pointer reverse");
	test_quicksort(vec_dataMaster.rbegin(), vec_dataMaster.rend(), vec_data.rbegin(), vec_data.rend(), dZero, "quicksort double vector reverse_iterator");
	test_quicksort(rtvdMpb, rtvdMpe, rtvdpb, rtvdpe, dZero, "quicksort double vector iterator reverse");
	test_quicksort(rrdMpb, rrdMpe, rrdpb, rrdpe, dZero, "quicksort double pointer reverse reverse");
	test_quicksort(rtrvdMpb, rtrvdMpe, rtrvdpb, rtrvdpe, dZero, "quicksort double vector reverse_iterator reverse");
	test_quicksort(rtrtvdMpb, rtrtvdMpe, rtrtvdpb, rtrtvdpe, dZero, "quicksort double vector iterator reverse reverse");


	
	test_heap_sort(dMpb, dMpe, dpb, dpe, dZero, "heap_sort double pointer verify2");
	test_heap_sort(vec_dataMaster.begin(), vec_dataMaster.end(), vec_data.begin(), vec_data.end(), dZero, "heap_sort double vector iterator");
	test_heap_sort(rdMpb, rdMpe, rdpb, rdpe, dZero, "heap_sort double pointer reverse");
	test_heap_sort(vec_dataMaster.rbegin(), vec_dataMaster.rend(), vec_data.rbegin(), vec_data.rend(), dZero, "heap_sort double vector reverse_iterator");
	test_heap_sort(rtvdMpb, rtvdMpe, rtvdpb, rtvdpe, dZero, "heap_sort double vector iterator reverse");
	test_heap_sort(rrdMpb, rrdMpe, rrdpb, rrdpe, dZero, "heap_sort double pointer reverse reverse");
	test_heap_sort(rtrvdMpb, rtrvdMpe, rtrvdpb, rtrvdpe, dZero, "heap_sort double vector reverse_iterator reverse");
	test_heap_sort(rtrtvdMpb, rtrtvdMpe, rtrtvdpb, rtrtvdpe, dZero, "heap_sort double vector iterator reverse reverse");




	return 0;
}

// the end
/******************************************************************************/
/******************************************************************************/

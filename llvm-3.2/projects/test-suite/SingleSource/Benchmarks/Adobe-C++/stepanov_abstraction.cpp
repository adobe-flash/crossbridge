/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html )


Goal:  examine any change in performance when adding abstraction to simple data types
	in other words:  what happens when adding {} around a type.


Assumptions:
	
	1) A value wrapped in a struct or class should not perform worse than a raw value
	
	2) A value recursively wrapped in a struct or class should not perform worse than the raw value


History:
	Alex Stepanov created the abstraction penalty benchmark. 
	Recently, Alex suggested that I take ownership of his benchmark and extend it.
	
	The original accumulation tests used to show large penalties for using abstraction,
	but compilers have improved.  I have added three sorting tests with non-trivial
	value and pointer usage that show some compilers still have more
	opportunities for optimization.
	
	Chris Cox
	February 2008

*/

#include <cstddef>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include "benchmark_results.h"
#include "benchmark_timer.h"
#include "benchmark_algorithms.h"

/******************************************************************************/

// a value wrapped in a struct, recursively

template <typename T>
struct ValueWrapper {
	T value;
	ValueWrapper() {}
	template<typename TT>
		inline operator TT () const { return (TT)value; }
	template<typename TT>
		ValueWrapper(const TT& x) : value(x) {}
	T& operator*() const { return *value; }
};

template <typename T>
inline ValueWrapper<T> operator+(const ValueWrapper<T>& x, const ValueWrapper<T>& y) {
	return ValueWrapper<T>(x.value + y.value);
}

template <typename T>
inline bool operator<(const ValueWrapper<T>& x, const ValueWrapper<T>& y) {
	return (x.value < y.value);
}

/******************************************************************************/

typedef ValueWrapper<double>	DoubleValueWrapper;
typedef ValueWrapper< ValueWrapper< ValueWrapper< ValueWrapper< ValueWrapper< ValueWrapper< ValueWrapper< ValueWrapper< ValueWrapper< ValueWrapper<double> > > > > > > > > >	DoubleValueWrapper10;

/******************************************************************************/

// a pointer wrapped in a struct, aka an iterator

template<typename T>
struct PointerWrapper {
	T* current;
	PointerWrapper() {}
	PointerWrapper(T* x) : current(x) {}
	T& operator*() const { return *current; }
};

// really a distance between pointers, which must return ptrdiff_t
// because (ptr - ptr) --> ptrdiff_t
template <typename T>
inline ptrdiff_t operator-(PointerWrapper<T>& xx, PointerWrapper<T>& yy) {
	return (ptrdiff_t)( xx.current - yy.current );
}

template <typename T>
inline PointerWrapper<T>& operator++(PointerWrapper<T> &xx) {
	++xx.current;
	return xx;
}

template <typename T>
inline PointerWrapper<T>& operator--(PointerWrapper<T> &xx) {
	--xx.current;
	return xx;
}

template <typename T>
inline PointerWrapper<T> operator++(PointerWrapper<T> &xx, int) {
	PointerWrapper<T> tmp = xx;
	++xx;
	return tmp;
}

template <typename T>
inline PointerWrapper<T> operator--(PointerWrapper<T> &xx, int) {
	PointerWrapper<T> tmp = xx;
	--xx;
	return tmp;
}

template <typename T>
inline PointerWrapper<T> operator-(PointerWrapper<T> &xx, ptrdiff_t inc) {
	PointerWrapper<T> tmp = xx;
	tmp.current -= inc;
	return tmp;
}

template <typename T>
inline PointerWrapper<T> operator+(PointerWrapper<T> &xx, ptrdiff_t inc) {
	PointerWrapper<T> tmp = xx;
	tmp.current += inc;
	return tmp;
}

template <typename T>
inline PointerWrapper<T>& operator+=(PointerWrapper<T> &xx, ptrdiff_t inc) {
	xx.current += inc;
	return xx;
}

template <typename T>
inline PointerWrapper<T>& operator-=(PointerWrapper<T> &xx, ptrdiff_t inc) {
	xx.current -= inc;
	return xx;
}

template <typename T>
inline bool operator<(const PointerWrapper<T>& x, const PointerWrapper<T>& y) {
	return (x.current < y.current);
}

template <typename T>
inline bool operator==(const PointerWrapper<T>& x, const PointerWrapper<T>& y) {
	return (x.current == y.current);
}

template <typename T>
inline bool operator!=(const PointerWrapper<T>& x, const PointerWrapper<T>& y) {
	return (x.current != y.current);
}

/******************************************************************************/

typedef PointerWrapper<double> double_pointer;
typedef PointerWrapper<DoubleValueWrapper> doubleValueWrapper_pointer;
typedef PointerWrapper<DoubleValueWrapper10> doubleValueWrapper10_pointer;

/******************************************************************************/
/******************************************************************************/

// this constant may need to be adjusted to give reasonable minimum times
// For best results, times should be about 1.0 seconds for the minimum test run
#ifdef SMALL_PROBLEM_SIZE
int iterations = 100;
#else
int iterations = 200000;
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
/******************************************************************************/

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
DoubleValueWrapper VData[SIZE];
DoubleValueWrapper10 V10Data[SIZE];

double dataMaster[SIZE];
DoubleValueWrapper VDataMaster[SIZE];
DoubleValueWrapper10 V10DataMaster[SIZE];

/******************************************************************************/

// declaration of our iterator types and begin/end pairs
typedef double* dp;
dp dpb = data;
dp dpe = data + SIZE;
dp dMpb = dataMaster;
dp dMpe = dataMaster + SIZE;

typedef DoubleValueWrapper* DVp;
DVp DVpb = VData;
DVp DVpe = VData + SIZE;
DVp DVMpb = VDataMaster;
DVp DVMpe = VDataMaster + SIZE;

typedef DoubleValueWrapper10* DV10p;
DV10p DV10pb = V10Data;
DV10p DV10pe = V10Data + SIZE;
DV10p DV10Mpb = V10DataMaster;
DV10p DV10Mpe = V10DataMaster + SIZE;

typedef double_pointer dP;
dP dPb(dpb);
dP dPe(dpe);
dP dMPb(dMpb);
dP dMPe(dMpe);

typedef doubleValueWrapper_pointer DVP;
DVP DVPb(DVpb);
DVP DVPe(DVpe);
DVP DVMPb(DVMpb);
DVP DVMPe(DVMpe);

typedef doubleValueWrapper10_pointer DV10P;
DV10P DV10Pb(DV10pb);
DV10P DV10Pe(DV10pe);
DV10P DV10MPb(DV10Mpb);
DV10P DV10MPe(DV10Mpe);

/******************************************************************************/
/******************************************************************************/

int main(int argc, char** argv) {

	double dZero = 0.0;
	DoubleValueWrapper DVZero = 0.0;
	DoubleValueWrapper10 DV10Zero = DoubleValueWrapper10(0.0);

	if (argc > 1) iterations = atoi(argv[1]);
	if (argc > 2) init_value = (double) atof(argv[2]);

	// seed the random number generator so we get repeatable results
	srand( (int)init_value + 123 );


	fill(dpb, dpe, double(init_value));
	fill(DVpb, DVpe, DoubleValueWrapper(init_value));
	fill(DV10pb, DV10pe, DoubleValueWrapper10(init_value));

	test_accumulate(dpb, dpe, dZero, "double pointer");
	test_accumulate(dPb, dPe, dZero, "double pointer_class");
	test_accumulate(DVpb, DVpe, DVZero, "DoubleValueWrapper pointer");
	test_accumulate(DVPb, DVPe, DVZero, "DoubleValueWrapper pointer_class");
	test_accumulate(DV10pb, DV10pe, DV10Zero, "DoubleValueWrapper10 pointer");
	test_accumulate(DV10Pb, DV10Pe, DV10Zero, "DoubleValueWrapper10 pointer_class");


	// the sorting tests are much slower than the accumulation tests - O(N^2)
	iterations = iterations / 2000;
	
	// fill one set of random numbers
	fill_random<double *, double>( dMpb, dMpe );
	// copy to the other sets, so we have the same numbers
	::copy( dMpb, dMpe, DVMpb );
	::copy( dMpb, dMpe, DV10Mpb );

	test_insertion_sort(dMpb, dMpe, dpb, dpe, dZero, "insertion_sort double pointer");
	test_insertion_sort(dMPb, dMPe, dPb, dPe, dZero, "insertion_sort double pointer_class");
	test_insertion_sort(DVMpb, DVMpe, DVpb, DVpe, DVZero, "insertion_sort DoubleValueWrapper pointer");
	test_insertion_sort(DVMPb, DVMPe, DVPb, DVPe, DVZero, "insertion_sort DoubleValueWrapper pointer_class");
	test_insertion_sort(DV10Mpb, DV10Mpe, DV10pb, DV10pe, DV10Zero, "insertion_sort DoubleValueWrapper10 pointer");
	test_insertion_sort(DV10MPb, DV10MPe, DV10Pb, DV10Pe, DV10Zero, "insertion_sort DoubleValueWrapper10 pointer_class");


	// these are slightly faster - O(NLog2(N))
	iterations = iterations * 8;

	test_quicksort(dMpb, dMpe, dpb, dpe, dZero, "quicksort double pointer");
	test_quicksort(dMPb, dMPe, dPb, dPe, dZero, "quicksort double pointer_class");
	test_quicksort(DVMpb, DVMpe, DVpb, DVpe, DVZero, "quicksort DoubleValueWrapper pointer");
	test_quicksort(DVMPb, DVMPe, DVPb, DVPe, DVZero, "quicksort DoubleValueWrapper pointer_class");
	test_quicksort(DV10Mpb, DV10Mpe, DV10pb, DV10pe, DV10Zero, "quicksort DoubleValueWrapper10 pointer");
	test_quicksort(DV10MPb, DV10MPe, DV10Pb, DV10Pe, DV10Zero, "quicksort DoubleValueWrapper10 pointer_class");


	test_heap_sort(dMpb, dMpe, dpb, dpe, dZero, "heap_sort double pointer");
	test_heap_sort(dMPb, dMPe, dPb, dPe, dZero, "heap_sort double pointer_class");
	test_heap_sort(DVMpb, DVMpe, DVpb, DVpe, DVZero, "heap_sort DoubleValueWrapper pointer");
	test_heap_sort(DVMPb, DVMPe, DVPb, DVPe, DVZero, "heap_sort DoubleValueWrapper pointer_class");
	test_heap_sort(DV10Mpb, DV10Mpe, DV10pb, DV10pe, DV10Zero, "heap_sort DoubleValueWrapper10 pointer");
	test_heap_sort(DV10MPb, DV10MPe, DV10Pb, DV10Pe, DV10Zero, "heap_sort DoubleValueWrapper10 pointer_class");


	return 0;
}

// the end
/******************************************************************************/
/******************************************************************************/

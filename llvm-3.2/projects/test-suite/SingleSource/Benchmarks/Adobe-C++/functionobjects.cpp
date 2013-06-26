/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html )
	

	This test file started life as 
		ISO/IEC TR 18015:2006(E) Appendix D.4


Goals:
	
	Compare the performance of function pointers, functors, inline functors,
		standard functors, and native comparison operators

	Also compare the performance of qsort(), quicksort template, and std::sort


Assumptions:
	
	1) inline functors, standard functors and inlined native
		comparisons will perform similarly
	
	2) using functors is faster than using function pointers
	
	3) inline functors are as fast or faster than out of line functors
	
	4) a template is at least as fast as a hard coded function of
		the same algorithm, sometimes faster
	
	5) std::sort is faster than qsort()
	
	6) std::sort is faster than a naive quicksort template using the same functor


Since qsort's comparison function must return int (less than 0, 0, greater than 0)
	and std::sort's must return a bool, it is not possible to test them with each
	other's comparator.

*/


/******************************************************************************/

#include <functional>
#include <algorithm>
#include <cstdlib>
#include "benchmark_results.h"
#include "benchmark_timer.h"

using namespace std;

/******************************************************************************/

template <class Iterator>
void verify_sorted(Iterator first, Iterator last) {
	Iterator prev = first;
	first++;
	while (first != last) {
		if (*first++ < *prev++) {
			printf("test %i failed\n", current_test);
			break;
		}
	}
}

/******************************************************************************/
// --------- helper functions --------------------------------------------

// qsort passes void * arguments to its comparison function,
// which must return negative, 0, or positive value
int
less_than_function1( const void * lhs, const void * rhs )
	{
	if( *(const double *) lhs < *(const double *) rhs ) return -1;
	if( *(const double *) lhs > *(const double *) rhs ) return 1;
	return 0;
	}

// std::sort, on the other hand, needs a comparator that returns true or false
bool
less_than_function2( const double lhs, const double rhs )
	{
	return( lhs < rhs? true : false );
	}

// the comparison operator in the following functor is defined out of line
struct less_than_functor
{
	bool operator()( const double& lhs, const double& rhs ) const;
};

bool
less_than_functor::operator()( const double& lhs, const double& rhs ) const
	{
	return( lhs < rhs? true : false );
	}

// the comparison operator in the following functor is defined inline
struct inline_less_than_functor
{
	inline bool operator()( const double& lhs, const double& rhs ) const
		{
		return( lhs < rhs? true : false );
		}
};

/******************************************************************************/

// hard coded comparison function
template<class Iterator>
void quicksort(Iterator begin, Iterator end)
{
	// this only works for pointers and STL iterators
	typedef typename iterator_traits<Iterator>::value_type T;
	
	if ( (end - begin) > 1 ) {

		T middleValue = *begin;
		Iterator left = begin;
		Iterator right = end;

		for(;;) {

			while ( middleValue < *(--right) );
			if ( !(left < right ) ) break;
			
			while ( *(left) < middleValue )
				++left;
			if ( !(left < right ) ) break;

			// swap
			T temp = *right;
			*right = *left;
			*left = temp;
		}
		
		quicksort( begin, right + 1 );
		quicksort( right + 1, end );
	}
}

/******************************************************************************/

// comparison function passed in as a functor
template<class Iterator, typename Comparator>
void quicksort(Iterator begin, Iterator end, Comparator compare)
{
	// this only works for pointers and STL iterators
	typedef typename iterator_traits<Iterator>::value_type T;
	
	if ( (end - begin) > 1 ) {

		T middleValue = *begin;
		Iterator left = begin;
		Iterator right = end;

		for(;;) {

			while ( compare( middleValue, *(--right) ) );
			if ( !(left < right ) ) break;
			while ( compare( *(left), middleValue ) )
				++left;
			if ( !(left < right ) ) break;

			// swap
			T temp = *right;
			*right = *left;
			*left = temp;
		}
		
		quicksort( begin, right + 1, compare );
		quicksort( right + 1, end, compare );
	}
}

/******************************************************************************/

typedef bool comparator_function( const double x, const double y );

// use a pointer to function as a template parameter
// exact function is known at compile time, and can be inlined
template<class Iterator, comparator_function compare>
void quicksort(Iterator begin, Iterator end)
{
	// this only works for pointers and STL iterators
	typedef typename iterator_traits<Iterator>::value_type T;
	
	if ( (end - begin) > 1 ) {

		T middleValue = *begin;
		Iterator left = begin;
		Iterator right = end;

		for(;;) {

			while ( compare( middleValue, *(--right) ) );
			if ( !(left < right ) ) break;
			while ( compare( *(left), middleValue ) )
				++left;
			if ( !(left < right ) ) break;

			// swap
			T temp = *right;
			*right = *left;
			*left = temp;
		}
		
		quicksort( begin, right + 1, compare );
		quicksort( right + 1, end, compare );
	}
}

/******************************************************************************/

// use a function pointer
// most compilers will not inline the function argument
void quicksort_function(double* begin, double* end, comparator_function compare)
{
	if ( (end - begin) > 1 ) {

		double middleValue = *begin;
		double* left = begin;
		double* right = end;

		for(;;) {

			while ( compare( middleValue, *(--right) ) );
			if ( !(left < right ) ) break;
			while ( compare( *(left), middleValue ) )
				++left;
			if ( !(left < right ) ) break;

			// swap
			double temp = *right;
			*right = *left;
			*left = temp;
		}
		
		quicksort( begin, right + 1, compare );
		quicksort( right + 1, end, compare );
	}
}

/******************************************************************************/

int main(int argc, char* argv[])
{
	int i;
#ifdef SMALL_PROBLEM_SIZE
	int iterations = (1 < argc) ? atoi(argv[1]) : 30; // number of iterations
#else
	int iterations = (1 < argc) ? atoi(argv[1]) : 300; // number of iterations
#endif
	int tablesize = (2 < argc) ? atoi(argv[2]) : 10000; // size of array
	
	
	// seed the random number generator, so we get repeatable results
	srand( tablesize + 123 );
	
	
	// initialize the table to sort
	double * master_table = new double[tablesize];
	for( int n = 0; n < tablesize; ++n )
		{
		master_table[n] = static_cast<double>( rand() );
		}
	
	double * table = new double[tablesize]; // working copy
	
	
	
	// here is where the timing starts
	
	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		qsort( table, tablesize, sizeof(double), less_than_function1 );
		verify_sorted( table, table + tablesize );
		}
	
	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		quicksort_function( table, table + tablesize, less_than_function2 );
		verify_sorted( table, table + tablesize );
		}
	
	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		quicksort( table, table + tablesize, less_than_function2 );
		verify_sorted( table, table + tablesize );
		}
	
	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		quicksort<double *, less_than_function2 >( table, table + tablesize  );
		verify_sorted( table, table + tablesize );
		}

	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		sort( table, table + tablesize, less_than_function2 );
		verify_sorted( table, table + tablesize );
		}

	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		quicksort( table, table + tablesize, less_than_functor() );
		verify_sorted( table, table + tablesize );
		}
	
	for (i = 0; i < iterations; ++i)
		{
		sort( table, table + tablesize, less_than_functor() );
		verify_sorted( table, table + tablesize );
		copy(master_table, master_table+tablesize, table);
		}

	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		quicksort( table, table + tablesize, inline_less_than_functor() );
		verify_sorted( table, table + tablesize );
		}
	
	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		sort( table, table + tablesize, inline_less_than_functor() );
		verify_sorted( table, table + tablesize );
		}
	
	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		quicksort( table, table + tablesize, less<double>() );
		verify_sorted( table, table + tablesize );
		}
	
	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		sort( table, table + tablesize, less<double>() );
		verify_sorted( table, table + tablesize );
		}
	
	for (i = 0; i < iterations; ++i)
		{
		copy(master_table, master_table+tablesize, table);
		quicksort( table, table + tablesize );
		verify_sorted( table, table + tablesize );
		}

	for (i = 0; i < iterations; ++i)
		{
		copy( master_table, master_table+tablesize, table );
		sort( table, table + tablesize );
		verify_sorted( table, table + tablesize );
		}
	
	
	delete[] table;
	delete[] master_table;
	
	return 0;
}

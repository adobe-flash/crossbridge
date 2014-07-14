/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
    
    Shared source file for algorithms used in multiple benchmark files
*/

namespace benchmark {

/******************************************************************************/

template <typename Iterator>
bool is_sorted(Iterator first, Iterator last) {
	Iterator prev = first;
	first++;
	while (first != last) {
		if ( *first++ < *prev++)
			return false;
	}
	return true;
}

/******************************************************************************/

template <typename Iterator, typename T>
void fill(Iterator first, Iterator last, T value) {
	while (first != last) *first++ = value;
}

/******************************************************************************/

template <typename Iterator, typename T>
void fill_random(Iterator first, Iterator last) {
	while (first != last) {
		*first++ = static_cast<T>( rand() );
	}
}

/******************************************************************************/

template <class Iterator, class T>
void fill_descending(Iterator first, Iterator last, unsigned count) {
	while (first != last) {
		*first++ = static_cast<T>( --count );
	}
}

/******************************************************************************/

template <typename Iterator1, typename Iterator2>
void copy(Iterator1 firstSource, Iterator1 lastSource, Iterator2 firstDest) {
	while (firstSource != lastSource) *(firstDest++) = *(firstSource++);
}

/******************************************************************************/

template <class Iterator, class Swapper>
void reverse(Iterator begin, Iterator end, Swapper doswap)
{
	while (begin != end)
	{
		--end;
		if (begin == end)
			break;
		doswap(begin, end);
		++begin;
	}
}

/******************************************************************************/

// our accumulator function template, using iterators or pointers
template <typename Iterator, typename Number>
Number accumulate(Iterator first, Iterator last, Number result) {
	while (first != last) result =  result + *first++;
	return result;
}

/******************************************************************************/

template <typename Iterator, typename T>
void insertionSort( Iterator begin, Iterator end )
{
	Iterator p = begin;
	p++;

	while ( p != end ) {
		T tmp = *p;
		Iterator j = p;
		Iterator prev = j;

		for (  ; j != begin && tmp < *--prev; --j ) {
			*j = *prev;
		}

		*j = tmp;
		p++;
	}
}

/******************************************************************************/

template<typename Iterator, typename T>
void quicksort(Iterator begin, Iterator end)
{
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

		quicksort<Iterator,T>( begin, right + 1 );
		quicksort<Iterator,T>( right + 1, end );
	}
}

/******************************************************************************/

template<typename Iterator, typename T, class Swapper>
void quicksort(Iterator begin, Iterator end, Swapper doswap)
{
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
			doswap( right, left );
		}

		quicksort<Iterator,T, Swapper>( begin, right + 1, doswap );
		quicksort<Iterator,T, Swapper>( right + 1, end, doswap );
	}
}

/******************************************************************************/

template<typename Iterator, typename T>
void sift_in(ptrdiff_t count, Iterator begin, ptrdiff_t free_in, T next)
{
	ptrdiff_t i;
	ptrdiff_t free = free_in;

	// sift up the free node 
	for ( i = 2*(free+1); i < count; i += i) {
		if ( *(begin+(i-1)) < *(begin+i))
			i++;
		*(begin + free) = *(begin+(i-1));
		free = i-1;
	}

	// special case in sift up if the last inner node has only 1 child
	if (i == count) {
		*(begin + free) = *(begin+(i-1));
		free = i-1;
	}

	// sift down the new item next
	i = (free-1)/2;
	while( (free > free_in)  &&  *(begin+i) < next) {
		*(begin + free) = *(begin+i);
		free = i;
		i = (free-1)/2;
	}

	*(begin + free) = next;
}

template<typename Iterator, typename T>
void heapsort(Iterator begin, Iterator end)
{
	ptrdiff_t  j;
	ptrdiff_t count = end - begin;

	// build the heap structure 
	for( j = (count / 2) - 1; j >= 0; --j) {
		T  next = *(begin+j);
		sift_in< Iterator, T>(count, begin, j, next);
	}

	// search next by next remaining extremal element
	for( j = count - 1; j >= 1; --j) {
		T next = *(begin+j);
		*(begin+j) = *(begin);
		sift_in< Iterator, T>(j, begin, 0, next);
	}
}

}	// end namespace benchmark

using namespace benchmark;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

// -*- mode: c++ -*-
// $Id: lists1.cpp 53840 2008-07-21 06:03:44Z nicholas $
// http://www.bagley.org/~doug/shootout/

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <numeric>

template <class _ForwardIterator, class _Tp>
void 
iota(_ForwardIterator __first, _ForwardIterator __last, _Tp __value)
{
  while (__first != __last)
    *__first++ = __value++;
}

using namespace std;

void list_print_n (list<int> L, int n) {
    int c, lastc = n - 1;
    list<int>::iterator i;
    for (c = 0, i = L.begin(); i != L.end() && c < n; ++i, ++c) {
	cout << (*i);
	if (c < lastc) cout << " ";
    }
    cout << endl;
}

int main(int argc, char* argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 100000
#else
#define LENGTH 1000000
#endif
    int N = (argc == 2 ? (atoi(argv[1]) < 1 ? 1 : atoi(argv[1])): LENGTH);
    list<int>::iterator i;

    // create empty list B
    list<int> B;

    // create list (A) of integers from 1 through N
    list<int> A(N);
    iota(A.begin(), A.end(), 1);

    // move each individual item from A to B, in a loop, reversing order
    while (! A.empty()) {
        B.push_front(A.front());
        A.pop_front();
    }
    
    // print first 2 elements of B
    list_print_n(B, 2);

    // reverse B (can be done in place)
    B.reverse();
    // reverse(B.begin(), B.end());

    // is 0 a member of B?
    cout << ((find(B.begin(), B.end(), 0) == B.end()) ? "false" : "true") << endl;

    // is N a member of B?
    cout << ((find(B.begin(), B.end(), N) == B.end()) ? "false" : "true") << endl;

    // filter values from B to A that are less than N/2, preserving order
    int mid = N/2;
    for (i = B.begin(); i != B.end(); ++i) {
	if ((*i) < mid) A.push_back(*i);
    }

    // print first ten items of A
    list_print_n(A, 10);

    // print sum of items in A that are less than 1000
    int sum = 0;
    for (i = A.begin(); i != A.end(); ++i) {
	if ((*i) < 1000) sum += (*i);
    }
    cout << sum << endl;

    // append B to end of A
    A.splice(A.end(), B);

    // print length and last element of A
    cout << A.size() << " " << A.back() << endl;
}

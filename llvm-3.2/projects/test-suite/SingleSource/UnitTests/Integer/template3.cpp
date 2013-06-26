//===--- template3.cpp --- Test Cases for Bit Accurate Types --------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a test for partial specialization.
//
//===----------------------------------------------------------------------===//


#include <stdio.h>

typedef int __attribute__ ((bitwidth(17))) int17;
typedef int __attribute__ ((bitwidth(15))) int15;

template<class T, class U, int I> struct X
{ void f() { printf("Primary template\n"); } };

template<class T, int I> struct X<T, T*, I>
{ void f() { printf("Partial specialization 1\n"); } };

template<class T, class U, int I> struct X<T*, U, I>
{ void f() { printf("Partial specialization 2\n"); } };

template<class T> struct X<int, T*, 10>
{ void f() { printf("Partial specialization 3\n"); } };

template<class T, class U, int I> struct X<T, U*, I>
{ void f() { printf("Partial specialization 4\n"); } };

int main() {
  X<int17, int17, 10> a;
  X<int15, int15*, 5> b;
  X<int17, int15*, 5> bb;

  X<int17*, float, 10> c;
  X<int15, char*, 10> d;
  X<float, int15*, 10> e;

  a.f();
  b.f();
  bb.f();
  c.f();
  d.f();
  e.f();
  return 0;
}

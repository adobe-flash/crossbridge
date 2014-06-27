//===--- override.cpp --- Test Cases for Bit Accurate Types ---------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a test of override test.
//
//===----------------------------------------------------------------------===//


#include <stdio.h>

typedef int __attribute__ ((bitwidth(31))) int31;
typedef int __attribute__ ((bitwidth(32))) int32;

void func(int32 i)
{
  printf("call func with int32: %d\n", (int)i);
}

void func(int31 s)
{
  printf("call func with int31: %d\n", (int)s);
}

int main()
{
  func( (int31) 1);
  func( (int32) 2);
  return 0;
}

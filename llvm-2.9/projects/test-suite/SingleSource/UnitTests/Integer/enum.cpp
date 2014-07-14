//===--- enum.cpp --- Test Cases for Bit Accurate Types -------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a test of bitwidth attribute with enum data.
//
//===----------------------------------------------------------------------===//


#include <stdio.h>

class myClass
{
  public:
  enum X {R= 0x1, G=0x100, B=0x1ff}  __attribute__ ((bitwidth(9)))  x;
  void set_X(enum X t) { x = t; }
  enum X get_X(void) { return x; }
};

int main()
{
  myClass c;
  c.set_X(myClass::B);
  c.set_X((myClass::X)0x2ff);
  int i = (int)c.get_X();
  printf("%x\n", i);

  return 0;
}



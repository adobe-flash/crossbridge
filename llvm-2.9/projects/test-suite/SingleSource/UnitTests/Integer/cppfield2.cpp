//===-- cppfield.cpp - Test C++ Fields With Bit Accurate Types ------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This is a test for conversion between different int types.
//
//===----------------------------------------------------------------------===//


#include <stdio.h>
#include "bits.h"


class bitFieldStruct {
  public:
  int i;
  unsigned char c:7;
  int s:17;
  char c2;
};

class bitAccurateStruct {
  public:
  uint32 i;
  uint7 c : 7;
  uint17 s : 17;
  uint8 c2;
};

int main()
{
  printf("sizeof(bitFieldStruct) == %d\n", sizeof(bitFieldStruct));
  printf("sizeof(bitAccurateStruct) == %d\n", sizeof(bitAccurateStruct));

  if (sizeof(bitAccurateStruct) != 2 * sizeof(int))
    printf("bitAccurrateStruct should be %d but is %d \n", 
            2 * sizeof(int), sizeof(bitAccurateStruct));

  if (sizeof(bitFieldStruct) != 2 * sizeof(int))
    printf("bitFieldStruct should be %d but is %d \n", 
            2 * sizeof(int), sizeof(bitAccurateStruct));

  bitFieldStruct x;
  bitAccurateStruct y;

  char* yip = (char*) &y.i;
  char* yc2p = (char*) &y.c2;
  printf("Offset bitAccurateStruct.i = %d\n", yip - yip);
  printf("Offset bitAccurateStruct.c2 = %d\n", yc2p - yip);
  
  char* xip = (char*) &x.i;
  char* xc2p = (char*) &x.c2;
  printf("Offset bitFieldStruct.i = %d\n", xip - xip);
  printf("Offset bitFieldStruct.c2 = %d\n", xc2p - xip);

  return 0;
}

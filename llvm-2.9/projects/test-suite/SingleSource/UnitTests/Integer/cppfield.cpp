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

typedef unsigned char __attribute__ ((bitwidth(7))) int7;
typedef unsigned int __attribute__ ((bitwidth(17))) int17;
typedef unsigned int __attribute__ ((bitwidth(32))) int32;
typedef unsigned int __attribute__ ((bitwidth(8))) int8;

class bitFieldStruct {
  public:
    int i;
    unsigned char c:7;
    int s:17;
    char c2;
};

class bitAccurateStruct {
  public:
    int32 i;
    int7 c;
    int17 s;
    int8 c2;
};

int main()
{
  printf("sizeof(bitFieldStruct) == %d\n", sizeof(bitFieldStruct));
  printf("sizeof(bitAccurateStruct) == %d\n", sizeof(bitAccurateStruct));

  if (sizeof(bitAccurateStruct) != 4 * sizeof(int))
    printf("bitAccurrateStruct should be %d but is %d \n", 
            4 * sizeof(int), sizeof(bitAccurateStruct));

  if (sizeof(bitFieldStruct) != 2 * sizeof(int))
    printf("bitFieldStruct should be %d but is %d \n", 
            2 * sizeof(int), sizeof(bitAccurateStruct));

  bitFieldStruct x;
  bitAccurateStruct y;

  char* yip = (char*) &y.i;
  char* ycp = (char*) &y.c;
  char* ysp = (char*) &y.s;
  char* yc2p = (char*) &y.c2;
  printf("Offset bitAccurateStruct.i = %d\n", yip - yip);
  printf("Offset bitAccurateStruct.c = %d\n", ycp - yip);
  printf("Offset bitAccurateStruct.s = %d\n", ysp - yip);
  printf("Offset bitAccurateStruct.c2 = %d\n", yc2p - yip);
  
  char* xip = (char*) &x.i;
  char* xc2p = (char*) &x.c2;
  printf("Offset bitFieldStruct.i = %d\n", xip - xip);
  printf("Offset bitFieldStruct.c2 = %d\n", xc2p - xip);

  return 0;
}

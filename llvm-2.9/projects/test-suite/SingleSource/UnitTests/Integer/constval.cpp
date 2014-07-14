//===--- constval.c - Test constant APInt values --------------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a general test parsing constant APInt values
//
//===----------------------------------------------------------------------===//

#include "bits.h"
#include <stdio.h>

void my_test()
{
  uint21 x = bitsFromString("123456", 21);
  printf( "x = %d = ", int(x));
  printBits(x);

  uint16 y = bitsFromString("123456", 16);
  printf( "\ny = %d = ", int(y));
  printBits(y);

  int256 z = bitsFromHexString("0fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", 256); 
  printf( "\nz = " );
  printBits(z);

  int69 a = bitsFromOctalString("01234567012345670123456", 69);
  printf( "\na = " );
  printBits(a);

  int68 b = bitsFromBinaryString("10101010101010101010101010101010101010101010101010101010101010101010", 68);
  printf( "\nb = " );
  printBits(b);

  int169 c = bitsFromString("-1234567890123456789012345678901234567890", 169);
  printf( "\nc = " );
  printBits(c);

  printf("\n");
}

int main(int argc, char** argv)
{
  my_test();
  return 0;
}


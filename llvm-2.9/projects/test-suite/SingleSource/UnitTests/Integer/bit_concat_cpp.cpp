//===--- part_select.c --- Test The bit_select builtin --------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This test case tests the __builtin_part_select builtin function llvm-gcc.
// bit_select selects one bit out of a larger 
//
//===----------------------------------------------------------------------===//

#include <stdio.h>
#include <stdlib.h>
#include "bits.h"

int main(int argc, char** argv)
{
  int count = (argc > 1 ? atoi(argv[1]) % 128 : 128);

  uint17 X = 0; 
  uint19 Y = 0; 
  uint36 Z = 0;

  srand(count);

  for (int i = 0; i < count; i++) {
    Y = X = 0;
    for (int j = 0; j < bitwidthof(X); j++) {
      X <<= 1;
      X += (rand() % 2 == 0 ? 0 : 1);
    }
    for (int j = 0; j < bitwidthof(Y); j++) {
      Y <<= 1;
      Y += (rand() % 2 == 0 ? 0 : 1);
    }
    Z = bit_concat(X, Y);
    printf("bit_concat(");
    printBits(X);
    printf("(%d),",int(X));
    printf(",");
    printBits(Y);
    printf("(%d)) = ",int(Y));
    printBits(Z);
    printf("\n");
  }
  return 0;
}

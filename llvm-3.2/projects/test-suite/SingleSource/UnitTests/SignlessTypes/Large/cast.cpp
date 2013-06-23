/* 
 * This file is used to test combinations of eliminable cast 
 * operations to ensure that the logic in InstCombine is correct.
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char**argv) {
  int testVal = 42;
  int i;
  if (argc > 1)
    testVal = atoi(argv[1]);
  printf("testVal = %d\n", testVal);

  // First check for elimination of A -> B -> C type casts.

  // Make sure the loop goes through 256 values to catch at
  // least some single byte signed issues.
  for (i = -128+testVal; i < 128+testVal; ++i) {

#define TRIPLECAST(num, first, second, third) \
{ \
  first  X = (first)  num; \
  second Y = (second) X; \
  third  Z = (third)  Y; \
  printf("Test %d(%s) -> %s -> %s = %d\n", \
    num, #first, #second, #third,(int)Z); \
}

    // Test same size with sign rotation
    TRIPLECAST(i, unsigned char, unsigned char, unsigned char);
    TRIPLECAST(i, unsigned char, unsigned char, signed   char);
    TRIPLECAST(i, unsigned char, signed   char, unsigned char);
    TRIPLECAST(i, unsigned char, signed   char, signed   char);
    TRIPLECAST(i, signed   char, unsigned char, unsigned char);
    TRIPLECAST(i, signed   char, unsigned char, signed   char);
    TRIPLECAST(i, signed   char, signed   char, unsigned char);
    TRIPLECAST(i, signed   char, signed   char, signed   char);
    TRIPLECAST(i, unsigned short, unsigned short, unsigned short);
    TRIPLECAST(i, unsigned short, unsigned short, signed   short);
    TRIPLECAST(i, unsigned short, signed   short, unsigned short);
    TRIPLECAST(i, unsigned short, signed   short, signed   short);
    TRIPLECAST(i, signed   short, unsigned short, unsigned short);
    TRIPLECAST(i, signed   short, unsigned short, signed   short);
    TRIPLECAST(i, signed   short, signed   short, unsigned short);
    TRIPLECAST(i, signed   short, signed   short, signed   short);
    TRIPLECAST(i, unsigned int, unsigned int, unsigned int);
    TRIPLECAST(i, unsigned int, unsigned int, signed   int);
    TRIPLECAST(i, unsigned int, signed   int, unsigned int);
    TRIPLECAST(i, unsigned int, signed   int, signed   int);
    TRIPLECAST(i, signed   int, unsigned int, unsigned int);
    TRIPLECAST(i, signed   int, unsigned int, signed   int);
    TRIPLECAST(i, signed   int, signed   int, unsigned int);
    TRIPLECAST(i, signed   int, signed   int, signed   int);
    TRIPLECAST(i, unsigned long long, unsigned long long, unsigned long long);
    TRIPLECAST(i, unsigned long long, unsigned long long, signed   long long);
    TRIPLECAST(i, unsigned long long, signed   long long, unsigned long long);
    TRIPLECAST(i, unsigned long long, signed   long long, signed   long long);
    TRIPLECAST(i, signed   long long, unsigned long long, unsigned long long);
    TRIPLECAST(i, signed   long long, unsigned long long, signed   long long);
    TRIPLECAST(i, signed   long long, signed   long long, unsigned long long);
    TRIPLECAST(i, signed   long long, signed   long long, signed   long long);

    // Same size through larger size
    TRIPLECAST(i, unsigned char, unsigned short, unsigned char);
    TRIPLECAST(i, unsigned char, unsigned short, signed   char);
    TRIPLECAST(i, unsigned char, signed   short, unsigned char);
    TRIPLECAST(i, unsigned char, signed   short, signed   char);
    TRIPLECAST(i, signed   char, unsigned short, unsigned char);
    TRIPLECAST(i, signed   char, unsigned short, signed   char);
    TRIPLECAST(i, signed   char, signed   short, unsigned char);
    TRIPLECAST(i, signed   char, signed   short, signed   char);
    TRIPLECAST(i, unsigned short, unsigned int, unsigned short);
    TRIPLECAST(i, unsigned short, unsigned int, signed   short);
    TRIPLECAST(i, unsigned short, signed   int, unsigned short);
    TRIPLECAST(i, unsigned short, signed   int, signed   short);
    TRIPLECAST(i, signed   short, unsigned int, unsigned short);
    TRIPLECAST(i, signed   short, unsigned int, signed   short);
    TRIPLECAST(i, signed   short, signed   int, unsigned short);
    TRIPLECAST(i, signed   short, signed   int, signed   short);
    TRIPLECAST(i, unsigned int, unsigned long long, unsigned int);
    TRIPLECAST(i, unsigned int, unsigned long long, signed   int);
    TRIPLECAST(i, unsigned int, signed   long long, unsigned int);
    TRIPLECAST(i, unsigned int, signed   long long, signed   int);
    TRIPLECAST(i, signed   int, unsigned long long, unsigned int);
    TRIPLECAST(i, signed   int, unsigned long long, signed   int);
    TRIPLECAST(i, signed   int, signed   long long, unsigned int);
    TRIPLECAST(i, signed   int, signed   long long, signed   int);

    // Larger size through smaller size
    TRIPLECAST(i, unsigned short, unsigned char, unsigned short);
    TRIPLECAST(i, unsigned short, unsigned char, signed   short);
    TRIPLECAST(i, unsigned short, signed   char, unsigned short);
    TRIPLECAST(i, unsigned short, signed   char, signed   short);
    TRIPLECAST(i, signed   short, unsigned char, unsigned short);
    TRIPLECAST(i, signed   short, unsigned char, signed   short);
    TRIPLECAST(i, signed   short, signed   char, unsigned short);
    TRIPLECAST(i, signed   short, signed   char, signed   short);
    TRIPLECAST(i, unsigned int, unsigned short, unsigned int);
    TRIPLECAST(i, unsigned int, unsigned short, signed   int);
    TRIPLECAST(i, unsigned int, signed   short, unsigned int);
    TRIPLECAST(i, unsigned int, signed   short, signed   int);
    TRIPLECAST(i, signed   int, unsigned short, unsigned int);
    TRIPLECAST(i, signed   int, unsigned short, signed   int);
    TRIPLECAST(i, signed   int, signed   short, unsigned int);
    TRIPLECAST(i, signed   int, signed   short, signed   int);
    TRIPLECAST(i, unsigned long long, unsigned int, unsigned long long);
    TRIPLECAST(i, unsigned long long, unsigned int, signed   long long);
    TRIPLECAST(i, unsigned long long, signed   int, unsigned long long);
    TRIPLECAST(i, unsigned long long, signed   int, signed   long long);
    TRIPLECAST(i, signed   long long, unsigned int, unsigned long long);
    TRIPLECAST(i, signed   long long, unsigned int, signed   long long);
    TRIPLECAST(i, signed   long long, signed   int, unsigned long long);
    TRIPLECAST(i, signed   long long, signed   int, signed   long long);
  }

  // Check cast to bool
#define CASTTOBOOL(num, type) \
{ \
  type Y = (type) num; \
  bool X = (bool) num; \
  printf("%d(%s) -> bool = %d\n", num, #type, (int)X); \
}
  CASTTOBOOL(testVal, float);
  CASTTOBOOL(testVal, long long);
  CASTTOBOOL(testVal, unsigned int);
  CASTTOBOOL(testVal, signed char);
  CASTTOBOOL(testVal, bool);

  // Check useless getelementptr
  {
    static int sillyArray[8] = { 2, 3, 5, 7, 11, 13, 17, 19 };
    int* i_ptr = &sillyArray[ 0 ];
    long long* l_ptr = (long long*) i_ptr;
    printf("%lld\n", *l_ptr);
  }
  
  // Cast of malloc result to another type
  {
    // Note: must use volatile, otherwise g++ miscompiles at -O2 (but not LLVM)
    volatile float* float_ptr = (float*) malloc(sizeof(unsigned));
    *float_ptr = 10;
    volatile unsigned* unsigned_ptr = (volatile unsigned*) float_ptr;
    printf("%x\n", *unsigned_ptr);
  }

  // Propagation of single-use casts into other instructions
  {
  }
  return 0;
}

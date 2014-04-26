/* APPLE LOCAL file test of inttypes.h SCN* and PRI* macros */

/* { dg-do compile } */
/* { dg-options "-Wall -W" } */

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#define TEST(tn, scn, pri)			\
do {						\
  tn x;						\
  scanf ("%" scn "\n", &x);			\
  printf ("%" pri "\n", x);			\
} while (0)

void foo(void)
{
  TEST (int8_t, SCNd8, PRId8);
  TEST (uint8_t, SCNu8, PRIu8);
  TEST (int16_t, SCNd16, PRId16);
  TEST (uint16_t, SCNu16, PRIu16);
  TEST (int32_t, SCNd32, PRId32);
  TEST (uint32_t, SCNu32, PRIu32);
  TEST (int64_t, SCNd64, PRId64);
  TEST (uint64_t, SCNu64, PRIu64);
  
  TEST (int_least8_t, SCNdLEAST8, PRIdLEAST8);
  TEST (uint_least8_t, SCNuLEAST8, PRIuLEAST8);
  TEST (int_least16_t, SCNdLEAST16, PRIdLEAST16);
  TEST (uint_least16_t, SCNuLEAST16, PRIuLEAST16);
  TEST (int_least32_t, SCNdLEAST32, PRIdLEAST32);
  TEST (uint_least32_t, SCNuLEAST32, PRIuLEAST32);
  TEST (int_least64_t, SCNdLEAST64, PRIdLEAST64);
  TEST (uint_least64_t, SCNuLEAST64, PRIuLEAST64);
  
  TEST (int_fast8_t, SCNdFAST8, PRIdFAST8);
  TEST (uint_fast8_t, SCNuFAST8, PRIuFAST8);
  TEST (int_fast16_t, SCNdFAST16, PRIdFAST16);
  TEST (uint_fast16_t, SCNuFAST16, PRIuFAST16);
  TEST (int_fast32_t, SCNdFAST32, PRIdFAST32);
  TEST (uint_fast32_t, SCNuFAST32, PRIuFAST32);
  TEST (int_fast64_t, SCNdFAST64, PRIdFAST64);
  TEST (uint_fast64_t, SCNuFAST64, PRIuFAST64);

  TEST (intptr_t, SCNdPTR, PRIdPTR);
  TEST (uintptr_t, SCNuPTR, PRIuPTR);
  
  TEST (intmax_t, SCNdMAX, PRIdMAX);
  TEST (uintmax_t, SCNuMAX, PRIuMAX);
}

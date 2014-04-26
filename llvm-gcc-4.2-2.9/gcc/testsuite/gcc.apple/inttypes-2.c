/* APPLE LOCAL file test of inttypes.h [U]INT*_C macros */

/* { dg-do compile } */
/* { dg-options "-Wall -W" } */

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

#if INT8_C(3) != 3
#error int8 comparison failed
#endif

#if INT16_C(-3) != -3
#error int16 comparison failed
#endif

#if INT32_C(536870912) != 536870912L
#error int32 comparison failed
#endif

#if INT64_C(536870912) != 536870912LL
#error int64 comparison failed
#endif

#if UINT8_C(3) != 3
#error int8 comparison failed
#endif

#if UINT16_C(3) != 3
#error int16 comparison failed
#endif

#if UINT32_C(536870912) != 536870912UL
#error int32 comparison failed
#endif

#if UINT64_C(536870912) != 536870912ULL
#error int64 comparison failed
#endif

int main()
{
  return INT32_C(0);
}

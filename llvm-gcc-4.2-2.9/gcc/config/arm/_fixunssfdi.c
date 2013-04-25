/* APPLE LOCAL file 5316398 improved float/double -> int64 functions */
#include <stdint.h>

uint64_t
__fixunssfdi (float x)
{
  union { float f; uint32_t u; } u = {x};
  uint32_t hi, lo;

  /* early out for common small positive numbers. */
  if (__builtin_expect (u.u < 0x4f800000U, 1))
    return (uint64_t) ((uint32_t) x);

  /* larger non-overflowing cases are all exact, so we just need to do
     the conversion in integer code */
  /* if( 0x1.0p32f <= x < 0x1.0p63f ) */
  if (__builtin_expect (u.u < 0x5f800000U, 1))
    {
      uint32_t bits = (u.u & 0x007fffffU) | 0x00800000U;
      uint32_t shift = (u.u >> 23) - (127 + 23);
      if (shift < 32)
	{
	  hi = bits >> (32 - shift);
	  lo = bits << shift;
	}
      else
	{
	  hi = bits << (shift - 32);
	  lo = 0;
	}
      return ((uint64_t) hi << 32) | lo;
    }

  /* Overflow or NaN: convert value to unsigned int, set invalid as
     necessary */
  hi = x;

  /* extend to 64-bits. */
  lo = (hi << 1) | (hi & 1);
  return ((uint64_t) hi << 32) | lo;
}

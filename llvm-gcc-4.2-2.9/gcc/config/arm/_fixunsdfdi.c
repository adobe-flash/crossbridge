/* APPLE LOCAL file 5316398 improved float/double -> int64 functions */
#include <stdint.h>

uint64_t
__fixunsdfdi (double x)
{
  union { double d; uint64_t u; uint32_t u32[2]; } u = {x};
  uint32_t hi = u.u >> 32;
  uint32_t lo;

  /* Early out for the common case: +0 <= x < 0x1.0p32 */
  if (__builtin_expect (hi < 0x41f00000U, 1))
    return (uint64_t) ((uint32_t) x);

  /* 0x1.0p32 <= x < 0x1.0p64 */
  if (__builtin_expect (hi < 0x43f00000U, 1))
    {
      /* if x < 0x1.0p52 */
      if (__builtin_expect (hi < 0x43400000U, 1))
	{
	  if (__builtin_expect (hi < 0x43300000U, 1))
	    {
	      uint32_t shift = (1023 + 52) - (hi >> 20);
	      uint32_t unitBit = 1U << shift;
	      uint32_t fractMask = unitBit - 1;
	      u.u32[0] = lo = (uint32_t) u.u & ~fractMask;
	      x -= u.d;
	      hi &= 0x000FFFFFU;
	      hi |= 0x00100000U;
	      lo = (lo >> shift) | (hi << (32 - shift));
	      /* (int32_t) x is always zero here. This sets the inexact 
	         flag. */
	      hi = (hi >> shift) + (int32_t) x;
	    }
	  else
	    {
	      u.u &= 0x000FFFFFFFFFFFFFULL;
	      u.u |= 0x0010000000000000ULL;
	      return u.u;
	    }
	}
      else
	{
	  uint32_t shift = (hi >> 20) - (1023 + 52);
	  hi &= 0x000FFFFFU;
	  lo = u.u;
	  hi |= 0x00100000U;

	  hi = (hi << shift) | (lo >> (32 - shift));
	  lo = lo << shift;
	}

      /* return the result; */
      return ((uint64_t) hi << 32) | lo;
    }

  /* x <= -0 or x >= 0x1.0p64 or x is NaN. set invalid as necessary.
     Pin according to ARM rules. */
  hi = x;

  /* promote to 64-bits */
  lo = (hi << 1) | (hi & 1);
  return ((uint64_t) hi << 32) | lo;
}

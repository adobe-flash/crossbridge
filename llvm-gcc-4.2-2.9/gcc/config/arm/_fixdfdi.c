/* APPLE LOCAL file 5316398 improved float/double -> int64 functions */
#include <stdint.h>

int64_t
__fixdfdi (double x)
{
  union { double d; uint64_t u; } u = {x};
  uint64_t fabsx = u.u & 0x7fffffffffffffffULL;
  uint32_t exp = fabsx >> 52;
  int64_t result = 0;

  /* for very large and reasonably small values, regular int converter
     works fine */
  if (exp >= 52U + 1023U)	/* if( |x| >= 0x1.0p52 || isnan( x ) ) */
    {
      /* early out for error cases |x| >= 0x1.0p63 || isnan(x) */
      if (exp >= 1023U + 63U)
	{
	  /* special case for x == -0x1.0p63 */
	  if (-0x1.0p63 == x)
	    return 0x8000000000000000ULL;

	  /* huge, Inf, NaN */
	  result = (int32_t) x;	/* grab sign bit */
	  result >>= 63;	/* splat it across value */
	  /* return either 0x8000000000000000 or 0x7fffffffffffffff
	     according to sign bit */
	  result ^= 0x7fffffffffffffffULL;

	  return result;
	}

      /* 0x1.0p52 <= |x| < 0x1.0p63 always integer, but too big. Chop
         off some of the top. */
      u.u &= 0xFFFFFFFF00000000ULL;	/* truncate off some low bits */
      x -= u.d;			/* get remainder */

      /* accumulate the high part into result */
      int32_t hi = u.d * 0x1.0p-32;
      result += (int64_t) hi << 32;
    }
  else
    {				/* |x| < 0x1.0p52 */

      /* early out for |x| < 0x1.0p31 -- use hardware 32-bit conversion */
      if (exp < 1023U + 31U)
	return (int64_t) ((int32_t) x);

      /* The integer result fits in the significand, but there may be
         some fractional bits. Value is too large to use 32-bit
         hardware.

         create a mask that covers the high 32-bit part of the number
         and the whole integer part. */
      uint64_t intMask = (int64_t) 0xFFF0000000000000LL >> (exp - 1023);

      /* extract the full integer (round to integer in round to zero
         rounding mode) */
      u.u &= intMask;

      /* find the fractional part */
      double fraction = x - u.d;

      /* save the integer part */
      x = u.d;

      /* set inexact as needed */
      result = (int32_t) fraction;	/* always 0 */
    }

  /* xi is < 2**53 now and integer. Convert to integer representation. */
  if (x < 0.0)
    {
      u.d = x - 0x1.0p52;
      result -= u.u & 0x000FFFFFFFFFFFFFULL;
    }
  else
    {
      u.d = x + 0x1.0p52;
      result += u.u & 0x000FFFFFFFFFFFFFULL;
    }

  return result;
}

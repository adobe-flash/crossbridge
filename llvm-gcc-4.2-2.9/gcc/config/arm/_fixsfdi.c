/* APPLE LOCAL file 5316398 improved float/double -> int64 functions */
#include <stdint.h>

int64_t
__fixsfdi (float x)
{
  union { float f; uint32_t u; } u = {x};
  uint32_t fabsx = u.u & 0x7fffffffU;
  uint32_t exp = fabsx >> 23;
  int64_t result = 0;

  /* for small ints, overflow and NaN, the int32_t converter works fine 
     if( |x| < 0x1.0p31f || |x| >= 1.0p64f || isnan(x) ) unsigned
     compare */
  if (exp - (127U + 31U) >= (63U - 31U))
    {
      if (exp > (127 + 31))
	{
	  if (x == -0x1.0p63f)
	    return 0x8000000000000000LL;

	  uint32_t r = (int32_t) x;
	  result = (int64_t) r << 32;
	  r = (r << 1) | (r & 1);
	  result |= r;
	  return result;
	}

      /* small number. Regular int32_t conversion will work fine here. */
      result = (int32_t) x;
      return result;
    }

  /* 0x1.0p31 <= |x| <0x1.0p64, x is always an integer in this range */

  /* convert float to fixed */
  result = (fabsx & 0x007fffffU) | 0x00800000;

  /* signMask = x < 0.0f ? -1LL : 0 */
  int64_t signMask = (int64_t) u.u << 32;
  signMask >>= 63;

  /* Calculate shift value to move fixed point to right place */
  int32_t leftShift = exp - (127 + 23);

  /* move the fixed point into place */
  result <<= leftShift;

  /* Fix sign */
  result ^= signMask;
  result -= signMask;

  return result;
}

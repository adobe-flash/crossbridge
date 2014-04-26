/* APPLE LOCAL file x86_64 */
/* Test that we emit literal16 for 64-bit targets right now.  */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-m64" } */
/* { dg-final { scan-assembler "literal16" } } */
#include <math.h>

int a;
int b;

int
test (float val)
{
  if ((sizeof (val) ==
       sizeof (float) ? __inline_isinff ((float) (val)) : sizeof (val) ==
       sizeof (double) ? __inline_isinfd ((double) (val)) :
       __inline_isinf ((long double) (val))))
    return (val < 0.0) ? a : b;
  return 0;
}

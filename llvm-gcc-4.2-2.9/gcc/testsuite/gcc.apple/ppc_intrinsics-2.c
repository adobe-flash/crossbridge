/* { dg-options "-Wshorten-64-to-32 -Wno-long-long" } */
/* { dg-do run { target "powerpc*-*-darwin*" } } */
#include "ppc_intrinsics.h"
#include <stdlib.h>

int main( void )
{
  int intconst = 0x5;
  long long longlongconst = 0x5;
  int intloc, intloc1, intloc2;
  unsigned int uintloc, uintloc1, uintloc2;
  long longloc;
  long long longlongloc;
  int fails = 0;

  intloc = __cntlzw(intconst);
  if (intloc != 29)
    ++fails;

  longlongloc = __cntlzd(longlongconst);
  if (longlongloc != 61)
    ++fails;

  intloc = 0xffff;
  __rlwimi(intloc, 0x21876543, 8, 0, 23);
  if (intloc != 0x876543ff)
    ++fails;

  intloc = __rlwinm(0x21876543, 8, 0, 31);
  if (intloc != 0x87654321)
    ++fails;

  longloc = __rlwinm(0x45, 5, 24, 31);
  if (longloc != 0xa0)
    ++fails;

  longloc = __rlwnm(0x47, 5, 24, 31);
  if (longloc != 0xe0)
    ++fails;

  intloc1 = 1 << 20;
  intloc2 = 1 << 21;
  intloc = __mulhw (intloc1, intloc2);
  if (intloc != (1 << 9))
    ++fails;

  intloc1 = 0xfffe0000;
  intloc2 = 0xfffc0000;
  intloc = __mulhw (intloc1, intloc2);
  if (intloc != 8)
    ++fails;

  uintloc1 = 0xffffffff;
  uintloc2 = 0xfffffffe;
  uintloc = __mulhwu (uintloc1, uintloc2);
  if (uintloc != 0xfffffffd)
    ++fails;

  uintloc1 = 0xfffe0000;
  uintloc2 = 0xfffc0000;
  uintloc = __mulhwu (uintloc1, uintloc2);
  if (uintloc != 0xfffa0008)
    ++fails;

  return fails;
}

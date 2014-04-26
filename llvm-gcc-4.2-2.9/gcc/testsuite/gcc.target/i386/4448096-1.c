/* APPLE LOCAL file 4448096 */
/* { dg-do compile { target i?86-*-* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O2 -mpreferred-stack-boundary=2 -mfpmath=sse -msse2" } */
#include <stdlib.h>
double d = 42.0;
unsigned int i;
main ()
{
  i = d;
  return 0;
}
/* The SSE-only double -> uint32 conversion uses several vector
   instructions (including "maxsd"), and any spills amongst these
   requires an SSE-aligned stack.  -mpreferred-stack-boundary=2 means
   "4-byte aligned stack," therefore the compiler must not emit any
   vector-mode instructions for this testcase.  */
/* { dg-final { scan-assembler-not "maxsd" } } */

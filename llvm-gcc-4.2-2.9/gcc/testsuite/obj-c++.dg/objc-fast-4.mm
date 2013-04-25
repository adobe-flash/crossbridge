/* APPLE LOCAL file ObjC direct dispatch */
/* Check that 4015820 is fixed.   That does not appear except
   with -O0, so the sibcall case cannot occur.  */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-O0 -fobjc-direct-dispatch" } */
/* { dg-require-effective-target ilp32 } */

#include <objc/Object.h>

void foo(void) {
  Object *o;
  [o++ free];
}
/* { dg-final { scan-assembler-not "objc_msgSend" } } */
/* { dg-final { scan-assembler "bla.*fffeff00" } } */

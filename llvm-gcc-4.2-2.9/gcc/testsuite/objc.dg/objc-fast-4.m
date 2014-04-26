/* APPLE LOCAL file ObjC direct dispatch */
/* The code should call to fffeff00 directly, not through objc_msgSend.  */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* APPLE LOCAL ObjC direct dispatch */
/* { dg-options "-O0 -fobjc-direct-dispatch -fnext-runtime" } */
/* { dg-require-effective-target ilp32 } */
/* Radar 4015820 */

/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */
/* Radar 4015820 */

#include <objc/Object.h>

void foo(void) {
  Object *o;
  [o++ free];
}
/* APPLE LOCAL begin ObjC direct dispatch */
/* { dg-final { scan-assembler-not "objc_msgSend" } } */
/* { dg-final { scan-assembler "bla.*fffeff00" } } */
/* APPLE LOCAL end ObjC direct dispatch */

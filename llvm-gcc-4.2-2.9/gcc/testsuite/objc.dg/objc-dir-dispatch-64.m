/* APPLE LOCAL file radar 4590221 */
/* Check that -fobjc-direct-dispatch is on by default for 32-bit ppc with 
   -mmacosx-version-min=10.4. */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.4 -m64 -fobjc-abi-version=1" } */

#include <objc/Object.h>

@interface Derived: Object {
@public
  Object *other;
}
@end

void foo(void) {
  Derived *o = [Derived new];
  o->other = 0;  
}

/* { dg-final { scan-assembler "objc_msgSend" { target i?86-*-* } } } */
/* { dg-final { scan-assembler "objc_msgSend" { target powerpc*-*-darwin* } } } */

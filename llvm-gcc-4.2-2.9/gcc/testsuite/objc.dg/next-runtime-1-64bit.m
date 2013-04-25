/* APPLE LOCAL file 4492976 */
/* Test that the correct version number (7) is set in the module descriptor
   when compiling for the NeXT runtime.  */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -m64 -fobjc-abi-version=1" } */

#include <objc/Object.h>

@interface FooBar: Object
- (void)boo;
@end

@implementation FooBar
- (void)boo { }
@end

/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler "L_OBJC_MODULES:.*\n\[ \t\]*\.quad\t7\n" } } */

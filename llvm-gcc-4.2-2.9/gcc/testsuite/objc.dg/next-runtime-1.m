/* APPLE LOCAL radar 4585769 */
/* Test that the correct version number (7) is set in the module descriptor
   when compiling for the NeXT runtime.  */
/* Author: Ziemowit Laski <zlaski@apple.com>  */

/* APPLE LOCAL ARM objc2 */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime" } */
/* APPLE LOCAL 64-bit 4492976 */
/* { dg-require-effective-target ilp32 } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface FooBar: Object
- (void)boo;
@end

@implementation FooBar
- (void)boo { }
@end

/* APPLE LOCAL radar 4585769 */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler "L_OBJC_MODULES:.*\n\[ \t\]*\.long\t7.*\n" } } */

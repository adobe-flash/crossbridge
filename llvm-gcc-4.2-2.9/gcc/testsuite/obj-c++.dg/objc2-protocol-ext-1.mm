/* APPLE LOCAL file 5192466 */
/* Check for addition of two new fiellds at end of protocol meta-data. */
/* { dg-options "-mmacosx-version-min=10.5 -m64" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

@protocol Proto1
@end

@protocol Proto2
@end

@interface Super <Proto1, Proto2> { id isa; } @end
@implementation Super @end
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".long\t72.*\n\t.(long\t0|space\t4)" } } */

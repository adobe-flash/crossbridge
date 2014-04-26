/* APPLE LOCAL file radar 6803242 */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -m64" } */

@interface INTF
@end
@implementation  INTF
@end
/* LLVM LOCAL allow different spelling */
/* { dg-final { scan-assembler "L_OBJC_IMAGE_INFO:\n\t(.long\t0|.space\t4)\n\t.long\t16" } } */

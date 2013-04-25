/* APPLE LOCAL file radar 4810587 */
/* Check that default option also results in generation of objc section 
   with flag of 0 */

/* { dg-do compile } */
/* { dg-require-effective-target ilp32 } */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */

@interface INTF
@end
@implementation  INTF
@end
/* LLVM LOCAL begin llvm syntax */
/* { dg-final { scan-assembler ".section.*__OBJC,.*__image_info" } } */
/* { dg-final { scan-assembler "L_OBJC_IMAGE_INFO:.*\n\t.space( |\t)8" } } */
/* LLVM LOCAL end */

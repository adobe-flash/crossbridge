/* APPLE LOCAL file 4492976 */
/* Check if the objc_symtab descriptor is being laid out correctly.  */
/* { dg-options "-fnext-runtime -m64 -fobjc-abi-version=1" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#include <objc/Object.h>

@interface Base: Object 
- (void)setValues;
@end

@interface Derived: Base
- (void)checkValues;
@end

@implementation Base
-(void)setValues { }
@end

@implementation Derived
-(void)checkValues { }
@end

/* APPLE LOCAL testing */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler "L_OBJC_SYMBOLS.*:.*\n\t(.quad\t0|.space\t8)\n\t(.long\t0\n\t.space 4|.space\t8)\n\t.(short|word)\t2.*\n\t.((short|word)\t0|space\t2)\n\t.space( |\t)4\n\t.quad\tL_OBJC_CLASS_Derived.*\n\t.quad\tL_OBJC_CLASS_Base.*\n" } } */

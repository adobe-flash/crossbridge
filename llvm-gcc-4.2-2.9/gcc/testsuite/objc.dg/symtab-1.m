/* Check if the objc_symtab descriptor is being laid out correctly.  */
/* Contributed by Ziemowit Laski <zlaski@apple.com>.  */
/* { dg-options "-fnext-runtime" } */
/* { dg-do compile { target *-*-darwin* } } */
/* APPLE LOCAL radar 4492976 */
/* { dg-require-effective-target ilp32 } */
/* APPLE LOCAL ARM not available on arm-darwin targets */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

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
/* LLVM LOCAL llvm syntax */
/* { dg-final { scan-assembler "L_OBJC_SYMBOLS.*:.*\n\t(.long\t0|.space\t4)\n\t(.long\t0|.space\t4)\n\t.(short|word)\t2.*\n\t.((short|word)\t0|space\t2)\n\t.long\tL_OBJC_CLASS_Derived.*\n\t.long\tL_OBJC_CLASS_Base.*\n" } } */

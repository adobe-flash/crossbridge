/* APPLE LOCAL file radar 6255595 */
/* Check for generation of new sections for objc2 meta-data. */
/* { dg-options "-fobjc-abi-version=2 -mmacosx-version-min=10.6" } */
/* { dg-do compile */
@protocol PROTO
@end

@interface INTF <PROTO> @end

@implementation INTF @end
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__DATA,__objc_const" } } */
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__DATA,__objc_data" } } */

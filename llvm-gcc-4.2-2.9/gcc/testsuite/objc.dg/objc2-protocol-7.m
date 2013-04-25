/* APPLE LOCAL file 4695109 */
/* Another test for definition of both protocol meta-data. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */


#include <objc/objc.h>
#include <objc/objc-api.h>

@protocol Proto1
+(void)doItClass1;
-(void)doItInstance1;
@end

@protocol Proto2
+(void)doItClass2;
-(void)doItInstance2;
@end

@interface MyClass1 <Proto1>
{
  Class isa;
}
@end
@implementation MyClass1
+(void)doItClass1{}
-(void)doItInstance1{}
@end

@interface MyClass2 : MyClass1 <Proto2>
@end
@implementation MyClass2
+(void)doItClass2{}
-(void)doItInstance2{}
@end

@interface MyClass3
{
  Class isa;
}
@end
@interface MyClass4 : MyClass3 <Proto1>
@end

/*----------------------------------------*/

Class cls = 0;
Class <Proto1> clsP1 = 0;
Class <Proto2> clsP2 = 0;
/* { dg-final { scan-assembler "_OBJC_PROTOCOL_\\\$_Proto1:" } } */
/* { dg-final { scan-assembler "_OBJC_PROTOCOL_\\\$_Proto2:" } } */

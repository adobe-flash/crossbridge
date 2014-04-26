/* APPLE LOCAL file radar 5407792 */
/* 'nonatomic' property requires a new 'N' property attribute encoding */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#import <Foundation/Foundation.h>

@interface NonAtomicTest : NSObject {
  NSString *foo;
}
@property(copy, nonatomic) NSString *foo;
@end

@implementation NonAtomicTest
@synthesize foo;
@end
/* { dg-final { scan-assembler ".*C,N,Vfoo" } } */


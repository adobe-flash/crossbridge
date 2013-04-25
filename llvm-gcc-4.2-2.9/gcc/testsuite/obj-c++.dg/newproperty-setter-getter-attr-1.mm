/* APPLE LOCAL file radar 5207415 */
/* Test that user named setter/getter method names will be used in synthesis
   of setter/getter. Program must run with no abort. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */
#import <Foundation/Foundation.h>

@interface Foo : NSObject
{
    int counterThing;    
}
@property(setter=setCounterValue:, getter=currentCounterValue) int counterThing;
@end

@implementation Foo
@synthesize counterThing;
@end

int main (void) {
    NSAutoreleasePool *p = [NSAutoreleasePool new];
    Foo *foo = [Foo new];
    foo.counterThing = 5;
    if ([foo currentCounterValue] != 5)
      abort ();
    [foo setCounterValue: 42];
    if (foo.counterThing != 42)
      abort ();
    [foo release];
    [p release];
    return 0;
}


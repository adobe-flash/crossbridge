/* APPLE LOCAL file radar 5285911 */
/* Test that can have a 'setter' method only without a property or the corresponding
   'getter' method. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#import <Foundation/Foundation.h>

/* { dg-do run } */

static int g_val;

@interface Subclass : NSObject
{
    int setterOnly;
}
- (void) setSetterOnly:(int)value;
@end

@implementation Subclass
- (void) setSetterOnly:(int)value {
    setterOnly = value;
    g_val = setterOnly;
}
@end

int main (void) {
    Subclass *x = [[Subclass alloc] init];

    x.setterOnly = 4;
    if (g_val != 4)
      abort ();
    return 0;
}

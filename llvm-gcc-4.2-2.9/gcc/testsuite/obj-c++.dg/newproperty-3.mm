/* APPLE LOCAL file radar 4805321 */
/* This program tests use of property provided setter/getter functions. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property -lobjc" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -lobjc" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Bar : Object
{
  int iVar;
}
@property (assign, getter = MyGetter) int FooBar;
- (int) MyGetter;
@end

@implementation Bar
@synthesize FooBar = iVar;
- (void) MySetter : (int) value { iVar = 1000; }
- (int) MyGetter { return iVar; }
@end

int main(int argc, char *argv[]) {
    Bar *f = [Bar new];
    f.FooBar = 1;

    f.FooBar += 3;

    f.FooBar -= 4;
    return f.FooBar;
}


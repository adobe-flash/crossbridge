/* APPLE LOCAL file radar 4712415 - radar 5285911 */
/* This program tests use of deprecated attribute on property. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Bar : Object
{
  int iVar;
}
@property (assign, setter = MySetter:) int FooBar __attribute__ ((deprecated));
- (void) MySetter : (int) value;
@end

@implementation Bar
@synthesize FooBar = iVar;
- (void) MySetter : (int) value { iVar = value; }

@end

int main(int argc, char *argv[]) {
    Bar *f = [Bar new];
    f.FooBar = 1;	/* { dg-warning "\'MySetter:\' is deprecated" } */
    return f.FooBar;	/* { dg-warning "\'FooBar\' is deprecated" } */
}

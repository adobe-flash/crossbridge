/* APPLE LOCAL file radar 4965989 */
/* Test for checking out property execution with anonymous category adding setter to
   primary category. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/objc.h>
#include <objc/objc.h>
#include "../objc/execute/Object2.h"

@interface Bar : Object
{
  int iVar;
}
@property (readonly) int FooBar;
@end


@interface Bar ()
@property (readwrite) int FooBar;
@end

@implementation Bar
@synthesize FooBar = iVar;
@end

int main(int argc, char *argv[]) {
    Bar *f = [Bar new];
    f.FooBar = 1;

    f.FooBar += 3;

    f.FooBar -= 4;
    return f.FooBar;
}


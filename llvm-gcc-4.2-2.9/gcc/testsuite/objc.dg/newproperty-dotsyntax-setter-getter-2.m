/* APPLE LOCAL file radar 4805321 */
/* Test sequence of assignment to setters. */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5 -std=c99 -lobjc" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -std=c99 -lobjc" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"
extern void abort (void);

@interface Bar : Object
{
  int iVar;
  int iBar;
  float f;
}
- (int) prop1;
- (void) setProp1 : (int) arg;
- (int) prop2;
- (void) setProp2 : (int) arg;
- (float) fprop;
- (void) setFprop : (float) arg;

@end

@implementation Bar
- (int) prop1 { return iVar; }
- (int) prop2 { return iBar; }
- (float) fprop { return f; }
- (void) setProp1 : (int) value { iVar = value; }
- (void) setProp2 : (int) value { iBar = value; }
- (void) setFprop : (float) value { f = value; }

@end

int main(int argc, char *argv[]) {
    Bar *f = [Bar new];
    if (f.prop2 = 1)
      f.prop2 = f.prop1 = -4;
    if (f.prop1 == -4)
      f.prop2 = f.prop1 = 5;

    if (f.prop1 != 5 || f.prop2 != 5)
      abort ();

    f.fprop = 3.14;
    f.prop1 = f.prop2 = f.fprop;
    if (f.prop1 != 3 || f.prop2 != 3)
      abort ();
    while (f.prop1)
      f.prop1 -= 1;
    return f.prop1;
}


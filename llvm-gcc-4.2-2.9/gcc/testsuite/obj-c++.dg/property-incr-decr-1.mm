/* APPLE LOCAL file 4712269 */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface SomeClass : Object
{
  int _myValue;
}
@property int myValue;
@end

@implementation SomeClass
@synthesize myValue=_myValue;
@end

int main()
{
    int val;
    SomeClass *o = [SomeClass new];
    o.myValue = -1;
    val = o.myValue++; /* val -1, o.myValue 0 */
    val += o.myValue--; /* val -1. o.myValue -1 */
    val += ++o.myValue; /* val -1, o.myValue 0 */
    val += --o.myValue; /* val -2, o.myValue -1 */
    return ++o.myValue + (val+2);
}


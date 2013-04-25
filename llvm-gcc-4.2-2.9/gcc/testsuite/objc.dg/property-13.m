/* APPLE LOCAL file radar 4660579 */
/* Test that property can be declared 'readonly' in interface but it can be
   overridden in the anonymous category and can be assigned to.
*/
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5 -std=c99" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-std=c99" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface ReadOnly : Object
{
  int _object;
  int _Anotherobject;
}
@property(readonly) int object;
@property(readonly) int Anotherobject;
@end

@interface ReadOnly ()
@property(readwrite) int object;
@property(readwrite, setter = myAnotherobjectSetter:) int Anotherobject;
@end

@implementation ReadOnly
@synthesize object = _object;
@synthesize  Anotherobject = _Anotherobject;
- (void) myAnotherobjectSetter : (int)val {
    _Anotherobject = val;
}
@end

int main(int argc, char **argv) {
    ReadOnly *test = [ReadOnly new];
    test.object = 12345;
    test.Anotherobject = 200;
    return test.object - 12345 + test.Anotherobject - 200;
}


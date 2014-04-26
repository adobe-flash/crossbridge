/* APPLE LOCAL file radar 4523837 */
/* Test for correct property setter lookup when a similarl method name
   exists in an unrelated class. Test should compile with no error.
*/
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5 -std=c99" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-std=c99" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

typedef struct {int x, y, w, h;} st1;
typedef struct {int x, y, w, h;} st2;

@interface bar : Object
- (void)setFrame:(st1)frameRect;
@end

@interface bar1 : Object
- (void)setFrame:(int)frameRect;
@end

@interface foo : Object
{
	st2 ivar;
}
@property (assign) st2 frame;
@end

@implementation foo
@synthesize frame = ivar;
@end

extern void abort();

static   st2 r = {1,2,3,4};
st2 test (void)
{
    foo *obj = [foo new];
    id objid = [foo new];;

    obj.frame = r;

    ((foo*)objid).frame = obj.frame;

    return ((foo*)objid).frame;
}

int main ()
{
  st2 res = test ();
  if (res.x != 1 || res.h != 4)
    abort();
  return 0;
}

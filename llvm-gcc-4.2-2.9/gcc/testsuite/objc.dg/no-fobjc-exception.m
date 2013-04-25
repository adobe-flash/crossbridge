/* APPLE LOCAL file 4512786 */
/* { dg-do run } */
#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface M : Object
- (void) foo;
@end

@implementation M : Object
- (void) foo
{
    @try {
    } @catch (Object *localException) {
    }

}
@end

int main()
{
	M *p = [M new];
	[p foo];
	return 0;
}

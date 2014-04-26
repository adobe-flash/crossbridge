/* APPLE LOCAL file 4696522 */
/* Test that static_cast of objective-c pointer to objects works. */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"


static int res = 1;

@protocol foo
- (void) callMe;
@end

typedef id<foo> fooPtr;


@interface MyObject : Object
- (void) callMe;
@end

@implementation MyObject
- (void) callMe
{
  res = 0;
}
@end
int main (int argc, const char * argv[]) {
    MyObject * pool = [[MyObject alloc] init];

    [static_cast<fooPtr>(pool) callMe];
    
    [pool free];
    return res;
}



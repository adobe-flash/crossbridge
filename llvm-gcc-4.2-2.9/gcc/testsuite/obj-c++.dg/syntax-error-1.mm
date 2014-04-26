/* APPLE LOCAL file 4185810 */
/* Graceful handling of a syntax error.  */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

class foo {
  public:
    foo();
    virtual ~foo();
};


extern void NXLog(const char *, ...);

@interface Test2 : Object {
}
- (void) foo2;
@end

@implementation Test2
- (void) foo2
  NXLog("Hello, world!"); /* { dg-error "expected .\{. before .NXLog." } */
			  /* { dg-error "expected constructor, destructor" "" { target *-*-* } 24 } */
}			  /* { dg-error "stray" } */
@end

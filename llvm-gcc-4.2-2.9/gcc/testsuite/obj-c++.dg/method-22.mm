/* APPLE LOCAL file 4175534 */
/* Ensure that overload resolution does not produce warnings as
   side-effects.  */
/* { dg-do run } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"
#include <stdlib.h>

#define CHECK_IF(E) if(!(E)) abort ()

@interface MyCursor: Object
+ (MyCursor *)crosshairCursor;
@end

@class MyImage;

class A {
public:
    A();
    
    int foo(MyImage *);
    int foo(MyCursor *);
};

A::A() {}
int A::foo(MyCursor * c) { return 17; }
int A::foo(MyImage * i) { return 29; }

@implementation MyCursor
+ (MyCursor *)crosshairCursor {
  return self;
}
@end

int main(void) {
  A a;
  
  int r = a.foo([MyCursor crosshairCursor]);

  CHECK_IF (r == 17);    
  return 0;
}


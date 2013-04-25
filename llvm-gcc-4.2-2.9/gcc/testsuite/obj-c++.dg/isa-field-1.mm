/* Ensure there are no bizarre difficulties with accessing the 'isa' field of objects.  */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Object (Test)
- (Class) test1: (id)object;
@end

@interface Derived: Object
- (Class) test2: (id)object;
@end

@implementation Object (Test)

Class test1(id object) {
  Class cls = object->isa;
  return cls;
}
- (Class) test1: (id)object {
  Class cls = object->isa;
  return cls;
}

@end

@implementation Derived

Class test2(id object) {
  Class cls = object->isa;
  return cls;
}
- (Class) test2: (id)object {
  Class cls = object->isa;
  return cls;
}

@end

Class test3(id object) {
  Class cls = object->isa;
  return cls;
}

/* APPLE LOCAL file 4795703  - radar 5251502 */
/* Compile with no ICE. */
/* { dg-do compile } */

@interface Object
{
 Class isa;
}
@end

namespace Fusion {

class Object {

public:

operator double() const {  }

static void staticvoidargsthunk(Object function)
{
 union { double d; } u;
 u.d = Object(function);
}

};
}



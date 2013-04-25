/* APPLE LOCAL file 4174166 */
/* When assigning function pointers, allow for covariant return types
   and contravariant argument types.  */
/* { dg-do compile } */
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@class Derived;

Object *ExternFunc (Object *filePath, Object *key);
typedef id FuncSignature (Object *arg1, Derived *arg2);

@interface Derived: Object
+ (void)registerFunc:(FuncSignature *)function;
@end

void foo(void)
{
  [Derived registerFunc: ExternFunc];
}


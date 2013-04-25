/* APPLE LOCAL file radar 5245963 */
/* Contributed by David Ayers - Fri Jun  4 03:16:17 BST 2004 */
/* Test that a protocol is not equal to something which is not a protocol.  */
#include <objc/Protocol.h>

@protocol Foo
- (void)foo;
@end

int main (void)
{
  /* A Protocol object should not be equal to a Class object.  */
#   if ((MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5) && __NEXT_RUNTIME__)
  if (protocol_isEqual (@protocol(Foo), [Protocol class]))
#else
  if ([@protocol(Foo) isEqual: [Protocol class]])
#endif
    {
      abort ();
    }
  
  return 0;
}


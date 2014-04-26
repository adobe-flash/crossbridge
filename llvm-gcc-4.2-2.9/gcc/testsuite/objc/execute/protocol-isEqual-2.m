/* APPLE LOCAL file radar 5245963 */
/* Contributed by Nicola Pero - Fri Jun  4 03:16:17 BST 2004 */
/* Test that protocols with different names are different.  */
#include <objc/Protocol.h>

@protocol Foo1
- (void)foo1;
@end

@protocol Foo2
- (void)foo2;
@end

int main (void)
{
#   if ((MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5) && __NEXT_RUNTIME__)
  if (protocol_isEqual (@protocol(Foo1), @protocol(Foo2)))
#else
  if ([@protocol(Foo1) isEqual: @protocol(Foo2)])
#endif
    {
      abort ();
    }
  
  return 0;
}


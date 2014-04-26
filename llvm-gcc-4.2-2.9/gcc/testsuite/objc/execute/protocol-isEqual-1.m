/* APPLE LOCAL file radar 5245963 */
/* Contributed by Nicola Pero - Fri Jun  4 03:16:17 BST 2004 */
/* Test that a protocol is equal to itself.  */
#include <objc/Protocol.h>

@protocol Foo
- (void)foo;
@end

int main (void)
{
  Protocol *protocol = @protocol(Foo);

#   if ((MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5) && __NEXT_RUNTIME__)
  if (!protocol_isEqual (protocol, protocol))
#else
  if (! [protocol isEqual: protocol])
#endif
    {
      abort ();
    }
  
  return 0;
}


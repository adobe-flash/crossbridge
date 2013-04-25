/* Contributed by Nicola Pero - Fri Mar  9 21:35:47 CET 2001 */
#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "Object2.h"

@interface Object (StopProtocol)
- (void) stop;
@end

int main (void)
{
  return 0;
}


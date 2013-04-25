/* APPLE LOCAL file radar 5245946 */
/* Crash due to descriptionFor(Instance|Class)Method applied to
   a protocol with no instance/class methods respectively.
   Problem report and original fix by richard@brainstorm.co.uk.  */
/* { dg-do run } */
#include <objc/objc.h>
#include <objc/Object.h>
#include <objc/Protocol.h>

@protocol NoInstanceMethods
+ testMethod;
@end

@protocol NoClassMethods
- testMethod;
@end

int
main()
{
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
(void)protocol_getMethodDescription (@protocol(NoInstanceMethods),
				     @selector(name), YES, YES);
(void)protocol_getMethodDescription (@protocol(NoInstanceMethods),
				     @selector(name), YES, NO);
(void)protocol_getMethodDescription (@protocol(NoClassMethods),
				     @selector(name), YES, YES);
(void)protocol_getMethodDescription (@protocol(NoClassMethods),
				     @selector(name), YES, NO);
#else
[@protocol(NoInstanceMethods) descriptionForInstanceMethod: @selector(name)];
[@protocol(NoInstanceMethods) descriptionForClassMethod: @selector(name)];
[@protocol(NoClassMethods) descriptionForInstanceMethod: @selector(name)];
[@protocol(NoClassMethods) descriptionForClassMethod: @selector(name)];
#endif
return 0;
}

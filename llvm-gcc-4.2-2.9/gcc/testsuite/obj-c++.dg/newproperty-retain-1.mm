/* APPLE LOCAL file radar 4805321 */
/* This program tests use of properties . */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property -framework Foundation -fobjc-exceptions" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -framework Foundation -fobjc-exceptions" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <Foundation/Foundation.h>

static id
object_getProperty_byref (id self, SEL _cmd, unsigned int offset)
{
  id *slot = (id*) ((char*)self + offset);
  return *slot;
}

static void 
object_setProperty_byref (id self, SEL _cmd, id value, unsigned int offset)
{
  id *slot = (id*) ((char*)self + offset);
  id oldValue = *slot;
  if (oldValue != value)
    *slot = value;
}

@interface Person : NSObject
{
  NSString *ifirstName;
  NSString *ilastName;
  NSString *ifullName;
}
@property (retain) NSString *firstName, *lastName;
@property(retain, readonly) NSString *fullName;
@end

@interface Group : NSObject
{
  Person *itechLead;
  Person *iruntimeGuru;
  Person *ipropertiesMaven;
}
@property (retain) Person *techLead, *runtimeGuru, *propertiesMaven;
@end

@implementation Group
@synthesize techLead=itechLead, runtimeGuru=iruntimeGuru, propertiesMaven=ipropertiesMaven;
- init {
  self.techLead = [[Person alloc] init];
  self.runtimeGuru = [[Person alloc] init];
  self.propertiesMaven = [[Person alloc] init];
  return self;
}
@end

@implementation Person
@synthesize lastName=ilastName, firstName=ifirstName;
@synthesize fullName=ifullName;
- (NSString*)fullName { // computed getter
    return [NSString stringWithFormat:@"%@ %@", self.firstName, self.lastName];
}
@end

NSString *playWithProperties()
{
  Group *g = [[Group alloc] init] ;

  g.techLead.firstName = @"Blaine";
  g.techLead.lastName = @"Garst";
  g.runtimeGuru.firstName = @"Greg";
  g.runtimeGuru.lastName = @"Parker";
  g.propertiesMaven.firstName = @"Patrick";
  g.propertiesMaven.lastName = @"Beard";

  return [NSString stringWithFormat:@"techlead %@ runtimeGuru %@ propertiesMaven %@",
                        g.techLead.fullName, g.runtimeGuru.fullName, g.propertiesMaven.fullName];
}

main()
{
    char buf [256];
    NSAutoreleasePool* pool  = [[NSAutoreleasePool alloc] init];
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
    sprintf(buf, "%s", [playWithProperties() UTF8String]);
#else
    sprintf(buf, "%s", [playWithProperties() cString]);
#endif
    [pool release];
    return strcmp (buf, "techlead Blaine Garst runtimeGuru Greg Parker propertiesMaven Patrick Beard");
}


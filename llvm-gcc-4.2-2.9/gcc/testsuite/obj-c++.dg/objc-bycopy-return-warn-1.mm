/* APPLE LOCAL file radar 4869979 */
/* Check that an instance method with 'bycopy' return type issues a warning when the 
   object it is returning does not conform to NSCoding protocol. */
/* { dg-do compile { target *-*-darwin* } } */
/* APPLE LOCAL ARM Cocoa not available on arm-darwin targets */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */

#include <Cocoa/Cocoa.h>
@interface MyClass : NSObject
{
}
- (bycopy NSManagedObjectID*) myMethod; /* { dg-warning "\"bycopy\" specified on instance method type" } */
@end

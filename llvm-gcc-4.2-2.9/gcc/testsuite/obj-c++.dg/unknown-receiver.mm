/* APPLE LOCAL file radar 4180592 */
/* Test that undeclared object does not result in compiler crash. */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface CrashTheCompiler : Object
{
 id obj;
}

-(void)updateObject:(id)nObj;

@end

@implementation CrashTheCompiler: Object

-(void)updateObject:(id)nObj
{
 if (obj == 0)
 {
  UnknownObject* obj2 = [[UnknownObject alloc] init]; /* { dg-error "\\\'UnknownObject\\\' was not declared" } */
  						      /* { dg-error "\\\'obj2\\\' was not declared" "" { target *-*-* } 23 } */
 }
}
@end

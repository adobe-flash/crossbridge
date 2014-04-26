/* Don't forget to look in protocols if a class (and its superclasses) do not
   provide a suitable method.  */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@protocol Zot
-(void) zot;
@end

@interface Foo : Object <Zot>
@end

int foo()
{
	Foo *f=nil;
	[f zot]; /* There should be no warnings here! */
	return 0;
}


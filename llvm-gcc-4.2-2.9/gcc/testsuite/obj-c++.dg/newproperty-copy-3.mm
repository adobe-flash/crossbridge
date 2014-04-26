/* APPLE LOCAL file radar 4805321 */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#include <Foundation/Foundation.h>

@interface NamedObject : NSObject
@property(copy) NSString *name;
@end

@implementation NamedObject
- (id)init {
	if (self = [super init]) {
		self.name = @"no name";	// no warning or error.
	}
	
	return self;
}

- (void)dealloc {
	self.name = nil;	// no warning or error.
	[super dealloc];
}
@dynamic name;
@end

int main(int argc, char **argv) {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NamedObject *object = [[NamedObject alloc] init];
	NSLog(@"object.name == %@", object.name);
	object.name = @"foo";
	NSLog(@"object.name == %@", object.name);
	[object release];
	
	[pool release];
	
	return 0;
}

/* APPLE LOCAL file radar 4476365 */
/* This that pass-by-referencong workd in obj-c++ */
/* { dg-do run { target *-*-darwin* } } */
/* { dg-options "-framework Foundation" } */
#include <Foundation/Foundation.h>

@interface Test : NSObject 
- (void) process: (int)r3 :(int)r4 :(int)r5 :(int)r6 :(int)r7 :(int)r8 :(int)r9 :(int)r10 :(int &)i;
@end

@implementation Test
- (void) process: (int)r3 :(int)r4 :(int)r5 :(int)r6 :(int)r7 :(int)r8 :(int)r9 :(int)r10 :(int &)i {
	if (i != 10)
	  abort ();
}
@end

@interface Proxy : NSProxy {
	id ob;
}

- (id) initWithObject:(id)_ob;
- (void) dealloc;
@end

@implementation Proxy
- (id) initWithObject:(id)_ob {
	ob = [_ob retain];
	return self;
}

- (void) dealloc {
	[ob release];
	[super dealloc];
}

- (void) forwardInvocation:(NSInvocation *)invocation {
	[invocation invokeWithTarget:ob];
}

- (BOOL) respondsToSelector:(SEL)selector {
	return [ob respondsToSelector:selector];
}

- (NSMethodSignature*) methodSignatureForSelector:(SEL)selector {
	return [ob methodSignatureForSelector:selector];
}
@end

int main() {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	Test *t = [[[Test alloc] init] autorelease];

	id p = [[[Proxy alloc] initWithObject:t] autorelease];
        int i = 10;
        [p process:3:4:5:6:7:8:9:10:i];
	[pool release];
	if (i != 10)
	  abort();
	return 0;
}

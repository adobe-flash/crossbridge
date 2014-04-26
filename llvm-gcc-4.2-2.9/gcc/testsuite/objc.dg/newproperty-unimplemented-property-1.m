/* APPLE LOCAL file radar 4805321 */
/* Test that property need not be declared in @implementation for it to be used. */
/* { dg-options "-fobjc-new-property -mmacosx-version-min=10.5 -framework Cocoa" } */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
#include <Foundation/Foundation.h>

@interface Foo : NSObject
{
@private
  int _userDefined;
  int ibar;
}
@property (assign) int bar;
@property (assign) int userDefined;
@end

@implementation Foo
@synthesize bar=ibar;

- (void)setUserDefined:(int)f
{
  _userDefined = f;
  if (self.userDefined != _userDefined)
    abort ();
}
- (int)userDefined
{
  return _userDefined;
}
@end

@interface MyObserver : NSObject 
@end

@implementation MyObserver

-(void)observeValueForKeyPath:(NSString*)keypath ofObject:obj change:(NSDictionary *)change context:(void *) context
{
#   if (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5 || __OBJC2__)
  printf("observing %s is now %d\n", [keypath UTF8String], ((Foo *)obj).bar);
#else
  printf("observing %s is now %d\n", [keypath cString], ((Foo *)obj).bar);
#endif
}
@end

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	Foo *f = [[Foo alloc] init];
	[f addObserver:[[MyObserver alloc] init] forKeyPath:@"bar" options:NSKeyValueObservingOptionNew context:0];
	f.bar = 707;
	f.bar = 808;
	f.bar = 909;
	f.userDefined = 707;
	f.userDefined = 808;
	f.userDefined = 909;
    [pool release];
    return 0;
}


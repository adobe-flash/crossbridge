/* APPLE LOCAL file radar 4897159 */
/* Test that a redundant user declaration of a setter which is later to be synthesize 
   does not ICE. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property -framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run } */
#include <Foundation/Foundation.h>

@interface MyClass : NSObject {
    NSString	*_myName;
}

@property(copy) NSString *myName;

- (NSString *)myName;
- (void)setMyName:(NSString *)name;

@end

@implementation MyClass

@synthesize myName = _myName;

@end

int main(int argc, const char *argv[], const char *envp[]) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool allocWithZone:NULL] init];
    MyClass *myObject = [[MyClass allocWithZone:NULL] init];

    [myObject setMyName:@"Foo"];

    [pool release];

    exit(EXIT_SUCCESS);
    return 0;
}

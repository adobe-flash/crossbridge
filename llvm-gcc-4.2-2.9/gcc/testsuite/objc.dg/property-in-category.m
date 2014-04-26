/* APPLE LOCAL file radar 5962694 */
/* Test that a property declared in category's protocol list can be
   synthesize (dynamic only) in category's implementation. */
/* { dg-options "-mmacosx-version-min=10.5" { target *-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

#import <Foundation/Foundation.h>

@protocol MyProtocol
@property float                 myFloat;
@end

@interface MyObject : NSObject {
        }
@end

@interface MyObject (MyProtocol) <MyProtocol>
@end

@implementation MyObject (MyProtocol)
@dynamic myFloat;
@end

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    NSLog(@"Hello, World!");
    [pool drain];
    return 0;
}


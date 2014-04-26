/* APPLE LOCAL file radar 5733674 */
/* Test that static GC'able objects are garbage collected by generating write barrier. 
   Test should build and run. */
/* { dg-options "-fobjc-gc -mmacosx-version-min=10.5 -framework Foundation" } */
/* { dg-do run { target *-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

#import <Foundation/Foundation.h>

@interface Test : NSObject
{
}
@end
@implementation Test

static NSString *createMacOSXVersionString()
{
    return [[NSString alloc] initWithString:[[NSDate date] description]];
}

- (void)getString
{
    static NSString *string = createMacOSXVersionString();
    NSLog(@"string: %@", string);
    NSLog(@"string: %p", string);
}

@end



int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    Test *testClass = [[Test alloc] init];
    unsigned counter, max = 4;
    for (counter = 0; counter < max; counter++) {
        [testClass getString];
        [[NSGarbageCollector defaultCollector] collectExhaustively];
    }
    [testClass release];
    
    // insert code here...
    NSLog(@"Hello, World!");
    [pool drain];
    return 0;
}

/** The problem looks like clang getting confused when a single translation unit 
    contains a protocol with a property and two classes that implement that protocol 
    and synthesize the property.
*/

#import <Foundation/Foundation.h>
#include <objc/runtime.h>

@protocol Proto
@property (assign) id prop;
@end

@interface Foo : NSObject <Proto> { id prop; } @end
@interface Bar : NSObject <Proto> { id prop; } @end

@implementation Foo
@synthesize prop;
@end
@implementation Bar
@synthesize prop;
@end

int main() {
    IMP Foo_prop = class_getMethodImplementation([Foo class], @selector(prop));
    IMP Bar_prop = class_getMethodImplementation([Bar class], @selector(prop));
    assert(Foo_prop != Bar_prop);
    return 0;
}


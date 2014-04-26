/* APPLE LOCAL file radar 5595325 */
/* Test for implementation of __attribute__ ((NSObject)) to make a struct pointer
   a first class objective-c pointer to object. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -Werror -framework Foundation" } */

#import <Foundation/Foundation.h>

typedef struct _foo  *__attribute__((NSObject)) Foo_ref;

@interface TestObject : NSObject {
    Foo_ref dict;
}
@property(retain) Foo_ref dict;
@end

@implementation TestObject
@synthesize dict;
@end

int main(int argc, char *argv[]) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSDictionary *dictRef = [NSDictionary dictionaryWithObject:@"Object" forKey:@"key"];
    Foo_ref foo = (Foo_ref)dictRef;
    TestObject *to = [[TestObject alloc] init];

    // do Properties retain?
    int before = [dictRef retainCount];
    to.dict = foo;
    int after = [dictRef retainCount];

    if (after <= before) {
        abort();
    }
    if ([foo retainCount] != [dictRef retainCount]) {
        abort();
    }

    // do Blocks retain?
    {
        void (^block)(void) = ^{
            [foo self];
        };
        before = [foo retainCount];
        id save = [block copy];
        after = [foo retainCount];
        if (after <= before) {
            abort();
        }
    }
    return 0;
}

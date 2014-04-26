/* APPLE LOCAL radar 5847976 */
/* Test __weak attribute on __block objects. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -ObjC -fobjc-gc -framework Foundation" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>

// provide our own version for testing

int GotCalled = 0;

void objc_assign_weak(id value, id * location) {
    ++GotCalled;
    *location = value;
}

int recovered = 0;

@interface TestObject : NSObject {
}
@end

@implementation TestObject
- (id)retain {
    printf("Whoops, retain called!\n");
    exit(1);
}
- (void)finalize {
    ++recovered;
    [super finalize];
}
- (void)dealloc {
    ++recovered;
    [super dealloc];
}
@end


void testRR() {
    // create test object
    TestObject *to = [[TestObject alloc] init];
    __block TestObject *__weak  testObject = to;    // initialization does NOT require support function
    
    // there could be a Block that references "testObject" and that block could have been copied to the
    // heap and the Block_byref forwarding pointer aims at the heap object.
    // Assigning to it should trigger, under GC, the objc_assign_weak call
    testObject = [NSObject new];    // won't last long :-)
}

int main(int argc, char *argv[]) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    GotCalled = 0;
    testRR();
    if (GotCalled == 0) {
        printf("didn't call out to support function on assignment\n");
        return 1;
    }
    printf("%s: Success\n", argv[0]);
    return 0;
}


/* APPLE LOCAL file 5782740 - blocks */
/* Test generation of copy/destroy helper function. */
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC -fblocks" { target *-*-darwin* } } */

#import <Foundation/Foundation.h>

@interface TestObject : NSObject {
@public
    int refcount;
    id aSlot;
}
- (int) testVerbosely:(int)verbose;

@end

@implementation TestObject

- (id) retain {
    ++refcount;
    return self;
}

- (int)retainCounter {
    return refcount + 1;
}

- (void) release {
   if (refcount == 0) [self dealloc];
   else --refcount;
}

- (int) testVerbosely:(int)verbose
 {
    int errors = 0;
    aSlot = [[NSObject alloc] init];

    int initialRetainCounter = [self retainCounter];

    void (^myBlock)(void) = ^{
        printf("[aSlot retainCount] == %d\n", (int)[aSlot retainCount]);
    };

    int afterBlockRetainCounter = [self retainCounter];

    void (^myBlockCopy)(void) = Block_copy(myBlock);

    int afterBlockCopyRetainCounter = [self retainCounter];

    if (afterBlockRetainCounter > initialRetainCounter) {
        printf("testVerbosely: after block, retain count is %d vs before %d\n", afterBlockRetainCounter, initialRetainCounter);
        ++errors;
    }

    if (afterBlockCopyRetainCounter <= afterBlockRetainCounter) {
        printf("testVerbosely: block copy did not retain interior object\n");
        ++errors;
    }

    if (errors == 0 && verbose) printf("testVerbosely: objc import object test success\n");
    return errors;

}
@end
/* { dg-final { scan-assembler "___copy_helper_block_" } } */
/* { dg-final { scan-assembler "___destroy_helper_block_" } } */


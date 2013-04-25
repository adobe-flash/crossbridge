/* APPLE LOCAL file radar 5811191 */
/* Test that all 'ivars' are treated as implicit byref. Its inclusing inside |...|
   block is treated with a warning and ignored.
*/
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-ObjC++ -mmacosx-version-min=10.5 -fobjc-gc -framework Foundation" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>

void * _NSConcreteStackBlock[32];

enum {
    BLOCK_NEEDS_FREE =        (1 << 24),
    BLOCK_HAS_COPY_DISPOSE =  (1 << 25),
    BLOCK_NO_COPY =           (1 << 26), // interim byref: no copies allowed
    BLOCK_IS_GC =             (1 << 27),
};

struct Block_basic {
    void *isa;
    //char flags; char size; short refcount;
    int Block_flags;  // int32_t
    int Block_size; // XXX should be packed into Block_flags
    void (*Block_invoke)(void *);
    void (*Block_copy)(void *dst, void *src);
    void (*Block_dispose)(void *);
    //long params[0];  // generic space for const & byref hidden params, return value, variable on needs of course
};

@interface TestObject : NSObject {
@public
    id aSlot;
}
- (int) testVerbosely:(int)verbose;


@end

@implementation TestObject


- (int) testVerbosely:(int)verbose
 {
    aSlot = [[NSObject alloc] init];


    void (^myBlock)(void) = ^{
        printf("[aSlot retainCount] == %d\n", (int)[aSlot retainCount]);
    };
    struct Block_basic *basic = (struct Block_basic *)(void *)myBlock;
    if (basic->Block_flags & BLOCK_NO_COPY)
      abort ();

   return 0;
}


@end

int main(int argc, char *argv[]) {
    TestObject *to = [[TestObject alloc] init];

    [to testVerbosely:1];
    return 0;
}


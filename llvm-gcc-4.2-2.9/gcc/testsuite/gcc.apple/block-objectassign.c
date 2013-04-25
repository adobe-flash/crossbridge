/* APPLE LOCAL file radar 6175959 */
/* Test for generation of two new APIs; _Block_object_assign and _Block_object_dispose */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -fblocks" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>

#define    BLOCK_HAS_COPY_DISPOSE  (1 << 25)

struct Block_descriptor {
    unsigned long int reserved;
    unsigned long int size;
    void (*copy)(void *dst, void *src);
    void (*dispose)(void *);
};

struct Block_layout {
    void *isa;
    int flags;
    int reserved;
    void (*invoke)(void *, ...);
    struct Block_descriptor *descriptor;
    // imported variables
};

int AssignCalled = 0;
int DisposeCalled = 0;

// local copy instead of libSystem.B.dylib copy
void _Block_object_assign(void *destAddr, const void *object, const int isWeak) {
    printf("_Block_object_assign(%p, %p, %d) called\n", destAddr, object, isWeak);
    AssignCalled = 1;
}

void _Block_object_dispose(const void *object, const int isWeak) {
    printf("_Block_object_dispose(%p, %d) called\n", object, isWeak);
    DisposeCalled = 1;
}

struct MyStruct {
    long isa;
    long field;
};

typedef struct MyStruct *__attribute__((NSObject)) MyStruct_t;

int main(int argc, char *argv[]) {
    // create a block
    struct MyStruct X;
    MyStruct_t xp = (MyStruct_t)&X;
    xp->field = 10;
    void (^myBlock)(void) = ^{ printf("field is %ld\n", xp->field); };
    // should be a copy helper generated with a calls to above routines
    // Lets find out!
    struct Block_layout *bl = (struct Block_layout *)(void *)myBlock;
    if ((bl->flags & BLOCK_HAS_COPY_DISPOSE) != BLOCK_HAS_COPY_DISPOSE) {
        printf("no copy dispose!!!!\n");
        return 1;
    }
    // call helper routines directly.  These will, in turn, we hope, call the stubs above
    long destBuffer[256];
    printf("destbuffer is at %p, block at %p\n", destBuffer, (void *)bl);
    bl->descriptor->copy(destBuffer, bl);
    bl->descriptor->dispose(bl);
    if (AssignCalled == 0) {
        printf("did not call assign helper!\n");
        return 1;
    }
    if (DisposeCalled == 0) {
        printf("did not call dispose helper\n");
        return 1;
    }
    return 0;
}

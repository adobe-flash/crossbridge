/* APPLE LOCAL file radar 5732232 - blocks */
/* Test that mixup of a pointer type and a block pointer type does not cause program to crash. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

void * _NSConcreteStackBlock[32];
#ifndef _BLOCK_PRIVATE_H_
#define _BLOCK_PRIVATE_H_

enum {
    BLOCK_NEEDS_FREE =        (1 << 24),
    BLOCK_HAS_COPY_DISPOSE =  (1 << 25),
    BLOCK_NO_COPY =           (1 << 26), // interim byref: no copies allowed
    BLOCK_IS_GC =             (1 << 27),
};

struct Block_basic {
    long reserved;
    //char flags; char size; short refcount;
    int Block_flags;  // int32_t
    int Block_size; // XXX should be packed into Block_flags
    void (*Block_invoke)(void *);
    void (*Block_copy)(void *dst, void *src);
    void (*Block_dispose)(void *);
};

struct Block_byref {
    long reserved;
    struct Block_byref *forwarding;
    int refcount;
    int size;
    void (*byref_keep)(struct Block_byref *dst, struct Block_byref *src);
    void (*byref_destroy)(struct Block_byref *);
};

/* runtime entry for destroying shared data blocks */
void Block_destroy_byref(struct Block_byref *shared_struct);
/* runtime entry for sharing shared data blocks */
struct Block_byref *Block_share_byref(struct Block_byref *shared_struct);

/* runtime entry to get total size of a block */
int Block_size(struct Block_basic *aBlock);

// set the allocator/deallocator pair (used by ObjC to establish GC)
void _Block_set_allocator_pair(void *(*alloc)(const unsigned long), void (*dealloc)(const void *));

#endif

int GlobalInt;
void setGlobalInt(int value) { GlobalInt = value; }
int getGlobalInt() { int tmp = GlobalInt; GlobalInt = 0; return tmp; }


//
// Example block code generated for specific Blocks
//

//
// Partially bound block referencing const and byref args
//

/* Inclusion of this block block causes program to crash, even though it does not envoke it */
#if __BLOCKS__
int parameters_example_real(int verbose) {
    int desiredValue = 100;
    void (^myBlock)(int);
    myBlock = ^ (int param) {
        setGlobalInt(param);
    };
    myBlock(desiredValue);
    int globalValue = getGlobalInt();
   // if (error_found("parameters_real", globalValue, desiredValue, verbose)) return 1;
    return 0;
}

#endif __BLOCKS__

struct parameters_example_struct {
    struct Block_basic base;
};

// the "thunks" compiled for the invoke entry point of the parameters_example

void invoke_parameters_example(struct parameters_example_struct *aBlock, int param) {
  {
    setGlobalInt(param);
  }
}


// The rewritten version of the code above

int parameters_example(int verbose) {
    int desiredValue = 100;
    struct parameters_example_struct literal = {
        { 0, 0, sizeof(struct parameters_example_struct),
            (void (*)(void *))invoke_parameters_example,
        },
    };
    struct parameters_example_struct *myBlock = &literal;

    // get a type correct function pointer for the invocation function
    void (*correct)(struct parameters_example_struct *, int);
    correct = (void (*)(struct parameters_example_struct *, int))myBlock->base.Block_invoke;
    // call the block with itself as first arg and the parameter 100
    correct(myBlock, desiredValue);

    return 0;

}

int main(int argc, char *argv[]) { parameters_example(1); return 0; }


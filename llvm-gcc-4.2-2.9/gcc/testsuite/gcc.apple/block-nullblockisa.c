/* APPLE LOCAL file radar 6244520 */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -fblocks" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>
#include <stdlib.h>

void check(void *ptr) {
    struct inner {
        void* isa; // should be zero
        long forwarding;
        int flags;
        int size;
        // long copyhelper not needed
        // long disposehelper not needed
        int i;  //
    };
/* APPLE LOCAL begin radar 5847213 - radar 6329245 */
    struct block_with_blocki {
    void *isa; // initialized to &_NSConcreteStackBlock or &_NSConcreteGlobalBlock
    int Block_flags;
    int reserved;
    void *FuncPtr;
    struct Block_descriptor_1 {
        unsigned long int reserved;     // NULL
        unsigned long int size;  // sizeof(struct Block_literal_1)

        // optional helper functions
        void (*Block_copy)(void *dst, void *src);
        void (*Block_dispose)(void *src);
    } *descriptor;
    struct inner *blocki;
    } *block = (struct block_with_blocki *)ptr;
/* APPLE LOCAL end radar 5847213 - radar 6329245 */

    // sanity checks
    if (block->descriptor->size != sizeof(struct block_with_blocki)) {
        // layout funny
        printf("layout is funny, struct size is %d vs runtime size %ld\n",
                    (int)block->descriptor->size,
                    sizeof(struct block_with_blocki));
        exit(1);
    }
    if (block->blocki->size != sizeof(struct inner)) {
        printf("inner is funny; sizeof is %ld, runtime says %d\n", sizeof(struct inner),
            block->blocki->size);
        exit(1);
    }
    if (block->blocki->isa != (void*)NULL) {
        printf("not a NULL __block isa\n");
        exit(1);
    }
    return;
}
        
int main(int argc, char *argv[]) {

   __block int i;
   
   check(^{ printf("%d\n", ++i); });
   return 0;
}
   

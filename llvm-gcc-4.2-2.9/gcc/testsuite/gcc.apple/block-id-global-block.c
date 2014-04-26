/* APPLE LOCAL file radar 5822844 - global blocks */
/* Test that a block pointer can be converted back-and-forth to 'id' in
   both initializsation and assignment without requiring a cast. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 -ObjC" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>

void * _NSConcreteGlobalBlock;

typedef void (^blockATYPE)(void);

id myBlockFunc (void (^blockA)(void))
{
  return blockA;
}

blockATYPE MyBlockIdFunc (id id_arg)
{
  return id_arg;
}

void (^blockA)(void) = ^ { printf("hello\n"); };

int main(char *argc, char *argv[]) {
    // an 'id' can be initialized wit a block.
    id aBlock = blockA;

    // a block should be assignable to an id
    aBlock = blockA;

    // an 'id' should be assignable to a block
    blockA = aBlock;

    blockA = myBlockFunc (^ { printf("hello\n"); });

    aBlock = MyBlockIdFunc (aBlock);

    return 0;
}

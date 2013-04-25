/* APPLE LOCAL file 6386976  */
/* Compilation check for APPLE DWARF runtime class markers. */
/* { dg-options "-g -dA -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-g -dA" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-final { scan-assembler "DW_AT_APPLE_major_runtime_vers" } } */
/* { dg-final { scan-assembler "DW_AT_APPLE_runtime_class" } } */
#include <Foundation/Foundation.h>

@interface Block : NSObject {
@public
    unsigned _address;
    unsigned _age;
}
@end
@implementation Block
@end

extern "C" void abort ();

#define ITER 10000
#define BREAK_ITER 5000
#define MOD 100

int countByIterating(id collection, NSSet *set) {
    unsigned objectCount = 0;
    for (Block *block in collection) {
        ++objectCount;
    }
    return objectCount;
}

int countByIteratingAndBreak(id collection, NSSet *set) {
    unsigned objectCount = 0;
    for (Block *block in collection) {
        ++objectCount;
	if (objectCount == BREAK_ITER)
	  break;
    }
    return objectCount;
}

static int res = 0;
int countByIteratingAndMore(id collection, NSSet *set) {
    unsigned objectCount = 0;
    for (Block *block in collection) {
        ++objectCount;
	if (objectCount % MOD == 0)
	  res++;
	else
	  continue;
    }
    return objectCount;
}

int main (int argc, const char * argv[]) {
    NSAutoreleasePool *pool = [NSAutoreleasePool new];

    NSMutableArray *array = [NSMutableArray array];
    unsigned i;
    for (i = 0; i < ITER; ++i) {
        Block* block = [[Block new] autorelease];
        block->_address  = i;
        [array addObject:block];
    }
    NSSet *set = [NSSet setWithArray:array];
    if (countByIterating(set, set) != ITER)
      abort (); 
    if (countByIterating(array, set) != ITER)
      abort ();
    
    if (countByIteratingAndBreak(set, set) != BREAK_ITER)
      abort (); 
    if (countByIteratingAndBreak(array, set) != BREAK_ITER)
      abort ();
    if (countByIteratingAndMore(set, set) != ITER)
      abort (); 
    if (countByIteratingAndMore(array, set) != ITER)
      abort ();
    if (res != 200)
     abort ();
    [pool drain];
    return 0;
}

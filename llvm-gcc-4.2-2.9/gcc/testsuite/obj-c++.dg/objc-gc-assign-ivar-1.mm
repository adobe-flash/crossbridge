/* APPLE LOCAL file radar 4982951 */
/* This routine checks that both the destination address and offset passed to
   objc_assign_ivar in objc-gc abi is correct for both objc1 and objc2 (cause of the bug)
   ABIs. */
/* { dg-options "-framework Foundation -fobjc-gc" } */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

#define objc_assign_strongCast X_objc_assign_strongCast 
#define objc_assign_global X_objc_assign_global 
#define objc_assign_ivar X_objc_assign_ivar 

#import <Foundation/Foundation.h>
#undef objc_assign_strongCast
#undef objc_assign_global
#undef objc_assign_ivar

void * WantedVal, *WantedDest;
ptrdiff_t WantedOffset;

static
id objc_assign_strongCast(id value, id *dest) {
    /* APPLE LOCAL default to Wformat-security 5764921 */
     printf("assign_ivar got value %p, dest %p\n", (void*)value, (void*)dest);
     printf("wanted                %p,      %p\n", WantedVal, WantedDest);
     return nil;
}
static
id objc_assign_global(id value, id *dest) {
     /* APPLE LOCAL default to Wformat-security 5764921 */
     printf("assign_ivar got value %p, dest %p\n", (void*)value, (void*)dest);
     printf("wanted                %p,      %p\n", WantedVal, WantedDest);
     return nil;
}

static
id objc_assign_ivar(id value, id dest, ptrdiff_t offset) {
     /* APPLE LOCAL default to Wformat-security 5764921 */
     printf("assign_ivar got value %p, dest %p, offset %d\n", (void*)value, (void*)dest, (int)offset);
     /* APPLE LOCAL default to Wformat-security 5764921 */
     printf("wanted                %p,      %p,        %d\n", WantedVal, WantedDest, (int)WantedOffset);
     if (value != WantedVal)
	abort ();
     if (dest != WantedDest)
	abort ();
     if (offset != WantedOffset)
	abort ();
     return nil;
}

@interface Frump : NSObject {
@public
    id slot;
    id slot2;
    struct S {
      id slot3;
      id slot4;
      id ar_slot2 [32];
    } st_slot;
    id ar_slot1 [32];

   struct S  ar_st_slot [32];
}
@end

@implementation Frump
@end

id Global;

void offsetTest() {
    struct {
        Class isa;
        id slot;
        id slot2;
    struct S {
      id slot3;
      id slot4;
    } st_slot;
    id ar_slot1 [32];

    struct S  ar_st_slot [32];
    } FakeFrump;
    Frump *frump = (Frump *)&FakeFrump;
    WantedVal = (void *)@"string";
    WantedDest = (void *)frump;
    
    printf("testing slot\n");
    WantedOffset = ((char *)&(frump->slot)) - (char *)frump;
    
    frump->slot = @"string";
    
    printf("\ntesting slot2\n");
    WantedOffset = ((char *)&(frump->slot2)) - (char *)frump;
    frump->slot2 = @"string";

   printf ("\ntesting st_slot.slot3\n");
   WantedOffset = ((char *)&(frump->st_slot.slot3)) - (char *)frump;
   frump->st_slot.slot3 = @"string";
    
   printf ("\ntesting st_slot.slot4\n");
   WantedOffset = ((char *)&(frump->st_slot.slot4)) - (char *)frump;
   frump->st_slot.slot4 = @"string";

   printf ("\ntesting st_slot.ar_slot2[0]\n");
   WantedOffset = ((char *)&(frump->st_slot.ar_slot2[0])) - (char *)frump;
   frump->st_slot.ar_slot2[0] = @"string";

   printf ("\ntesting st_slot.ar_slot2[31]\n");
   WantedOffset = ((char *)&(frump->st_slot.ar_slot2[31])) - (char *)frump;
   frump->st_slot.ar_slot2[31] = @"string";

   printf ("\ntesting st_slot.ar_slot1[0]\n");
   WantedOffset = ((char *)&(frump->ar_slot1[0])) - (char *)frump;
   frump->ar_slot1[0] = @"string";

   printf ("\ntesting st_slot.ar_slot1[10]\n");
   WantedOffset = ((char *)&(frump->ar_slot1[10])) - (char *)frump;
   frump->ar_slot1[10] = @"string";
    
   printf ("\ntesting ar_st_slot[10].slot3\n");
   WantedOffset = ((char *)&(frump->ar_st_slot[10].slot3)) - (char *)frump;
   frump->ar_st_slot[10].slot3 = @"string";

   printf ("\ntesting ar_st_slot[10].slot4\n");
   WantedOffset = ((char *)&(frump->ar_st_slot[10].slot4)) - (char *)frump;
   frump->ar_st_slot[10].slot4 = @"string";
}

int main(int argc, char *argv[]) {
     offsetTest();
     return 0;
}

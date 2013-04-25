/* APPLE LOCAL begin radar 4187164 */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

#include <stdio.h>
extern void abort();

int myGlobal = 12;
            
int GetMyGlobal(void) __attribute((noinline));
asm int GetMyGlobal(void)
{
                nofralloc
                
                mflr        r0
// save the old LR
                bcl        20, 31, @1
// get the address of @1 into the LR
            @1:     mflr        r2
// move the address of @1 from the LR to r2
//    picbase     r2, @1
// define r2 and @1 as the base addr for accessing globals
                mtlr        r0
// restore the old LR
                
                addis        r3, r2, ha16(myGlobal-@1)
// compute hi-16 bits of addr of myGlobal and shift
                la        r3, lo16(myGlobal-@1)(r3)
// add lo-16 bits of addr and add to existing hi-16
                lwz        r3, 0(r3)
// dereference the addr to get the value in myGlobal
                blr
// and return that value
}

int main()
{
	if (GetMyGlobal() != 12)
	  abort();

	myGlobal = 123;
	if (GetMyGlobal() != 123)
	  abort();

	return 0;
}
/* APPLE LOCAL end radar 4187164 */

/* APPLE LOCAL begin radar 4187164 */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* This testcase isn't 64-bit safe.  */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-fasm-blocks" } */

#include <stdio.h>
extern void abort();

asm int FibonacciAsm(register int n)
{
    // prolog.  Save return addr and three scratch registers
    nofralloc
    mflr        r0
    stw        r31, -4(sp)
    stw        r30, -8(sp)
    stw        r29, -12(sp)
    stw        r0, 8(sp)
    stwu        sp, -80(sp)

    // body of function
    
    // is n <= 1?  if so, n is our return value.  f(0) = 0, f(1) = 1, f(2) starts the actual math
    cmpwi        cr7, n, 1        
    ble        cr7,LDone
    
    /// ok, n > 2.  save n, compute f(n-1) and f(n-2), then add them together and we're done
    mr        r31, n
    
    subi        r3, r31, 1
    bl        FibonacciAsm
    mr        r30, r3
    
    subi        r3, r31, 2
    bl        FibonacciAsm
    mr        r29, r3
    
    add        r3, r29, r30
LDone:
    // epilog.  Restore scratch registers and return to caller
    addi        sp, sp, 80
    lwz        r0, 8(sp)
    lwz        r29, -12(sp)
    lwz        r30, -8(sp)
    lwz        r31, -4(sp)
    mtlr        r0
    blr
}

int main(int argc, char* argv[])
{
    int x;
    
    x = FibonacciAsm(12);
    if (x != 144)
      abort();
    return 0;
}
/* APPLE LOCAL end radar 4187164 */

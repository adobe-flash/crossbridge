/* APPLE LOCAL file CW asm blocks */
/* Test macros with . in the opcode.  */

/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

#define foodef() rlwinm. r4,r4,31,16,31

asm void foo (void)
{
      foodef()
}

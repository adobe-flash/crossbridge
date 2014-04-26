/* APPLE LOCAL file 4443946 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-fasm-blocks -O2" } */
/* The compiler used to flag any write to %ebp as an error.  */
t2 ()
{
  __asm {
    movd    mm4,            ebp
    movd    ebp,            mm3
    movntq  [edi][ebp],     mm0
    movd    ebp,            mm4
  }
}


/* APPLE LOCAL file CW asm blocks */
/* Test la16(TABLE), lo16(TABLE), and bdnz+.  */

/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */

double  TABLE[4]={2.0, 1./2., 3., 1.};

asm void SQRT()
{
      lis         r3,ha16(TABLE)
      addi        r3,r3,lo16(TABLE)
@cycle:
      fmul        fp6,fp1,fp1
      bdnz+       @cycle
      blr
}

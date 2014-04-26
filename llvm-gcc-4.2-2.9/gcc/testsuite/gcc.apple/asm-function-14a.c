/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */

asm void foo()
{
  dcbst  r0,r5				/* { dg-error "r0 not allowed for parameter 1" } */
}

/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */

asm void foo()
{
  li     r1, kUndefindedConstant	/* { dg-error "kUndefindedConstant.* used but not defined" } */
}

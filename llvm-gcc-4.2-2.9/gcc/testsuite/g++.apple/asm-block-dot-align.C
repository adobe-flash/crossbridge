/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */
/* Radar 4205577 */

asm void Dummy()
{
  .align 4
Function1:
  li r3, 1
  li r4, 2
  li r5, 3
  li r6, 4
  li r7, 5
  b Function2

  .align 4
Function2:
  add r8, r3, r4
  add r8, r8, r5
}

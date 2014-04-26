/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Shouldn't be necessary, but it doesn't work yet.  */
/* { dg-require-effective-target ilp32 } */
/* Radar 4300108 */

void foo() {
  asm L1:  mov edx, OFFSET L1
  asm L2:  mov edx, [OFFSET L2][edx]
}

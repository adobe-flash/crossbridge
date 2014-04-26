/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4399388 */

void foo() {
  asm movd [edi-4][edx], mm3

  asm movd mm1, [esi + 4 - 1];
  asm movd mm2, [esi + 4 + 2];
}

/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4363798 */

void foo() {
  asm {
    movd [edi-8+4], mm1
    movq [eax+32+24], mm3
  }
}

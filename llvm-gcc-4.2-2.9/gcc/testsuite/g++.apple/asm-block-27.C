/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */

void foo() {
  asm {
    /* Radar 4300186 */
    movd mm7, [esi][eax][4]
    movd mm3, [edi][eax][4]
    movd mm1, [edx][eax][4]

    /* Radar 4300207 */
    movd mm7, [esi][eax+4]
    movd mm3, [edi][4+eax]
    movd mm7, [esi+4][eax]
    movd mm3, [4+edi][eax]
  }
}

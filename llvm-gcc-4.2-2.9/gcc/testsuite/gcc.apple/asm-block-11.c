/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4211971 */

void foo() {
  const int Aligned64data_last_mult = 4;
  asm {
    movd mm1, [-1][eax][esi]	// middle row, misaligned to prevent GP
    movd mm0, [-1][eax]		// top row, misaligned to prevent GP
    movd mm2, [-1][eax][esi*2]	// bottom row, misaligned to prevent GP
    movq [edi][Aligned64data_last_mult], mm0
    lea  eax, [esi][ecx*2]	// ptr to 2 rows down in sPtr
    movd mm4, [esi]
    movd mm5, [esi][ecx]
    movd [-4][edi], mm3		// store 4 results
  }
}

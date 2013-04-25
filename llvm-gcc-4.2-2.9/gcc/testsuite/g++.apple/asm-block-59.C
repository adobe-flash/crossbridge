/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 -O2 } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-final { scan-assembler "-\(40|44\)\\\(%ebp,%edx,2\\\), %ebx" } } */
/* Radar 4699545 */

void bar() {
  short gaussianWeights[8];
  asm {    	
    movzx	ebx, word ptr gaussianWeights[edx+edx]
  }
}

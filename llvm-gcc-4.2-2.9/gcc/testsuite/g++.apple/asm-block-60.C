/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 -O2 } } */
/* { dg-final { scan-assembler "movsl \n" } } */
/* Radar 4736998 */

void bar() {
  asm movs dword ptr[edi], dword ptr [esi]
}

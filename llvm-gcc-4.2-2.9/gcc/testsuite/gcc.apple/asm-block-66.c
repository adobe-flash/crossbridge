/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-options { -fasm-blocks } } */
/* { dg-final { scan-assembler "filds" } } */
/* { dg-final { scan-assembler "fstps" } } */
/* Radar 4427695 */

void bar() {
  asm fild WORD PTR [esi]
  asm fstp DWORD PTR [edi+12]
}

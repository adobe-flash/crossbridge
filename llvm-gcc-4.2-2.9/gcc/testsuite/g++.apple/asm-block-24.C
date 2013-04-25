/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4300095 */

char color;
short source;

void foo() {
  asm {
    movzx   eax, BYTE PTR source
    movzx   eax, BYTE PTR color
    mov     ax, WORD PTR source
    mov     ax, WORD PTR color
  }
}

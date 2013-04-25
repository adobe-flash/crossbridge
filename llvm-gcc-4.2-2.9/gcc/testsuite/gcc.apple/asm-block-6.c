/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { "-fasm-blocks" } } */
/* Radar 4211978 */

void foo() {
  __asm {
    push ebx
    mov  ebx, 0x07
    pop ebx
  }
}

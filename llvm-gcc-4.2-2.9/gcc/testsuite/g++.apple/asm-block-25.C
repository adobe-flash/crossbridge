/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4309942 */

int myvar;

void foo() {
  asm {
    mov eax,0x00[edx]

    mov eax, offset foo
    mov eax, offset myvar
    mov eax, &foo
    mov eax, &myvar
  }
}

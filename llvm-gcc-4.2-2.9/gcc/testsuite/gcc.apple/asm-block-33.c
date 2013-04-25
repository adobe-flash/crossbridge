/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4300193 */

int i, j, k;
int rows;

void foo() {
  int r;
  for (r = 0; r < rows; r++) {
  }
  asm {
    mov ah, 1
    push eax
    mov al, 1
    push eax
    mov esi, eax
    mov ebx, i
    mov edi, j
    mov edx, ecx
    push esi
    push ebx
    push edi
    push edx
  }
}

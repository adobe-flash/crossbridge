/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks } } */
/* Radar 4739936 */

int i = 9h;
int j = 1st;			/* { dg-error "invalid suffix on integer constant" } */

void foo() {
  asm {
    mov eax, 1st		/* { dg-error "invalid suffix on integer constant" } */
    mov eax, 1h
    ; foo 1st
    ; bye bye
  }
}

/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4419735 */

void foo() {
  asm fisubr DWORD PTR[WORD PTR esi]	 /* { dg-error "too many types in []" } */
}

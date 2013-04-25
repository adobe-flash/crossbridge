/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks } } */
/* Radar 5474682 */

void foo() {
   asm { jmp C }
   asm { C: nop }
}

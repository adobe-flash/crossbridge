/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options { -fasm-blocks } } */
/* Radar 5474682 */

void foo() {
   asm { jmp C }
   asm { C: nop }
}

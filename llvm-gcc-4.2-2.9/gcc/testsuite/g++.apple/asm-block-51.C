/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks } } */
/* Radar 4505697 */

void foo() {
   asm movzx eax, [edi+8]
}

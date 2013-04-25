/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* Radar 4443942 */

void foo() {
  asm movntq qword ptr [edi+ecx+0], mm0
  asm movq qword ptr [edi+ecx+0], mm0
}

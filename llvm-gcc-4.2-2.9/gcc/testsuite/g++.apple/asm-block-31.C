/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 -mdynamic-no-pic } } */
/* Radar 4319881 */

void foo() {
  static const int aMoveMasks[4] = { 0 };
  asm {
    movdqa xmm6, [aMoveMasks+edx*8]
  }
}

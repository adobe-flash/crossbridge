/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 } } */
/* LLVM LOCAL */
/* { dg-final { scan-assembler "movq -\(16|36|32|24\)\\\(%ebp\\\), %mm0" } } */
/* Radar 4515069 */

void foo() {
  const int packedw1[2] = { ((1*0x10000)+1), ((1*0x10000)+1) };

  asm movq mm0, packedw1
}

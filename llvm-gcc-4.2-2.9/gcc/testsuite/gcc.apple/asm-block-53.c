/* { dg-do compile { target i?86*-*-darwin* x86_64*-*-darwin*} } */
/* { dg-options { -fasm-blocks } } */
/* { dg-final { scan-assembler "0\\\(%eax,%eax,2\\\), %edx" } } */
/* Radar 4560301 */

void foo() {
  asm lea edx, [eax + eax * 2]
}

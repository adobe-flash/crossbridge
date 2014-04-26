/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options { -fasm-blocks -msse3 -O2 } } */
/* Radar 4248228 */

asm inline void foo() {
  a: nop
  b: nop
  jmp a
}

void bar() {
  foo();
  foo();
}

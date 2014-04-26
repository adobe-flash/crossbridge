/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { "-fasm-blocks" } } */
/* Radar 4211954 */

void foo() {
  asm {
    and ebx, 0fffffffCh// zero low two bits
  }
}

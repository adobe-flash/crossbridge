/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64*-*-darwin* } } */
/* { dg-options { -fasm-blocks -msse3 -O } } */
/* Radar 4300156 */

int i;

void bar (short source) {
   i = 4;
}

void foo (short source) {
  bar (source);
  asm {
    mov ax, source
  }
}

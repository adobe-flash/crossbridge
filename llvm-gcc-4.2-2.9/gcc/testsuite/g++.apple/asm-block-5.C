/* APPLE LOCAL file CW asm blocks */
/* Test -40(sp) in asm-syntax blocks within functions.  */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

void foo() {
  asm {
    stfd f9,-40(sp);
  }
}

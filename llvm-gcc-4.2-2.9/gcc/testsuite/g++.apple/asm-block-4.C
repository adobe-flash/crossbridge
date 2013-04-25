/* APPLE LOCAL file CW asm blocks */
/* Test C++ keywords that are binary operators in asm-syntax blocks within functions.  */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

int baz (int arg1) {
  asm {
    mr  r2, arg1
    add r3, r2, r2
    and r3, arg1, r3
  }
}

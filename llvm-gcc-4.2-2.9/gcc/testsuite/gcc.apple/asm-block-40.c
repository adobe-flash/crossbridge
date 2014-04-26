/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */
/* Radar 4407059 */

inline void foo(int offset) {
  asm {
    add r3,r3,offset
  }
}

/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */
/* Radar 4257049 */

typedef unsigned long testAsmFuncType;

extern testAsmFuncType testAsmFunction();

asm testAsmFuncType

testAsmFunction() {
  nofralloc
  li      r3, 1
  blr
}

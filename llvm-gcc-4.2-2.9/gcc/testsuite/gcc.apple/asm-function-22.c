/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */
/* Radar 4501833 */

asm void myasm1(register int i) {
  nofralloc
  cmpi 7, 0, i, 0
  mr r29, i
  blr
}

asm void myasm2(register int i) {
  fralloc
  cmpi 7, 0, i, 0
  mr r29, i
  frfree
  blr
}

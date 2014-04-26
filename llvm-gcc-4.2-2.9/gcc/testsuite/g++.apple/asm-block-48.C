/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */
/* Radar 4456673 */

asm void Foo() {
  stwu r1, -64(r1)
}

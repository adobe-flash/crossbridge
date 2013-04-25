/* APPLE LOCAL file CW asm blocks */
/* Test lo16(foo)(v) and ha16(foo).  */

/* { dg-do assemble { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */

int foo;

asm int fun1 ()
{
  int v;
  addis  v,0,ha16(foo)
  lwz    r3,lo16(foo)(v)
}

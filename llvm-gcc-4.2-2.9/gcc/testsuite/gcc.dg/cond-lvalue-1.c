/* Test for deprecation of conditional expressions as lvalues.  */
/* Origin: Joseph Myers <jsm@polyomino.org.uk> */
/* { dg-do compile } */
/* APPLE LOCAL non lvalue assign */
/* { dg-options "-fno-non-lvalue-assign" } */

int x, y, z;

void
foo (void)
{
  (x ? y : z) = 1; /* { dg-bogus "warning" "warning in place of error" } */
}
/* APPLE LOCAL non lvalue assign */
/* { dg-error "lvalue" "conditional expression as lvalue" { target *-*-* } 12 } */

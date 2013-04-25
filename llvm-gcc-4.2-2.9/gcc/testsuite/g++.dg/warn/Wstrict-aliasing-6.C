/* { dg-do compile } */
/* APPLE LOCAL begin radar 4872051 */
/* { dg-options "-Wstrict-aliasing=2 -O2 -fstrict-aliasing" } */
/* APPLE LOCAL end radar 4872051 */

int foo ()
{
  char buf[8];
  return *((int *)buf); /* { dg-warning "strict-aliasing" } */
}


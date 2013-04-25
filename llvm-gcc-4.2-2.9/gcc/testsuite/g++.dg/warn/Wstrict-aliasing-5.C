/* { dg-do compile } */
/* APPLE LOCAL begin radar 4872051 */
/* { dg-options "-Wstrict-aliasing=2 -O2 -fstrict-aliasing" } */
/* APPLE LOCAL end radar 4872051 */

float foo ()
{
  unsigned int MASK = 0x80000000;
  return (float &) MASK; /* { dg-warning "strict-aliasing" } */
}


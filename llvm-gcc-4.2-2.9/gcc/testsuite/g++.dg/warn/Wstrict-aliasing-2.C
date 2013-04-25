/* { dg-do compile } */
/* APPLE LOCAL begin radar 4872051 */
/* { dg-options "-Wstrict-aliasing=2 -O2 -fstrict-aliasing" } */
/* APPLE LOCAL end radar 4872051 */

double x;

template <typename T>
T *foo(void)
{
  return (T *)&x; /* { dg-bogus "strict-aliasing" } */
}

template double *foo<double>(void);


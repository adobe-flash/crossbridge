/* { dg-do compile } */
/* APPLE LOCAL begin radar 4872051 */
/* { dg-options "-Wstrict-aliasing=2 -O2 -fstrict-aliasing" } */
/* APPLE LOCAL end radar 4872051 */

double x;

template <typename T>
T *foo(void)
{
  int a[2];
  float *y = (float *)a; /* { dg-bogus "strict-aliasing" } */
  return (T *)&x; /* { dg-bogus "strict-aliasing" } */
}


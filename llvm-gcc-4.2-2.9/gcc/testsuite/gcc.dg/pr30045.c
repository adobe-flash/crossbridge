/* { dg-do compile } */
/* APPLE LOCAL -fnested-functions */
/* { dg-options "-O2 -fno-inline -fnested-functions" }  */
int f(int *a)
{
  int  __attribute__((nonnull(1))) g(int *b)
  {
    int **c = &a;
    if (b)
      return *a + **c;
    return *b;
  }
  if (a)
    return g(a);
  return 1;
}

/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fnested-functions" } */
void h(void (*)(void));
_Complex int g (void)
{
  _Complex int x;
  void f(void)
  {
     x = x + x;
  }
  h(f);
  return x;
}

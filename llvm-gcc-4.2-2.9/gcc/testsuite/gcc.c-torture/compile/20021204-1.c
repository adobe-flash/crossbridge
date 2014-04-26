/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fnested-functions" } */
/* PR c/7622 */

/* Verify that GCC can handle the mix of
   extern inline and nested functions. */

extern inline int t()
{
  int q() { return 0; }

  return q();
}

int foo()
{
  return t();
}

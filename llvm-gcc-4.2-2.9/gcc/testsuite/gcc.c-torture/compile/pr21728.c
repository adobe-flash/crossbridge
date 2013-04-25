/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fnested-functions" } */
/* LLVM LOCAL no nested functions */
/* { dg-require-fdump "" } */
int main (void)
{
  __label__ l1;
  void __attribute__((used)) q(void)
  {
    goto l1;
  }

  l1:;
}

/* { dg-do compile } */
/* APPLE LOCAL testsuite nested functions */
/* { dg-options "--param ggc-min-expand=0 --param ggc-min-heapsize=0 -fnested-functions" } */
void foo()
{
  void bar()
  {
    bar();
  }
}

void foo1(int i)
{
    void bar (char c[1][i]) { }
}

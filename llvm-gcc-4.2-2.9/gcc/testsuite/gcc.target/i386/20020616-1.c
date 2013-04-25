/* PR opt/6722 */
/* { dg-do run { target i?86-*-* x86_64-*-* } } */
/* APPLE LOCAL 5951276 */
/* LLVM LOCAL I need {"*"} and {""} explicitly for some reason */
/* { dg-skip-if "<rdar://problem/5951276>" { i?86-apple-darwin* } {"*"} {""} } */
/* { dg-options "-O2" } */

register int k asm("%ebx");

void __attribute__((noinline))
foo()
{
  k = 1;
}

void test()
{
  int i;
  for (i = 0; i < 10; i += k)
    {
      k = 0;
      foo();
    }
}

int main()
{
  int old = k;
  test();
  k = old;
  return 0;
}

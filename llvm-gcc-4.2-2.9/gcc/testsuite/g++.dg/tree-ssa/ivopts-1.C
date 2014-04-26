/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-ivopts" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */

struct Foo {
  Foo() : s(1) {}
  int s;
};
void foo(Foo&);
void bar(void)
{
  Foo x[4];
  foo(x[0]);
}

/* { dg-final { scan-tree-dump-not "-&x" "ivopts" } } */
/* APPLE LOCAL begin 5431747 */
/* { dg-final { scan-tree-dump-not "offset: -4B" "ivopts" { xfail hppa*-*-* } } } */
/* { dg-final { scan-tree-dump-not "&x\\\[5\\\]" "ivopts" { xfail hppa*-*-* } } } */
/* APPLE LOCAL end 5431747 */
/* { dg-final { cleanup-tree-dump "ivopts" } } */

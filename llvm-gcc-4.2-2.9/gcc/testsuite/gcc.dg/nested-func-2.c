/* PR middle-end/18820 */
/* Check that we reject nested functions as initializers
   of static variables.  */

/* { dg-do compile } */
/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fnested-functions" } */

struct S {
  void (*f)(int);
};

extern void baz(struct S *);
extern void p(int);

void foo(void)
{
  int u;

  void bar(int val)
    {
      u = val;
    }

  static struct S s = { bar }; /* { dg-error "(is not constant)|(near initialization)" } */

  baz(&s);
  p(u);
}

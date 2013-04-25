/* APPLE LOCAL file ARM 6077274 */
/* { dg-do compile { target arm*-*-darwin* } } */
/* { dg-options "-fvisibility=hidden" } */
/* { dg-final { scan-assembler "_bar\\\$non_lazy_ptr" } } */
void foo (void (*fnptr) (void));

void bar (void) __attribute__((weak));
void bar (void)
{
}

void baz (void)
{
  foo (bar);
}


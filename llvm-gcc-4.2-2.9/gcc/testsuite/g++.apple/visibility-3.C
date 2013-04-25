/* APPLE LOCAL file ARM 6062215 */
/* { dg-do compile { target arm*-*-darwin* } } */
/* { dg-options "-fvisibility-inlines-hidden" } */
/* { dg-final { scan-assembler "__ZN8my_class2fnEv\\\$lazy_ptr" } } */

struct my_class {
  inline void fn (void) {}
};

void foo (void)
{
  my_class mci;
  mci.fn ();
}


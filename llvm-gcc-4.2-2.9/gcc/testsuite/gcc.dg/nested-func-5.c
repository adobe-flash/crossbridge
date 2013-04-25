/* { dg-do compile } */
/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fexceptions -fnested-functions" } */
/* PR28516: ICE generating ARM unwind directives for nested functions.  */

void ex(int (*)(void));
void foo(int i)
{
  int bar(void)
  {
    return i;
  }
  ex(bar);
}

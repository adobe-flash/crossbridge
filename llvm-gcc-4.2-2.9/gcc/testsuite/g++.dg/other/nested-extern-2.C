/* APPLE LOCAL file mainline 2007-05-09 5173149 */
/* { dg-do run } */
// { dg-additional-sources "nested-extern.cc" }
/* PR 31775 */
extern "C" void abort();
static int i;
int *p = &i;
int main()
{ 
  int i;
  { 
    extern int i;
    i = 1;
    *p = 2;
    if (i == 2)
      abort ();
  }
  return 0;
}

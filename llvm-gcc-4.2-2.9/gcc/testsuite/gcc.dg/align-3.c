/* APPLE LOCAL file for-fsf-4_4 3274130 5295549 */
/* { dg-do compile } */
/* { dg-options "-falign-functions=1 -falign-labels=1 -falign-loops=1" } */
/* { dg-final { scan-assembler "align\[ \t]6" } } */

int foo(int * p)
{
  int i = 0;
  int c = 0;
  
 label: __attribute__((aligned(64)))
  c += p[i];
  i++;
  if (i < 100) goto label;
  return c;
}

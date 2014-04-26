/* APPLE LOCAL file for-fsf-4_4 3274130 5295549 */
/* { dg-do compile } */
/* { dg-options "-falign-functions=1 -falign-labels=1 -falign-loops=1" } */
/* { dg-final { scan-assembler "align\[ \t]6" } } */

int foo(int * p)
{
  int i, c;
  
  for __attribute__((aligned(64))) (i = c = 0; i < 100; i++)
    c += p[i];
  return c;
}

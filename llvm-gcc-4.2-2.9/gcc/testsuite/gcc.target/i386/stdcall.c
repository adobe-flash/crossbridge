/* APPLE LOCAL file 4284121 */
/* { dg-do run } */
/* APPLE LOCAL x86_64 */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O2" } */
/* Test stack computations with stdcall attribute.  */
/* Primarily for darwin, although should work elsewhere. */
extern void abort();
int ww = 5;
int __attribute__((__stdcall__)) __attribute__((noinline)) bar(int x)
{
  return ww+x;
}
int y = 5;
int main() {
  volatile int xx = 3;
  int i;
  int z = bar(y);
  for (i=0; i<20; i++)
    {
      ww += 5;
      z += bar(y);
    }
  if (z!=1260)
    abort();
  if (xx != 3)
    abort();
  return 0;
}

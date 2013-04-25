/* APPLE LOCAL file */
/* { dg-do compile } */ 
/* { dg-options "-O2 -fno-tree-dominator-opts -funroll-loops" } */

void foo(void)
{
  int n = 16875;

  while (n)
    {
      if (n&1)
	bar (n);
      n >>= 1;
    }
}

static inline int power (long x, unsigned int n)
{
  long y = n % 2 ? x : 1;

  while (n >>= 1)
    {
      x = x * x;
      if (n % 2)
	y = y * x;
    }

  return y;
}

void test(long x)
{
  bar (power (x, 10));
  bar (power (x, 27));
}

/* All loops should be completely unrolled, so there should be no local labels.  */
/* { dg-final { scan-assembler-not "L\[0-9\]*:" } } */

/* APPLE LOCAL file 4233898 */
/* When enough of 4.1 has been merged that gcc.dg/tree-ssa/loop-11.c
   passes, this test can be removed. */
void abort (void);

int main (void)
{
  int j, a, b;
  
  for (j = 0; j < 2; j++)
    {
      a = j * j;
      b = a - 2 * j;
    }
  if (b != -1)
    abort ();
  return 0;
}

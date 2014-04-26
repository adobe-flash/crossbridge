/* APPLE LOCAL file 6251664 */
/* Verify that correct code is generated for a multiply-and-decrement
   operation.  */
/* { dg-options "-O2" } */
/* { dg-do run } */
float f1 = 1.0, f2 = 2.0, f3 = 3.0, f4 = 4.0;

void abort (void);

int main (void)
{
  if (((f3 * f4) - (f1 * f2)) != 10.0)
    abort();
  return 0;
}


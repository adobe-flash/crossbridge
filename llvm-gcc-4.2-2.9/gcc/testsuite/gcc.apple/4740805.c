/* APPLE LOCAL file 4740805 4741439 */
/* Testcase from MathWorks.  */
/* { dg-do run } */
/* { dg-options "-O" } */
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <assert.h>

double DPad2[16];

double Constant_Value[4];

void __attribute__ ((noinline)) initialize_it(void);

int main(int argc, const char *argv[])
{
  const double *u = Constant_Value;

  double *y = DPad2;

  const double *uCpy;
  int i,j;

  initialize_it ();

  /* OUTER LOOP 1 */
  for (i = 0; i < 2; i++) {
    uCpy = u;

    *y++ = *uCpy;/* INCREMENT 1 */

    /* INNER LOOP */
    for (j = 0; j < 2; j++) {
      *y++ = *uCpy++;/* INCREMENT 2 */
    }

    uCpy--;

    *y++ = *uCpy;/* INCREMENT 3 */
  }

  /* OUTER LOOP 2 */
  for (i = 0; i<2; i++) {
    *y++ = *u;/* INCREMENT 4 */

    *y++ = *u++;/* INCREMENT 5 */
    *y++ = *u;/* INCREMENT 6 */

    *y++ = *u++;/* INCREMENT 7 */
  }

  if (y - DPad2 != 16)
    abort ();

  return 0;
}

void __attribute__ ((noinline))
initialize_it()
{
  int i;

  for (i=0; i<4; i++)
    Constant_Value[i] = (double) i + 1;
}

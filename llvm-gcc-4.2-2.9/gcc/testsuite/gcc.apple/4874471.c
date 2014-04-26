/* APPLE LOCAL file 4874471 */
/* Cloned from PR c/29091 */
/* { dg-do run { target "powerpc*-*-darwin*" } } */
/* { dg-options "-O2 -mpim-altivec" } */

extern void abort (void);

__attribute__ ((vector_size (sizeof (int) * 4))) int a = { 1, 2 };
int d = 3;
__attribute__ ((vector_size (sizeof (int) * 4))) int b = { 4, 5, 6 };
int e = 7;
__attribute__ ((vector_size (sizeof (int) * 4))) int c = { };

int
main (void)
{
  int *p = (int *) &a;
  if (p[0] != 1)
    abort ();
  if (p[1] != 2)
    abort ();
  if (p[2] != 2)
    abort ();
  if (p[3] != 2)
    abort ();
  p = (int *) &b;
  if (p[0] != 4)
    abort ();
  if (p[1] != 5)
    abort ();
  if (p[2] != 6)
    abort ();
  if (p[3] != 6)
    abort ();
  p = (int *) &c;
  if (p[0] != 0)
    abort ();
  if (p[1] != 0)
    abort ();
  if (p[2] != 0)
    abort ();
  if (p[3] != 0)
    abort ();
  return 0;
}

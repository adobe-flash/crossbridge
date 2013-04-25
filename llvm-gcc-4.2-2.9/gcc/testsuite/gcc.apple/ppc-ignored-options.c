/* Test options to ignore.  */

/* { dg-do run { target powerpc-*-darwin* } } */
/* { dg-options "-msse -msse2 -march=pentium4 -mcpu=pentium4" } */

int
main ()
{
  return 0;
}


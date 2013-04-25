/* Test options to ignore.  */

/* { dg-do run { target i?86-*-darwin* } } */
/* { dg-options "-faltivec -mno-fused-madd -mlong-branch -mlongcall -mcpu=G4 -mcpu=G5" } */

int
main ()
{
  return 0;
}


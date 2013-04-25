/* APPLE LOCAL file CW asm blocks */
/* More tests of asm functions.  */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks -O2" } */

extern "C" void abort(void);

/* Return 0 if xer and spr1 both have the same data.  */

asm int
test_spr ()
{
  mfspr r5,xer
  mfspr r4,spr1
  sub r3,r4,r5
}

int
main ()
{
  if (test_spr ())
    abort ();
  return 0;
}

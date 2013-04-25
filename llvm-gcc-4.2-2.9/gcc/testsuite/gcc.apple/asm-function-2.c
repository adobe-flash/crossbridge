/* APPLE LOCAL file CW asm blocks */
/* Test whole asm functions.  */

/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks -O2" } */

void abort(void);

asm void normal_fn ()
{
  nop
}

asm void nofralloc_fn ()
{
  nofralloc
  nop
  blr
}

asm void fralloc_fn ()
{
  fralloc
  nop
}

asm int fralloc_fn_1 ()
{
  fralloc
  mr r4,r1
  frfree
  sub r3,r4,r1
  blr
}

asm int fralloc_fn_2 ()
{
  fralloc 1000
  nop
  frfree
  blr
}

asm int fralloc_fn_3 ()
{
  int loc1, loc2;
  fralloc 41
  nop
  mr r3, loc2
  frfree
  mr r3, loc1
  blr
}

asm int f6 ()
{
  opword 0x386004d2 /* li r3,1234 */
}

main()
{
  normal_fn ();
  nofralloc_fn ();
  fralloc_fn ();
  fralloc_fn_1 ();
  fralloc_fn_2 ();
  fralloc_fn_3 ();
  if (f6 () != 1234)
    abort ();
  return 0;
}

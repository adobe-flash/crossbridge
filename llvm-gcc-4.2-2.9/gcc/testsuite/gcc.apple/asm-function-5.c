/* APPLE LOCAL file CW asm blocks */
/* Test function calls in asm functions.  */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks -O2" } */
/* { dg-require-effective-target ilp32 } */

void abort(void);

int glob = 0;

int other ();
int stubfn ();
int localfn () { return other (); }

asm void foo(int arg)
{
  nofralloc
  mflr r0
  stmw r30,-8(r1)
  stw r0,8(r1)
  stwu r1,-80(r1)
  bl stubfn
    /*  bl L_stubfn$stub */
  lwz r0,88(r1)
  addi r1,r1,80
  mtlr r0
  lmw r30,-8(r1)
  b localfn
}

void bar (int arg)
{
  stubfn ();
  localfn ();
}

int stubfn () { return other(); }

int other () { return ++glob; }

int main ()
{
  bar(34);
  foo(92);
  if (glob != 4)
    abort ();
  return 0;
}

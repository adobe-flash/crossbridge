/* APPLE LOCAL file CW asm blocks */
/* Test function calls in asm functions.  */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks -O2" } */
/* { dg-require-effective-target ilp32 } */

void function_with_asm_stmts () {
  asm ("nop");
  asm volatile ("nop");
  __asm__ ("nop");
  __asm__ __volatile__ ("nop");
}

extern "C" void abort (void);

int glob = 0;

int other ();
extern "C" int stubfn ();
int localfn () { return other (); }

asm void foo(int arg)
{
  nofralloc
  mflr r0
  stmw r30,(-8)(r1)
  stw r0,8(r1)
  stwu r1,(-80)(r1)
  bl stubfn
  addi r1,r1,80
  lwz r0,8(r1)
  lmw r30,(-8)(r1)
  mtlr r0
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

/* APPLE LOCAL file CW asm blocks */
/* Test asm with macros.  */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks -O2" } */

extern "C" void abort(void);

#define mac1 add r3, argx, argy

asm int
foo (register int argx, register int argy)
{
  mac1
}

#define rsltreg r3
#define mac2(x,y) add rsltreg,x,y

asm int
foo1 (register int argx, register int argy)
{
  register int loc1, loc2;
  nop
    li loc1, 100
    li rsltreg,0
    b @a34
    mac2(argx,argy)
@a34
    mac2(rsltreg,loc1)
    blr
}

#define limac li
#define num 48

asm int foo2(int x, float y)
{
#pragma unused(x)
#pragma unused(x,y)
  limac rsltreg, num;
#define mac3(zzz) \
  limac r4, zzz
#pragma unused(y)
  nop
    mac3(num)
#pragma unused(x)
    add r3,r3,r4
}

int
main ()
{
  if (foo (22, 23) != 45)
    abort ();
  if (foo1 (1, 2) != 100)
    abort ();
  if (foo2 (100, 1.2) != 2 * num)
    abort ();
  return 0;
}

/* APPLE LOCAL file CW asm blocks */
/* Test entry points in asm functions.  */

/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks -O2" } */

void abort(void);

int entry1(int);
int entry2(int);
int entry3(int);

asm int foo(int x)
{
    addi r3,r3,45
  entry entry1
    addi r3,r3,1
  entry static entry2
    addi r3,r3,1
  entry extern entry3
    addi r3,r3,1
}

int main ()
{
  if (entry1(0) != 3)
    abort();
  if (entry2(89) != 91)
    abort();
  if (entry3(100) != 101)
    abort();
  return 0;
}

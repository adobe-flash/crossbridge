/* PR target/8340 */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-fPIC" } */

int foo ()
{
  static int a;

/* LLVM LOCAL begin we don't reserve EBX as the pic register, it's variable,
/* so there is no reason this code shouldn't work.  dg-error removed. */
  __asm__ __volatile__ (
/* LLVM LOCAL end */
    "xorl %%ebx, %%ebx\n"
    "movl %%ebx, %0\n"
    : "=m" (a)
    :
    : "%ebx"
  );

  return a;
}

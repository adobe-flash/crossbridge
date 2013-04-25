/* APPLE LOCAL file 5774356 */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-require-effective-target powerpc_altivec_ok } */

#include <stdlib.h>
#include <altivec.h>

#define NOINLINE __attribute__ ((__noinline__))

typedef int __attribute__((vector_size(16))) v4si;

int global_count;
v4si ref1 = { 1, 1, 1, 1 };
v4si ref2 = { 2, 2, 2, 2 };
v4si var1 = { 1, 1, 1, 1 };
v4si var2 = { 2, 2, 2, 2 };

void NOINLINE f0 ()
{
  global_count++;
}

void NOINLINE f1 ()
{
  v4si t = var1;
  f0 ();
  var1 = t;
}

void NOINLINE f2 ()
{
  v4si t = var2;
  f1 ();
  var2 = t;
}

main ()
{
  f2 ();
  if (!vec_all_eq (var1, ref1))
    abort ();
  if (!vec_all_eq (var2, ref2))
    abort ();
  return 0;
}

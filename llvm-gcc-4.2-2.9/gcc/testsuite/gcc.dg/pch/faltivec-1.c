/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc-*-darwin* } } */
/* { dg-options "-faltivec -I. -mcpu=G5" } */

#include "faltivec-1.h" 
/* { dg-error "No such file or directory" "" { target *-*-* } 5 } */
/* { dg-error "one or more PCH files were found" "" { target *-*-* } 5 } */
/* { dg-error "use -Winvalid-pch for more information" "" { target *-*-* } 5 } */

/* Test for correct PCH validation of -faltivec.  */
void foo()
{
  const vector signed short vconst = (vector signed short) (0,0,0,0,0,0,0,0);
  const vector signed short vconst2 = vec_splat(vconst, 0);
  bar();
}


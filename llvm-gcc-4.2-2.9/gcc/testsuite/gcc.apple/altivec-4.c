/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

extern void abort();

#include <stdio.h>
int main()
{
  int i;
  union u {
	vector signed short uv;
	signed short val[8];
  } v;

  vector signed short     SpecialConstants = (vector signed short)( 1, 2, 3, 4, 5, 6,7,8);
  v.uv = SpecialConstants;
  for (i=0; i < 8; i++)
     if (v.val[i] != i+1)
       abort();
  return 0;
}


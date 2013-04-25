/* APPLE LOCAL file AltiVec */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

#include <stdio.h>

typedef vector signed int VSI;

VSI VI(
	      VSI v2,
	      int gpr3,
	      VSI v3,
	      int gpr4,
	      VSI v4,
	      int gpr5,
	      VSI v5,
	      int gpr6,
	      int gpr7,
	      int gpr8,
	      int gpr9,
	      VSI v6,
	      int gpr10,	/* gpr3-gpr10 in registers */
	      VSI v7,
	      VSI v8,
	      VSI v9,
	      VSI v10,
	      VSI v11,
	      VSI v12,
	      VSI v13,	/* All in v2-v13 */
	      volatile VSI v14,
	      int gpr11,
	      int gpr12,
	      int gpr13,
	      int gpr14,
	      VSI v15,
	      VSI v16, int* sum)
{
	*sum = gpr3 + gpr4 + gpr5 + gpr6 + gpr7 + gpr8 + gpr9 + gpr10 + gpr11 + gpr12 + gpr13 + gpr14;
	return v2+v3+v4+v5+v6+v7+v8+v9+v10+v11+v12+v13+v14+v15+v16;
}

extern void abort();

int main()
{
  union u {
    VSI uv;
    int val[4];
  } union_var;

  int i;

  int sum;
  union_var.uv = 
	VI((VSI){1,1,1,1},3,(VSI){1,1,1,1},4,(VSI){1,1,1,1},5,(VSI){1,1,1,1},6,7,8,9,
	   (VSI){1,1,1,1},10,(VSI){1,1,1,1},(VSI){1,1,1,1},(VSI){1,1,1,1},(VSI){1,1,1,1},
	   (VSI){1,1,1,1},(VSI){1,1,1,1},(VSI){1,1,1,1},(VSI){1,1,1,1},11,12,13,14,
	   (VSI){1,1,1,1},(VSI){1,1,1,1}, &sum);
  if (sum != 102)
    abort();

  for (i=0; i < 4; i++)
     if (union_var.val[i] != 15)
       abort();
  return 0;
}

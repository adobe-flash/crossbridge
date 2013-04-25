/* APPLE LOCAL file AltiVec */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

extern "C" void abort();

void check_eq (vector signed short v1, vector signed short v2)
{
  	if (!vec_all_eq (v1, v2))
	  abort();
	return;
}

int main()
{
  	int r;

        vector signed short vboundary[8] = {
                                              (vector signed short)(0),
                                              (vector signed short)(1),
                                              (vector signed short)(2),
                                              (vector signed short)(3),
                                              (vector signed short)(4),
                                              (vector signed short)(5),
                                              (vector signed short)(6),
                                              (vector signed short)(7)
					};
	for (r = 0; r < 8; r++)
	   check_eq (vboundary[r], (vector signed short)(r,r,r,r,r,r,r,r));
	return 0;
}

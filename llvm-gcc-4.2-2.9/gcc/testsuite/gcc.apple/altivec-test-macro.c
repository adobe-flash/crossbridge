/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-maltivec" } */

#include <altivec.h>

#define TYPE unsigned int
union U {
	TYPE ui[4];
	vector TYPE uv;
} data;

extern void abort();

int main( void )
{
        vector unsigned int v = {1,2,3,4};
        vector TYPE     UUUUUU = {10,10,10,10};
	int i;

        v = vec_add( v, UUUUUU );

	data.uv = v;

	for (i=0; i < 4; i++)
	   if (data.ui[i] != (10+i+1))
	     abort();

        return 0;
}


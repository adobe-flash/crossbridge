/* APPLE LOCAL file CW asm blocks with AltiVec insn. */
/* Test asm-syntax blocks within functions.  */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec -fasm-blocks" } */

extern void abort();

int main( void )
{
	int i;
        register vector float a = (vector float) (1.0f);
        register vector float b = (vector float) (2.0f);
        register vector float c = (vector float) (3.0f);
	float result[4] = {5.00, 5.00, 5.00, 5.00};
	union u {
	  vector float v;
	  float fv[4];
	} uv;

        asm
        {
                vmaddfp c, a, c, b
        }

	uv.v = c;

	for (i=0; i < 4; i++)
	  if (uv.fv[i] != result[i])
	    abort();

        return 0;
}



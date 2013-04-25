/* APPLE LOCAL file Radar 4109832 */
/* { dg-do run { target "i?86-*-darwin*" } } */

#include <xmmintrin.h>

extern void abort(void);

int main(int argc, char **argv) {
	__m128 x1,x2,x3,x4;
	float f[16];
	int i;

	x1 = _mm_setr_ps(1.0,2.0,3.0,4.0);
	x2 = _mm_setr_ps(5.0,6.0,7.0,8.0);
	x3 = _mm_setr_ps(9.0,10.0,11.0,12.0);
	x4 = _mm_setr_ps(13.0,14.0,15.0,16.0);

	_mm_storeu_ps(&f[0], x1);
	_mm_storeu_ps(&f[4], x2);
	_mm_storeu_ps(&f[8], x3);
	_mm_storeu_ps(&f[12], x4);

        _MM_TRANSPOSE4_PS (x1,x2,x3,x4);

        _mm_storeu_ps(&f[0], x1);
        _mm_storeu_ps(&f[4], x2);
        _mm_storeu_ps(&f[8], x3);
        _mm_storeu_ps(&f[12], x4);

        if (!(f[0] == 1.0 && f[1] == 5.0 && f[2] == 9.0 && f[3] == 13.0))
            abort();

	return 0;
}	

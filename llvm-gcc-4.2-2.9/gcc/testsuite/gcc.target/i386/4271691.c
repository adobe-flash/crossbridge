/* APPLE LOCAL file 4271691 */
#include <xmmintrin.h>
#include <emmintrin.h>
/* { dg-do compile { target i?86-*-* } } */
/* { dg-options "-O2 -msse2" } */

void Coefs(unsigned char *current_part_ptr, int current_part_stride, unsigned char *ref_part_ptr, int ref_part_stride, unsigned char *coef_buf, int n)	{


static const unsigned short c_32[8] = {32, 32, 32, 32, 32, 32, 32, 32};			

int i;			

__m128i v_row0_0, v_row0_1;			
__m128i v_temp_0, v_temp_1;			
__m128i v_result;			

__m128i vZero;
vZero = _mm_setzero_si128();			
__m128i v_32 = _mm_loadu_si128((__m128i*)c_32);			

__m128i* coef_ptr = (__m128i*) coef_buf;			

v_row0_0 = _mm_loadl_epi64((__m128i*)ref_part_ptr);			
v_row0_1 = _mm_shufflelo_epi16(v_row0_0, 0xf9);			
v_row0_1 = _mm_insert_epi16(v_row0_1, *(unsigned short*)(ref_part_ptr+8), 3);			
ref_part_ptr += ref_part_stride;			
// row0: 0 1 2 3 4 5 6 7			
// row1: 2 3 4 5 6 7 8 9			

v_row0_0 = _mm_unpacklo_epi8(v_row0_0, vZero);			
v_row0_1 = _mm_unpacklo_epi8(v_row0_1, vZero);			

for ( i = 0; i < n; i++ )			
{			
v_row0_0 = _mm_mullo_epi16(v_row0_0, coef_ptr[0]);			
v_row0_1 = _mm_mullo_epi16(v_row0_1, coef_ptr[1]);			

v_result = v_32;			
v_result = _mm_add_epi16(v_result, v_row0_0);			
v_result = _mm_add_epi16(v_result, v_row0_1);			

v_row0_0 = _mm_loadl_epi64((__m128i*)ref_part_ptr);			
v_row0_1 = _mm_shufflelo_epi16(v_row0_0, 0xf9);			
v_row0_1 = _mm_insert_epi16(v_row0_1, *(unsigned short*)(ref_part_ptr+8), 3);			
ref_part_ptr += ref_part_stride;			
v_row0_0 = _mm_unpacklo_epi8(v_row0_0, vZero);			
v_row0_1 = _mm_unpacklo_epi8(v_row0_1, vZero);			
v_temp_0 = _mm_mullo_epi16(v_row0_0, coef_ptr[2]);			
v_temp_1 = _mm_mullo_epi16(v_row0_1, coef_ptr[3]);			

v_result = _mm_add_epi16(v_result, v_temp_0);			
v_result = _mm_add_epi16(v_result, v_temp_1);			
v_result = _mm_srli_epi16(v_result, 6);			

_mm_store_si128((__m128i*)(current_part_ptr), v_result);			
current_part_ptr += current_part_stride;			
}			

}			

/* Should be no reg-reg copies. */
/* { dg-final { scan-assembler-not "movdqa\t%xmm\[0-7\], %xmm\[0-7\]" } } */

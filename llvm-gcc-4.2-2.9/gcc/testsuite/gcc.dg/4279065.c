/* APPLE LOCAL file 4279065 */
/* { dg-do compile { target "i?86*-*-darwin*" } } */
/* { dg-options "-O3" } */
#include <emmintrin.h>

void S_Interpolate_4x4_IntPel_Mono_Add_Later(unsigned char *current_part_ptr, int current_part_stride, unsigned char *ref_part_ptr, int ref_part_stride){

  static const unsigned int c_0[4] = { 0, 0, 0, 0 };
  unsigned long s_row0_0, s_row1_0, s_row2_0, s_row3_0;
  __m128i v_row0_0, v_row1_0, v_row2_0, v_row3_0;

  __m128i v_Zero = _mm_loadu_si128((__m128i*)c_0);


  s_row0_0  = *(unsigned long*)(ref_part_ptr+(0*ref_part_stride));
  s_row1_0  = *(unsigned long*)(ref_part_ptr+(1*ref_part_stride));
  s_row2_0  = *(unsigned long*)(ref_part_ptr+(2*ref_part_stride));
  s_row3_0  = *(unsigned long*)(ref_part_ptr+(3*ref_part_stride));

  v_row0_0  = _mm_cvtsi32_si128(s_row0_0);
  v_row1_0  = _mm_cvtsi32_si128(s_row1_0);
  v_row2_0  = _mm_cvtsi32_si128(s_row2_0);
  v_row3_0  = _mm_cvtsi32_si128(s_row3_0);

  v_row0_0  = _mm_unpacklo_epi8(v_row0_0,  v_Zero);
  v_row1_0  = _mm_unpacklo_epi8(v_row1_0,  v_Zero);
  v_row2_0  = _mm_unpacklo_epi8(v_row2_0,  v_Zero);
  v_row3_0  = _mm_unpacklo_epi8(v_row3_0,  v_Zero);

  _mm_storel_epi64((__m128i*)(current_part_ptr+(0*current_part_stride)), v_row0_0);
  _mm_storel_epi64((__m128i*)(current_part_ptr+(1*current_part_stride)), v_row1_0);
  _mm_storel_epi64((__m128i*)(current_part_ptr+(2*current_part_stride)), v_row2_0);
  _mm_storel_epi64((__m128i*)(current_part_ptr+(3*current_part_stride)), v_row3_0);
}
/* { dg-final { scan-assembler-not "-24\\\(%ebp\\\)" } } */

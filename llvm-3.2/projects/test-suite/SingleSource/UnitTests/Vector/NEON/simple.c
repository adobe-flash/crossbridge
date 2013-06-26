#include <stdio.h>
#include <arm_neon.h>

typedef int8x16_t v16i8;
typedef int16x8_t v8i16;
typedef int32x4_t v4i32;

typedef int8x16x2_t v16i8x2;
typedef int16x8x2_t v8i16x2;
typedef int32x4x2_t v4i32x2;

v16i8 init_v16i8(int8_t a0, int8_t a1, int8_t a2, int8_t a3,
                 int8_t a4, int8_t a5, int8_t a6, int8_t a7,
                 int8_t a8, int8_t a9, int8_t a10, int8_t a11,
                 int8_t a12, int8_t a13, int8_t a14, int8_t a15) {
  int8_t d[16];

  d[0] = a0; d[1] = a1; d[2] = a2; d[3] = a3;
  d[4] = a4; d[5] = a5; d[6] = a6; d[7] = a7;
  d[8] = a8; d[9] = a9; d[10] = a10; d[11] = a11;
  d[12] = a12; d[13] = a13; d[14] = a14; d[15] = a15;

  return vld1q_s8(d);
}

v8i16 init_v8i16(int16_t a0, int16_t a1, int16_t a2, int16_t a3,
                 int16_t a4, int16_t a5, int16_t a6, int16_t a7) {
  int16_t d[8];

  d[0] = a0; d[1] = a1; d[2] = a2; d[3] = a3;
  d[4] = a4; d[5] = a5; d[6] = a6; d[7] = a7;

  return vld1q_s16(d);
}

v4i32 init_v4i32(int32_t a0, int32_t a1, int32_t a2, int32_t a3) {
  int32_t d[8];

  d[0] = a0; d[1] = a1; d[2] = a2; d[3] = a3;

  return vld1q_s32(d);
}

void print_v16i8(v16i8 a) {
  int8_t d[16];
  vst1q_s8(d, a);
  printf("(%d, %d, %d, %d, %d, %d, %d, %d, "
          "%d, %d, %d, %d, %d, %d, %d, %d)",
          d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],
          d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15]);
}
void print_v8i16(v8i16 a) {
  int16_t d[8];
  vst1q_s16(d, a);
  printf("(%d, %d, %d, %d, %d, %d, %d, %d)",
          d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
}
void print_v4i32(v4i32 a) {
  int32_t d[4];
  vst1q_s32(d, a);
  printf("(%d, %d, %d, %d)", d[0], d[1], d[2], d[3]);
}

void test_basic() {
  printf("%s\n", __FUNCTION__);

  v16i8 a0_0 = vmovq_n_s8(1);
  v16i8 a0_1 = init_v16i8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  v16i8 a0_2 = vaddq_s8(a0_0, a0_1);
  printf("a0_0: ");
  print_v16i8(a0_0);
  printf("\n");
  printf("a0_1: ");
  print_v16i8(a0_1);
  printf("\n");
  printf("a0_2: ");
  print_v16i8(a0_2);
  printf("\n");

  v8i16 a1_0 = vmovq_n_s16(1);
  v8i16 a1_1 = init_v8i16(0, 1, 2, 3, 4, 5, 6, 7);
  v8i16 a1_2 = vaddq_s16(a1_0, a1_1);
  printf("a1_0: ");
  print_v8i16(a1_0);
  printf("\n");
  printf("a1_1: ");
  print_v8i16(a1_1);
  printf("\n");
  printf("a1_2: ");
  print_v8i16(a1_2);
  printf("\n");

  v4i32 a2_0 = vmovq_n_s32(1);
  v4i32 a2_1 = init_v4i32(0, 1, 2, 3);
  v4i32 a2_2 = vaddq_s32(a2_0, a2_1);
  printf("a2_0: ");
  print_v4i32(a2_0);
  printf("\n");
  printf("a2_1: ");
  print_v4i32(a2_1);
  printf("\n");
  printf("a2_2: ");
  print_v4i32(a2_2);
  printf("\n");
}

void test_zip() {
  printf("%s\n", __FUNCTION__);

  v16i8 a0_0 = init_v16i8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  v16i8 a0_1 = init_v16i8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
  v16i8x2 a0_2 = vzipq_s8(a0_0, a0_1);
  v16i8x2 a0_3 = vuzpq_s8(a0_2.val[0], a0_2.val[1]);
  printf("a0_2.val[0]: ");
  print_v16i8(a0_2.val[0]);
  printf("\n");
  printf("a0_2.val[1]: ");
  print_v16i8(a0_2.val[1]);
  printf("\n");
  printf("a0_3.val[0]: ");
  print_v16i8(a0_3.val[0]);
  printf("\n");
  printf("a0_3.val[1]: ");
  print_v16i8(a0_3.val[1]);
  printf("\n");

  v8i16 a1_0 = init_v8i16(0, 1, 2, 3, 4, 5, 6, 7);
  v8i16 a1_1 = init_v8i16(7, 6, 5, 4, 3, 2, 1, 0);
  v8i16x2 a1_2 = vzipq_s16(a1_0, a1_1);
  v8i16x2 a1_3 = vuzpq_s16(a1_2.val[0], a1_2.val[1]);
  printf("a1_2.val[0]: ");
  print_v8i16(a1_2.val[0]);
  printf("\n");
  printf("a1_2.val[1]: ");
  print_v8i16(a1_2.val[1]);
  printf("\n");
  printf("a1_3.val[0]: ");
  print_v8i16(a1_3.val[0]);
  printf("\n");
  printf("a1_3.val[1]: ");
  print_v8i16(a1_3.val[1]);
  printf("\n");

  v4i32 a2_0 = init_v4i32(0, 1, 2, 3);
  v4i32 a2_1 = init_v4i32(3, 2, 1, 0);
  v4i32x2 a2_2 = vzipq_s32(a2_0, a2_1);
  v4i32x2 a2_3 = vuzpq_s32(a2_2.val[0], a2_2.val[1]);
  printf("a2_2.val[0]: ");
  print_v4i32(a2_2.val[0]);
  printf("\n");
  printf("a2_2.val[1]: ");
  print_v4i32(a2_2.val[1]);
  printf("\n");
  printf("a2_3.val[0]: ");
  print_v4i32(a2_3.val[0]);
  printf("\n");
  printf("a2_3.val[1]: ");
  print_v4i32(a2_3.val[1]);
  printf("\n");
}

int main() {
  test_basic();
  test_zip();
  return 0;
}

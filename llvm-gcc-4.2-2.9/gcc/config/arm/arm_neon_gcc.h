/* Internal definitions for GCC-compatible NEON types and intrinsics.
   Do not include this file directly; please use <arm_neon.h> and define
   the ARM_NEON_GCC_COMPATIBILITY macro.

   This file is generated automatically using neon-gen.ml.
   Please do not edit manually.

   Copyright (C) 2006, 2007 Free Software Foundation, Inc.
   Contributed by CodeSourcery.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you include this header file into source
   files compiled by GCC, this header file does not by itself cause
   the resulting executable to be covered by the GNU General Public
   License.  This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU General
   Public License.  */

#ifndef _GCC_ARM_NEON_H
#define _GCC_ARM_NEON_H 1

#ifndef __ARM_NEON__
#error You must enable NEON instructions (e.g. -mfloat-abi=softfp -mfpu=neon) to use arm_neon.h
#else

#ifdef __cplusplus
extern "C" {
#define __neon_ptr_cast(ty, ptr) reinterpret_cast<ty>(ptr)
#else
#define __neon_ptr_cast(ty, ptr) (ty)(ptr)
#endif

#include <stdint.h>

typedef __builtin_neon_sf float32_t;
typedef __builtin_neon_poly8 poly8_t;
typedef __builtin_neon_poly16 poly16_t;

typedef __neon_int8x8_t int8x8_t;
typedef __neon_int16x4_t int16x4_t;
typedef __neon_int32x2_t int32x2_t;
typedef __neon_int64x1_t int64x1_t;
typedef __neon_float32x2_t float32x2_t;
typedef __neon_poly8x8_t poly8x8_t;
typedef __neon_poly16x4_t poly16x4_t;
typedef __neon_uint8x8_t uint8x8_t;
typedef __neon_uint16x4_t uint16x4_t;
typedef __neon_uint32x2_t uint32x2_t;
typedef __neon_uint64x1_t uint64x1_t;
typedef __neon_int8x16_t int8x16_t;
typedef __neon_int16x8_t int16x8_t;
typedef __neon_int32x4_t int32x4_t;
typedef __neon_int64x2_t int64x2_t;
typedef __neon_float32x4_t float32x4_t;
typedef __neon_poly8x16_t poly8x16_t;
typedef __neon_poly16x8_t poly16x8_t;
typedef __neon_uint8x16_t uint8x16_t;
typedef __neon_uint16x8_t uint16x8_t;
typedef __neon_uint32x4_t uint32x4_t;
typedef __neon_uint64x2_t uint64x2_t;

typedef struct int8x8x2_t
{
  int8x8_t val[2];
} int8x8x2_t;

typedef struct int8x16x2_t
{
  int8x16_t val[2];
} int8x16x2_t;

typedef struct int16x4x2_t
{
  int16x4_t val[2];
} int16x4x2_t;

typedef struct int16x8x2_t
{
  int16x8_t val[2];
} int16x8x2_t;

typedef struct int32x2x2_t
{
  int32x2_t val[2];
} int32x2x2_t;

typedef struct int32x4x2_t
{
  int32x4_t val[2];
} int32x4x2_t;

typedef struct int64x1x2_t
{
  int64x1_t val[2];
} int64x1x2_t;

typedef struct int64x2x2_t
{
  int64x2_t val[2];
} int64x2x2_t;

typedef struct uint8x8x2_t
{
  uint8x8_t val[2];
} uint8x8x2_t;

typedef struct uint8x16x2_t
{
  uint8x16_t val[2];
} uint8x16x2_t;

typedef struct uint16x4x2_t
{
  uint16x4_t val[2];
} uint16x4x2_t;

typedef struct uint16x8x2_t
{
  uint16x8_t val[2];
} uint16x8x2_t;

typedef struct uint32x2x2_t
{
  uint32x2_t val[2];
} uint32x2x2_t;

typedef struct uint32x4x2_t
{
  uint32x4_t val[2];
} uint32x4x2_t;

typedef struct uint64x1x2_t
{
  uint64x1_t val[2];
} uint64x1x2_t;

typedef struct uint64x2x2_t
{
  uint64x2_t val[2];
} uint64x2x2_t;

typedef struct float32x2x2_t
{
  float32x2_t val[2];
} float32x2x2_t;

typedef struct float32x4x2_t
{
  float32x4_t val[2];
} float32x4x2_t;

typedef struct poly8x8x2_t
{
  poly8x8_t val[2];
} poly8x8x2_t;

typedef struct poly8x16x2_t
{
  poly8x16_t val[2];
} poly8x16x2_t;

typedef struct poly16x4x2_t
{
  poly16x4_t val[2];
} poly16x4x2_t;

typedef struct poly16x8x2_t
{
  poly16x8_t val[2];
} poly16x8x2_t;

typedef struct int8x8x3_t
{
  int8x8_t val[3];
} int8x8x3_t;

typedef struct int8x16x3_t
{
  int8x16_t val[3];
} int8x16x3_t;

typedef struct int16x4x3_t
{
  int16x4_t val[3];
} int16x4x3_t;

typedef struct int16x8x3_t
{
  int16x8_t val[3];
} int16x8x3_t;

typedef struct int32x2x3_t
{
  int32x2_t val[3];
} int32x2x3_t;

typedef struct int32x4x3_t
{
  int32x4_t val[3];
} int32x4x3_t;

typedef struct int64x1x3_t
{
  int64x1_t val[3];
} int64x1x3_t;

typedef struct int64x2x3_t
{
  int64x2_t val[3];
} int64x2x3_t;

typedef struct uint8x8x3_t
{
  uint8x8_t val[3];
} uint8x8x3_t;

typedef struct uint8x16x3_t
{
  uint8x16_t val[3];
} uint8x16x3_t;

typedef struct uint16x4x3_t
{
  uint16x4_t val[3];
} uint16x4x3_t;

typedef struct uint16x8x3_t
{
  uint16x8_t val[3];
} uint16x8x3_t;

typedef struct uint32x2x3_t
{
  uint32x2_t val[3];
} uint32x2x3_t;

typedef struct uint32x4x3_t
{
  uint32x4_t val[3];
} uint32x4x3_t;

typedef struct uint64x1x3_t
{
  uint64x1_t val[3];
} uint64x1x3_t;

typedef struct uint64x2x3_t
{
  uint64x2_t val[3];
} uint64x2x3_t;

typedef struct float32x2x3_t
{
  float32x2_t val[3];
} float32x2x3_t;

typedef struct float32x4x3_t
{
  float32x4_t val[3];
} float32x4x3_t;

typedef struct poly8x8x3_t
{
  poly8x8_t val[3];
} poly8x8x3_t;

typedef struct poly8x16x3_t
{
  poly8x16_t val[3];
} poly8x16x3_t;

typedef struct poly16x4x3_t
{
  poly16x4_t val[3];
} poly16x4x3_t;

typedef struct poly16x8x3_t
{
  poly16x8_t val[3];
} poly16x8x3_t;

typedef struct int8x8x4_t
{
  int8x8_t val[4];
} int8x8x4_t;

typedef struct int8x16x4_t
{
  int8x16_t val[4];
} int8x16x4_t;

typedef struct int16x4x4_t
{
  int16x4_t val[4];
} int16x4x4_t;

typedef struct int16x8x4_t
{
  int16x8_t val[4];
} int16x8x4_t;

typedef struct int32x2x4_t
{
  int32x2_t val[4];
} int32x2x4_t;

typedef struct int32x4x4_t
{
  int32x4_t val[4];
} int32x4x4_t;

typedef struct int64x1x4_t
{
  int64x1_t val[4];
} int64x1x4_t;

typedef struct int64x2x4_t
{
  int64x2_t val[4];
} int64x2x4_t;

typedef struct uint8x8x4_t
{
  uint8x8_t val[4];
} uint8x8x4_t;

typedef struct uint8x16x4_t
{
  uint8x16_t val[4];
} uint8x16x4_t;

typedef struct uint16x4x4_t
{
  uint16x4_t val[4];
} uint16x4x4_t;

typedef struct uint16x8x4_t
{
  uint16x8_t val[4];
} uint16x8x4_t;

typedef struct uint32x2x4_t
{
  uint32x2_t val[4];
} uint32x2x4_t;

typedef struct uint32x4x4_t
{
  uint32x4_t val[4];
} uint32x4x4_t;

typedef struct uint64x1x4_t
{
  uint64x1_t val[4];
} uint64x1x4_t;

typedef struct uint64x2x4_t
{
  uint64x2_t val[4];
} uint64x2x4_t;

typedef struct float32x2x4_t
{
  float32x2_t val[4];
} float32x2x4_t;

typedef struct float32x4x4_t
{
  float32x4_t val[4];
} float32x4x4_t;

typedef struct poly8x8x4_t
{
  poly8x8_t val[4];
} poly8x8x4_t;

typedef struct poly8x16x4_t
{
  poly8x16_t val[4];
} poly8x16x4_t;

typedef struct poly16x4x4_t
{
  poly16x4_t val[4];
} poly16x4x4_t;

typedef struct poly16x8x4_t
{
  poly16x8_t val[4];
} poly16x8x4_t;


#define vadd_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vaddv8qi (__a, __b, 1)

#define vadd_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vaddv4hi (__a, __b, 1)

#define vadd_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vaddv2si (__a, __b, 1)

#define vadd_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vaddv1di (__a, __b, 1)

#define vadd_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vaddv2sf (__a, __b, 5)

#define vadd_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vadd_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vadd_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vaddv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vadd_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vaddv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define vaddq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vaddv16qi (__a, __b, 1)

#define vaddq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vaddv8hi (__a, __b, 1)

#define vaddq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vaddv4si (__a, __b, 1)

#define vaddq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vaddv2di (__a, __b, 1)

#define vaddq_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vaddv4sf (__a, __b, 5)

#define vaddq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vaddv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vaddq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vaddv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vaddq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vaddv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vaddq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vaddv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vaddl_s8(__a, __b) \
  (int16x8_t)__builtin_neon_vaddlv8qi (__a, __b, 1)

#define vaddl_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vaddlv4hi (__a, __b, 1)

#define vaddl_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vaddlv2si (__a, __b, 1)

#define vaddl_u8(__a, __b) \
  (uint16x8_t)__builtin_neon_vaddlv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vaddl_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vaddlv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vaddl_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vaddlv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vaddw_s8(__a, __b) \
  (int16x8_t)__builtin_neon_vaddwv8qi (__a, __b, 1)

#define vaddw_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vaddwv4hi (__a, __b, 1)

#define vaddw_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vaddwv2si (__a, __b, 1)

#define vaddw_u8(__a, __b) \
  (uint16x8_t)__builtin_neon_vaddwv8qi ((int16x8_t) __a, (int8x8_t) __b, 0)

#define vaddw_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vaddwv4hi ((int32x4_t) __a, (int16x4_t) __b, 0)

#define vaddw_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vaddwv2si ((int64x2_t) __a, (int32x2_t) __b, 0)

#define vhadd_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vhaddv8qi (__a, __b, 1)

#define vhadd_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vhaddv4hi (__a, __b, 1)

#define vhadd_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vhaddv2si (__a, __b, 1)

#define vhadd_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vhaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vhadd_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vhaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vhadd_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vhaddv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vhaddq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vhaddv16qi (__a, __b, 1)

#define vhaddq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vhaddv8hi (__a, __b, 1)

#define vhaddq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vhaddv4si (__a, __b, 1)

#define vhaddq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vhaddv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vhaddq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vhaddv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vhaddq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vhaddv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vrhadd_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vhaddv8qi (__a, __b, 3)

#define vrhadd_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vhaddv4hi (__a, __b, 3)

#define vrhadd_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vhaddv2si (__a, __b, 3)

#define vrhadd_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vhaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 2)

#define vrhadd_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vhaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 2)

#define vrhadd_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vhaddv2si ((int32x2_t) __a, (int32x2_t) __b, 2)

#define vrhaddq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vhaddv16qi (__a, __b, 3)

#define vrhaddq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vhaddv8hi (__a, __b, 3)

#define vrhaddq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vhaddv4si (__a, __b, 3)

#define vrhaddq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vhaddv16qi ((int8x16_t) __a, (int8x16_t) __b, 2)

#define vrhaddq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vhaddv8hi ((int16x8_t) __a, (int16x8_t) __b, 2)

#define vrhaddq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vhaddv4si ((int32x4_t) __a, (int32x4_t) __b, 2)

#define vqadd_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vqaddv8qi (__a, __b, 1)

#define vqadd_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqaddv4hi (__a, __b, 1)

#define vqadd_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqaddv2si (__a, __b, 1)

#define vqadd_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vqaddv1di (__a, __b, 1)

#define vqadd_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vqaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vqadd_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vqaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vqadd_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vqaddv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vqadd_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vqaddv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define vqaddq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vqaddv16qi (__a, __b, 1)

#define vqaddq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqaddv8hi (__a, __b, 1)

#define vqaddq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqaddv4si (__a, __b, 1)

#define vqaddq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vqaddv2di (__a, __b, 1)

#define vqaddq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vqaddv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vqaddq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vqaddv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vqaddq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vqaddv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vqaddq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vqaddv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vaddhn_s16(__a, __b) \
  (int8x8_t)__builtin_neon_vaddhnv8hi (__a, __b, 1)

#define vaddhn_s32(__a, __b) \
  (int16x4_t)__builtin_neon_vaddhnv4si (__a, __b, 1)

#define vaddhn_s64(__a, __b) \
  (int32x2_t)__builtin_neon_vaddhnv2di (__a, __b, 1)

#define vaddhn_u16(__a, __b) \
  (uint8x8_t)__builtin_neon_vaddhnv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vaddhn_u32(__a, __b) \
  (uint16x4_t)__builtin_neon_vaddhnv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vaddhn_u64(__a, __b) \
  (uint32x2_t)__builtin_neon_vaddhnv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vraddhn_s16(__a, __b) \
  (int8x8_t)__builtin_neon_vaddhnv8hi (__a, __b, 3)

#define vraddhn_s32(__a, __b) \
  (int16x4_t)__builtin_neon_vaddhnv4si (__a, __b, 3)

#define vraddhn_s64(__a, __b) \
  (int32x2_t)__builtin_neon_vaddhnv2di (__a, __b, 3)

#define vraddhn_u16(__a, __b) \
  (uint8x8_t)__builtin_neon_vaddhnv8hi ((int16x8_t) __a, (int16x8_t) __b, 2)

#define vraddhn_u32(__a, __b) \
  (uint16x4_t)__builtin_neon_vaddhnv4si ((int32x4_t) __a, (int32x4_t) __b, 2)

#define vraddhn_u64(__a, __b) \
  (uint32x2_t)__builtin_neon_vaddhnv2di ((int64x2_t) __a, (int64x2_t) __b, 2)

#define vmul_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vmulv8qi (__a, __b, 1)

#define vmul_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vmulv4hi (__a, __b, 1)

#define vmul_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vmulv2si (__a, __b, 1)

#define vmul_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vmulv2sf (__a, __b, 5)

#define vmul_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vmulv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vmul_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vmulv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vmul_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vmulv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vmul_p8(__a, __b) \
  (poly8x8_t)__builtin_neon_vmulv8qi ((int8x8_t) __a, (int8x8_t) __b, 4)

#define vmulq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vmulv16qi (__a, __b, 1)

#define vmulq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vmulv8hi (__a, __b, 1)

#define vmulq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vmulv4si (__a, __b, 1)

#define vmulq_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vmulv4sf (__a, __b, 5)

#define vmulq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vmulv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vmulq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vmulv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vmulq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vmulv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vmulq_p8(__a, __b) \
  (poly8x16_t)__builtin_neon_vmulv16qi ((int8x16_t) __a, (int8x16_t) __b, 4)

#define vqdmulh_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqdmulhv4hi (__a, __b, 1)

#define vqdmulh_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqdmulhv2si (__a, __b, 1)

#define vqdmulhq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqdmulhv8hi (__a, __b, 1)

#define vqdmulhq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqdmulhv4si (__a, __b, 1)

#define vqrdmulh_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqdmulhv4hi (__a, __b, 3)

#define vqrdmulh_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqdmulhv2si (__a, __b, 3)

#define vqrdmulhq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqdmulhv8hi (__a, __b, 3)

#define vqrdmulhq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqdmulhv4si (__a, __b, 3)

#define vmull_s8(__a, __b) \
  (int16x8_t)__builtin_neon_vmullv8qi (__a, __b, 1)

#define vmull_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vmullv4hi (__a, __b, 1)

#define vmull_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vmullv2si (__a, __b, 1)

#define vmull_u8(__a, __b) \
  (uint16x8_t)__builtin_neon_vmullv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vmull_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vmullv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vmull_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vmullv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vmull_p8(__a, __b) \
  (poly16x8_t)__builtin_neon_vmullv8qi ((int8x8_t) __a, (int8x8_t) __b, 4)

#define vqdmull_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vqdmullv4hi (__a, __b, 1)

#define vqdmull_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vqdmullv2si (__a, __b, 1)

#define vmla_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vmlav8qi (__a, __b, __c, 1)

#define vmla_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vmlav4hi (__a, __b, __c, 1)

#define vmla_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vmlav2si (__a, __b, __c, 1)

#define vmla_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vmlav2sf (__a, __b, __c, 5)

#define vmla_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vmlav8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0)

#define vmla_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vmlav4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0)

#define vmla_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vmlav2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0)

#define vmlaq_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vmlav16qi (__a, __b, __c, 1)

#define vmlaq_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vmlav8hi (__a, __b, __c, 1)

#define vmlaq_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmlav4si (__a, __b, __c, 1)

#define vmlaq_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vmlav4sf (__a, __b, __c, 5)

#define vmlaq_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vmlav16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c, 0)

#define vmlaq_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vmlav8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c, 0)

#define vmlaq_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmlav4si ((int32x4_t) __a, (int32x4_t) __b, (int32x4_t) __c, 0)

#define vmlal_s8(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vmlalv8qi (__a, __b, __c, 1)

#define vmlal_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmlalv4hi (__a, __b, __c, 1)

#define vmlal_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vmlalv2si (__a, __b, __c, 1)

#define vmlal_u8(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vmlalv8qi ((int16x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0)

#define vmlal_u16(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmlalv4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0)

#define vmlal_u32(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vmlalv2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0)

#define vqdmlal_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vqdmlalv4hi (__a, __b, __c, 1)

#define vqdmlal_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vqdmlalv2si (__a, __b, __c, 1)

#define vmls_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vmlsv8qi (__a, __b, __c, 1)

#define vmls_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vmlsv4hi (__a, __b, __c, 1)

#define vmls_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vmlsv2si (__a, __b, __c, 1)

#define vmls_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vmlsv2sf (__a, __b, __c, 5)

#define vmls_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vmlsv8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0)

#define vmls_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vmlsv4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0)

#define vmls_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vmlsv2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0)

#define vmlsq_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vmlsv16qi (__a, __b, __c, 1)

#define vmlsq_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vmlsv8hi (__a, __b, __c, 1)

#define vmlsq_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmlsv4si (__a, __b, __c, 1)

#define vmlsq_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vmlsv4sf (__a, __b, __c, 5)

#define vmlsq_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vmlsv16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c, 0)

#define vmlsq_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vmlsv8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c, 0)

#define vmlsq_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmlsv4si ((int32x4_t) __a, (int32x4_t) __b, (int32x4_t) __c, 0)

#define vmlsl_s8(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vmlslv8qi (__a, __b, __c, 1)

#define vmlsl_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmlslv4hi (__a, __b, __c, 1)

#define vmlsl_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vmlslv2si (__a, __b, __c, 1)

#define vmlsl_u8(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vmlslv8qi ((int16x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0)

#define vmlsl_u16(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmlslv4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0)

#define vmlsl_u32(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vmlslv2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0)

#define vqdmlsl_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vqdmlslv4hi (__a, __b, __c, 1)

#define vqdmlsl_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vqdmlslv2si (__a, __b, __c, 1)

#define vsub_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vsubv8qi (__a, __b, 1)

#define vsub_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vsubv4hi (__a, __b, 1)

#define vsub_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vsubv2si (__a, __b, 1)

#define vsub_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vsubv1di (__a, __b, 1)

#define vsub_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vsubv2sf (__a, __b, 5)

#define vsub_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vsubv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vsub_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vsubv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vsub_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vsubv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vsub_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vsubv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define vsubq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vsubv16qi (__a, __b, 1)

#define vsubq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vsubv8hi (__a, __b, 1)

#define vsubq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vsubv4si (__a, __b, 1)

#define vsubq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vsubv2di (__a, __b, 1)

#define vsubq_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vsubv4sf (__a, __b, 5)

#define vsubq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vsubv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vsubq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vsubv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vsubq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vsubv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vsubq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vsubv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vsubl_s8(__a, __b) \
  (int16x8_t)__builtin_neon_vsublv8qi (__a, __b, 1)

#define vsubl_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vsublv4hi (__a, __b, 1)

#define vsubl_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vsublv2si (__a, __b, 1)

#define vsubl_u8(__a, __b) \
  (uint16x8_t)__builtin_neon_vsublv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vsubl_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vsublv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vsubl_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vsublv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vsubw_s8(__a, __b) \
  (int16x8_t)__builtin_neon_vsubwv8qi (__a, __b, 1)

#define vsubw_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vsubwv4hi (__a, __b, 1)

#define vsubw_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vsubwv2si (__a, __b, 1)

#define vsubw_u8(__a, __b) \
  (uint16x8_t)__builtin_neon_vsubwv8qi ((int16x8_t) __a, (int8x8_t) __b, 0)

#define vsubw_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vsubwv4hi ((int32x4_t) __a, (int16x4_t) __b, 0)

#define vsubw_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vsubwv2si ((int64x2_t) __a, (int32x2_t) __b, 0)

#define vhsub_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vhsubv8qi (__a, __b, 1)

#define vhsub_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vhsubv4hi (__a, __b, 1)

#define vhsub_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vhsubv2si (__a, __b, 1)

#define vhsub_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vhsubv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vhsub_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vhsubv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vhsub_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vhsubv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vhsubq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vhsubv16qi (__a, __b, 1)

#define vhsubq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vhsubv8hi (__a, __b, 1)

#define vhsubq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vhsubv4si (__a, __b, 1)

#define vhsubq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vhsubv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vhsubq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vhsubv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vhsubq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vhsubv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vqsub_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vqsubv8qi (__a, __b, 1)

#define vqsub_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqsubv4hi (__a, __b, 1)

#define vqsub_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqsubv2si (__a, __b, 1)

#define vqsub_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vqsubv1di (__a, __b, 1)

#define vqsub_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vqsubv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vqsub_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vqsubv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vqsub_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vqsubv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vqsub_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vqsubv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define vqsubq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vqsubv16qi (__a, __b, 1)

#define vqsubq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqsubv8hi (__a, __b, 1)

#define vqsubq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqsubv4si (__a, __b, 1)

#define vqsubq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vqsubv2di (__a, __b, 1)

#define vqsubq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vqsubv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vqsubq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vqsubv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vqsubq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vqsubv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vqsubq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vqsubv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vsubhn_s16(__a, __b) \
  (int8x8_t)__builtin_neon_vsubhnv8hi (__a, __b, 1)

#define vsubhn_s32(__a, __b) \
  (int16x4_t)__builtin_neon_vsubhnv4si (__a, __b, 1)

#define vsubhn_s64(__a, __b) \
  (int32x2_t)__builtin_neon_vsubhnv2di (__a, __b, 1)

#define vsubhn_u16(__a, __b) \
  (uint8x8_t)__builtin_neon_vsubhnv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vsubhn_u32(__a, __b) \
  (uint16x4_t)__builtin_neon_vsubhnv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vsubhn_u64(__a, __b) \
  (uint32x2_t)__builtin_neon_vsubhnv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vrsubhn_s16(__a, __b) \
  (int8x8_t)__builtin_neon_vsubhnv8hi (__a, __b, 3)

#define vrsubhn_s32(__a, __b) \
  (int16x4_t)__builtin_neon_vsubhnv4si (__a, __b, 3)

#define vrsubhn_s64(__a, __b) \
  (int32x2_t)__builtin_neon_vsubhnv2di (__a, __b, 3)

#define vrsubhn_u16(__a, __b) \
  (uint8x8_t)__builtin_neon_vsubhnv8hi ((int16x8_t) __a, (int16x8_t) __b, 2)

#define vrsubhn_u32(__a, __b) \
  (uint16x4_t)__builtin_neon_vsubhnv4si ((int32x4_t) __a, (int32x4_t) __b, 2)

#define vrsubhn_u64(__a, __b) \
  (uint32x2_t)__builtin_neon_vsubhnv2di ((int64x2_t) __a, (int64x2_t) __b, 2)

#define vceq_s8(__a, __b) \
  (uint8x8_t)__builtin_neon_vceqv8qi (__a, __b, 1)

#define vceq_s16(__a, __b) \
  (uint16x4_t)__builtin_neon_vceqv4hi (__a, __b, 1)

#define vceq_s32(__a, __b) \
  (uint32x2_t)__builtin_neon_vceqv2si (__a, __b, 1)

#define vceq_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vceqv2sf (__a, __b, 5)

#define vceq_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vceqv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vceq_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vceqv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vceq_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vceqv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vceq_p8(__a, __b) \
  (uint8x8_t)__builtin_neon_vceqv8qi ((int8x8_t) __a, (int8x8_t) __b, 4)

#define vceqq_s8(__a, __b) \
  (uint8x16_t)__builtin_neon_vceqv16qi (__a, __b, 1)

#define vceqq_s16(__a, __b) \
  (uint16x8_t)__builtin_neon_vceqv8hi (__a, __b, 1)

#define vceqq_s32(__a, __b) \
  (uint32x4_t)__builtin_neon_vceqv4si (__a, __b, 1)

#define vceqq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vceqv4sf (__a, __b, 5)

#define vceqq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vceqv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vceqq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vceqv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vceqq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vceqv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vceqq_p8(__a, __b) \
  (uint8x16_t)__builtin_neon_vceqv16qi ((int8x16_t) __a, (int8x16_t) __b, 4)

#define vcge_s8(__a, __b) \
  (uint8x8_t)__builtin_neon_vcgev8qi (__a, __b, 1)

#define vcge_s16(__a, __b) \
  (uint16x4_t)__builtin_neon_vcgev4hi (__a, __b, 1)

#define vcge_s32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgev2si (__a, __b, 1)

#define vcge_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgev2sf (__a, __b, 5)

#define vcge_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vcgev8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vcge_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vcgev4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vcge_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgev2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vcgeq_s8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcgev16qi (__a, __b, 1)

#define vcgeq_s16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcgev8hi (__a, __b, 1)

#define vcgeq_s32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgev4si (__a, __b, 1)

#define vcgeq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgev4sf (__a, __b, 5)

#define vcgeq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcgev16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vcgeq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcgev8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vcgeq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgev4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vcle_s8(__a, __b) \
  (uint8x8_t)__builtin_neon_vcgev8qi (__b, __a, 1)

#define vcle_s16(__a, __b) \
  (uint16x4_t)__builtin_neon_vcgev4hi (__b, __a, 1)

#define vcle_s32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgev2si (__b, __a, 1)

#define vcle_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgev2sf (__b, __a, 5)

#define vcle_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vcgev8qi ((int8x8_t) __b, (int8x8_t) __a, 0)

#define vcle_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vcgev4hi ((int16x4_t) __b, (int16x4_t) __a, 0)

#define vcle_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgev2si ((int32x2_t) __b, (int32x2_t) __a, 0)

#define vcleq_s8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcgev16qi (__b, __a, 1)

#define vcleq_s16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcgev8hi (__b, __a, 1)

#define vcleq_s32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgev4si (__b, __a, 1)

#define vcleq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgev4sf (__b, __a, 5)

#define vcleq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcgev16qi ((int8x16_t) __b, (int8x16_t) __a, 0)

#define vcleq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcgev8hi ((int16x8_t) __b, (int16x8_t) __a, 0)

#define vcleq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgev4si ((int32x4_t) __b, (int32x4_t) __a, 0)

#define vcgt_s8(__a, __b) \
  (uint8x8_t)__builtin_neon_vcgtv8qi (__a, __b, 1)

#define vcgt_s16(__a, __b) \
  (uint16x4_t)__builtin_neon_vcgtv4hi (__a, __b, 1)

#define vcgt_s32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgtv2si (__a, __b, 1)

#define vcgt_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgtv2sf (__a, __b, 5)

#define vcgt_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vcgtv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vcgt_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vcgtv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vcgt_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgtv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vcgtq_s8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcgtv16qi (__a, __b, 1)

#define vcgtq_s16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcgtv8hi (__a, __b, 1)

#define vcgtq_s32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgtv4si (__a, __b, 1)

#define vcgtq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgtv4sf (__a, __b, 5)

#define vcgtq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcgtv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vcgtq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcgtv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vcgtq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgtv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vclt_s8(__a, __b) \
  (uint8x8_t)__builtin_neon_vcgtv8qi (__b, __a, 1)

#define vclt_s16(__a, __b) \
  (uint16x4_t)__builtin_neon_vcgtv4hi (__b, __a, 1)

#define vclt_s32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgtv2si (__b, __a, 1)

#define vclt_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgtv2sf (__b, __a, 5)

#define vclt_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vcgtv8qi ((int8x8_t) __b, (int8x8_t) __a, 0)

#define vclt_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vcgtv4hi ((int16x4_t) __b, (int16x4_t) __a, 0)

#define vclt_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcgtv2si ((int32x2_t) __b, (int32x2_t) __a, 0)

#define vcltq_s8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcgtv16qi (__b, __a, 1)

#define vcltq_s16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcgtv8hi (__b, __a, 1)

#define vcltq_s32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgtv4si (__b, __a, 1)

#define vcltq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgtv4sf (__b, __a, 5)

#define vcltq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcgtv16qi ((int8x16_t) __b, (int8x16_t) __a, 0)

#define vcltq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcgtv8hi ((int16x8_t) __b, (int16x8_t) __a, 0)

#define vcltq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcgtv4si ((int32x4_t) __b, (int32x4_t) __a, 0)

#define vcage_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcagev2sf (__a, __b, 5)

#define vcageq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcagev4sf (__a, __b, 5)

#define vcale_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcagev2sf (__b, __a, 5)

#define vcaleq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcagev4sf (__b, __a, 5)

#define vcagt_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcagtv2sf (__a, __b, 5)

#define vcagtq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcagtv4sf (__a, __b, 5)

#define vcalt_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcagtv2sf (__b, __a, 5)

#define vcaltq_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcagtv4sf (__b, __a, 5)

#define vtst_s8(__a, __b) \
  (uint8x8_t)__builtin_neon_vtstv8qi (__a, __b, 1)

#define vtst_s16(__a, __b) \
  (uint16x4_t)__builtin_neon_vtstv4hi (__a, __b, 1)

#define vtst_s32(__a, __b) \
  (uint32x2_t)__builtin_neon_vtstv2si (__a, __b, 1)

#define vtst_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vtstv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vtst_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vtstv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vtst_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vtstv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vtst_p8(__a, __b) \
  (uint8x8_t)__builtin_neon_vtstv8qi ((int8x8_t) __a, (int8x8_t) __b, 4)

#define vtstq_s8(__a, __b) \
  (uint8x16_t)__builtin_neon_vtstv16qi (__a, __b, 1)

#define vtstq_s16(__a, __b) \
  (uint16x8_t)__builtin_neon_vtstv8hi (__a, __b, 1)

#define vtstq_s32(__a, __b) \
  (uint32x4_t)__builtin_neon_vtstv4si (__a, __b, 1)

#define vtstq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vtstv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vtstq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vtstv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vtstq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vtstv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vtstq_p8(__a, __b) \
  (uint8x16_t)__builtin_neon_vtstv16qi ((int8x16_t) __a, (int8x16_t) __b, 4)

#define vabd_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vabdv8qi (__a, __b, 1)

#define vabd_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vabdv4hi (__a, __b, 1)

#define vabd_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vabdv2si (__a, __b, 1)

#define vabd_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vabdv2sf (__a, __b, 5)

#define vabd_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vabdv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vabd_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vabdv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vabd_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vabdv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vabdq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vabdv16qi (__a, __b, 1)

#define vabdq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vabdv8hi (__a, __b, 1)

#define vabdq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vabdv4si (__a, __b, 1)

#define vabdq_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vabdv4sf (__a, __b, 5)

#define vabdq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vabdv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vabdq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vabdv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vabdq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vabdv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vabdl_s8(__a, __b) \
  (int16x8_t)__builtin_neon_vabdlv8qi (__a, __b, 1)

#define vabdl_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vabdlv4hi (__a, __b, 1)

#define vabdl_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vabdlv2si (__a, __b, 1)

#define vabdl_u8(__a, __b) \
  (uint16x8_t)__builtin_neon_vabdlv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vabdl_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vabdlv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vabdl_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vabdlv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vaba_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vabav8qi (__a, __b, __c, 1)

#define vaba_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vabav4hi (__a, __b, __c, 1)

#define vaba_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vabav2si (__a, __b, __c, 1)

#define vaba_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vabav8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0)

#define vaba_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vabav4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0)

#define vaba_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vabav2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0)

#define vabaq_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vabav16qi (__a, __b, __c, 1)

#define vabaq_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vabav8hi (__a, __b, __c, 1)

#define vabaq_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vabav4si (__a, __b, __c, 1)

#define vabaq_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vabav16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c, 0)

#define vabaq_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vabav8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c, 0)

#define vabaq_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vabav4si ((int32x4_t) __a, (int32x4_t) __b, (int32x4_t) __c, 0)

#define vabal_s8(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vabalv8qi (__a, __b, __c, 1)

#define vabal_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vabalv4hi (__a, __b, __c, 1)

#define vabal_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vabalv2si (__a, __b, __c, 1)

#define vabal_u8(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vabalv8qi ((int16x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0)

#define vabal_u16(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vabalv4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0)

#define vabal_u32(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vabalv2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0)

#define vmax_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vmaxv8qi (__a, __b, 1)

#define vmax_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vmaxv4hi (__a, __b, 1)

#define vmax_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vmaxv2si (__a, __b, 1)

#define vmax_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vmaxv2sf (__a, __b, 5)

#define vmax_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vmaxv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vmax_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vmaxv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vmax_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vmaxv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vmaxq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vmaxv16qi (__a, __b, 1)

#define vmaxq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vmaxv8hi (__a, __b, 1)

#define vmaxq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vmaxv4si (__a, __b, 1)

#define vmaxq_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vmaxv4sf (__a, __b, 5)

#define vmaxq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vmaxv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vmaxq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vmaxv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vmaxq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vmaxv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vmin_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vminv8qi (__a, __b, 1)

#define vmin_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vminv4hi (__a, __b, 1)

#define vmin_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vminv2si (__a, __b, 1)

#define vmin_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vminv2sf (__a, __b, 5)

#define vmin_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vminv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vmin_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vminv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vmin_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vminv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vminq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vminv16qi (__a, __b, 1)

#define vminq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vminv8hi (__a, __b, 1)

#define vminq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vminv4si (__a, __b, 1)

#define vminq_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vminv4sf (__a, __b, 5)

#define vminq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vminv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vminq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vminv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vminq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vminv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vpadd_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vpaddv8qi (__a, __b, 1)

#define vpadd_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vpaddv4hi (__a, __b, 1)

#define vpadd_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vpaddv2si (__a, __b, 1)

#define vpadd_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vpaddv2sf (__a, __b, 5)

#define vpadd_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vpaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vpadd_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vpaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vpadd_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vpaddv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vpaddl_s8(__a) \
  (int16x4_t)__builtin_neon_vpaddlv8qi (__a, 1)

#define vpaddl_s16(__a) \
  (int32x2_t)__builtin_neon_vpaddlv4hi (__a, 1)

#define vpaddl_s32(__a) \
  (int64x1_t)__builtin_neon_vpaddlv2si (__a, 1)

#define vpaddl_u8(__a) \
  (uint16x4_t)__builtin_neon_vpaddlv8qi ((int8x8_t) __a, 0)

#define vpaddl_u16(__a) \
  (uint32x2_t)__builtin_neon_vpaddlv4hi ((int16x4_t) __a, 0)

#define vpaddl_u32(__a) \
  (uint64x1_t)__builtin_neon_vpaddlv2si ((int32x2_t) __a, 0)

#define vpaddlq_s8(__a) \
  (int16x8_t)__builtin_neon_vpaddlv16qi (__a, 1)

#define vpaddlq_s16(__a) \
  (int32x4_t)__builtin_neon_vpaddlv8hi (__a, 1)

#define vpaddlq_s32(__a) \
  (int64x2_t)__builtin_neon_vpaddlv4si (__a, 1)

#define vpaddlq_u8(__a) \
  (uint16x8_t)__builtin_neon_vpaddlv16qi ((int8x16_t) __a, 0)

#define vpaddlq_u16(__a) \
  (uint32x4_t)__builtin_neon_vpaddlv8hi ((int16x8_t) __a, 0)

#define vpaddlq_u32(__a) \
  (uint64x2_t)__builtin_neon_vpaddlv4si ((int32x4_t) __a, 0)

#define vpadal_s8(__a, __b) \
  (int16x4_t)__builtin_neon_vpadalv8qi (__a, __b, 1)

#define vpadal_s16(__a, __b) \
  (int32x2_t)__builtin_neon_vpadalv4hi (__a, __b, 1)

#define vpadal_s32(__a, __b) \
  (int64x1_t)__builtin_neon_vpadalv2si (__a, __b, 1)

#define vpadal_u8(__a, __b) \
  (uint16x4_t)__builtin_neon_vpadalv8qi ((int16x4_t) __a, (int8x8_t) __b, 0)

#define vpadal_u16(__a, __b) \
  (uint32x2_t)__builtin_neon_vpadalv4hi ((int32x2_t) __a, (int16x4_t) __b, 0)

#define vpadal_u32(__a, __b) \
  (uint64x1_t)__builtin_neon_vpadalv2si ((int64x1_t) __a, (int32x2_t) __b, 0)

#define vpadalq_s8(__a, __b) \
  (int16x8_t)__builtin_neon_vpadalv16qi (__a, __b, 1)

#define vpadalq_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vpadalv8hi (__a, __b, 1)

#define vpadalq_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vpadalv4si (__a, __b, 1)

#define vpadalq_u8(__a, __b) \
  (uint16x8_t)__builtin_neon_vpadalv16qi ((int16x8_t) __a, (int8x16_t) __b, 0)

#define vpadalq_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vpadalv8hi ((int32x4_t) __a, (int16x8_t) __b, 0)

#define vpadalq_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vpadalv4si ((int64x2_t) __a, (int32x4_t) __b, 0)

#define vpmax_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vpmaxv8qi (__a, __b, 1)

#define vpmax_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vpmaxv4hi (__a, __b, 1)

#define vpmax_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vpmaxv2si (__a, __b, 1)

#define vpmax_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vpmaxv2sf (__a, __b, 5)

#define vpmax_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vpmaxv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vpmax_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vpmaxv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vpmax_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vpmaxv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vpmin_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vpminv8qi (__a, __b, 1)

#define vpmin_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vpminv4hi (__a, __b, 1)

#define vpmin_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vpminv2si (__a, __b, 1)

#define vpmin_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vpminv2sf (__a, __b, 5)

#define vpmin_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vpminv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vpmin_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vpminv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vpmin_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vpminv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vrecps_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vrecpsv2sf (__a, __b, 5)

#define vrecpsq_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vrecpsv4sf (__a, __b, 5)

#define vrsqrts_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vrsqrtsv2sf (__a, __b, 5)

#define vrsqrtsq_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vrsqrtsv4sf (__a, __b, 5)

#define vshl_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vshlv8qi (__a, __b, 1)

#define vshl_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vshlv4hi (__a, __b, 1)

#define vshl_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vshlv2si (__a, __b, 1)

#define vshl_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vshlv1di (__a, __b, 1)

#define vshl_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vshlv8qi ((int8x8_t) __a, __b, 0)

#define vshl_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vshlv4hi ((int16x4_t) __a, __b, 0)

#define vshl_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vshlv2si ((int32x2_t) __a, __b, 0)

#define vshl_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vshlv1di ((int64x1_t) __a, __b, 0)

#define vshlq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vshlv16qi (__a, __b, 1)

#define vshlq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vshlv8hi (__a, __b, 1)

#define vshlq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vshlv4si (__a, __b, 1)

#define vshlq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vshlv2di (__a, __b, 1)

#define vshlq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vshlv16qi ((int8x16_t) __a, __b, 0)

#define vshlq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vshlv8hi ((int16x8_t) __a, __b, 0)

#define vshlq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vshlv4si ((int32x4_t) __a, __b, 0)

#define vshlq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vshlv2di ((int64x2_t) __a, __b, 0)

#define vrshl_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vshlv8qi (__a, __b, 3)

#define vrshl_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vshlv4hi (__a, __b, 3)

#define vrshl_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vshlv2si (__a, __b, 3)

#define vrshl_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vshlv1di (__a, __b, 3)

#define vrshl_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vshlv8qi ((int8x8_t) __a, __b, 2)

#define vrshl_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vshlv4hi ((int16x4_t) __a, __b, 2)

#define vrshl_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vshlv2si ((int32x2_t) __a, __b, 2)

#define vrshl_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vshlv1di ((int64x1_t) __a, __b, 2)

#define vrshlq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vshlv16qi (__a, __b, 3)

#define vrshlq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vshlv8hi (__a, __b, 3)

#define vrshlq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vshlv4si (__a, __b, 3)

#define vrshlq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vshlv2di (__a, __b, 3)

#define vrshlq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vshlv16qi ((int8x16_t) __a, __b, 2)

#define vrshlq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vshlv8hi ((int16x8_t) __a, __b, 2)

#define vrshlq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vshlv4si ((int32x4_t) __a, __b, 2)

#define vrshlq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vshlv2di ((int64x2_t) __a, __b, 2)

#define vqshl_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vqshlv8qi (__a, __b, 1)

#define vqshl_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqshlv4hi (__a, __b, 1)

#define vqshl_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqshlv2si (__a, __b, 1)

#define vqshl_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vqshlv1di (__a, __b, 1)

#define vqshl_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vqshlv8qi ((int8x8_t) __a, __b, 0)

#define vqshl_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vqshlv4hi ((int16x4_t) __a, __b, 0)

#define vqshl_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vqshlv2si ((int32x2_t) __a, __b, 0)

#define vqshl_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vqshlv1di ((int64x1_t) __a, __b, 0)

#define vqshlq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vqshlv16qi (__a, __b, 1)

#define vqshlq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqshlv8hi (__a, __b, 1)

#define vqshlq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqshlv4si (__a, __b, 1)

#define vqshlq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vqshlv2di (__a, __b, 1)

#define vqshlq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vqshlv16qi ((int8x16_t) __a, __b, 0)

#define vqshlq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vqshlv8hi ((int16x8_t) __a, __b, 0)

#define vqshlq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vqshlv4si ((int32x4_t) __a, __b, 0)

#define vqshlq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vqshlv2di ((int64x2_t) __a, __b, 0)

#define vqrshl_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vqshlv8qi (__a, __b, 3)

#define vqrshl_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqshlv4hi (__a, __b, 3)

#define vqrshl_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqshlv2si (__a, __b, 3)

#define vqrshl_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vqshlv1di (__a, __b, 3)

#define vqrshl_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vqshlv8qi ((int8x8_t) __a, __b, 2)

#define vqrshl_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vqshlv4hi ((int16x4_t) __a, __b, 2)

#define vqrshl_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vqshlv2si ((int32x2_t) __a, __b, 2)

#define vqrshl_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vqshlv1di ((int64x1_t) __a, __b, 2)

#define vqrshlq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vqshlv16qi (__a, __b, 3)

#define vqrshlq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqshlv8hi (__a, __b, 3)

#define vqrshlq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqshlv4si (__a, __b, 3)

#define vqrshlq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vqshlv2di (__a, __b, 3)

#define vqrshlq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vqshlv16qi ((int8x16_t) __a, __b, 2)

#define vqrshlq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vqshlv8hi ((int16x8_t) __a, __b, 2)

#define vqrshlq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vqshlv4si ((int32x4_t) __a, __b, 2)

#define vqrshlq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vqshlv2di ((int64x2_t) __a, __b, 2)

#define vshr_n_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vshr_nv8qi (__a, __b, 1)

#define vshr_n_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vshr_nv4hi (__a, __b, 1)

#define vshr_n_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vshr_nv2si (__a, __b, 1)

#define vshr_n_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vshr_nv1di (__a, __b, 1)

#define vshr_n_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vshr_nv8qi ((int8x8_t) __a, __b, 0)

#define vshr_n_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vshr_nv4hi ((int16x4_t) __a, __b, 0)

#define vshr_n_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vshr_nv2si ((int32x2_t) __a, __b, 0)

#define vshr_n_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vshr_nv1di ((int64x1_t) __a, __b, 0)

#define vshrq_n_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vshr_nv16qi (__a, __b, 1)

#define vshrq_n_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vshr_nv8hi (__a, __b, 1)

#define vshrq_n_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vshr_nv4si (__a, __b, 1)

#define vshrq_n_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vshr_nv2di (__a, __b, 1)

#define vshrq_n_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vshr_nv16qi ((int8x16_t) __a, __b, 0)

#define vshrq_n_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vshr_nv8hi ((int16x8_t) __a, __b, 0)

#define vshrq_n_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vshr_nv4si ((int32x4_t) __a, __b, 0)

#define vshrq_n_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vshr_nv2di ((int64x2_t) __a, __b, 0)

#define vrshr_n_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vshr_nv8qi (__a, __b, 3)

#define vrshr_n_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vshr_nv4hi (__a, __b, 3)

#define vrshr_n_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vshr_nv2si (__a, __b, 3)

#define vrshr_n_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vshr_nv1di (__a, __b, 3)

#define vrshr_n_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vshr_nv8qi ((int8x8_t) __a, __b, 2)

#define vrshr_n_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vshr_nv4hi ((int16x4_t) __a, __b, 2)

#define vrshr_n_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vshr_nv2si ((int32x2_t) __a, __b, 2)

#define vrshr_n_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vshr_nv1di ((int64x1_t) __a, __b, 2)

#define vrshrq_n_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vshr_nv16qi (__a, __b, 3)

#define vrshrq_n_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vshr_nv8hi (__a, __b, 3)

#define vrshrq_n_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vshr_nv4si (__a, __b, 3)

#define vrshrq_n_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vshr_nv2di (__a, __b, 3)

#define vrshrq_n_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vshr_nv16qi ((int8x16_t) __a, __b, 2)

#define vrshrq_n_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vshr_nv8hi ((int16x8_t) __a, __b, 2)

#define vrshrq_n_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vshr_nv4si ((int32x4_t) __a, __b, 2)

#define vrshrq_n_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vshr_nv2di ((int64x2_t) __a, __b, 2)

#define vshrn_n_s16(__a, __b) \
  (int8x8_t)__builtin_neon_vshrn_nv8hi (__a, __b, 1)

#define vshrn_n_s32(__a, __b) \
  (int16x4_t)__builtin_neon_vshrn_nv4si (__a, __b, 1)

#define vshrn_n_s64(__a, __b) \
  (int32x2_t)__builtin_neon_vshrn_nv2di (__a, __b, 1)

#define vshrn_n_u16(__a, __b) \
  (uint8x8_t)__builtin_neon_vshrn_nv8hi ((int16x8_t) __a, __b, 0)

#define vshrn_n_u32(__a, __b) \
  (uint16x4_t)__builtin_neon_vshrn_nv4si ((int32x4_t) __a, __b, 0)

#define vshrn_n_u64(__a, __b) \
  (uint32x2_t)__builtin_neon_vshrn_nv2di ((int64x2_t) __a, __b, 0)

#define vrshrn_n_s16(__a, __b) \
  (int8x8_t)__builtin_neon_vshrn_nv8hi (__a, __b, 3)

#define vrshrn_n_s32(__a, __b) \
  (int16x4_t)__builtin_neon_vshrn_nv4si (__a, __b, 3)

#define vrshrn_n_s64(__a, __b) \
  (int32x2_t)__builtin_neon_vshrn_nv2di (__a, __b, 3)

#define vrshrn_n_u16(__a, __b) \
  (uint8x8_t)__builtin_neon_vshrn_nv8hi ((int16x8_t) __a, __b, 2)

#define vrshrn_n_u32(__a, __b) \
  (uint16x4_t)__builtin_neon_vshrn_nv4si ((int32x4_t) __a, __b, 2)

#define vrshrn_n_u64(__a, __b) \
  (uint32x2_t)__builtin_neon_vshrn_nv2di ((int64x2_t) __a, __b, 2)

#define vqshrn_n_s16(__a, __b) \
  (int8x8_t)__builtin_neon_vqshrn_nv8hi (__a, __b, 1)

#define vqshrn_n_s32(__a, __b) \
  (int16x4_t)__builtin_neon_vqshrn_nv4si (__a, __b, 1)

#define vqshrn_n_s64(__a, __b) \
  (int32x2_t)__builtin_neon_vqshrn_nv2di (__a, __b, 1)

#define vqshrn_n_u16(__a, __b) \
  (uint8x8_t)__builtin_neon_vqshrn_nv8hi ((int16x8_t) __a, __b, 0)

#define vqshrn_n_u32(__a, __b) \
  (uint16x4_t)__builtin_neon_vqshrn_nv4si ((int32x4_t) __a, __b, 0)

#define vqshrn_n_u64(__a, __b) \
  (uint32x2_t)__builtin_neon_vqshrn_nv2di ((int64x2_t) __a, __b, 0)

#define vqrshrn_n_s16(__a, __b) \
  (int8x8_t)__builtin_neon_vqshrn_nv8hi (__a, __b, 3)

#define vqrshrn_n_s32(__a, __b) \
  (int16x4_t)__builtin_neon_vqshrn_nv4si (__a, __b, 3)

#define vqrshrn_n_s64(__a, __b) \
  (int32x2_t)__builtin_neon_vqshrn_nv2di (__a, __b, 3)

#define vqrshrn_n_u16(__a, __b) \
  (uint8x8_t)__builtin_neon_vqshrn_nv8hi ((int16x8_t) __a, __b, 2)

#define vqrshrn_n_u32(__a, __b) \
  (uint16x4_t)__builtin_neon_vqshrn_nv4si ((int32x4_t) __a, __b, 2)

#define vqrshrn_n_u64(__a, __b) \
  (uint32x2_t)__builtin_neon_vqshrn_nv2di ((int64x2_t) __a, __b, 2)

#define vqshrun_n_s16(__a, __b) \
  (uint8x8_t)__builtin_neon_vqshrun_nv8hi (__a, __b, 1)

#define vqshrun_n_s32(__a, __b) \
  (uint16x4_t)__builtin_neon_vqshrun_nv4si (__a, __b, 1)

#define vqshrun_n_s64(__a, __b) \
  (uint32x2_t)__builtin_neon_vqshrun_nv2di (__a, __b, 1)

#define vqrshrun_n_s16(__a, __b) \
  (uint8x8_t)__builtin_neon_vqshrun_nv8hi (__a, __b, 3)

#define vqrshrun_n_s32(__a, __b) \
  (uint16x4_t)__builtin_neon_vqshrun_nv4si (__a, __b, 3)

#define vqrshrun_n_s64(__a, __b) \
  (uint32x2_t)__builtin_neon_vqshrun_nv2di (__a, __b, 3)

#define vshl_n_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vshl_nv8qi (__a, __b, 1)

#define vshl_n_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vshl_nv4hi (__a, __b, 1)

#define vshl_n_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vshl_nv2si (__a, __b, 1)

#define vshl_n_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vshl_nv1di (__a, __b, 1)

#define vshl_n_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vshl_nv8qi ((int8x8_t) __a, __b, 0)

#define vshl_n_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vshl_nv4hi ((int16x4_t) __a, __b, 0)

#define vshl_n_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vshl_nv2si ((int32x2_t) __a, __b, 0)

#define vshl_n_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vshl_nv1di ((int64x1_t) __a, __b, 0)

#define vshlq_n_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vshl_nv16qi (__a, __b, 1)

#define vshlq_n_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vshl_nv8hi (__a, __b, 1)

#define vshlq_n_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vshl_nv4si (__a, __b, 1)

#define vshlq_n_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vshl_nv2di (__a, __b, 1)

#define vshlq_n_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vshl_nv16qi ((int8x16_t) __a, __b, 0)

#define vshlq_n_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vshl_nv8hi ((int16x8_t) __a, __b, 0)

#define vshlq_n_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vshl_nv4si ((int32x4_t) __a, __b, 0)

#define vshlq_n_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vshl_nv2di ((int64x2_t) __a, __b, 0)

#define vqshl_n_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vqshl_nv8qi (__a, __b, 1)

#define vqshl_n_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqshl_nv4hi (__a, __b, 1)

#define vqshl_n_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqshl_nv2si (__a, __b, 1)

#define vqshl_n_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vqshl_nv1di (__a, __b, 1)

#define vqshl_n_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vqshl_nv8qi ((int8x8_t) __a, __b, 0)

#define vqshl_n_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vqshl_nv4hi ((int16x4_t) __a, __b, 0)

#define vqshl_n_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vqshl_nv2si ((int32x2_t) __a, __b, 0)

#define vqshl_n_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vqshl_nv1di ((int64x1_t) __a, __b, 0)

#define vqshlq_n_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vqshl_nv16qi (__a, __b, 1)

#define vqshlq_n_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqshl_nv8hi (__a, __b, 1)

#define vqshlq_n_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqshl_nv4si (__a, __b, 1)

#define vqshlq_n_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vqshl_nv2di (__a, __b, 1)

#define vqshlq_n_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vqshl_nv16qi ((int8x16_t) __a, __b, 0)

#define vqshlq_n_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vqshl_nv8hi ((int16x8_t) __a, __b, 0)

#define vqshlq_n_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vqshl_nv4si ((int32x4_t) __a, __b, 0)

#define vqshlq_n_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vqshl_nv2di ((int64x2_t) __a, __b, 0)

#define vqshlu_n_s8(__a, __b) \
  (uint8x8_t)__builtin_neon_vqshlu_nv8qi (__a, __b, 1)

#define vqshlu_n_s16(__a, __b) \
  (uint16x4_t)__builtin_neon_vqshlu_nv4hi (__a, __b, 1)

#define vqshlu_n_s32(__a, __b) \
  (uint32x2_t)__builtin_neon_vqshlu_nv2si (__a, __b, 1)

#define vqshlu_n_s64(__a, __b) \
  (uint64x1_t)__builtin_neon_vqshlu_nv1di (__a, __b, 1)

#define vqshluq_n_s8(__a, __b) \
  (uint8x16_t)__builtin_neon_vqshlu_nv16qi (__a, __b, 1)

#define vqshluq_n_s16(__a, __b) \
  (uint16x8_t)__builtin_neon_vqshlu_nv8hi (__a, __b, 1)

#define vqshluq_n_s32(__a, __b) \
  (uint32x4_t)__builtin_neon_vqshlu_nv4si (__a, __b, 1)

#define vqshluq_n_s64(__a, __b) \
  (uint64x2_t)__builtin_neon_vqshlu_nv2di (__a, __b, 1)

#define vshll_n_s8(__a, __b) \
  (int16x8_t)__builtin_neon_vshll_nv8qi (__a, __b, 1)

#define vshll_n_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vshll_nv4hi (__a, __b, 1)

#define vshll_n_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vshll_nv2si (__a, __b, 1)

#define vshll_n_u8(__a, __b) \
  (uint16x8_t)__builtin_neon_vshll_nv8qi ((int8x8_t) __a, __b, 0)

#define vshll_n_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vshll_nv4hi ((int16x4_t) __a, __b, 0)

#define vshll_n_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vshll_nv2si ((int32x2_t) __a, __b, 0)

#define vsra_n_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vsra_nv8qi (__a, __b, __c, 1)

#define vsra_n_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vsra_nv4hi (__a, __b, __c, 1)

#define vsra_n_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vsra_nv2si (__a, __b, __c, 1)

#define vsra_n_s64(__a, __b, __c) \
  (int64x1_t)__builtin_neon_vsra_nv1di (__a, __b, __c, 1)

#define vsra_n_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vsra_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c, 0)

#define vsra_n_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vsra_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c, 0)

#define vsra_n_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vsra_nv2si ((int32x2_t) __a, (int32x2_t) __b, __c, 0)

#define vsra_n_u64(__a, __b, __c) \
  (uint64x1_t)__builtin_neon_vsra_nv1di ((int64x1_t) __a, (int64x1_t) __b, __c, 0)

#define vsraq_n_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vsra_nv16qi (__a, __b, __c, 1)

#define vsraq_n_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vsra_nv8hi (__a, __b, __c, 1)

#define vsraq_n_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vsra_nv4si (__a, __b, __c, 1)

#define vsraq_n_s64(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vsra_nv2di (__a, __b, __c, 1)

#define vsraq_n_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vsra_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c, 0)

#define vsraq_n_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vsra_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c, 0)

#define vsraq_n_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vsra_nv4si ((int32x4_t) __a, (int32x4_t) __b, __c, 0)

#define vsraq_n_u64(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vsra_nv2di ((int64x2_t) __a, (int64x2_t) __b, __c, 0)

#define vrsra_n_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vsra_nv8qi (__a, __b, __c, 3)

#define vrsra_n_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vsra_nv4hi (__a, __b, __c, 3)

#define vrsra_n_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vsra_nv2si (__a, __b, __c, 3)

#define vrsra_n_s64(__a, __b, __c) \
  (int64x1_t)__builtin_neon_vsra_nv1di (__a, __b, __c, 3)

#define vrsra_n_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vsra_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c, 2)

#define vrsra_n_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vsra_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c, 2)

#define vrsra_n_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vsra_nv2si ((int32x2_t) __a, (int32x2_t) __b, __c, 2)

#define vrsra_n_u64(__a, __b, __c) \
  (uint64x1_t)__builtin_neon_vsra_nv1di ((int64x1_t) __a, (int64x1_t) __b, __c, 2)

#define vrsraq_n_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vsra_nv16qi (__a, __b, __c, 3)

#define vrsraq_n_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vsra_nv8hi (__a, __b, __c, 3)

#define vrsraq_n_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vsra_nv4si (__a, __b, __c, 3)

#define vrsraq_n_s64(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vsra_nv2di (__a, __b, __c, 3)

#define vrsraq_n_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vsra_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c, 2)

#define vrsraq_n_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vsra_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c, 2)

#define vrsraq_n_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vsra_nv4si ((int32x4_t) __a, (int32x4_t) __b, __c, 2)

#define vrsraq_n_u64(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vsra_nv2di ((int64x2_t) __a, (int64x2_t) __b, __c, 2)

#define vsri_n_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vsri_nv8qi (__a, __b, __c)

#define vsri_n_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vsri_nv4hi (__a, __b, __c)

#define vsri_n_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vsri_nv2si (__a, __b, __c)

#define vsri_n_s64(__a, __b, __c) \
  (int64x1_t)__builtin_neon_vsri_nv1di (__a, __b, __c)

#define vsri_n_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vsri_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c)

#define vsri_n_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vsri_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c)

#define vsri_n_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vsri_nv2si ((int32x2_t) __a, (int32x2_t) __b, __c)

#define vsri_n_u64(__a, __b, __c) \
  (uint64x1_t)__builtin_neon_vsri_nv1di ((int64x1_t) __a, (int64x1_t) __b, __c)

#define vsri_n_p8(__a, __b, __c) \
  (poly8x8_t)__builtin_neon_vsri_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c)

#define vsri_n_p16(__a, __b, __c) \
  (poly16x4_t)__builtin_neon_vsri_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c)

#define vsriq_n_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vsri_nv16qi (__a, __b, __c)

#define vsriq_n_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vsri_nv8hi (__a, __b, __c)

#define vsriq_n_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vsri_nv4si (__a, __b, __c)

#define vsriq_n_s64(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vsri_nv2di (__a, __b, __c)

#define vsriq_n_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vsri_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c)

#define vsriq_n_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vsri_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c)

#define vsriq_n_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vsri_nv4si ((int32x4_t) __a, (int32x4_t) __b, __c)

#define vsriq_n_u64(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vsri_nv2di ((int64x2_t) __a, (int64x2_t) __b, __c)

#define vsriq_n_p8(__a, __b, __c) \
  (poly8x16_t)__builtin_neon_vsri_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c)

#define vsriq_n_p16(__a, __b, __c) \
  (poly16x8_t)__builtin_neon_vsri_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c)

#define vsli_n_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vsli_nv8qi (__a, __b, __c)

#define vsli_n_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vsli_nv4hi (__a, __b, __c)

#define vsli_n_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vsli_nv2si (__a, __b, __c)

#define vsli_n_s64(__a, __b, __c) \
  (int64x1_t)__builtin_neon_vsli_nv1di (__a, __b, __c)

#define vsli_n_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vsli_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c)

#define vsli_n_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vsli_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c)

#define vsli_n_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vsli_nv2si ((int32x2_t) __a, (int32x2_t) __b, __c)

#define vsli_n_u64(__a, __b, __c) \
  (uint64x1_t)__builtin_neon_vsli_nv1di ((int64x1_t) __a, (int64x1_t) __b, __c)

#define vsli_n_p8(__a, __b, __c) \
  (poly8x8_t)__builtin_neon_vsli_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c)

#define vsli_n_p16(__a, __b, __c) \
  (poly16x4_t)__builtin_neon_vsli_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c)

#define vsliq_n_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vsli_nv16qi (__a, __b, __c)

#define vsliq_n_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vsli_nv8hi (__a, __b, __c)

#define vsliq_n_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vsli_nv4si (__a, __b, __c)

#define vsliq_n_s64(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vsli_nv2di (__a, __b, __c)

#define vsliq_n_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vsli_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c)

#define vsliq_n_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vsli_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c)

#define vsliq_n_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vsli_nv4si ((int32x4_t) __a, (int32x4_t) __b, __c)

#define vsliq_n_u64(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vsli_nv2di ((int64x2_t) __a, (int64x2_t) __b, __c)

#define vsliq_n_p8(__a, __b, __c) \
  (poly8x16_t)__builtin_neon_vsli_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c)

#define vsliq_n_p16(__a, __b, __c) \
  (poly16x8_t)__builtin_neon_vsli_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c)

#define vabs_s8(__a) \
  (int8x8_t)__builtin_neon_vabsv8qi (__a, 1)

#define vabs_s16(__a) \
  (int16x4_t)__builtin_neon_vabsv4hi (__a, 1)

#define vabs_s32(__a) \
  (int32x2_t)__builtin_neon_vabsv2si (__a, 1)

#define vabs_f32(__a) \
  (float32x2_t)__builtin_neon_vabsv2sf (__a, 5)

#define vabsq_s8(__a) \
  (int8x16_t)__builtin_neon_vabsv16qi (__a, 1)

#define vabsq_s16(__a) \
  (int16x8_t)__builtin_neon_vabsv8hi (__a, 1)

#define vabsq_s32(__a) \
  (int32x4_t)__builtin_neon_vabsv4si (__a, 1)

#define vabsq_f32(__a) \
  (float32x4_t)__builtin_neon_vabsv4sf (__a, 5)

#define vqabs_s8(__a) \
  (int8x8_t)__builtin_neon_vqabsv8qi (__a, 1)

#define vqabs_s16(__a) \
  (int16x4_t)__builtin_neon_vqabsv4hi (__a, 1)

#define vqabs_s32(__a) \
  (int32x2_t)__builtin_neon_vqabsv2si (__a, 1)

#define vqabsq_s8(__a) \
  (int8x16_t)__builtin_neon_vqabsv16qi (__a, 1)

#define vqabsq_s16(__a) \
  (int16x8_t)__builtin_neon_vqabsv8hi (__a, 1)

#define vqabsq_s32(__a) \
  (int32x4_t)__builtin_neon_vqabsv4si (__a, 1)

#define vneg_s8(__a) \
  (int8x8_t)__builtin_neon_vnegv8qi (__a, 1)

#define vneg_s16(__a) \
  (int16x4_t)__builtin_neon_vnegv4hi (__a, 1)

#define vneg_s32(__a) \
  (int32x2_t)__builtin_neon_vnegv2si (__a, 1)

#define vneg_f32(__a) \
  (float32x2_t)__builtin_neon_vnegv2sf (__a, 5)

#define vnegq_s8(__a) \
  (int8x16_t)__builtin_neon_vnegv16qi (__a, 1)

#define vnegq_s16(__a) \
  (int16x8_t)__builtin_neon_vnegv8hi (__a, 1)

#define vnegq_s32(__a) \
  (int32x4_t)__builtin_neon_vnegv4si (__a, 1)

#define vnegq_f32(__a) \
  (float32x4_t)__builtin_neon_vnegv4sf (__a, 5)

#define vqneg_s8(__a) \
  (int8x8_t)__builtin_neon_vqnegv8qi (__a, 1)

#define vqneg_s16(__a) \
  (int16x4_t)__builtin_neon_vqnegv4hi (__a, 1)

#define vqneg_s32(__a) \
  (int32x2_t)__builtin_neon_vqnegv2si (__a, 1)

#define vqnegq_s8(__a) \
  (int8x16_t)__builtin_neon_vqnegv16qi (__a, 1)

#define vqnegq_s16(__a) \
  (int16x8_t)__builtin_neon_vqnegv8hi (__a, 1)

#define vqnegq_s32(__a) \
  (int32x4_t)__builtin_neon_vqnegv4si (__a, 1)

#define vmvn_s8(__a) \
  (int8x8_t)__builtin_neon_vmvnv8qi (__a, 1)

#define vmvn_s16(__a) \
  (int16x4_t)__builtin_neon_vmvnv4hi (__a, 1)

#define vmvn_s32(__a) \
  (int32x2_t)__builtin_neon_vmvnv2si (__a, 1)

#define vmvn_u8(__a) \
  (uint8x8_t)__builtin_neon_vmvnv8qi ((int8x8_t) __a, 0)

#define vmvn_u16(__a) \
  (uint16x4_t)__builtin_neon_vmvnv4hi ((int16x4_t) __a, 0)

#define vmvn_u32(__a) \
  (uint32x2_t)__builtin_neon_vmvnv2si ((int32x2_t) __a, 0)

#define vmvn_p8(__a) \
  (poly8x8_t)__builtin_neon_vmvnv8qi ((int8x8_t) __a, 4)

#define vmvnq_s8(__a) \
  (int8x16_t)__builtin_neon_vmvnv16qi (__a, 1)

#define vmvnq_s16(__a) \
  (int16x8_t)__builtin_neon_vmvnv8hi (__a, 1)

#define vmvnq_s32(__a) \
  (int32x4_t)__builtin_neon_vmvnv4si (__a, 1)

#define vmvnq_u8(__a) \
  (uint8x16_t)__builtin_neon_vmvnv16qi ((int8x16_t) __a, 0)

#define vmvnq_u16(__a) \
  (uint16x8_t)__builtin_neon_vmvnv8hi ((int16x8_t) __a, 0)

#define vmvnq_u32(__a) \
  (uint32x4_t)__builtin_neon_vmvnv4si ((int32x4_t) __a, 0)

#define vmvnq_p8(__a) \
  (poly8x16_t)__builtin_neon_vmvnv16qi ((int8x16_t) __a, 4)

#define vcls_s8(__a) \
  (int8x8_t)__builtin_neon_vclsv8qi (__a, 1)

#define vcls_s16(__a) \
  (int16x4_t)__builtin_neon_vclsv4hi (__a, 1)

#define vcls_s32(__a) \
  (int32x2_t)__builtin_neon_vclsv2si (__a, 1)

#define vclsq_s8(__a) \
  (int8x16_t)__builtin_neon_vclsv16qi (__a, 1)

#define vclsq_s16(__a) \
  (int16x8_t)__builtin_neon_vclsv8hi (__a, 1)

#define vclsq_s32(__a) \
  (int32x4_t)__builtin_neon_vclsv4si (__a, 1)

#define vclz_s8(__a) \
  (int8x8_t)__builtin_neon_vclzv8qi (__a, 1)

#define vclz_s16(__a) \
  (int16x4_t)__builtin_neon_vclzv4hi (__a, 1)

#define vclz_s32(__a) \
  (int32x2_t)__builtin_neon_vclzv2si (__a, 1)

#define vclz_u8(__a) \
  (uint8x8_t)__builtin_neon_vclzv8qi ((int8x8_t) __a, 0)

#define vclz_u16(__a) \
  (uint16x4_t)__builtin_neon_vclzv4hi ((int16x4_t) __a, 0)

#define vclz_u32(__a) \
  (uint32x2_t)__builtin_neon_vclzv2si ((int32x2_t) __a, 0)

#define vclzq_s8(__a) \
  (int8x16_t)__builtin_neon_vclzv16qi (__a, 1)

#define vclzq_s16(__a) \
  (int16x8_t)__builtin_neon_vclzv8hi (__a, 1)

#define vclzq_s32(__a) \
  (int32x4_t)__builtin_neon_vclzv4si (__a, 1)

#define vclzq_u8(__a) \
  (uint8x16_t)__builtin_neon_vclzv16qi ((int8x16_t) __a, 0)

#define vclzq_u16(__a) \
  (uint16x8_t)__builtin_neon_vclzv8hi ((int16x8_t) __a, 0)

#define vclzq_u32(__a) \
  (uint32x4_t)__builtin_neon_vclzv4si ((int32x4_t) __a, 0)

#define vcnt_s8(__a) \
  (int8x8_t)__builtin_neon_vcntv8qi (__a, 1)

#define vcnt_u8(__a) \
  (uint8x8_t)__builtin_neon_vcntv8qi ((int8x8_t) __a, 0)

#define vcnt_p8(__a) \
  (poly8x8_t)__builtin_neon_vcntv8qi ((int8x8_t) __a, 4)

#define vcntq_s8(__a) \
  (int8x16_t)__builtin_neon_vcntv16qi (__a, 1)

#define vcntq_u8(__a) \
  (uint8x16_t)__builtin_neon_vcntv16qi ((int8x16_t) __a, 0)

#define vcntq_p8(__a) \
  (poly8x16_t)__builtin_neon_vcntv16qi ((int8x16_t) __a, 4)

#define vrecpe_f32(__a) \
  (float32x2_t)__builtin_neon_vrecpev2sf (__a, 5)

#define vrecpe_u32(__a) \
  (uint32x2_t)__builtin_neon_vrecpev2si ((int32x2_t) __a, 0)

#define vrecpeq_f32(__a) \
  (float32x4_t)__builtin_neon_vrecpev4sf (__a, 5)

#define vrecpeq_u32(__a) \
  (uint32x4_t)__builtin_neon_vrecpev4si ((int32x4_t) __a, 0)

#define vrsqrte_f32(__a) \
  (float32x2_t)__builtin_neon_vrsqrtev2sf (__a, 5)

#define vrsqrte_u32(__a) \
  (uint32x2_t)__builtin_neon_vrsqrtev2si ((int32x2_t) __a, 0)

#define vrsqrteq_f32(__a) \
  (float32x4_t)__builtin_neon_vrsqrtev4sf (__a, 5)

#define vrsqrteq_u32(__a) \
  (uint32x4_t)__builtin_neon_vrsqrtev4si ((int32x4_t) __a, 0)

#define vget_lane_s8(__a, __b) \
  (int8_t)__builtin_neon_vget_lanev8qi (__a, __b, 1)

#define vget_lane_s16(__a, __b) \
  (int16_t)__builtin_neon_vget_lanev4hi (__a, __b, 1)

#define vget_lane_s32(__a, __b) \
  (int32_t)__builtin_neon_vget_lanev2si (__a, __b, 1)

#define vget_lane_f32(__a, __b) \
  (float32_t)__builtin_neon_vget_lanev2sf (__a, __b, 5)

#define vget_lane_u8(__a, __b) \
  (uint8_t)__builtin_neon_vget_lanev8qi ((int8x8_t) __a, __b, 0)

#define vget_lane_u16(__a, __b) \
  (uint16_t)__builtin_neon_vget_lanev4hi ((int16x4_t) __a, __b, 0)

#define vget_lane_u32(__a, __b) \
  (uint32_t)__builtin_neon_vget_lanev2si ((int32x2_t) __a, __b, 0)

#define vget_lane_p8(__a, __b) \
  (poly8_t)__builtin_neon_vget_lanev8qi ((int8x8_t) __a, __b, 4)

#define vget_lane_p16(__a, __b) \
  (poly16_t)__builtin_neon_vget_lanev4hi ((int16x4_t) __a, __b, 4)

#define vget_lane_s64(__a, __b) \
  (int64_t)__builtin_neon_vget_lanev1di (__a, __b, 1)

#define vget_lane_u64(__a, __b) \
  (uint64_t)__builtin_neon_vget_lanev1di ((int64x1_t) __a, __b, 0)

#define vgetq_lane_s8(__a, __b) \
  (int8_t)__builtin_neon_vget_lanev16qi (__a, __b, 1)

#define vgetq_lane_s16(__a, __b) \
  (int16_t)__builtin_neon_vget_lanev8hi (__a, __b, 1)

#define vgetq_lane_s32(__a, __b) \
  (int32_t)__builtin_neon_vget_lanev4si (__a, __b, 1)

#define vgetq_lane_f32(__a, __b) \
  (float32_t)__builtin_neon_vget_lanev4sf (__a, __b, 5)

#define vgetq_lane_u8(__a, __b) \
  (uint8_t)__builtin_neon_vget_lanev16qi ((int8x16_t) __a, __b, 0)

#define vgetq_lane_u16(__a, __b) \
  (uint16_t)__builtin_neon_vget_lanev8hi ((int16x8_t) __a, __b, 0)

#define vgetq_lane_u32(__a, __b) \
  (uint32_t)__builtin_neon_vget_lanev4si ((int32x4_t) __a, __b, 0)

#define vgetq_lane_p8(__a, __b) \
  (poly8_t)__builtin_neon_vget_lanev16qi ((int8x16_t) __a, __b, 4)

#define vgetq_lane_p16(__a, __b) \
  (poly16_t)__builtin_neon_vget_lanev8hi ((int16x8_t) __a, __b, 4)

#define vgetq_lane_s64(__a, __b) \
  (int64_t)__builtin_neon_vget_lanev2di (__a, __b, 1)

#define vgetq_lane_u64(__a, __b) \
  (uint64_t)__builtin_neon_vget_lanev2di ((int64x2_t) __a, __b, 0)

#define vset_lane_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __a, __b, __c)

#define vset_lane_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __a, __b, __c)

#define vset_lane_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vset_lanev2si ((__builtin_neon_si) __a, __b, __c)

#define vset_lane_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vset_lanev2sf (__a, __b, __c)

#define vset_lane_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __a, (int8x8_t) __b, __c)

#define vset_lane_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __a, (int16x4_t) __b, __c)

#define vset_lane_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vset_lanev2si ((__builtin_neon_si) __a, (int32x2_t) __b, __c)

#define vset_lane_p8(__a, __b, __c) \
  (poly8x8_t)__builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __a, (int8x8_t) __b, __c)

#define vset_lane_p16(__a, __b, __c) \
  (poly16x4_t)__builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __a, (int16x4_t) __b, __c)

#define vset_lane_s64(__a, __b, __c) \
  (int64x1_t)__builtin_neon_vset_lanev1di ((__builtin_neon_di) __a, __b, __c)

#define vset_lane_u64(__a, __b, __c) \
  (uint64x1_t)__builtin_neon_vset_lanev1di ((__builtin_neon_di) __a, (int64x1_t) __b, __c)

#define vsetq_lane_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __a, __b, __c)

#define vsetq_lane_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __a, __b, __c)

#define vsetq_lane_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vset_lanev4si ((__builtin_neon_si) __a, __b, __c)

#define vsetq_lane_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vset_lanev4sf (__a, __b, __c)

#define vsetq_lane_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __a, (int8x16_t) __b, __c)

#define vsetq_lane_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __a, (int16x8_t) __b, __c)

#define vsetq_lane_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vset_lanev4si ((__builtin_neon_si) __a, (int32x4_t) __b, __c)

#define vsetq_lane_p8(__a, __b, __c) \
  (poly8x16_t)__builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __a, (int8x16_t) __b, __c)

#define vsetq_lane_p16(__a, __b, __c) \
  (poly16x8_t)__builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __a, (int16x8_t) __b, __c)

#define vsetq_lane_s64(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vset_lanev2di ((__builtin_neon_di) __a, __b, __c)

#define vsetq_lane_u64(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vset_lanev2di ((__builtin_neon_di) __a, (int64x2_t) __b, __c)

#define vcreate_s8(__a) \
  (int8x8_t)__builtin_neon_vcreatev8qi ((__builtin_neon_di) __a)

#define vcreate_s16(__a) \
  (int16x4_t)__builtin_neon_vcreatev4hi ((__builtin_neon_di) __a)

#define vcreate_s32(__a) \
  (int32x2_t)__builtin_neon_vcreatev2si ((__builtin_neon_di) __a)

#define vcreate_s64(__a) \
  (int64x1_t)__builtin_neon_vcreatev1di ((__builtin_neon_di) __a)

#define vcreate_f32(__a) \
  (float32x2_t)__builtin_neon_vcreatev2sf ((__builtin_neon_di) __a)

#define vcreate_u8(__a) \
  (uint8x8_t)__builtin_neon_vcreatev8qi ((__builtin_neon_di) __a)

#define vcreate_u16(__a) \
  (uint16x4_t)__builtin_neon_vcreatev4hi ((__builtin_neon_di) __a)

#define vcreate_u32(__a) \
  (uint32x2_t)__builtin_neon_vcreatev2si ((__builtin_neon_di) __a)

#define vcreate_u64(__a) \
  (uint64x1_t)__builtin_neon_vcreatev1di ((__builtin_neon_di) __a)

#define vcreate_p8(__a) \
  (poly8x8_t)__builtin_neon_vcreatev8qi ((__builtin_neon_di) __a)

#define vcreate_p16(__a) \
  (poly16x4_t)__builtin_neon_vcreatev4hi ((__builtin_neon_di) __a)

#define vdup_n_s8(__a) \
  (int8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a)

#define vdup_n_s16(__a) \
  (int16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a)

#define vdup_n_s32(__a) \
  (int32x2_t)__builtin_neon_vdup_nv2si ((__builtin_neon_si) __a)

#define vdup_n_f32(__a) \
  (float32x2_t)__builtin_neon_vdup_nv2sf (__a)

#define vdup_n_u8(__a) \
  (uint8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a)

#define vdup_n_u16(__a) \
  (uint16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a)

#define vdup_n_u32(__a) \
  (uint32x2_t)__builtin_neon_vdup_nv2si ((__builtin_neon_si) __a)

#define vdup_n_p8(__a) \
  (poly8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a)

#define vdup_n_p16(__a) \
  (poly16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a)

#define vdup_n_s64(__a) \
  (int64x1_t)__builtin_neon_vdup_nv1di ((__builtin_neon_di) __a)

#define vdup_n_u64(__a) \
  (uint64x1_t)__builtin_neon_vdup_nv1di ((__builtin_neon_di) __a)

#define vdupq_n_s8(__a) \
  (int8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a)

#define vdupq_n_s16(__a) \
  (int16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a)

#define vdupq_n_s32(__a) \
  (int32x4_t)__builtin_neon_vdup_nv4si ((__builtin_neon_si) __a)

#define vdupq_n_f32(__a) \
  (float32x4_t)__builtin_neon_vdup_nv4sf (__a)

#define vdupq_n_u8(__a) \
  (uint8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a)

#define vdupq_n_u16(__a) \
  (uint16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a)

#define vdupq_n_u32(__a) \
  (uint32x4_t)__builtin_neon_vdup_nv4si ((__builtin_neon_si) __a)

#define vdupq_n_p8(__a) \
  (poly8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a)

#define vdupq_n_p16(__a) \
  (poly16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a)

#define vdupq_n_s64(__a) \
  (int64x2_t)__builtin_neon_vdup_nv2di ((__builtin_neon_di) __a)

#define vdupq_n_u64(__a) \
  (uint64x2_t)__builtin_neon_vdup_nv2di ((__builtin_neon_di) __a)

#define vmov_n_s8(__a) \
  (int8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a)

#define vmov_n_s16(__a) \
  (int16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a)

#define vmov_n_s32(__a) \
  (int32x2_t)__builtin_neon_vdup_nv2si ((__builtin_neon_si) __a)

#define vmov_n_f32(__a) \
  (float32x2_t)__builtin_neon_vdup_nv2sf (__a)

#define vmov_n_u8(__a) \
  (uint8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a)

#define vmov_n_u16(__a) \
  (uint16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a)

#define vmov_n_u32(__a) \
  (uint32x2_t)__builtin_neon_vdup_nv2si ((__builtin_neon_si) __a)

#define vmov_n_p8(__a) \
  (poly8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a)

#define vmov_n_p16(__a) \
  (poly16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a)

#define vmov_n_s64(__a) \
  (int64x1_t)__builtin_neon_vdup_nv1di ((__builtin_neon_di) __a)

#define vmov_n_u64(__a) \
  (uint64x1_t)__builtin_neon_vdup_nv1di ((__builtin_neon_di) __a)

#define vmovq_n_s8(__a) \
  (int8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a)

#define vmovq_n_s16(__a) \
  (int16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a)

#define vmovq_n_s32(__a) \
  (int32x4_t)__builtin_neon_vdup_nv4si ((__builtin_neon_si) __a)

#define vmovq_n_f32(__a) \
  (float32x4_t)__builtin_neon_vdup_nv4sf (__a)

#define vmovq_n_u8(__a) \
  (uint8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a)

#define vmovq_n_u16(__a) \
  (uint16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a)

#define vmovq_n_u32(__a) \
  (uint32x4_t)__builtin_neon_vdup_nv4si ((__builtin_neon_si) __a)

#define vmovq_n_p8(__a) \
  (poly8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a)

#define vmovq_n_p16(__a) \
  (poly16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a)

#define vmovq_n_s64(__a) \
  (int64x2_t)__builtin_neon_vdup_nv2di ((__builtin_neon_di) __a)

#define vmovq_n_u64(__a) \
  (uint64x2_t)__builtin_neon_vdup_nv2di ((__builtin_neon_di) __a)

#define vdup_lane_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vdup_lanev8qi (__a, __b)

#define vdup_lane_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vdup_lanev4hi (__a, __b)

#define vdup_lane_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vdup_lanev2si (__a, __b)

#define vdup_lane_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vdup_lanev2sf (__a, __b)

#define vdup_lane_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vdup_lanev8qi ((int8x8_t) __a, __b)

#define vdup_lane_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vdup_lanev4hi ((int16x4_t) __a, __b)

#define vdup_lane_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vdup_lanev2si ((int32x2_t) __a, __b)

#define vdup_lane_p8(__a, __b) \
  (poly8x8_t)__builtin_neon_vdup_lanev8qi ((int8x8_t) __a, __b)

#define vdup_lane_p16(__a, __b) \
  (poly16x4_t)__builtin_neon_vdup_lanev4hi ((int16x4_t) __a, __b)

#define vdup_lane_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vdup_lanev1di (__a, __b)

#define vdup_lane_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vdup_lanev1di ((int64x1_t) __a, __b)

#define vdupq_lane_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vdup_lanev16qi (__a, __b)

#define vdupq_lane_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vdup_lanev8hi (__a, __b)

#define vdupq_lane_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vdup_lanev4si (__a, __b)

#define vdupq_lane_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vdup_lanev4sf (__a, __b)

#define vdupq_lane_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vdup_lanev16qi ((int8x8_t) __a, __b)

#define vdupq_lane_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vdup_lanev8hi ((int16x4_t) __a, __b)

#define vdupq_lane_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vdup_lanev4si ((int32x2_t) __a, __b)

#define vdupq_lane_p8(__a, __b) \
  (poly8x16_t)__builtin_neon_vdup_lanev16qi ((int8x8_t) __a, __b)

#define vdupq_lane_p16(__a, __b) \
  (poly16x8_t)__builtin_neon_vdup_lanev8hi ((int16x4_t) __a, __b)

#define vdupq_lane_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vdup_lanev2di (__a, __b)

#define vdupq_lane_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vdup_lanev2di ((int64x1_t) __a, __b)

#define vcombine_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vcombinev8qi (__a, __b)

#define vcombine_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vcombinev4hi (__a, __b)

#define vcombine_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vcombinev2si (__a, __b)

#define vcombine_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vcombinev1di (__a, __b)

#define vcombine_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vcombinev2sf (__a, __b)

#define vcombine_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vcombinev8qi ((int8x8_t) __a, (int8x8_t) __b)

#define vcombine_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vcombinev4hi ((int16x4_t) __a, (int16x4_t) __b)

#define vcombine_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcombinev2si ((int32x2_t) __a, (int32x2_t) __b)

#define vcombine_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vcombinev1di ((int64x1_t) __a, (int64x1_t) __b)

#define vcombine_p8(__a, __b) \
  (poly8x16_t)__builtin_neon_vcombinev8qi ((int8x8_t) __a, (int8x8_t) __b)

#define vcombine_p16(__a, __b) \
  (poly16x8_t)__builtin_neon_vcombinev4hi ((int16x4_t) __a, (int16x4_t) __b)

#define vget_high_s8(__a) \
  (int8x8_t)__builtin_neon_vget_highv16qi (__a)

#define vget_high_s16(__a) \
  (int16x4_t)__builtin_neon_vget_highv8hi (__a)

#define vget_high_s32(__a) \
  (int32x2_t)__builtin_neon_vget_highv4si (__a)

#define vget_high_s64(__a) \
  (int64x1_t)__builtin_neon_vget_highv2di (__a)

#define vget_high_f32(__a) \
  (float32x2_t)__builtin_neon_vget_highv4sf (__a)

#define vget_high_u8(__a) \
  (uint8x8_t)__builtin_neon_vget_highv16qi ((int8x16_t) __a)

#define vget_high_u16(__a) \
  (uint16x4_t)__builtin_neon_vget_highv8hi ((int16x8_t) __a)

#define vget_high_u32(__a) \
  (uint32x2_t)__builtin_neon_vget_highv4si ((int32x4_t) __a)

#define vget_high_u64(__a) \
  (uint64x1_t)__builtin_neon_vget_highv2di ((int64x2_t) __a)

#define vget_high_p8(__a) \
  (poly8x8_t)__builtin_neon_vget_highv16qi ((int8x16_t) __a)

#define vget_high_p16(__a) \
  (poly16x4_t)__builtin_neon_vget_highv8hi ((int16x8_t) __a)

#define vget_low_s8(__a) \
  (int8x8_t)__builtin_neon_vget_lowv16qi (__a)

#define vget_low_s16(__a) \
  (int16x4_t)__builtin_neon_vget_lowv8hi (__a)

#define vget_low_s32(__a) \
  (int32x2_t)__builtin_neon_vget_lowv4si (__a)

#define vget_low_s64(__a) \
  (int64x1_t)__builtin_neon_vget_lowv2di (__a)

#define vget_low_f32(__a) \
  (float32x2_t)__builtin_neon_vget_lowv4sf (__a)

#define vget_low_u8(__a) \
  (uint8x8_t)__builtin_neon_vget_lowv16qi ((int8x16_t) __a)

#define vget_low_u16(__a) \
  (uint16x4_t)__builtin_neon_vget_lowv8hi ((int16x8_t) __a)

#define vget_low_u32(__a) \
  (uint32x2_t)__builtin_neon_vget_lowv4si ((int32x4_t) __a)

#define vget_low_u64(__a) \
  (uint64x1_t)__builtin_neon_vget_lowv2di ((int64x2_t) __a)

#define vget_low_p8(__a) \
  (poly8x8_t)__builtin_neon_vget_lowv16qi ((int8x16_t) __a)

#define vget_low_p16(__a) \
  (poly16x4_t)__builtin_neon_vget_lowv8hi ((int16x8_t) __a)

#define vcvt_s32_f32(__a) \
  (int32x2_t)__builtin_neon_vcvtv2sf (__a, 1)

#define vcvt_f32_s32(__a) \
  (float32x2_t)__builtin_neon_vcvtv2si (__a, 1)

#define vcvt_f32_u32(__a) \
  (float32x2_t)__builtin_neon_vcvtv2si ((int32x2_t) __a, 0)

#define vcvt_u32_f32(__a) \
  (uint32x2_t)__builtin_neon_vcvtv2sf (__a, 0)

#define vcvtq_s32_f32(__a) \
  (int32x4_t)__builtin_neon_vcvtv4sf (__a, 1)

#define vcvtq_f32_s32(__a) \
  (float32x4_t)__builtin_neon_vcvtv4si (__a, 1)

#define vcvtq_f32_u32(__a) \
  (float32x4_t)__builtin_neon_vcvtv4si ((int32x4_t) __a, 0)

#define vcvtq_u32_f32(__a) \
  (uint32x4_t)__builtin_neon_vcvtv4sf (__a, 0)

#define vcvt_n_s32_f32(__a, __b) \
  (int32x2_t)__builtin_neon_vcvt_nv2sf (__a, __b, 1)

#define vcvt_n_f32_s32(__a, __b) \
  (float32x2_t)__builtin_neon_vcvt_nv2si (__a, __b, 1)

#define vcvt_n_f32_u32(__a, __b) \
  (float32x2_t)__builtin_neon_vcvt_nv2si ((int32x2_t) __a, __b, 0)

#define vcvt_n_u32_f32(__a, __b) \
  (uint32x2_t)__builtin_neon_vcvt_nv2sf (__a, __b, 0)

#define vcvtq_n_s32_f32(__a, __b) \
  (int32x4_t)__builtin_neon_vcvt_nv4sf (__a, __b, 1)

#define vcvtq_n_f32_s32(__a, __b) \
  (float32x4_t)__builtin_neon_vcvt_nv4si (__a, __b, 1)

#define vcvtq_n_f32_u32(__a, __b) \
  (float32x4_t)__builtin_neon_vcvt_nv4si ((int32x4_t) __a, __b, 0)

#define vcvtq_n_u32_f32(__a, __b) \
  (uint32x4_t)__builtin_neon_vcvt_nv4sf (__a, __b, 0)

#define vmovn_s16(__a) \
  (int8x8_t)__builtin_neon_vmovnv8hi (__a, 1)

#define vmovn_s32(__a) \
  (int16x4_t)__builtin_neon_vmovnv4si (__a, 1)

#define vmovn_s64(__a) \
  (int32x2_t)__builtin_neon_vmovnv2di (__a, 1)

#define vmovn_u16(__a) \
  (uint8x8_t)__builtin_neon_vmovnv8hi ((int16x8_t) __a, 0)

#define vmovn_u32(__a) \
  (uint16x4_t)__builtin_neon_vmovnv4si ((int32x4_t) __a, 0)

#define vmovn_u64(__a) \
  (uint32x2_t)__builtin_neon_vmovnv2di ((int64x2_t) __a, 0)

#define vqmovn_s16(__a) \
  (int8x8_t)__builtin_neon_vqmovnv8hi (__a, 1)

#define vqmovn_s32(__a) \
  (int16x4_t)__builtin_neon_vqmovnv4si (__a, 1)

#define vqmovn_s64(__a) \
  (int32x2_t)__builtin_neon_vqmovnv2di (__a, 1)

#define vqmovn_u16(__a) \
  (uint8x8_t)__builtin_neon_vqmovnv8hi ((int16x8_t) __a, 0)

#define vqmovn_u32(__a) \
  (uint16x4_t)__builtin_neon_vqmovnv4si ((int32x4_t) __a, 0)

#define vqmovn_u64(__a) \
  (uint32x2_t)__builtin_neon_vqmovnv2di ((int64x2_t) __a, 0)

#define vqmovun_s16(__a) \
  (uint8x8_t)__builtin_neon_vqmovunv8hi (__a, 1)

#define vqmovun_s32(__a) \
  (uint16x4_t)__builtin_neon_vqmovunv4si (__a, 1)

#define vqmovun_s64(__a) \
  (uint32x2_t)__builtin_neon_vqmovunv2di (__a, 1)

#define vmovl_s8(__a) \
  (int16x8_t)__builtin_neon_vmovlv8qi (__a, 1)

#define vmovl_s16(__a) \
  (int32x4_t)__builtin_neon_vmovlv4hi (__a, 1)

#define vmovl_s32(__a) \
  (int64x2_t)__builtin_neon_vmovlv2si (__a, 1)

#define vmovl_u8(__a) \
  (uint16x8_t)__builtin_neon_vmovlv8qi ((int8x8_t) __a, 0)

#define vmovl_u16(__a) \
  (uint32x4_t)__builtin_neon_vmovlv4hi ((int16x4_t) __a, 0)

#define vmovl_u32(__a) \
  (uint64x2_t)__builtin_neon_vmovlv2si ((int32x2_t) __a, 0)

#define vtbl1_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vtbl1v8qi (__a, __b)

#define vtbl1_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vtbl1v8qi ((int8x8_t) __a, (int8x8_t) __b)

#define vtbl1_p8(__a, __b) \
  (poly8x8_t)__builtin_neon_vtbl1v8qi ((int8x8_t) __a, (int8x8_t) __b)

#define vtbl2_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __au = { __a }; \
     (int8x8_t)__builtin_neon_vtbl2v8qi (__au.__o, __b); \
   })

#define vtbl2_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __au = { __a }; \
     (uint8x8_t)__builtin_neon_vtbl2v8qi (__au.__o, (int8x8_t) __b); \
   })

#define vtbl2_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __au = { __a }; \
     (poly8x8_t)__builtin_neon_vtbl2v8qi (__au.__o, (int8x8_t) __b); \
   })

#define vtbl3_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __au = { __a }; \
     (int8x8_t)__builtin_neon_vtbl3v8qi (__au.__o, __b); \
   })

#define vtbl3_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __au = { __a }; \
     (uint8x8_t)__builtin_neon_vtbl3v8qi (__au.__o, (int8x8_t) __b); \
   })

#define vtbl3_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __au = { __a }; \
     (poly8x8_t)__builtin_neon_vtbl3v8qi (__au.__o, (int8x8_t) __b); \
   })

#define vtbl4_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __au = { __a }; \
     (int8x8_t)__builtin_neon_vtbl4v8qi (__au.__o, __b); \
   })

#define vtbl4_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __au = { __a }; \
     (uint8x8_t)__builtin_neon_vtbl4v8qi (__au.__o, (int8x8_t) __b); \
   })

#define vtbl4_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __au = { __a }; \
     (poly8x8_t)__builtin_neon_vtbl4v8qi (__au.__o, (int8x8_t) __b); \
   })

#define vtbx1_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vtbx1v8qi (__a, __b, __c)

#define vtbx1_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vtbx1v8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c)

#define vtbx1_p8(__a, __b, __c) \
  (poly8x8_t)__builtin_neon_vtbx1v8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c)

#define vtbx2_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     (int8x8_t)__builtin_neon_vtbx2v8qi (__a, __bu.__o, __c); \
   })

#define vtbx2_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     (uint8x8_t)__builtin_neon_vtbx2v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c); \
   })

#define vtbx2_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     (poly8x8_t)__builtin_neon_vtbx2v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c); \
   })

#define vtbx3_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     (int8x8_t)__builtin_neon_vtbx3v8qi (__a, __bu.__o, __c); \
   })

#define vtbx3_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     (uint8x8_t)__builtin_neon_vtbx3v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c); \
   })

#define vtbx3_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     (poly8x8_t)__builtin_neon_vtbx3v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c); \
   })

#define vtbx4_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     (int8x8_t)__builtin_neon_vtbx4v8qi (__a, __bu.__o, __c); \
   })

#define vtbx4_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     (uint8x8_t)__builtin_neon_vtbx4v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c); \
   })

#define vtbx4_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     (poly8x8_t)__builtin_neon_vtbx4v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c); \
   })

#define vmul_lane_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vmul_lanev4hi (__a, __b, __c, 1)

#define vmul_lane_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vmul_lanev2si (__a, __b, __c, 1)

#define vmul_lane_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vmul_lanev2sf (__a, __b, __c, 5)

#define vmul_lane_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vmul_lanev4hi ((int16x4_t) __a, (int16x4_t) __b, __c, 0)

#define vmul_lane_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vmul_lanev2si ((int32x2_t) __a, (int32x2_t) __b, __c, 0)

#define vmulq_lane_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vmul_lanev8hi (__a, __b, __c, 1)

#define vmulq_lane_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmul_lanev4si (__a, __b, __c, 1)

#define vmulq_lane_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vmul_lanev4sf (__a, __b, __c, 5)

#define vmulq_lane_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vmul_lanev8hi ((int16x8_t) __a, (int16x4_t) __b, __c, 0)

#define vmulq_lane_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmul_lanev4si ((int32x4_t) __a, (int32x2_t) __b, __c, 0)

#define vmla_lane_s16(__a, __b, __c, __d) \
  (int16x4_t)__builtin_neon_vmla_lanev4hi (__a, __b, __c, __d, 1)

#define vmla_lane_s32(__a, __b, __c, __d) \
  (int32x2_t)__builtin_neon_vmla_lanev2si (__a, __b, __c, __d, 1)

#define vmla_lane_f32(__a, __b, __c, __d) \
  (float32x2_t)__builtin_neon_vmla_lanev2sf (__a, __b, __c, __d, 5)

#define vmla_lane_u16(__a, __b, __c, __d) \
  (uint16x4_t)__builtin_neon_vmla_lanev4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, __d, 0)

#define vmla_lane_u32(__a, __b, __c, __d) \
  (uint32x2_t)__builtin_neon_vmla_lanev2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, __d, 0)

#define vmlaq_lane_s16(__a, __b, __c, __d) \
  (int16x8_t)__builtin_neon_vmla_lanev8hi (__a, __b, __c, __d, 1)

#define vmlaq_lane_s32(__a, __b, __c, __d) \
  (int32x4_t)__builtin_neon_vmla_lanev4si (__a, __b, __c, __d, 1)

#define vmlaq_lane_f32(__a, __b, __c, __d) \
  (float32x4_t)__builtin_neon_vmla_lanev4sf (__a, __b, __c, __d, 5)

#define vmlaq_lane_u16(__a, __b, __c, __d) \
  (uint16x8_t)__builtin_neon_vmla_lanev8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x4_t) __c, __d, 0)

#define vmlaq_lane_u32(__a, __b, __c, __d) \
  (uint32x4_t)__builtin_neon_vmla_lanev4si ((int32x4_t) __a, (int32x4_t) __b, (int32x2_t) __c, __d, 0)

#define vmlal_lane_s16(__a, __b, __c, __d) \
  (int32x4_t)__builtin_neon_vmlal_lanev4hi (__a, __b, __c, __d, 1)

#define vmlal_lane_s32(__a, __b, __c, __d) \
  (int64x2_t)__builtin_neon_vmlal_lanev2si (__a, __b, __c, __d, 1)

#define vmlal_lane_u16(__a, __b, __c, __d) \
  (uint32x4_t)__builtin_neon_vmlal_lanev4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, __d, 0)

#define vmlal_lane_u32(__a, __b, __c, __d) \
  (uint64x2_t)__builtin_neon_vmlal_lanev2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, __d, 0)

#define vqdmlal_lane_s16(__a, __b, __c, __d) \
  (int32x4_t)__builtin_neon_vqdmlal_lanev4hi (__a, __b, __c, __d, 1)

#define vqdmlal_lane_s32(__a, __b, __c, __d) \
  (int64x2_t)__builtin_neon_vqdmlal_lanev2si (__a, __b, __c, __d, 1)

#define vmls_lane_s16(__a, __b, __c, __d) \
  (int16x4_t)__builtin_neon_vmls_lanev4hi (__a, __b, __c, __d, 1)

#define vmls_lane_s32(__a, __b, __c, __d) \
  (int32x2_t)__builtin_neon_vmls_lanev2si (__a, __b, __c, __d, 1)

#define vmls_lane_f32(__a, __b, __c, __d) \
  (float32x2_t)__builtin_neon_vmls_lanev2sf (__a, __b, __c, __d, 5)

#define vmls_lane_u16(__a, __b, __c, __d) \
  (uint16x4_t)__builtin_neon_vmls_lanev4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, __d, 0)

#define vmls_lane_u32(__a, __b, __c, __d) \
  (uint32x2_t)__builtin_neon_vmls_lanev2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, __d, 0)

#define vmlsq_lane_s16(__a, __b, __c, __d) \
  (int16x8_t)__builtin_neon_vmls_lanev8hi (__a, __b, __c, __d, 1)

#define vmlsq_lane_s32(__a, __b, __c, __d) \
  (int32x4_t)__builtin_neon_vmls_lanev4si (__a, __b, __c, __d, 1)

#define vmlsq_lane_f32(__a, __b, __c, __d) \
  (float32x4_t)__builtin_neon_vmls_lanev4sf (__a, __b, __c, __d, 5)

#define vmlsq_lane_u16(__a, __b, __c, __d) \
  (uint16x8_t)__builtin_neon_vmls_lanev8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x4_t) __c, __d, 0)

#define vmlsq_lane_u32(__a, __b, __c, __d) \
  (uint32x4_t)__builtin_neon_vmls_lanev4si ((int32x4_t) __a, (int32x4_t) __b, (int32x2_t) __c, __d, 0)

#define vmlsl_lane_s16(__a, __b, __c, __d) \
  (int32x4_t)__builtin_neon_vmlsl_lanev4hi (__a, __b, __c, __d, 1)

#define vmlsl_lane_s32(__a, __b, __c, __d) \
  (int64x2_t)__builtin_neon_vmlsl_lanev2si (__a, __b, __c, __d, 1)

#define vmlsl_lane_u16(__a, __b, __c, __d) \
  (uint32x4_t)__builtin_neon_vmlsl_lanev4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, __d, 0)

#define vmlsl_lane_u32(__a, __b, __c, __d) \
  (uint64x2_t)__builtin_neon_vmlsl_lanev2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, __d, 0)

#define vqdmlsl_lane_s16(__a, __b, __c, __d) \
  (int32x4_t)__builtin_neon_vqdmlsl_lanev4hi (__a, __b, __c, __d, 1)

#define vqdmlsl_lane_s32(__a, __b, __c, __d) \
  (int64x2_t)__builtin_neon_vqdmlsl_lanev2si (__a, __b, __c, __d, 1)

#define vmull_lane_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmull_lanev4hi (__a, __b, __c, 1)

#define vmull_lane_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vmull_lanev2si (__a, __b, __c, 1)

#define vmull_lane_u16(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmull_lanev4hi ((int16x4_t) __a, (int16x4_t) __b, __c, 0)

#define vmull_lane_u32(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vmull_lanev2si ((int32x2_t) __a, (int32x2_t) __b, __c, 0)

#define vqdmull_lane_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vqdmull_lanev4hi (__a, __b, __c, 1)

#define vqdmull_lane_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vqdmull_lanev2si (__a, __b, __c, 1)

#define vqdmulhq_lane_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vqdmulh_lanev8hi (__a, __b, __c, 1)

#define vqdmulhq_lane_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vqdmulh_lanev4si (__a, __b, __c, 1)

#define vqdmulh_lane_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vqdmulh_lanev4hi (__a, __b, __c, 1)

#define vqdmulh_lane_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vqdmulh_lanev2si (__a, __b, __c, 1)

#define vqrdmulhq_lane_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vqdmulh_lanev8hi (__a, __b, __c, 3)

#define vqrdmulhq_lane_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vqdmulh_lanev4si (__a, __b, __c, 3)

#define vqrdmulh_lane_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vqdmulh_lanev4hi (__a, __b, __c, 3)

#define vqrdmulh_lane_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vqdmulh_lanev2si (__a, __b, __c, 3)

#define vmul_n_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vmul_nv4hi (__a, (__builtin_neon_hi) __b, 1)

#define vmul_n_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vmul_nv2si (__a, (__builtin_neon_si) __b, 1)

#define vmul_n_f32(__a, __b) \
  (float32x2_t)__builtin_neon_vmul_nv2sf (__a, __b, 5)

#define vmul_n_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vmul_nv4hi ((int16x4_t) __a, (__builtin_neon_hi) __b, 0)

#define vmul_n_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vmul_nv2si ((int32x2_t) __a, (__builtin_neon_si) __b, 0)

#define vmulq_n_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vmul_nv8hi (__a, (__builtin_neon_hi) __b, 1)

#define vmulq_n_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vmul_nv4si (__a, (__builtin_neon_si) __b, 1)

#define vmulq_n_f32(__a, __b) \
  (float32x4_t)__builtin_neon_vmul_nv4sf (__a, __b, 5)

#define vmulq_n_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vmul_nv8hi ((int16x8_t) __a, (__builtin_neon_hi) __b, 0)

#define vmulq_n_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vmul_nv4si ((int32x4_t) __a, (__builtin_neon_si) __b, 0)

#define vmull_n_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vmull_nv4hi (__a, (__builtin_neon_hi) __b, 1)

#define vmull_n_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vmull_nv2si (__a, (__builtin_neon_si) __b, 1)

#define vmull_n_u16(__a, __b) \
  (uint32x4_t)__builtin_neon_vmull_nv4hi ((int16x4_t) __a, (__builtin_neon_hi) __b, 0)

#define vmull_n_u32(__a, __b) \
  (uint64x2_t)__builtin_neon_vmull_nv2si ((int32x2_t) __a, (__builtin_neon_si) __b, 0)

#define vqdmull_n_s16(__a, __b) \
  (int32x4_t)__builtin_neon_vqdmull_nv4hi (__a, (__builtin_neon_hi) __b, 1)

#define vqdmull_n_s32(__a, __b) \
  (int64x2_t)__builtin_neon_vqdmull_nv2si (__a, (__builtin_neon_si) __b, 1)

#define vqdmulhq_n_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqdmulh_nv8hi (__a, (__builtin_neon_hi) __b, 1)

#define vqdmulhq_n_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqdmulh_nv4si (__a, (__builtin_neon_si) __b, 1)

#define vqdmulh_n_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqdmulh_nv4hi (__a, (__builtin_neon_hi) __b, 1)

#define vqdmulh_n_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqdmulh_nv2si (__a, (__builtin_neon_si) __b, 1)

#define vqrdmulhq_n_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vqdmulh_nv8hi (__a, (__builtin_neon_hi) __b, 3)

#define vqrdmulhq_n_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vqdmulh_nv4si (__a, (__builtin_neon_si) __b, 3)

#define vqrdmulh_n_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vqdmulh_nv4hi (__a, (__builtin_neon_hi) __b, 3)

#define vqrdmulh_n_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vqdmulh_nv2si (__a, (__builtin_neon_si) __b, 3)

#define vmla_n_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vmla_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1)

#define vmla_n_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vmla_nv2si (__a, __b, (__builtin_neon_si) __c, 1)

#define vmla_n_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vmla_nv2sf (__a, __b, __c, 5)

#define vmla_n_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vmla_nv4hi ((int16x4_t) __a, (int16x4_t) __b, (__builtin_neon_hi) __c, 0)

#define vmla_n_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vmla_nv2si ((int32x2_t) __a, (int32x2_t) __b, (__builtin_neon_si) __c, 0)

#define vmlaq_n_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vmla_nv8hi (__a, __b, (__builtin_neon_hi) __c, 1)

#define vmlaq_n_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmla_nv4si (__a, __b, (__builtin_neon_si) __c, 1)

#define vmlaq_n_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vmla_nv4sf (__a, __b, __c, 5)

#define vmlaq_n_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vmla_nv8hi ((int16x8_t) __a, (int16x8_t) __b, (__builtin_neon_hi) __c, 0)

#define vmlaq_n_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmla_nv4si ((int32x4_t) __a, (int32x4_t) __b, (__builtin_neon_si) __c, 0)

#define vmlal_n_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmlal_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1)

#define vmlal_n_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vmlal_nv2si (__a, __b, (__builtin_neon_si) __c, 1)

#define vmlal_n_u16(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmlal_nv4hi ((int32x4_t) __a, (int16x4_t) __b, (__builtin_neon_hi) __c, 0)

#define vmlal_n_u32(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vmlal_nv2si ((int64x2_t) __a, (int32x2_t) __b, (__builtin_neon_si) __c, 0)

#define vqdmlal_n_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vqdmlal_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1)

#define vqdmlal_n_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vqdmlal_nv2si (__a, __b, (__builtin_neon_si) __c, 1)

#define vmls_n_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vmls_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1)

#define vmls_n_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vmls_nv2si (__a, __b, (__builtin_neon_si) __c, 1)

#define vmls_n_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vmls_nv2sf (__a, __b, __c, 5)

#define vmls_n_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vmls_nv4hi ((int16x4_t) __a, (int16x4_t) __b, (__builtin_neon_hi) __c, 0)

#define vmls_n_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vmls_nv2si ((int32x2_t) __a, (int32x2_t) __b, (__builtin_neon_si) __c, 0)

#define vmlsq_n_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vmls_nv8hi (__a, __b, (__builtin_neon_hi) __c, 1)

#define vmlsq_n_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmls_nv4si (__a, __b, (__builtin_neon_si) __c, 1)

#define vmlsq_n_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vmls_nv4sf (__a, __b, __c, 5)

#define vmlsq_n_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vmls_nv8hi ((int16x8_t) __a, (int16x8_t) __b, (__builtin_neon_hi) __c, 0)

#define vmlsq_n_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmls_nv4si ((int32x4_t) __a, (int32x4_t) __b, (__builtin_neon_si) __c, 0)

#define vmlsl_n_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vmlsl_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1)

#define vmlsl_n_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vmlsl_nv2si (__a, __b, (__builtin_neon_si) __c, 1)

#define vmlsl_n_u16(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vmlsl_nv4hi ((int32x4_t) __a, (int16x4_t) __b, (__builtin_neon_hi) __c, 0)

#define vmlsl_n_u32(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vmlsl_nv2si ((int64x2_t) __a, (int32x2_t) __b, (__builtin_neon_si) __c, 0)

#define vqdmlsl_n_s16(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vqdmlsl_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1)

#define vqdmlsl_n_s32(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vqdmlsl_nv2si (__a, __b, (__builtin_neon_si) __c, 1)

#define vext_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vextv8qi (__a, __b, __c)

#define vext_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vextv4hi (__a, __b, __c)

#define vext_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vextv2si (__a, __b, __c)

#define vext_s64(__a, __b, __c) \
  (int64x1_t)__builtin_neon_vextv1di (__a, __b, __c)

#define vext_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vextv2sf (__a, __b, __c)

#define vext_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vextv8qi ((int8x8_t) __a, (int8x8_t) __b, __c)

#define vext_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vextv4hi ((int16x4_t) __a, (int16x4_t) __b, __c)

#define vext_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vextv2si ((int32x2_t) __a, (int32x2_t) __b, __c)

#define vext_u64(__a, __b, __c) \
  (uint64x1_t)__builtin_neon_vextv1di ((int64x1_t) __a, (int64x1_t) __b, __c)

#define vext_p8(__a, __b, __c) \
  (poly8x8_t)__builtin_neon_vextv8qi ((int8x8_t) __a, (int8x8_t) __b, __c)

#define vext_p16(__a, __b, __c) \
  (poly16x4_t)__builtin_neon_vextv4hi ((int16x4_t) __a, (int16x4_t) __b, __c)

#define vextq_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vextv16qi (__a, __b, __c)

#define vextq_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vextv8hi (__a, __b, __c)

#define vextq_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vextv4si (__a, __b, __c)

#define vextq_s64(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vextv2di (__a, __b, __c)

#define vextq_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vextv4sf (__a, __b, __c)

#define vextq_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vextv16qi ((int8x16_t) __a, (int8x16_t) __b, __c)

#define vextq_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vextv8hi ((int16x8_t) __a, (int16x8_t) __b, __c)

#define vextq_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vextv4si ((int32x4_t) __a, (int32x4_t) __b, __c)

#define vextq_u64(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vextv2di ((int64x2_t) __a, (int64x2_t) __b, __c)

#define vextq_p8(__a, __b, __c) \
  (poly8x16_t)__builtin_neon_vextv16qi ((int8x16_t) __a, (int8x16_t) __b, __c)

#define vextq_p16(__a, __b, __c) \
  (poly16x8_t)__builtin_neon_vextv8hi ((int16x8_t) __a, (int16x8_t) __b, __c)

#define vrev64_s8(__a) \
  (int8x8_t)__builtin_neon_vrev64v8qi (__a, 1)

#define vrev64_s16(__a) \
  (int16x4_t)__builtin_neon_vrev64v4hi (__a, 1)

#define vrev64_s32(__a) \
  (int32x2_t)__builtin_neon_vrev64v2si (__a, 1)

#define vrev64_f32(__a) \
  (float32x2_t)__builtin_neon_vrev64v2sf (__a, 5)

#define vrev64_u8(__a) \
  (uint8x8_t)__builtin_neon_vrev64v8qi ((int8x8_t) __a, 0)

#define vrev64_u16(__a) \
  (uint16x4_t)__builtin_neon_vrev64v4hi ((int16x4_t) __a, 0)

#define vrev64_u32(__a) \
  (uint32x2_t)__builtin_neon_vrev64v2si ((int32x2_t) __a, 0)

#define vrev64_p8(__a) \
  (poly8x8_t)__builtin_neon_vrev64v8qi ((int8x8_t) __a, 4)

#define vrev64_p16(__a) \
  (poly16x4_t)__builtin_neon_vrev64v4hi ((int16x4_t) __a, 4)

#define vrev64q_s8(__a) \
  (int8x16_t)__builtin_neon_vrev64v16qi (__a, 1)

#define vrev64q_s16(__a) \
  (int16x8_t)__builtin_neon_vrev64v8hi (__a, 1)

#define vrev64q_s32(__a) \
  (int32x4_t)__builtin_neon_vrev64v4si (__a, 1)

#define vrev64q_f32(__a) \
  (float32x4_t)__builtin_neon_vrev64v4sf (__a, 5)

#define vrev64q_u8(__a) \
  (uint8x16_t)__builtin_neon_vrev64v16qi ((int8x16_t) __a, 0)

#define vrev64q_u16(__a) \
  (uint16x8_t)__builtin_neon_vrev64v8hi ((int16x8_t) __a, 0)

#define vrev64q_u32(__a) \
  (uint32x4_t)__builtin_neon_vrev64v4si ((int32x4_t) __a, 0)

#define vrev64q_p8(__a) \
  (poly8x16_t)__builtin_neon_vrev64v16qi ((int8x16_t) __a, 4)

#define vrev64q_p16(__a) \
  (poly16x8_t)__builtin_neon_vrev64v8hi ((int16x8_t) __a, 4)

#define vrev32_s8(__a) \
  (int8x8_t)__builtin_neon_vrev32v8qi (__a, 1)

#define vrev32_s16(__a) \
  (int16x4_t)__builtin_neon_vrev32v4hi (__a, 1)

#define vrev32_u8(__a) \
  (uint8x8_t)__builtin_neon_vrev32v8qi ((int8x8_t) __a, 0)

#define vrev32_u16(__a) \
  (uint16x4_t)__builtin_neon_vrev32v4hi ((int16x4_t) __a, 0)

#define vrev32_p8(__a) \
  (poly8x8_t)__builtin_neon_vrev32v8qi ((int8x8_t) __a, 4)

#define vrev32_p16(__a) \
  (poly16x4_t)__builtin_neon_vrev32v4hi ((int16x4_t) __a, 4)

#define vrev32q_s8(__a) \
  (int8x16_t)__builtin_neon_vrev32v16qi (__a, 1)

#define vrev32q_s16(__a) \
  (int16x8_t)__builtin_neon_vrev32v8hi (__a, 1)

#define vrev32q_u8(__a) \
  (uint8x16_t)__builtin_neon_vrev32v16qi ((int8x16_t) __a, 0)

#define vrev32q_u16(__a) \
  (uint16x8_t)__builtin_neon_vrev32v8hi ((int16x8_t) __a, 0)

#define vrev32q_p8(__a) \
  (poly8x16_t)__builtin_neon_vrev32v16qi ((int8x16_t) __a, 4)

#define vrev32q_p16(__a) \
  (poly16x8_t)__builtin_neon_vrev32v8hi ((int16x8_t) __a, 4)

#define vrev16_s8(__a) \
  (int8x8_t)__builtin_neon_vrev16v8qi (__a, 1)

#define vrev16_u8(__a) \
  (uint8x8_t)__builtin_neon_vrev16v8qi ((int8x8_t) __a, 0)

#define vrev16_p8(__a) \
  (poly8x8_t)__builtin_neon_vrev16v8qi ((int8x8_t) __a, 4)

#define vrev16q_s8(__a) \
  (int8x16_t)__builtin_neon_vrev16v16qi (__a, 1)

#define vrev16q_u8(__a) \
  (uint8x16_t)__builtin_neon_vrev16v16qi ((int8x16_t) __a, 0)

#define vrev16q_p8(__a) \
  (poly8x16_t)__builtin_neon_vrev16v16qi ((int8x16_t) __a, 4)

#define vbsl_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vbslv8qi ((int8x8_t) __a, __b, __c)

#define vbsl_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vbslv4hi ((int16x4_t) __a, __b, __c)

#define vbsl_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vbslv2si ((int32x2_t) __a, __b, __c)

#define vbsl_s64(__a, __b, __c) \
  (int64x1_t)__builtin_neon_vbslv1di ((int64x1_t) __a, __b, __c)

#define vbsl_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vbslv2sf ((int32x2_t) __a, __b, __c)

#define vbsl_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vbslv8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c)

#define vbsl_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vbslv4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c)

#define vbsl_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vbslv2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c)

#define vbsl_u64(__a, __b, __c) \
  (uint64x1_t)__builtin_neon_vbslv1di ((int64x1_t) __a, (int64x1_t) __b, (int64x1_t) __c)

#define vbsl_p8(__a, __b, __c) \
  (poly8x8_t)__builtin_neon_vbslv8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c)

#define vbsl_p16(__a, __b, __c) \
  (poly16x4_t)__builtin_neon_vbslv4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c)

#define vbslq_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vbslv16qi ((int8x16_t) __a, __b, __c)

#define vbslq_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vbslv8hi ((int16x8_t) __a, __b, __c)

#define vbslq_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vbslv4si ((int32x4_t) __a, __b, __c)

#define vbslq_s64(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vbslv2di ((int64x2_t) __a, __b, __c)

#define vbslq_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vbslv4sf ((int32x4_t) __a, __b, __c)

#define vbslq_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vbslv16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c)

#define vbslq_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vbslv8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c)

#define vbslq_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vbslv4si ((int32x4_t) __a, (int32x4_t) __b, (int32x4_t) __c)

#define vbslq_u64(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vbslv2di ((int64x2_t) __a, (int64x2_t) __b, (int64x2_t) __c)

#define vbslq_p8(__a, __b, __c) \
  (poly8x16_t)__builtin_neon_vbslv16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c)

#define vbslq_p16(__a, __b, __c) \
  (poly16x8_t)__builtin_neon_vbslv8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c)

#define vtrn_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8qi (__a, __b); \
     __rv.__i; \
   })

#define vtrn_s16(__a, __b) __extension__ \
  ({ \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4hi (__a, __b); \
     __rv.__i; \
   })

#define vtrn_s32(__a, __b) __extension__ \
  ({ \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv2si (__a, __b); \
     __rv.__i; \
   })

#define vtrn_f32(__a, __b) __extension__ \
  ({ \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv2sf (__a, __b); \
     __rv.__i; \
   })

#define vtrn_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8qi ((int8x8_t) __a, (int8x8_t) __b); \
     __rv.__i; \
   })

#define vtrn_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4hi ((int16x4_t) __a, (int16x4_t) __b); \
     __rv.__i; \
   })

#define vtrn_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv2si ((int32x2_t) __a, (int32x2_t) __b); \
     __rv.__i; \
   })

#define vtrn_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8qi ((int8x8_t) __a, (int8x8_t) __b); \
     __rv.__i; \
   })

#define vtrn_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4hi ((int16x4_t) __a, (int16x4_t) __b); \
     __rv.__i; \
   })

#define vtrnq_s8(__a, __b) __extension__ \
  ({ \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv16qi (__a, __b); \
     __rv.__i; \
   })

#define vtrnq_s16(__a, __b) __extension__ \
  ({ \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8hi (__a, __b); \
     __rv.__i; \
   })

#define vtrnq_s32(__a, __b) __extension__ \
  ({ \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4si (__a, __b); \
     __rv.__i; \
   })

#define vtrnq_f32(__a, __b) __extension__ \
  ({ \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4sf (__a, __b); \
     __rv.__i; \
   })

#define vtrnq_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv16qi ((int8x16_t) __a, (int8x16_t) __b); \
     __rv.__i; \
   })

#define vtrnq_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8hi ((int16x8_t) __a, (int16x8_t) __b); \
     __rv.__i; \
   })

#define vtrnq_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4si ((int32x4_t) __a, (int32x4_t) __b); \
     __rv.__i; \
   })

#define vtrnq_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv16qi ((int8x16_t) __a, (int8x16_t) __b); \
     __rv.__i; \
   })

#define vtrnq_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8hi ((int16x8_t) __a, (int16x8_t) __b); \
     __rv.__i; \
   })

#define vzip_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8qi (__a, __b); \
     __rv.__i; \
   })

#define vzip_s16(__a, __b) __extension__ \
  ({ \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4hi (__a, __b); \
     __rv.__i; \
   })

#define vzip_s32(__a, __b) __extension__ \
  ({ \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv2si (__a, __b); \
     __rv.__i; \
   })

#define vzip_f32(__a, __b) __extension__ \
  ({ \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv2sf (__a, __b); \
     __rv.__i; \
   })

#define vzip_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8qi ((int8x8_t) __a, (int8x8_t) __b); \
     __rv.__i; \
   })

#define vzip_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4hi ((int16x4_t) __a, (int16x4_t) __b); \
     __rv.__i; \
   })

#define vzip_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv2si ((int32x2_t) __a, (int32x2_t) __b); \
     __rv.__i; \
   })

#define vzip_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8qi ((int8x8_t) __a, (int8x8_t) __b); \
     __rv.__i; \
   })

#define vzip_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4hi ((int16x4_t) __a, (int16x4_t) __b); \
     __rv.__i; \
   })

#define vzipq_s8(__a, __b) __extension__ \
  ({ \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv16qi (__a, __b); \
     __rv.__i; \
   })

#define vzipq_s16(__a, __b) __extension__ \
  ({ \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8hi (__a, __b); \
     __rv.__i; \
   })

#define vzipq_s32(__a, __b) __extension__ \
  ({ \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4si (__a, __b); \
     __rv.__i; \
   })

#define vzipq_f32(__a, __b) __extension__ \
  ({ \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4sf (__a, __b); \
     __rv.__i; \
   })

#define vzipq_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv16qi ((int8x16_t) __a, (int8x16_t) __b); \
     __rv.__i; \
   })

#define vzipq_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8hi ((int16x8_t) __a, (int16x8_t) __b); \
     __rv.__i; \
   })

#define vzipq_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4si ((int32x4_t) __a, (int32x4_t) __b); \
     __rv.__i; \
   })

#define vzipq_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv16qi ((int8x16_t) __a, (int8x16_t) __b); \
     __rv.__i; \
   })

#define vzipq_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8hi ((int16x8_t) __a, (int16x8_t) __b); \
     __rv.__i; \
   })

#define vuzp_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8qi (__a, __b); \
     __rv.__i; \
   })

#define vuzp_s16(__a, __b) __extension__ \
  ({ \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4hi (__a, __b); \
     __rv.__i; \
   })

#define vuzp_s32(__a, __b) __extension__ \
  ({ \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv2si (__a, __b); \
     __rv.__i; \
   })

#define vuzp_f32(__a, __b) __extension__ \
  ({ \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv2sf (__a, __b); \
     __rv.__i; \
   })

#define vuzp_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8qi ((int8x8_t) __a, (int8x8_t) __b); \
     __rv.__i; \
   })

#define vuzp_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4hi ((int16x4_t) __a, (int16x4_t) __b); \
     __rv.__i; \
   })

#define vuzp_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv2si ((int32x2_t) __a, (int32x2_t) __b); \
     __rv.__i; \
   })

#define vuzp_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8qi ((int8x8_t) __a, (int8x8_t) __b); \
     __rv.__i; \
   })

#define vuzp_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4hi ((int16x4_t) __a, (int16x4_t) __b); \
     __rv.__i; \
   })

#define vuzpq_s8(__a, __b) __extension__ \
  ({ \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv16qi (__a, __b); \
     __rv.__i; \
   })

#define vuzpq_s16(__a, __b) __extension__ \
  ({ \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8hi (__a, __b); \
     __rv.__i; \
   })

#define vuzpq_s32(__a, __b) __extension__ \
  ({ \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4si (__a, __b); \
     __rv.__i; \
   })

#define vuzpq_f32(__a, __b) __extension__ \
  ({ \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4sf (__a, __b); \
     __rv.__i; \
   })

#define vuzpq_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv16qi ((int8x16_t) __a, (int8x16_t) __b); \
     __rv.__i; \
   })

#define vuzpq_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8hi ((int16x8_t) __a, (int16x8_t) __b); \
     __rv.__i; \
   })

#define vuzpq_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4si ((int32x4_t) __a, (int32x4_t) __b); \
     __rv.__i; \
   })

#define vuzpq_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv16qi ((int8x16_t) __a, (int8x16_t) __b); \
     __rv.__i; \
   })

#define vuzpq_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8hi ((int16x8_t) __a, (int16x8_t) __b); \
     __rv.__i; \
   })

#define vld1_s8(__a) \
  (int8x8_t)__builtin_neon_vld1v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1_s16(__a) \
  (int16x4_t)__builtin_neon_vld1v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1_s32(__a) \
  (int32x2_t)__builtin_neon_vld1v2si (__neon_ptr_cast(const __builtin_neon_si *, __a))

#define vld1_s64(__a) \
  (int64x1_t)__builtin_neon_vld1v1di (__neon_ptr_cast(const __builtin_neon_di *, __a))

#define vld1_f32(__a) \
  (float32x2_t)__builtin_neon_vld1v2sf (__a)

#define vld1_u8(__a) \
  (uint8x8_t)__builtin_neon_vld1v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1_u16(__a) \
  (uint16x4_t)__builtin_neon_vld1v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1_u32(__a) \
  (uint32x2_t)__builtin_neon_vld1v2si (__neon_ptr_cast(const __builtin_neon_si *, __a))

#define vld1_u64(__a) \
  (uint64x1_t)__builtin_neon_vld1v1di (__neon_ptr_cast(const __builtin_neon_di *, __a))

#define vld1_p8(__a) \
  (poly8x8_t)__builtin_neon_vld1v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1_p16(__a) \
  (poly16x4_t)__builtin_neon_vld1v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1q_s8(__a) \
  (int8x16_t)__builtin_neon_vld1v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1q_s16(__a) \
  (int16x8_t)__builtin_neon_vld1v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1q_s32(__a) \
  (int32x4_t)__builtin_neon_vld1v4si (__neon_ptr_cast(const __builtin_neon_si *, __a))

#define vld1q_s64(__a) \
  (int64x2_t)__builtin_neon_vld1v2di (__neon_ptr_cast(const __builtin_neon_di *, __a))

#define vld1q_f32(__a) \
  (float32x4_t)__builtin_neon_vld1v4sf (__a)

#define vld1q_u8(__a) \
  (uint8x16_t)__builtin_neon_vld1v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1q_u16(__a) \
  (uint16x8_t)__builtin_neon_vld1v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1q_u32(__a) \
  (uint32x4_t)__builtin_neon_vld1v4si (__neon_ptr_cast(const __builtin_neon_si *, __a))

#define vld1q_u64(__a) \
  (uint64x2_t)__builtin_neon_vld1v2di (__neon_ptr_cast(const __builtin_neon_di *, __a))

#define vld1q_p8(__a) \
  (poly8x16_t)__builtin_neon_vld1v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1q_p16(__a) \
  (poly16x8_t)__builtin_neon_vld1v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1_lane_s8(__a, __b, __c) \
  (int8x8_t)__builtin_neon_vld1_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __b, __c)

#define vld1_lane_s16(__a, __b, __c) \
  (int16x4_t)__builtin_neon_vld1_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __b, __c)

#define vld1_lane_s32(__a, __b, __c) \
  (int32x2_t)__builtin_neon_vld1_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __a), __b, __c)

#define vld1_lane_f32(__a, __b, __c) \
  (float32x2_t)__builtin_neon_vld1_lanev2sf (__a, __b, __c)

#define vld1_lane_u8(__a, __b, __c) \
  (uint8x8_t)__builtin_neon_vld1_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), (int8x8_t) __b, __c)

#define vld1_lane_u16(__a, __b, __c) \
  (uint16x4_t)__builtin_neon_vld1_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), (int16x4_t) __b, __c)

#define vld1_lane_u32(__a, __b, __c) \
  (uint32x2_t)__builtin_neon_vld1_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __a), (int32x2_t) __b, __c)

#define vld1_lane_p8(__a, __b, __c) \
  (poly8x8_t)__builtin_neon_vld1_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), (int8x8_t) __b, __c)

#define vld1_lane_p16(__a, __b, __c) \
  (poly16x4_t)__builtin_neon_vld1_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), (int16x4_t) __b, __c)

#define vld1_lane_s64(__a, __b, __c) \
  (int64x1_t)__builtin_neon_vld1_lanev1di (__neon_ptr_cast(const __builtin_neon_di *, __a), __b, __c)

#define vld1_lane_u64(__a, __b, __c) \
  (uint64x1_t)__builtin_neon_vld1_lanev1di (__neon_ptr_cast(const __builtin_neon_di *, __a), (int64x1_t) __b, __c)

#define vld1q_lane_s8(__a, __b, __c) \
  (int8x16_t)__builtin_neon_vld1_lanev16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __b, __c)

#define vld1q_lane_s16(__a, __b, __c) \
  (int16x8_t)__builtin_neon_vld1_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __b, __c)

#define vld1q_lane_s32(__a, __b, __c) \
  (int32x4_t)__builtin_neon_vld1_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __a), __b, __c)

#define vld1q_lane_f32(__a, __b, __c) \
  (float32x4_t)__builtin_neon_vld1_lanev4sf (__a, __b, __c)

#define vld1q_lane_u8(__a, __b, __c) \
  (uint8x16_t)__builtin_neon_vld1_lanev16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), (int8x16_t) __b, __c)

#define vld1q_lane_u16(__a, __b, __c) \
  (uint16x8_t)__builtin_neon_vld1_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), (int16x8_t) __b, __c)

#define vld1q_lane_u32(__a, __b, __c) \
  (uint32x4_t)__builtin_neon_vld1_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __a), (int32x4_t) __b, __c)

#define vld1q_lane_p8(__a, __b, __c) \
  (poly8x16_t)__builtin_neon_vld1_lanev16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), (int8x16_t) __b, __c)

#define vld1q_lane_p16(__a, __b, __c) \
  (poly16x8_t)__builtin_neon_vld1_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), (int16x8_t) __b, __c)

#define vld1q_lane_s64(__a, __b, __c) \
  (int64x2_t)__builtin_neon_vld1_lanev2di (__neon_ptr_cast(const __builtin_neon_di *, __a), __b, __c)

#define vld1q_lane_u64(__a, __b, __c) \
  (uint64x2_t)__builtin_neon_vld1_lanev2di (__neon_ptr_cast(const __builtin_neon_di *, __a), (int64x2_t) __b, __c)

#define vld1_dup_s8(__a) \
  (int8x8_t)__builtin_neon_vld1_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1_dup_s16(__a) \
  (int16x4_t)__builtin_neon_vld1_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1_dup_s32(__a) \
  (int32x2_t)__builtin_neon_vld1_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __a))

#define vld1_dup_f32(__a) \
  (float32x2_t)__builtin_neon_vld1_dupv2sf (__a)

#define vld1_dup_u8(__a) \
  (uint8x8_t)__builtin_neon_vld1_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1_dup_u16(__a) \
  (uint16x4_t)__builtin_neon_vld1_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1_dup_u32(__a) \
  (uint32x2_t)__builtin_neon_vld1_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __a))

#define vld1_dup_p8(__a) \
  (poly8x8_t)__builtin_neon_vld1_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1_dup_p16(__a) \
  (poly16x4_t)__builtin_neon_vld1_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1_dup_s64(__a) \
  (int64x1_t)__builtin_neon_vld1_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __a))

#define vld1_dup_u64(__a) \
  (uint64x1_t)__builtin_neon_vld1_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __a))

#define vld1q_dup_s8(__a) \
  (int8x16_t)__builtin_neon_vld1_dupv16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1q_dup_s16(__a) \
  (int16x8_t)__builtin_neon_vld1_dupv8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1q_dup_s32(__a) \
  (int32x4_t)__builtin_neon_vld1_dupv4si (__neon_ptr_cast(const __builtin_neon_si *, __a))

#define vld1q_dup_f32(__a) \
  (float32x4_t)__builtin_neon_vld1_dupv4sf (__a)

#define vld1q_dup_u8(__a) \
  (uint8x16_t)__builtin_neon_vld1_dupv16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1q_dup_u16(__a) \
  (uint16x8_t)__builtin_neon_vld1_dupv8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1q_dup_u32(__a) \
  (uint32x4_t)__builtin_neon_vld1_dupv4si (__neon_ptr_cast(const __builtin_neon_si *, __a))

#define vld1q_dup_p8(__a) \
  (poly8x16_t)__builtin_neon_vld1_dupv16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a))

#define vld1q_dup_p16(__a) \
  (poly16x8_t)__builtin_neon_vld1_dupv8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a))

#define vld1q_dup_s64(__a) \
  (int64x2_t)__builtin_neon_vld1_dupv2di (__neon_ptr_cast(const __builtin_neon_di *, __a))

#define vld1q_dup_u64(__a) \
  (uint64x2_t)__builtin_neon_vld1_dupv2di (__neon_ptr_cast(const __builtin_neon_di *, __a))

#define vst1_s8(__a, __b) \
  __builtin_neon_vst1v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __b)

#define vst1_s16(__a, __b) \
  __builtin_neon_vst1v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __b)

#define vst1_s32(__a, __b) \
  __builtin_neon_vst1v2si (__neon_ptr_cast(__builtin_neon_si *, __a), __b)

#define vst1_s64(__a, __b) \
  __builtin_neon_vst1v1di (__neon_ptr_cast(__builtin_neon_di *, __a), __b)

#define vst1_f32(__a, __b) \
  __builtin_neon_vst1v2sf (__a, __b)

#define vst1_u8(__a, __b) \
  __builtin_neon_vst1v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), (int8x8_t) __b)

#define vst1_u16(__a, __b) \
  __builtin_neon_vst1v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), (int16x4_t) __b)

#define vst1_u32(__a, __b) \
  __builtin_neon_vst1v2si (__neon_ptr_cast(__builtin_neon_si *, __a), (int32x2_t) __b)

#define vst1_u64(__a, __b) \
  __builtin_neon_vst1v1di (__neon_ptr_cast(__builtin_neon_di *, __a), (int64x1_t) __b)

#define vst1_p8(__a, __b) \
  __builtin_neon_vst1v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), (int8x8_t) __b)

#define vst1_p16(__a, __b) \
  __builtin_neon_vst1v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), (int16x4_t) __b)

#define vst1q_s8(__a, __b) \
  __builtin_neon_vst1v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __b)

#define vst1q_s16(__a, __b) \
  __builtin_neon_vst1v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __b)

#define vst1q_s32(__a, __b) \
  __builtin_neon_vst1v4si (__neon_ptr_cast(__builtin_neon_si *, __a), __b)

#define vst1q_s64(__a, __b) \
  __builtin_neon_vst1v2di (__neon_ptr_cast(__builtin_neon_di *, __a), __b)

#define vst1q_f32(__a, __b) \
  __builtin_neon_vst1v4sf (__a, __b)

#define vst1q_u8(__a, __b) \
  __builtin_neon_vst1v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), (int8x16_t) __b)

#define vst1q_u16(__a, __b) \
  __builtin_neon_vst1v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), (int16x8_t) __b)

#define vst1q_u32(__a, __b) \
  __builtin_neon_vst1v4si (__neon_ptr_cast(__builtin_neon_si *, __a), (int32x4_t) __b)

#define vst1q_u64(__a, __b) \
  __builtin_neon_vst1v2di (__neon_ptr_cast(__builtin_neon_di *, __a), (int64x2_t) __b)

#define vst1q_p8(__a, __b) \
  __builtin_neon_vst1v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), (int8x16_t) __b)

#define vst1q_p16(__a, __b) \
  __builtin_neon_vst1v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), (int16x8_t) __b)

#define vst1_lane_s8(__a, __b, __c) \
  __builtin_neon_vst1_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __b, __c)

#define vst1_lane_s16(__a, __b, __c) \
  __builtin_neon_vst1_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __b, __c)

#define vst1_lane_s32(__a, __b, __c) \
  __builtin_neon_vst1_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __a), __b, __c)

#define vst1_lane_f32(__a, __b, __c) \
  __builtin_neon_vst1_lanev2sf (__a, __b, __c)

#define vst1_lane_u8(__a, __b, __c) \
  __builtin_neon_vst1_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), (int8x8_t) __b, __c)

#define vst1_lane_u16(__a, __b, __c) \
  __builtin_neon_vst1_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), (int16x4_t) __b, __c)

#define vst1_lane_u32(__a, __b, __c) \
  __builtin_neon_vst1_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __a), (int32x2_t) __b, __c)

#define vst1_lane_p8(__a, __b, __c) \
  __builtin_neon_vst1_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), (int8x8_t) __b, __c)

#define vst1_lane_p16(__a, __b, __c) \
  __builtin_neon_vst1_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), (int16x4_t) __b, __c)

#define vst1_lane_s64(__a, __b, __c) \
  __builtin_neon_vst1_lanev1di (__neon_ptr_cast(__builtin_neon_di *, __a), __b, __c)

#define vst1_lane_u64(__a, __b, __c) \
  __builtin_neon_vst1_lanev1di (__neon_ptr_cast(__builtin_neon_di *, __a), (int64x1_t) __b, __c)

#define vst1q_lane_s8(__a, __b, __c) \
  __builtin_neon_vst1_lanev16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __b, __c)

#define vst1q_lane_s16(__a, __b, __c) \
  __builtin_neon_vst1_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __b, __c)

#define vst1q_lane_s32(__a, __b, __c) \
  __builtin_neon_vst1_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __a), __b, __c)

#define vst1q_lane_f32(__a, __b, __c) \
  __builtin_neon_vst1_lanev4sf (__a, __b, __c)

#define vst1q_lane_u8(__a, __b, __c) \
  __builtin_neon_vst1_lanev16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), (int8x16_t) __b, __c)

#define vst1q_lane_u16(__a, __b, __c) \
  __builtin_neon_vst1_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), (int16x8_t) __b, __c)

#define vst1q_lane_u32(__a, __b, __c) \
  __builtin_neon_vst1_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __a), (int32x4_t) __b, __c)

#define vst1q_lane_p8(__a, __b, __c) \
  __builtin_neon_vst1_lanev16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), (int8x16_t) __b, __c)

#define vst1q_lane_p16(__a, __b, __c) \
  __builtin_neon_vst1_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), (int16x8_t) __b, __c)

#define vst1q_lane_s64(__a, __b, __c) \
  __builtin_neon_vst1_lanev2di (__neon_ptr_cast(__builtin_neon_di *, __a), __b, __c)

#define vst1q_lane_u64(__a, __b, __c) \
  __builtin_neon_vst1_lanev2di (__neon_ptr_cast(__builtin_neon_di *, __a), (int64x2_t) __b, __c)

#define vld2_s8(__a) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2_s16(__a) __extension__ \
  ({ \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2_s32(__a) __extension__ \
  ({ \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld2_f32(__a) __extension__ \
  ({ \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v2sf (__a); \
     __rv.__i; \
   })

#define vld2_u8(__a) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2_u16(__a) __extension__ \
  ({ \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2_u32(__a) __extension__ \
  ({ \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld2_p8(__a) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2_p16(__a) __extension__ \
  ({ \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2_s64(__a) __extension__ \
  ({ \
     union { int64x1x2_t __i; __neon_int64x1x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld2_u64(__a) __extension__ \
  ({ \
     union { uint64x1x2_t __i; __neon_int64x1x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld2q_s8(__a) __extension__ \
  ({ \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2q_s16(__a) __extension__ \
  ({ \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2q_s32(__a) __extension__ \
  ({ \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld2q_f32(__a) __extension__ \
  ({ \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4sf (__a); \
     __rv.__i; \
   })

#define vld2q_u8(__a) __extension__ \
  ({ \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2q_u16(__a) __extension__ \
  ({ \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2q_u32(__a) __extension__ \
  ({ \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld2q_p8(__a) __extension__ \
  ({ \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2q_p16(__a) __extension__ \
  ({ \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __bu = { __b }; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev2sf (__a, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __bu = { __b }; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4sf (__a, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_dup_s8(__a) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_s16(__a) __extension__ \
  ({ \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_s32(__a) __extension__ \
  ({ \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_f32(__a) __extension__ \
  ({ \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv2sf (__a); \
     __rv.__i; \
   })

#define vld2_dup_u8(__a) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_u16(__a) __extension__ \
  ({ \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_u32(__a) __extension__ \
  ({ \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_p8(__a) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_p16(__a) __extension__ \
  ({ \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_s64(__a) __extension__ \
  ({ \
     union { int64x1x2_t __i; __neon_int64x1x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld2_dup_u64(__a) __extension__ \
  ({ \
     union { uint64x1x2_t __i; __neon_int64x1x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vst2_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst2_s16(__a, __b) __extension__ \
  ({ \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst2_s32(__a, __b) __extension__ \
  ({ \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst2_f32(__a, __b) __extension__ \
  ({ \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v2sf (__a, __bu.__o); \
   })

#define vst2_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst2_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst2_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst2_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst2_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst2_s64(__a, __b) __extension__ \
  ({ \
     union { int64x1x2_t __i; __neon_int64x1x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v1di (__neon_ptr_cast(__builtin_neon_di *, __a), __bu.__o); \
   })

#define vst2_u64(__a, __b) __extension__ \
  ({ \
     union { uint64x1x2_t __i; __neon_int64x1x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v1di (__neon_ptr_cast(__builtin_neon_di *, __a), __bu.__o); \
   })

#define vst2q_s8(__a, __b) __extension__ \
  ({ \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst2q_s16(__a, __b) __extension__ \
  ({ \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst2q_s32(__a, __b) __extension__ \
  ({ \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst2q_f32(__a, __b) __extension__ \
  ({ \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4sf (__a, __bu.__o); \
   })

#define vst2q_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst2q_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst2q_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst2q_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst2q_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst2_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst2_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst2_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst2_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev2sf (__a, __bu.__o, __c); \
   })

#define vst2_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst2_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst2_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst2_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst2_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst2q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst2q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst2q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4sf (__a, __bu.__o, __c); \
   })

#define vst2q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst2q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst2q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vld3_s8(__a) __extension__ \
  ({ \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3_s16(__a) __extension__ \
  ({ \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3_s32(__a) __extension__ \
  ({ \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld3_f32(__a) __extension__ \
  ({ \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v2sf (__a); \
     __rv.__i; \
   })

#define vld3_u8(__a) __extension__ \
  ({ \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3_u16(__a) __extension__ \
  ({ \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3_u32(__a) __extension__ \
  ({ \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld3_p8(__a) __extension__ \
  ({ \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3_p16(__a) __extension__ \
  ({ \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3_s64(__a) __extension__ \
  ({ \
     union { int64x1x3_t __i; __neon_int64x1x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld3_u64(__a) __extension__ \
  ({ \
     union { uint64x1x3_t __i; __neon_int64x1x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld3q_s8(__a) __extension__ \
  ({ \
     union { int8x16x3_t __i; __neon_int8x16x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3q_s16(__a) __extension__ \
  ({ \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3q_s32(__a) __extension__ \
  ({ \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld3q_f32(__a) __extension__ \
  ({ \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4sf (__a); \
     __rv.__i; \
   })

#define vld3q_u8(__a) __extension__ \
  ({ \
     union { uint8x16x3_t __i; __neon_int8x16x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3q_u16(__a) __extension__ \
  ({ \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3q_u32(__a) __extension__ \
  ({ \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld3q_p8(__a) __extension__ \
  ({ \
     union { poly8x16x3_t __i; __neon_int8x16x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3q_p16(__a) __extension__ \
  ({ \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __bu = { __b }; \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev2sf (__a, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __bu = { __b }; \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4sf (__a, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_dup_s8(__a) __extension__ \
  ({ \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_s16(__a) __extension__ \
  ({ \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_s32(__a) __extension__ \
  ({ \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_f32(__a) __extension__ \
  ({ \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv2sf (__a); \
     __rv.__i; \
   })

#define vld3_dup_u8(__a) __extension__ \
  ({ \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_u16(__a) __extension__ \
  ({ \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_u32(__a) __extension__ \
  ({ \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_p8(__a) __extension__ \
  ({ \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_p16(__a) __extension__ \
  ({ \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_s64(__a) __extension__ \
  ({ \
     union { int64x1x3_t __i; __neon_int64x1x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld3_dup_u64(__a) __extension__ \
  ({ \
     union { uint64x1x3_t __i; __neon_int64x1x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vst3_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst3_s16(__a, __b) __extension__ \
  ({ \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst3_s32(__a, __b) __extension__ \
  ({ \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst3_f32(__a, __b) __extension__ \
  ({ \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v2sf (__a, __bu.__o); \
   })

#define vst3_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst3_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst3_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst3_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst3_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst3_s64(__a, __b) __extension__ \
  ({ \
     union { int64x1x3_t __i; __neon_int64x1x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v1di (__neon_ptr_cast(__builtin_neon_di *, __a), __bu.__o); \
   })

#define vst3_u64(__a, __b) __extension__ \
  ({ \
     union { uint64x1x3_t __i; __neon_int64x1x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v1di (__neon_ptr_cast(__builtin_neon_di *, __a), __bu.__o); \
   })

#define vst3q_s8(__a, __b) __extension__ \
  ({ \
     union { int8x16x3_t __i; __neon_int8x16x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst3q_s16(__a, __b) __extension__ \
  ({ \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst3q_s32(__a, __b) __extension__ \
  ({ \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst3q_f32(__a, __b) __extension__ \
  ({ \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4sf (__a, __bu.__o); \
   })

#define vst3q_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x16x3_t __i; __neon_int8x16x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst3q_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst3q_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst3q_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x16x3_t __i; __neon_int8x16x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst3q_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst3_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst3_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst3_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst3_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev2sf (__a, __bu.__o, __c); \
   })

#define vst3_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst3_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst3_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst3_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst3_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst3q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst3q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst3q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4sf (__a, __bu.__o, __c); \
   })

#define vst3q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst3q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst3q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vld4_s8(__a) __extension__ \
  ({ \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4_s16(__a) __extension__ \
  ({ \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4_s32(__a) __extension__ \
  ({ \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld4_f32(__a) __extension__ \
  ({ \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v2sf (__a); \
     __rv.__i; \
   })

#define vld4_u8(__a) __extension__ \
  ({ \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4_u16(__a) __extension__ \
  ({ \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4_u32(__a) __extension__ \
  ({ \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld4_p8(__a) __extension__ \
  ({ \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4_p16(__a) __extension__ \
  ({ \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4_s64(__a) __extension__ \
  ({ \
     union { int64x1x4_t __i; __neon_int64x1x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld4_u64(__a) __extension__ \
  ({ \
     union { uint64x1x4_t __i; __neon_int64x1x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld4q_s8(__a) __extension__ \
  ({ \
     union { int8x16x4_t __i; __neon_int8x16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4q_s16(__a) __extension__ \
  ({ \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4q_s32(__a) __extension__ \
  ({ \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld4q_f32(__a) __extension__ \
  ({ \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4sf (__a); \
     __rv.__i; \
   })

#define vld4q_u8(__a) __extension__ \
  ({ \
     union { uint8x16x4_t __i; __neon_int8x16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4q_u16(__a) __extension__ \
  ({ \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4q_u32(__a) __extension__ \
  ({ \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld4q_p8(__a) __extension__ \
  ({ \
     union { poly8x16x4_t __i; __neon_int8x16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4q_p16(__a) __extension__ \
  ({ \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __bu = { __b }; \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev2sf (__a, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __bu = { __b }; \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4sf (__a, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __a), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_dup_s8(__a) __extension__ \
  ({ \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_s16(__a) __extension__ \
  ({ \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_s32(__a) __extension__ \
  ({ \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_f32(__a) __extension__ \
  ({ \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv2sf (__a); \
     __rv.__i; \
   })

#define vld4_dup_u8(__a) __extension__ \
  ({ \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_u16(__a) __extension__ \
  ({ \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_u32(__a) __extension__ \
  ({ \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_p8(__a) __extension__ \
  ({ \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_p16(__a) __extension__ \
  ({ \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_s64(__a) __extension__ \
  ({ \
     union { int64x1x4_t __i; __neon_int64x1x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vld4_dup_u64(__a) __extension__ \
  ({ \
     union { uint64x1x4_t __i; __neon_int64x1x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __a)); \
     __rv.__i; \
   })

#define vst4_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst4_s16(__a, __b) __extension__ \
  ({ \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst4_s32(__a, __b) __extension__ \
  ({ \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst4_f32(__a, __b) __extension__ \
  ({ \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v2sf (__a, __bu.__o); \
   })

#define vst4_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst4_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst4_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst4_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst4_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst4_s64(__a, __b) __extension__ \
  ({ \
     union { int64x1x4_t __i; __neon_int64x1x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v1di (__neon_ptr_cast(__builtin_neon_di *, __a), __bu.__o); \
   })

#define vst4_u64(__a, __b) __extension__ \
  ({ \
     union { uint64x1x4_t __i; __neon_int64x1x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v1di (__neon_ptr_cast(__builtin_neon_di *, __a), __bu.__o); \
   })

#define vst4q_s8(__a, __b) __extension__ \
  ({ \
     union { int8x16x4_t __i; __neon_int8x16x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst4q_s16(__a, __b) __extension__ \
  ({ \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst4q_s32(__a, __b) __extension__ \
  ({ \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst4q_f32(__a, __b) __extension__ \
  ({ \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4sf (__a, __bu.__o); \
   })

#define vst4q_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x16x4_t __i; __neon_int8x16x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst4q_u16(__a, __b) __extension__ \
  ({ \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst4q_u32(__a, __b) __extension__ \
  ({ \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o); \
   })

#define vst4q_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x16x4_t __i; __neon_int8x16x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v16qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o); \
   })

#define vst4q_p16(__a, __b) __extension__ \
  ({ \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o); \
   })

#define vst4_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst4_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst4_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst4_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev2sf (__a, __bu.__o, __c); \
   })

#define vst4_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst4_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst4_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst4_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __a), __bu.__o, __c); \
   })

#define vst4_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst4q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst4q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst4q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4sf (__a, __bu.__o, __c); \
   })

#define vst4q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vst4q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __a), __bu.__o, __c); \
   })

#define vst4q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __a), __bu.__o, __c); \
   })

#define vand_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vandv8qi (__a, __b, 1)

#define vand_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vandv4hi (__a, __b, 1)

#define vand_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vandv2si (__a, __b, 1)

#define vand_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vandv1di (__a, __b, 1)

#define vand_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vandv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vand_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vandv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vand_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vandv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vand_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vandv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define vandq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vandv16qi (__a, __b, 1)

#define vandq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vandv8hi (__a, __b, 1)

#define vandq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vandv4si (__a, __b, 1)

#define vandq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vandv2di (__a, __b, 1)

#define vandq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vandv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vandq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vandv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vandq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vandv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vandq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vandv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vorr_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vorrv8qi (__a, __b, 1)

#define vorr_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vorrv4hi (__a, __b, 1)

#define vorr_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vorrv2si (__a, __b, 1)

#define vorr_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vorrv1di (__a, __b, 1)

#define vorr_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vorrv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vorr_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vorrv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vorr_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vorrv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vorr_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vorrv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define vorrq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vorrv16qi (__a, __b, 1)

#define vorrq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vorrv8hi (__a, __b, 1)

#define vorrq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vorrv4si (__a, __b, 1)

#define vorrq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vorrv2di (__a, __b, 1)

#define vorrq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vorrv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vorrq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vorrv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vorrq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vorrv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vorrq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vorrv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define veor_s8(__a, __b) \
  (int8x8_t)__builtin_neon_veorv8qi (__a, __b, 1)

#define veor_s16(__a, __b) \
  (int16x4_t)__builtin_neon_veorv4hi (__a, __b, 1)

#define veor_s32(__a, __b) \
  (int32x2_t)__builtin_neon_veorv2si (__a, __b, 1)

#define veor_s64(__a, __b) \
  (int64x1_t)__builtin_neon_veorv1di (__a, __b, 1)

#define veor_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_veorv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define veor_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_veorv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define veor_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_veorv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define veor_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_veorv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define veorq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_veorv16qi (__a, __b, 1)

#define veorq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_veorv8hi (__a, __b, 1)

#define veorq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_veorv4si (__a, __b, 1)

#define veorq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_veorv2di (__a, __b, 1)

#define veorq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_veorv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define veorq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_veorv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define veorq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_veorv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define veorq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_veorv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vbic_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vbicv8qi (__a, __b, 1)

#define vbic_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vbicv4hi (__a, __b, 1)

#define vbic_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vbicv2si (__a, __b, 1)

#define vbic_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vbicv1di (__a, __b, 1)

#define vbic_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vbicv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vbic_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vbicv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vbic_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vbicv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vbic_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vbicv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define vbicq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vbicv16qi (__a, __b, 1)

#define vbicq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vbicv8hi (__a, __b, 1)

#define vbicq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vbicv4si (__a, __b, 1)

#define vbicq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vbicv2di (__a, __b, 1)

#define vbicq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vbicv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vbicq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vbicv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vbicq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vbicv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vbicq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vbicv2di ((int64x2_t) __a, (int64x2_t) __b, 0)

#define vorn_s8(__a, __b) \
  (int8x8_t)__builtin_neon_vornv8qi (__a, __b, 1)

#define vorn_s16(__a, __b) \
  (int16x4_t)__builtin_neon_vornv4hi (__a, __b, 1)

#define vorn_s32(__a, __b) \
  (int32x2_t)__builtin_neon_vornv2si (__a, __b, 1)

#define vorn_s64(__a, __b) \
  (int64x1_t)__builtin_neon_vornv1di (__a, __b, 1)

#define vorn_u8(__a, __b) \
  (uint8x8_t)__builtin_neon_vornv8qi ((int8x8_t) __a, (int8x8_t) __b, 0)

#define vorn_u16(__a, __b) \
  (uint16x4_t)__builtin_neon_vornv4hi ((int16x4_t) __a, (int16x4_t) __b, 0)

#define vorn_u32(__a, __b) \
  (uint32x2_t)__builtin_neon_vornv2si ((int32x2_t) __a, (int32x2_t) __b, 0)

#define vorn_u64(__a, __b) \
  (uint64x1_t)__builtin_neon_vornv1di ((int64x1_t) __a, (int64x1_t) __b, 0)

#define vornq_s8(__a, __b) \
  (int8x16_t)__builtin_neon_vornv16qi (__a, __b, 1)

#define vornq_s16(__a, __b) \
  (int16x8_t)__builtin_neon_vornv8hi (__a, __b, 1)

#define vornq_s32(__a, __b) \
  (int32x4_t)__builtin_neon_vornv4si (__a, __b, 1)

#define vornq_s64(__a, __b) \
  (int64x2_t)__builtin_neon_vornv2di (__a, __b, 1)

#define vornq_u8(__a, __b) \
  (uint8x16_t)__builtin_neon_vornv16qi ((int8x16_t) __a, (int8x16_t) __b, 0)

#define vornq_u16(__a, __b) \
  (uint16x8_t)__builtin_neon_vornv8hi ((int16x8_t) __a, (int16x8_t) __b, 0)

#define vornq_u32(__a, __b) \
  (uint32x4_t)__builtin_neon_vornv4si ((int32x4_t) __a, (int32x4_t) __b, 0)

#define vornq_u64(__a, __b) \
  (uint64x2_t)__builtin_neon_vornv2di ((int64x2_t) __a, (int64x2_t) __b, 0)


#define vreinterpret_p8_s8(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv8qi (__a)

#define vreinterpret_p8_s16(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv4hi (__a)

#define vreinterpret_p8_s32(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv2si (__a)

#define vreinterpret_p8_s64(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv1di (__a)

#define vreinterpret_p8_f32(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv2sf (__a)

#define vreinterpret_p8_u8(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv8qi ((int8x8_t) __a)

#define vreinterpret_p8_u16(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a)

#define vreinterpret_p8_u32(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv2si ((int32x2_t) __a)

#define vreinterpret_p8_u64(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv1di ((int64x1_t) __a)

#define vreinterpret_p8_p16(__a) \
  (poly8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a)

#define vreinterpretq_p8_s8(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv16qi (__a)

#define vreinterpretq_p8_s16(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv8hi (__a)

#define vreinterpretq_p8_s32(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv4si (__a)

#define vreinterpretq_p8_s64(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv2di (__a)

#define vreinterpretq_p8_f32(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv4sf (__a)

#define vreinterpretq_p8_u8(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv16qi ((int8x16_t) __a)

#define vreinterpretq_p8_u16(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a)

#define vreinterpretq_p8_u32(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv4si ((int32x4_t) __a)

#define vreinterpretq_p8_u64(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv2di ((int64x2_t) __a)

#define vreinterpretq_p8_p16(__a) \
  (poly8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a)

#define vreinterpret_p16_s8(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv8qi (__a)

#define vreinterpret_p16_s16(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv4hi (__a)

#define vreinterpret_p16_s32(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv2si (__a)

#define vreinterpret_p16_s64(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv1di (__a)

#define vreinterpret_p16_f32(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv2sf (__a)

#define vreinterpret_p16_u8(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a)

#define vreinterpret_p16_u16(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv4hi ((int16x4_t) __a)

#define vreinterpret_p16_u32(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv2si ((int32x2_t) __a)

#define vreinterpret_p16_u64(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv1di ((int64x1_t) __a)

#define vreinterpret_p16_p8(__a) \
  (poly16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a)

#define vreinterpretq_p16_s8(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv16qi (__a)

#define vreinterpretq_p16_s16(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv8hi (__a)

#define vreinterpretq_p16_s32(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv4si (__a)

#define vreinterpretq_p16_s64(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv2di (__a)

#define vreinterpretq_p16_f32(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv4sf (__a)

#define vreinterpretq_p16_u8(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a)

#define vreinterpretq_p16_u16(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv8hi ((int16x8_t) __a)

#define vreinterpretq_p16_u32(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv4si ((int32x4_t) __a)

#define vreinterpretq_p16_u64(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv2di ((int64x2_t) __a)

#define vreinterpretq_p16_p8(__a) \
  (poly16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a)

#define vreinterpret_f32_s8(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv8qi (__a)

#define vreinterpret_f32_s16(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv4hi (__a)

#define vreinterpret_f32_s32(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv2si (__a)

#define vreinterpret_f32_s64(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv1di (__a)

#define vreinterpret_f32_u8(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv8qi ((int8x8_t) __a)

#define vreinterpret_f32_u16(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv4hi ((int16x4_t) __a)

#define vreinterpret_f32_u32(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv2si ((int32x2_t) __a)

#define vreinterpret_f32_u64(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv1di ((int64x1_t) __a)

#define vreinterpret_f32_p8(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv8qi ((int8x8_t) __a)

#define vreinterpret_f32_p16(__a) \
  (float32x2_t)__builtin_neon_vreinterpretv2sfv4hi ((int16x4_t) __a)

#define vreinterpretq_f32_s8(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv16qi (__a)

#define vreinterpretq_f32_s16(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv8hi (__a)

#define vreinterpretq_f32_s32(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv4si (__a)

#define vreinterpretq_f32_s64(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv2di (__a)

#define vreinterpretq_f32_u8(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv16qi ((int8x16_t) __a)

#define vreinterpretq_f32_u16(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv8hi ((int16x8_t) __a)

#define vreinterpretq_f32_u32(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv4si ((int32x4_t) __a)

#define vreinterpretq_f32_u64(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv2di ((int64x2_t) __a)

#define vreinterpretq_f32_p8(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv16qi ((int8x16_t) __a)

#define vreinterpretq_f32_p16(__a) \
  (float32x4_t)__builtin_neon_vreinterpretv4sfv8hi ((int16x8_t) __a)

#define vreinterpret_s64_s8(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div8qi (__a)

#define vreinterpret_s64_s16(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div4hi (__a)

#define vreinterpret_s64_s32(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div2si (__a)

#define vreinterpret_s64_f32(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div2sf (__a)

#define vreinterpret_s64_u8(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div8qi ((int8x8_t) __a)

#define vreinterpret_s64_u16(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div4hi ((int16x4_t) __a)

#define vreinterpret_s64_u32(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div2si ((int32x2_t) __a)

#define vreinterpret_s64_u64(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div1di ((int64x1_t) __a)

#define vreinterpret_s64_p8(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div8qi ((int8x8_t) __a)

#define vreinterpret_s64_p16(__a) \
  (int64x1_t)__builtin_neon_vreinterpretv1div4hi ((int16x4_t) __a)

#define vreinterpretq_s64_s8(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div16qi (__a)

#define vreinterpretq_s64_s16(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div8hi (__a)

#define vreinterpretq_s64_s32(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div4si (__a)

#define vreinterpretq_s64_f32(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div4sf (__a)

#define vreinterpretq_s64_u8(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div16qi ((int8x16_t) __a)

#define vreinterpretq_s64_u16(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div8hi ((int16x8_t) __a)

#define vreinterpretq_s64_u32(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div4si ((int32x4_t) __a)

#define vreinterpretq_s64_u64(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div2di ((int64x2_t) __a)

#define vreinterpretq_s64_p8(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div16qi ((int8x16_t) __a)

#define vreinterpretq_s64_p16(__a) \
  (int64x2_t)__builtin_neon_vreinterpretv2div8hi ((int16x8_t) __a)

#define vreinterpret_u64_s8(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div8qi (__a)

#define vreinterpret_u64_s16(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div4hi (__a)

#define vreinterpret_u64_s32(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div2si (__a)

#define vreinterpret_u64_s64(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div1di (__a)

#define vreinterpret_u64_f32(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div2sf (__a)

#define vreinterpret_u64_u8(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div8qi ((int8x8_t) __a)

#define vreinterpret_u64_u16(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div4hi ((int16x4_t) __a)

#define vreinterpret_u64_u32(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div2si ((int32x2_t) __a)

#define vreinterpret_u64_p8(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div8qi ((int8x8_t) __a)

#define vreinterpret_u64_p16(__a) \
  (uint64x1_t)__builtin_neon_vreinterpretv1div4hi ((int16x4_t) __a)

#define vreinterpretq_u64_s8(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div16qi (__a)

#define vreinterpretq_u64_s16(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div8hi (__a)

#define vreinterpretq_u64_s32(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div4si (__a)

#define vreinterpretq_u64_s64(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div2di (__a)

#define vreinterpretq_u64_f32(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div4sf (__a)

#define vreinterpretq_u64_u8(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div16qi ((int8x16_t) __a)

#define vreinterpretq_u64_u16(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div8hi ((int16x8_t) __a)

#define vreinterpretq_u64_u32(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div4si ((int32x4_t) __a)

#define vreinterpretq_u64_p8(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div16qi ((int8x16_t) __a)

#define vreinterpretq_u64_p16(__a) \
  (uint64x2_t)__builtin_neon_vreinterpretv2div8hi ((int16x8_t) __a)

#define vreinterpret_s8_s16(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv4hi (__a)

#define vreinterpret_s8_s32(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv2si (__a)

#define vreinterpret_s8_s64(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv1di (__a)

#define vreinterpret_s8_f32(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv2sf (__a)

#define vreinterpret_s8_u8(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv8qi ((int8x8_t) __a)

#define vreinterpret_s8_u16(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a)

#define vreinterpret_s8_u32(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv2si ((int32x2_t) __a)

#define vreinterpret_s8_u64(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv1di ((int64x1_t) __a)

#define vreinterpret_s8_p8(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv8qi ((int8x8_t) __a)

#define vreinterpret_s8_p16(__a) \
  (int8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a)

#define vreinterpretq_s8_s16(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv8hi (__a)

#define vreinterpretq_s8_s32(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv4si (__a)

#define vreinterpretq_s8_s64(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv2di (__a)

#define vreinterpretq_s8_f32(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv4sf (__a)

#define vreinterpretq_s8_u8(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv16qi ((int8x16_t) __a)

#define vreinterpretq_s8_u16(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a)

#define vreinterpretq_s8_u32(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv4si ((int32x4_t) __a)

#define vreinterpretq_s8_u64(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv2di ((int64x2_t) __a)

#define vreinterpretq_s8_p8(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv16qi ((int8x16_t) __a)

#define vreinterpretq_s8_p16(__a) \
  (int8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a)

#define vreinterpret_s16_s8(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv8qi (__a)

#define vreinterpret_s16_s32(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv2si (__a)

#define vreinterpret_s16_s64(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv1di (__a)

#define vreinterpret_s16_f32(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv2sf (__a)

#define vreinterpret_s16_u8(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a)

#define vreinterpret_s16_u16(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv4hi ((int16x4_t) __a)

#define vreinterpret_s16_u32(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv2si ((int32x2_t) __a)

#define vreinterpret_s16_u64(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv1di ((int64x1_t) __a)

#define vreinterpret_s16_p8(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a)

#define vreinterpret_s16_p16(__a) \
  (int16x4_t)__builtin_neon_vreinterpretv4hiv4hi ((int16x4_t) __a)

#define vreinterpretq_s16_s8(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv16qi (__a)

#define vreinterpretq_s16_s32(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv4si (__a)

#define vreinterpretq_s16_s64(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv2di (__a)

#define vreinterpretq_s16_f32(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv4sf (__a)

#define vreinterpretq_s16_u8(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a)

#define vreinterpretq_s16_u16(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv8hi ((int16x8_t) __a)

#define vreinterpretq_s16_u32(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv4si ((int32x4_t) __a)

#define vreinterpretq_s16_u64(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv2di ((int64x2_t) __a)

#define vreinterpretq_s16_p8(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a)

#define vreinterpretq_s16_p16(__a) \
  (int16x8_t)__builtin_neon_vreinterpretv8hiv8hi ((int16x8_t) __a)

#define vreinterpret_s32_s8(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv8qi (__a)

#define vreinterpret_s32_s16(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv4hi (__a)

#define vreinterpret_s32_s64(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv1di (__a)

#define vreinterpret_s32_f32(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv2sf (__a)

#define vreinterpret_s32_u8(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv8qi ((int8x8_t) __a)

#define vreinterpret_s32_u16(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv4hi ((int16x4_t) __a)

#define vreinterpret_s32_u32(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv2si ((int32x2_t) __a)

#define vreinterpret_s32_u64(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv1di ((int64x1_t) __a)

#define vreinterpret_s32_p8(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv8qi ((int8x8_t) __a)

#define vreinterpret_s32_p16(__a) \
  (int32x2_t)__builtin_neon_vreinterpretv2siv4hi ((int16x4_t) __a)

#define vreinterpretq_s32_s8(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv16qi (__a)

#define vreinterpretq_s32_s16(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv8hi (__a)

#define vreinterpretq_s32_s64(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv2di (__a)

#define vreinterpretq_s32_f32(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv4sf (__a)

#define vreinterpretq_s32_u8(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv16qi ((int8x16_t) __a)

#define vreinterpretq_s32_u16(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv8hi ((int16x8_t) __a)

#define vreinterpretq_s32_u32(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv4si ((int32x4_t) __a)

#define vreinterpretq_s32_u64(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv2di ((int64x2_t) __a)

#define vreinterpretq_s32_p8(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv16qi ((int8x16_t) __a)

#define vreinterpretq_s32_p16(__a) \
  (int32x4_t)__builtin_neon_vreinterpretv4siv8hi ((int16x8_t) __a)

#define vreinterpret_u8_s8(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv8qi (__a)

#define vreinterpret_u8_s16(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv4hi (__a)

#define vreinterpret_u8_s32(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv2si (__a)

#define vreinterpret_u8_s64(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv1di (__a)

#define vreinterpret_u8_f32(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv2sf (__a)

#define vreinterpret_u8_u16(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a)

#define vreinterpret_u8_u32(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv2si ((int32x2_t) __a)

#define vreinterpret_u8_u64(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv1di ((int64x1_t) __a)

#define vreinterpret_u8_p8(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv8qi ((int8x8_t) __a)

#define vreinterpret_u8_p16(__a) \
  (uint8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a)

#define vreinterpretq_u8_s8(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv16qi (__a)

#define vreinterpretq_u8_s16(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv8hi (__a)

#define vreinterpretq_u8_s32(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv4si (__a)

#define vreinterpretq_u8_s64(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv2di (__a)

#define vreinterpretq_u8_f32(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv4sf (__a)

#define vreinterpretq_u8_u16(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a)

#define vreinterpretq_u8_u32(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv4si ((int32x4_t) __a)

#define vreinterpretq_u8_u64(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv2di ((int64x2_t) __a)

#define vreinterpretq_u8_p8(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv16qi ((int8x16_t) __a)

#define vreinterpretq_u8_p16(__a) \
  (uint8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a)

#define vreinterpret_u16_s8(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv8qi (__a)

#define vreinterpret_u16_s16(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv4hi (__a)

#define vreinterpret_u16_s32(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv2si (__a)

#define vreinterpret_u16_s64(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv1di (__a)

#define vreinterpret_u16_f32(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv2sf (__a)

#define vreinterpret_u16_u8(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a)

#define vreinterpret_u16_u32(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv2si ((int32x2_t) __a)

#define vreinterpret_u16_u64(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv1di ((int64x1_t) __a)

#define vreinterpret_u16_p8(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a)

#define vreinterpret_u16_p16(__a) \
  (uint16x4_t)__builtin_neon_vreinterpretv4hiv4hi ((int16x4_t) __a)

#define vreinterpretq_u16_s8(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv16qi (__a)

#define vreinterpretq_u16_s16(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv8hi (__a)

#define vreinterpretq_u16_s32(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv4si (__a)

#define vreinterpretq_u16_s64(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv2di (__a)

#define vreinterpretq_u16_f32(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv4sf (__a)

#define vreinterpretq_u16_u8(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a)

#define vreinterpretq_u16_u32(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv4si ((int32x4_t) __a)

#define vreinterpretq_u16_u64(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv2di ((int64x2_t) __a)

#define vreinterpretq_u16_p8(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a)

#define vreinterpretq_u16_p16(__a) \
  (uint16x8_t)__builtin_neon_vreinterpretv8hiv8hi ((int16x8_t) __a)

#define vreinterpret_u32_s8(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv8qi (__a)

#define vreinterpret_u32_s16(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv4hi (__a)

#define vreinterpret_u32_s32(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv2si (__a)

#define vreinterpret_u32_s64(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv1di (__a)

#define vreinterpret_u32_f32(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv2sf (__a)

#define vreinterpret_u32_u8(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv8qi ((int8x8_t) __a)

#define vreinterpret_u32_u16(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv4hi ((int16x4_t) __a)

#define vreinterpret_u32_u64(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv1di ((int64x1_t) __a)

#define vreinterpret_u32_p8(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv8qi ((int8x8_t) __a)

#define vreinterpret_u32_p16(__a) \
  (uint32x2_t)__builtin_neon_vreinterpretv2siv4hi ((int16x4_t) __a)

#define vreinterpretq_u32_s8(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv16qi (__a)

#define vreinterpretq_u32_s16(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv8hi (__a)

#define vreinterpretq_u32_s32(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv4si (__a)

#define vreinterpretq_u32_s64(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv2di (__a)

#define vreinterpretq_u32_f32(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv4sf (__a)

#define vreinterpretq_u32_u8(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv16qi ((int8x16_t) __a)

#define vreinterpretq_u32_u16(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv8hi ((int16x8_t) __a)

#define vreinterpretq_u32_u64(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv2di ((int64x2_t) __a)

#define vreinterpretq_u32_p8(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv16qi ((int8x16_t) __a)

#define vreinterpretq_u32_p16(__a) \
  (uint32x4_t)__builtin_neon_vreinterpretv4siv8hi ((int16x8_t) __a)

#ifdef __cplusplus
}
#endif
#endif
#endif

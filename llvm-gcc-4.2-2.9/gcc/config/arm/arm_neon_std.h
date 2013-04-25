/* Internal definitions for standard versions of NEON types and intrinsics.
   Do not include this file directly; please use <arm_neon.h>.

   This file is generated automatically using neon-gen-std.ml.
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

typedef struct __simd64_int8_t
{
  __neon_int8x8_t val;
} int8x8_t;

typedef struct __simd64_int16_t
{
  __neon_int16x4_t val;
} int16x4_t;

typedef struct __simd64_int32_t
{
  __neon_int32x2_t val;
} int32x2_t;

typedef struct __simd64_int64_t
{
  __neon_int64x1_t val;
} int64x1_t;

typedef struct __simd64_float32_t
{
  __neon_float32x2_t val;
} float32x2_t;

typedef struct __simd64_poly8_t
{
  __neon_poly8x8_t val;
} poly8x8_t;

typedef struct __simd64_poly16_t
{
  __neon_poly16x4_t val;
} poly16x4_t;

typedef struct __simd64_uint8_t
{
  __neon_uint8x8_t val;
} uint8x8_t;

typedef struct __simd64_uint16_t
{
  __neon_uint16x4_t val;
} uint16x4_t;

typedef struct __simd64_uint32_t
{
  __neon_uint32x2_t val;
} uint32x2_t;

typedef struct __simd64_uint64_t
{
  __neon_uint64x1_t val;
} uint64x1_t;

typedef struct __simd128_int8_t
{
  __neon_int8x16_t val;
} int8x16_t;

typedef struct __simd128_int16_t
{
  __neon_int16x8_t val;
} int16x8_t;

typedef struct __simd128_int32_t
{
  __neon_int32x4_t val;
} int32x4_t;

typedef struct __simd128_int64_t
{
  __neon_int64x2_t val;
} int64x2_t;

typedef struct __simd128_float32_t
{
  __neon_float32x4_t val;
} float32x4_t;

typedef struct __simd128_poly8_t
{
  __neon_poly8x16_t val;
} poly8x16_t;

typedef struct __simd128_poly16_t
{
  __neon_poly16x8_t val;
} poly16x8_t;

typedef struct __simd128_uint8_t
{
  __neon_uint8x16_t val;
} uint8x16_t;

typedef struct __simd128_uint16_t
{
  __neon_uint16x8_t val;
} uint16x8_t;

typedef struct __simd128_uint32_t
{
  __neon_uint32x4_t val;
} uint32x4_t;

typedef struct __simd128_uint64_t
{
  __neon_uint64x2_t val;
} uint64x2_t;

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


#define vadd_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vadd_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vadd_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vadd_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vadd_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vadd_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vadd_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vadd_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vadd_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vaddq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddl_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddlv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddl_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddlv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddl_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddlv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddl_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddlv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddl_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddlv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddl_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddlv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddw_s8(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddwv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddw_s16(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddwv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddw_s32(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddwv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddw_u8(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddwv8qi ((__neon_int16x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddw_u16(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddwv4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddw_u32(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddwv2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhadd_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhadd_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhadd_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhadd_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhadd_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhadd_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhaddq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhaddq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhaddq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhaddq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhaddq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhaddq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vrhadd_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv8qi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrhadd_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv4hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrhadd_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv2si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrhadd_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vrhadd_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vrhadd_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vrhaddq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv16qi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrhaddq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv8hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrhaddq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv4si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrhaddq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vrhaddq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vrhaddq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhaddv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vqadd_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqadd_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqadd_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqadd_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqadd_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqadd_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqadd_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqadd_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqaddq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqaddq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqaddq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqaddq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqaddq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqaddq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqaddq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqaddq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqaddv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddhn_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddhn_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddhn_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vaddhn_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddhn_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaddhn_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vraddhn_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv8hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vraddhn_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv4si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vraddhn_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv2di (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vraddhn_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vraddhn_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vraddhn_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vaddhnv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vmul_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmul_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmul_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmul_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vmul_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmul_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmul_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmul_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 4); \
     __rv.__i; \
   })

#define vmulq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmulq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmulq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmulq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vmulq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmulq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmulq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmulq_p8(__a, __b) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmulv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 4); \
     __rv.__i; \
   })

#define vqdmulh_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulhv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqdmulh_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulhv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqdmulhq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulhv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqdmulhq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulhv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqrdmulh_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulhv4hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrdmulh_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulhv2si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrdmulhq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulhv8hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrdmulhq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulhv4si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vmull_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmullv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmull_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmullv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmull_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmullv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmull_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmullv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmull_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmullv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmull_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmullv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmull_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmullv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 4); \
     __rv.__i; \
   })

#define vqdmull_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmullv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqdmull_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmullv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmla_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     int8x8_t __cx = __c; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav8qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmla_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav4hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmla_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav2si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmla_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     float32x2_t __cx = __c; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav2sf (__ax.val, __bx.val, __cx.val, 5); \
     __rv.__i; \
   })

#define vmla_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmla_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmla_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlaq_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     int8x16_t __cx = __c; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav16qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlaq_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     int16x8_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav8hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlaq_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     int32x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav4si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlaq_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     float32x4_t __cx = __c; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav4sf (__ax.val, __bx.val, __cx.val, 5); \
     __rv.__i; \
   })

#define vmlaq_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     uint8x16_t __cx = __c; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, (__neon_int8x16_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlaq_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     uint16x8_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__neon_int16x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlaq_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     uint32x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlav4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, (__neon_int32x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlal_s8(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     int8x8_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlalv8qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlal_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlalv4hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlal_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlalv2si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlal_u8(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlalv8qi ((__neon_int16x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlal_u16(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlalv4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlal_u32(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlalv2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vqdmlal_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlalv4hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vqdmlal_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlalv2si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmls_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     int8x8_t __cx = __c; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv8qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmls_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv4hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmls_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv2si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmls_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     float32x2_t __cx = __c; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv2sf (__ax.val, __bx.val, __cx.val, 5); \
     __rv.__i; \
   })

#define vmls_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmls_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmls_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlsq_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     int8x16_t __cx = __c; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv16qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlsq_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     int16x8_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv8hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlsq_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     int32x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv4si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlsq_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     float32x4_t __cx = __c; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv4sf (__ax.val, __bx.val, __cx.val, 5); \
     __rv.__i; \
   })

#define vmlsq_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     uint8x16_t __cx = __c; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, (__neon_int8x16_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlsq_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     uint16x8_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__neon_int16x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlsq_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     uint32x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, (__neon_int32x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlsl_s8(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     int8x8_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlslv8qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlsl_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlslv4hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlsl_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlslv2si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vmlsl_u8(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlslv8qi ((__neon_int16x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlsl_u16(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlslv4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmlsl_u32(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlslv2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vqdmlsl_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlslv4hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vqdmlsl_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlslv2si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vsub_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsub_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsub_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsub_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsub_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vsub_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsub_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsub_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsub_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vsubq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubl_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsublv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubl_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsublv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubl_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsublv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubl_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsublv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubl_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsublv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubl_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsublv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubw_s8(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubwv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubw_s16(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubwv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubw_s32(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubwv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubw_u8(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubwv8qi ((__neon_int16x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubw_u16(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubwv4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubw_u32(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubwv2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhsub_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhsub_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhsub_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhsub_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhsub_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhsub_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhsubq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhsubq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhsubq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vhsubq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhsubq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vhsubq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vhsubv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqsub_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqsub_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqsub_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqsub_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqsub_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqsub_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqsub_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqsub_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqsubq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqsubq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqsubq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqsubq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqsubq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqsubq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqsubq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vqsubq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqsubv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubhn_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubhn_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubhn_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vsubhn_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubhn_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vsubhn_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vrsubhn_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv8hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrsubhn_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv4si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrsubhn_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv2di (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrsubhn_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vrsubhn_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vrsubhn_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsubhnv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 2); \
     __rv.__i; \
   })

#define vceq_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vceq_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vceq_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vceq_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vceq_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vceq_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vceq_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vceq_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 4); \
     __rv.__i; \
   })

#define vceqq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vceqq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vceqq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vceqq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vceqq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vceqq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vceqq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vceqq_p8(__a, __b) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vceqv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 4); \
     __rv.__i; \
   })

#define vcge_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcge_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcge_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcge_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vcge_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcge_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcge_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcgeq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgeq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgeq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgeq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vcgeq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcgeq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcgeq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcle_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev8qi (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcle_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4hi (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcle_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev2si (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcle_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev2sf (__bx.val, __ax.val, 5); \
     __rv.__i; \
   })

#define vcle_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev8qi ((__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcle_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4hi ((__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcle_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev2si ((__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcleq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev16qi (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcleq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev8hi (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcleq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4si (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcleq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4sf (__bx.val, __ax.val, 5); \
     __rv.__i; \
   })

#define vcleq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev16qi ((__neon_int8x16_t) __bx.val, (__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcleq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev8hi ((__neon_int16x8_t) __bx.val, (__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcleq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgev4si ((__neon_int32x4_t) __bx.val, (__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcgt_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgt_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgt_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgt_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vcgt_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcgt_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcgt_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcgtq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgtq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgtq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vcgtq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vcgtq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcgtq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vcgtq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vclt_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv8qi (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vclt_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4hi (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vclt_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv2si (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vclt_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv2sf (__bx.val, __ax.val, 5); \
     __rv.__i; \
   })

#define vclt_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv8qi ((__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vclt_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4hi ((__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vclt_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv2si ((__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcltq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv16qi (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcltq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv8hi (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcltq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4si (__bx.val, __ax.val, 1); \
     __rv.__i; \
   })

#define vcltq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4sf (__bx.val, __ax.val, 5); \
     __rv.__i; \
   })

#define vcltq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv16qi ((__neon_int8x16_t) __bx.val, (__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcltq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv8hi ((__neon_int16x8_t) __bx.val, (__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcltq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcgtv4si ((__neon_int32x4_t) __bx.val, (__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcage_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcagev2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vcageq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcagev4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vcale_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcagev2sf (__bx.val, __ax.val, 5); \
     __rv.__i; \
   })

#define vcaleq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcagev4sf (__bx.val, __ax.val, 5); \
     __rv.__i; \
   })

#define vcagt_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcagtv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vcagtq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcagtv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vcalt_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcagtv2sf (__bx.val, __ax.val, 5); \
     __rv.__i; \
   })

#define vcaltq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcagtv4sf (__bx.val, __ax.val, 5); \
     __rv.__i; \
   })

#define vtst_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vtst_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vtst_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vtst_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vtst_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vtst_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vtst_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 4); \
     __rv.__i; \
   })

#define vtstq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vtstq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vtstq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vtstq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vtstq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vtstq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vtstq_p8(__a, __b) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtstv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 4); \
     __rv.__i; \
   })

#define vabd_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabd_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabd_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabd_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vabd_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vabd_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vabd_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vabdq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabdq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabdq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabdq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vabdq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vabdq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vabdq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vabdl_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdlv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabdl_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdlv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabdl_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdlv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vabdl_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdlv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vabdl_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdlv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vabdl_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabdlv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vaba_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     int8x8_t __cx = __c; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav8qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vaba_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav4hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vaba_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav2si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vaba_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vaba_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vaba_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vabaq_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     int8x16_t __cx = __c; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav16qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vabaq_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     int16x8_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav8hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vabaq_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     int32x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav4si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vabaq_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     uint8x16_t __cx = __c; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, (__neon_int8x16_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vabaq_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     uint16x8_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__neon_int16x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vabaq_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     uint32x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabav4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, (__neon_int32x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vabal_s8(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     int8x8_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabalv8qi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vabal_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabalv4hi (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vabal_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabalv2si (__ax.val, __bx.val, __cx.val, 1); \
     __rv.__i; \
   })

#define vabal_u8(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabalv8qi ((__neon_int16x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vabal_u16(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabalv4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vabal_u32(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabalv2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, 0); \
     __rv.__i; \
   })

#define vmax_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmax_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmax_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmax_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vmax_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmax_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmax_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmaxq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmaxq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmaxq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmaxq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vmaxq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmaxq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmaxq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmaxv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmin_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmin_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmin_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vmin_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vmin_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmin_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vmin_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vminq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vminq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vminq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vminq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vminq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vminq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vminq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vminv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpadd_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadd_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadd_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadd_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vpadd_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpadd_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpadd_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpaddl_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vpaddl_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vpaddl_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vpaddl_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv8qi ((__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vpaddl_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv4hi ((__neon_int16x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vpaddl_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv2si ((__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vpaddlq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vpaddlq_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vpaddlq_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vpaddlq_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv16qi ((__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vpaddlq_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv8hi ((__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vpaddlq_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpaddlv4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vpadal_s8(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadal_s16(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadal_s32(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadal_u8(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv8qi ((__neon_int16x4_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpadal_u16(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv4hi ((__neon_int32x2_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpadal_u32(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv2si ((__neon_int64x1_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpadalq_s8(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadalq_s16(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadalq_s32(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpadalq_u8(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv16qi ((__neon_int16x8_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpadalq_u16(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv8hi ((__neon_int32x4_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpadalq_u32(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpadalv4si ((__neon_int64x2_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpmax_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpmaxv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpmax_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpmaxv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpmax_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpmaxv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpmax_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpmaxv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vpmax_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpmaxv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpmax_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpmaxv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpmax_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpmaxv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpmin_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpminv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpmin_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpminv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpmin_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpminv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vpmin_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpminv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vpmin_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpminv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpmin_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpminv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vpmin_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vpminv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vrecps_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrecpsv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vrecpsq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrecpsv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vrsqrts_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrsqrtsv2sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vrsqrtsq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrsqrtsv4sf (__ax.val, __bx.val, 5); \
     __rv.__i; \
   })

#define vshl_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vshl_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vshl_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vshl_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vshl_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv8qi ((__neon_int8x8_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vshl_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv4hi ((__neon_int16x4_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vshl_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv2si ((__neon_int32x2_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vshl_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv1di ((__neon_int64x1_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vshlq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vshlq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vshlq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vshlq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vshlq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv16qi ((__neon_int8x16_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vshlq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv8hi ((__neon_int16x8_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vshlq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv4si ((__neon_int32x4_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vshlq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv2di ((__neon_int64x2_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vrshl_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv8qi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrshl_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv4hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrshl_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv2si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrshl_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv1di (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrshl_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv8qi ((__neon_int8x8_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vrshl_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv4hi ((__neon_int16x4_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vrshl_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv2si ((__neon_int32x2_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vrshl_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv1di ((__neon_int64x1_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vrshlq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv16qi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrshlq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv8hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrshlq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv4si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrshlq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv2di (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vrshlq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv16qi ((__neon_int8x16_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vrshlq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv8hi ((__neon_int16x8_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vrshlq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv4si ((__neon_int32x4_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vrshlq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshlv2di ((__neon_int64x2_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vqshl_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqshl_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqshl_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqshl_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqshl_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv8qi ((__neon_int8x8_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vqshl_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv4hi ((__neon_int16x4_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vqshl_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv2si ((__neon_int32x2_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vqshl_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv1di ((__neon_int64x1_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vqshlq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqshlq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqshlq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqshlq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vqshlq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv16qi ((__neon_int8x16_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vqshlq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv8hi ((__neon_int16x8_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vqshlq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv4si ((__neon_int32x4_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vqshlq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv2di ((__neon_int64x2_t) __ax.val, __bx.val, 0); \
     __rv.__i; \
   })

#define vqrshl_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv8qi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrshl_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv4hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrshl_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv2si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrshl_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv1di (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrshl_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv8qi ((__neon_int8x8_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vqrshl_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv4hi ((__neon_int16x4_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vqrshl_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv2si ((__neon_int32x2_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vqrshl_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv1di ((__neon_int64x1_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vqrshlq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv16qi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrshlq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv8hi (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrshlq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv4si (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrshlq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv2di (__ax.val, __bx.val, 3); \
     __rv.__i; \
   })

#define vqrshlq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv16qi ((__neon_int8x16_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vqrshlq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv8hi ((__neon_int16x8_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vqrshlq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv4si ((__neon_int32x4_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vqrshlq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlv2di ((__neon_int64x2_t) __ax.val, __bx.val, 2); \
     __rv.__i; \
   })

#define vshr_n_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv8qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshr_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv4hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshr_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv2si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshr_n_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv1di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshr_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv8qi ((__neon_int8x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshr_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv4hi ((__neon_int16x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshr_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv2si ((__neon_int32x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshr_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv1di ((__neon_int64x1_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshrq_n_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv16qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshrq_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv8hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshrq_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv4si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshrq_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv2di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshrq_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv16qi ((__neon_int8x16_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshrq_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv8hi ((__neon_int16x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshrq_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv4si ((__neon_int32x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshrq_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv2di ((__neon_int64x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vrshr_n_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv8qi (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshr_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv4hi (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshr_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv2si (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshr_n_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv1di (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshr_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv8qi ((__neon_int8x8_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshr_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv4hi ((__neon_int16x4_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshr_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv2si ((__neon_int32x2_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshr_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv1di ((__neon_int64x1_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshrq_n_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv16qi (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshrq_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv8hi (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshrq_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv4si (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshrq_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv2di (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshrq_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv16qi ((__neon_int8x16_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshrq_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv8hi ((__neon_int16x8_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshrq_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv4si ((__neon_int32x4_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshrq_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshr_nv2di ((__neon_int64x2_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vshrn_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv8hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshrn_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv4si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshrn_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv2di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshrn_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv8hi ((__neon_int16x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshrn_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv4si ((__neon_int32x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshrn_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv2di ((__neon_int64x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vrshrn_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv8hi (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshrn_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv4si (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshrn_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv2di (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vrshrn_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv8hi ((__neon_int16x8_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshrn_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv4si ((__neon_int32x4_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vrshrn_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshrn_nv2di ((__neon_int64x2_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vqshrn_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv8hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshrn_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv4si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshrn_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv2di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshrn_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv8hi ((__neon_int16x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshrn_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv4si ((__neon_int32x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshrn_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv2di ((__neon_int64x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqrshrn_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv8hi (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vqrshrn_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv4si (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vqrshrn_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv2di (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vqrshrn_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv8hi ((__neon_int16x8_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vqrshrn_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv4si ((__neon_int32x4_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vqrshrn_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrn_nv2di ((__neon_int64x2_t) __ax.val, __b, 2); \
     __rv.__i; \
   })

#define vqshrun_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrun_nv8hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshrun_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrun_nv4si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshrun_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrun_nv2di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqrshrun_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrun_nv8hi (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vqrshrun_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrun_nv4si (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vqrshrun_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshrun_nv2di (__ax.val, __b, 3); \
     __rv.__i; \
   })

#define vshl_n_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv8qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshl_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv4hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshl_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv2si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshl_n_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv1di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshl_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv8qi ((__neon_int8x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshl_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv4hi ((__neon_int16x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshl_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv2si ((__neon_int32x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshl_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv1di ((__neon_int64x1_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshlq_n_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv16qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshlq_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv8hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshlq_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv4si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshlq_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv2di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshlq_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv16qi ((__neon_int8x16_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshlq_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv8hi ((__neon_int16x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshlq_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv4si ((__neon_int32x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshlq_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshl_nv2di ((__neon_int64x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshl_n_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv8qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshl_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv4hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshl_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv2si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshl_n_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv1di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshl_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv8qi ((__neon_int8x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshl_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv4hi ((__neon_int16x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshl_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv2si ((__neon_int32x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshl_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv1di ((__neon_int64x1_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshlq_n_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv16qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshlq_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv8hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshlq_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv4si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshlq_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv2di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshlq_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv16qi ((__neon_int8x16_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshlq_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv8hi ((__neon_int16x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshlq_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv4si ((__neon_int32x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshlq_n_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshl_nv2di ((__neon_int64x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vqshlu_n_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlu_nv8qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshlu_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlu_nv4hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshlu_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlu_nv2si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshlu_n_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlu_nv1di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshluq_n_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlu_nv16qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshluq_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlu_nv8hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshluq_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlu_nv4si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vqshluq_n_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqshlu_nv2di (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshll_n_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshll_nv8qi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshll_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshll_nv4hi (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshll_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshll_nv2si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vshll_n_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshll_nv8qi ((__neon_int8x8_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshll_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshll_nv4hi ((__neon_int16x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vshll_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vshll_nv2si ((__neon_int32x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vsra_n_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv8qi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vsra_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv4hi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vsra_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv2si (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vsra_n_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv1di (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vsra_n_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vsra_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vsra_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vsra_n_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vsraq_n_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv16qi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vsraq_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv8hi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vsraq_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv4si (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vsraq_n_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv2di (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vsraq_n_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vsraq_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vsraq_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vsraq_n_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vrsra_n_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv8qi (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vrsra_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv4hi (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vrsra_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv2si (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vrsra_n_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv1di (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vrsra_n_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, __c, 2); \
     __rv.__i; \
   })

#define vrsra_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c, 2); \
     __rv.__i; \
   })

#define vrsra_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, __c, 2); \
     __rv.__i; \
   })

#define vrsra_n_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, __c, 2); \
     __rv.__i; \
   })

#define vrsraq_n_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv16qi (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vrsraq_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv8hi (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vrsraq_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv4si (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vrsraq_n_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv2di (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vrsraq_n_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, __c, 2); \
     __rv.__i; \
   })

#define vrsraq_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, __c, 2); \
     __rv.__i; \
   })

#define vrsraq_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, __c, 2); \
     __rv.__i; \
   })

#define vrsraq_n_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsra_nv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, __c, 2); \
     __rv.__i; \
   })

#define vsri_n_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv8qi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv4hi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv2si (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv1di (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsri_n_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv16qi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv8hi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv4si (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv2di (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsriq_n_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     poly16x8_t __bx = __b; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsri_nv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv8qi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv4hi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv2si (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv1di (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsli_n_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv16qi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv8hi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv4si (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv2di (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsliq_n_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     poly16x8_t __bx = __b; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vsli_nv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vabs_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabsv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vabs_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabsv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vabs_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabsv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vabs_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabsv2sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vabsq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabsv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vabsq_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabsv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vabsq_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabsv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vabsq_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vabsv4sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vqabs_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqabsv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqabs_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqabsv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqabs_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqabsv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vqabsq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqabsv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqabsq_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqabsv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqabsq_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqabsv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vneg_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vnegv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vneg_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vnegv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vneg_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vnegv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vneg_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vnegv2sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vnegq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vnegv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vnegq_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vnegv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vnegq_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vnegv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vnegq_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vnegv4sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vqneg_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqnegv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqneg_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqnegv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqneg_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqnegv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vqnegq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqnegv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqnegq_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqnegv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqnegq_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqnegv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vmvn_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vmvn_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vmvn_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vmvn_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv8qi ((__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmvn_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv4hi ((__neon_int16x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmvn_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv2si ((__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmvn_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv8qi ((__neon_int8x8_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vmvnq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vmvnq_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vmvnq_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vmvnq_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv16qi ((__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmvnq_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv8hi ((__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmvnq_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmvnq_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmvnv16qi ((__neon_int8x16_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vcls_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclsv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vcls_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclsv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vcls_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclsv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vclsq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclsv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vclsq_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclsv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vclsq_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclsv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vclz_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vclz_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vclz_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vclz_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv8qi ((__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vclz_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv4hi ((__neon_int16x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vclz_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv2si ((__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vclzq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vclzq_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vclzq_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vclzq_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv16qi ((__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vclzq_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv8hi ((__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vclzq_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vclzv4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcnt_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcntv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vcnt_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcntv8qi ((__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcnt_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcntv8qi ((__neon_int8x8_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vcntq_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcntv16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vcntq_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcntv16qi ((__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcntq_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcntv16qi ((__neon_int8x16_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrecpe_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrecpev2sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vrecpe_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrecpev2si ((__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrecpeq_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrecpev4sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vrecpeq_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrecpev4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrsqrte_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrsqrtev2sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vrsqrte_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrsqrtev2si ((__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrsqrteq_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrsqrtev4sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vrsqrteq_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrsqrtev4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vget_lane_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     (int8_t)__builtin_neon_vget_lanev8qi (__ax.val, __b, 1); \
   })

#define vget_lane_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     (int16_t)__builtin_neon_vget_lanev4hi (__ax.val, __b, 1); \
   })

#define vget_lane_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     (int32_t)__builtin_neon_vget_lanev2si (__ax.val, __b, 1); \
   })

#define vget_lane_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     (float32_t)__builtin_neon_vget_lanev2sf (__ax.val, __b, 5); \
   })

#define vget_lane_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     (uint8_t)__builtin_neon_vget_lanev8qi ((__neon_int8x8_t) __ax.val, __b, 0); \
   })

#define vget_lane_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     (uint16_t)__builtin_neon_vget_lanev4hi ((__neon_int16x4_t) __ax.val, __b, 0); \
   })

#define vget_lane_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     (uint32_t)__builtin_neon_vget_lanev2si ((__neon_int32x2_t) __ax.val, __b, 0); \
   })

#define vget_lane_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     (poly8_t)__builtin_neon_vget_lanev8qi ((__neon_int8x8_t) __ax.val, __b, 4); \
   })

#define vget_lane_p16(__a, __b) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     (poly16_t)__builtin_neon_vget_lanev4hi ((__neon_int16x4_t) __ax.val, __b, 4); \
   })

#define vget_lane_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     (int64_t)__builtin_neon_vget_lanev1di (__ax.val, __b, 1); \
   })

#define vget_lane_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     (uint64_t)__builtin_neon_vget_lanev1di ((__neon_int64x1_t) __ax.val, __b, 0); \
   })

#define vgetq_lane_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     (int8_t)__builtin_neon_vget_lanev16qi (__ax.val, __b, 1); \
   })

#define vgetq_lane_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     (int16_t)__builtin_neon_vget_lanev8hi (__ax.val, __b, 1); \
   })

#define vgetq_lane_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     (int32_t)__builtin_neon_vget_lanev4si (__ax.val, __b, 1); \
   })

#define vgetq_lane_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     (float32_t)__builtin_neon_vget_lanev4sf (__ax.val, __b, 5); \
   })

#define vgetq_lane_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     (uint8_t)__builtin_neon_vget_lanev16qi ((__neon_int8x16_t) __ax.val, __b, 0); \
   })

#define vgetq_lane_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     (uint16_t)__builtin_neon_vget_lanev8hi ((__neon_int16x8_t) __ax.val, __b, 0); \
   })

#define vgetq_lane_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     (uint32_t)__builtin_neon_vget_lanev4si ((__neon_int32x4_t) __ax.val, __b, 0); \
   })

#define vgetq_lane_p8(__a, __b) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     (poly8_t)__builtin_neon_vget_lanev16qi ((__neon_int8x16_t) __ax.val, __b, 4); \
   })

#define vgetq_lane_p16(__a, __b) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     (poly16_t)__builtin_neon_vget_lanev8hi ((__neon_int16x8_t) __ax.val, __b, 4); \
   })

#define vgetq_lane_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     (int64_t)__builtin_neon_vget_lanev2di (__ax.val, __b, 1); \
   })

#define vgetq_lane_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     (uint64_t)__builtin_neon_vget_lanev2di ((__neon_int64x2_t) __ax.val, __b, 0); \
   })

#define vset_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     int8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev2si ((__builtin_neon_si) __ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev2sf (__ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __ax, (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __ax, (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev2si ((__builtin_neon_si) __ax, (__neon_int32x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __ax, (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __ax, (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_s64(__a, __b, __c) __extension__ \
  ({ \
     int64_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev1di ((__builtin_neon_di) __ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vset_lane_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev1di ((__builtin_neon_di) __ax, (__neon_int64x1_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     int8_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev4si ((__builtin_neon_si) __ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev4sf (__ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __ax, (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __ax, (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev4si ((__builtin_neon_si) __ax, (__neon_int32x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __ax, (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t __ax = __a; \
     poly16x8_t __bx = __b; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __ax, (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_s64(__a, __b, __c) __extension__ \
  ({ \
     int64_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev2di ((__builtin_neon_di) __ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vsetq_lane_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vset_lanev2di ((__builtin_neon_di) __ax, (__neon_int64x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vcreate_s8(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev8qi ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_s16(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev4hi ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_s32(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev2si ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_s64(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev1di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_f32(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev2sf ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_u8(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev8qi ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_u16(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev4hi ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_u32(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev2si ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_u64(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev1di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_p8(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev8qi ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vcreate_p16(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcreatev4hi ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vdup_n_s8(__a) __extension__ \
  ({ \
     int8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vdup_n_s16(__a) __extension__ \
  ({ \
     int16_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vdup_n_s32(__a) __extension__ \
  ({ \
     int32_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2si ((__builtin_neon_si) __ax); \
     __rv.__i; \
   })

#define vdup_n_f32(__a) __extension__ \
  ({ \
     float32_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2sf (__ax); \
     __rv.__i; \
   })

#define vdup_n_u8(__a) __extension__ \
  ({ \
     uint8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vdup_n_u16(__a) __extension__ \
  ({ \
     uint16_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vdup_n_u32(__a) __extension__ \
  ({ \
     uint32_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2si ((__builtin_neon_si) __ax); \
     __rv.__i; \
   })

#define vdup_n_p8(__a) __extension__ \
  ({ \
     poly8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vdup_n_p16(__a) __extension__ \
  ({ \
     poly16_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vdup_n_s64(__a) __extension__ \
  ({ \
     int64_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv1di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vdup_n_u64(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv1di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vdupq_n_s8(__a) __extension__ \
  ({ \
     int8_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vdupq_n_s16(__a) __extension__ \
  ({ \
     int16_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vdupq_n_s32(__a) __extension__ \
  ({ \
     int32_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4si ((__builtin_neon_si) __ax); \
     __rv.__i; \
   })

#define vdupq_n_f32(__a) __extension__ \
  ({ \
     float32_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4sf (__ax); \
     __rv.__i; \
   })

#define vdupq_n_u8(__a) __extension__ \
  ({ \
     uint8_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vdupq_n_u16(__a) __extension__ \
  ({ \
     uint16_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vdupq_n_u32(__a) __extension__ \
  ({ \
     uint32_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4si ((__builtin_neon_si) __ax); \
     __rv.__i; \
   })

#define vdupq_n_p8(__a) __extension__ \
  ({ \
     poly8_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vdupq_n_p16(__a) __extension__ \
  ({ \
     poly16_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vdupq_n_s64(__a) __extension__ \
  ({ \
     int64_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vdupq_n_u64(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vmov_n_s8(__a) __extension__ \
  ({ \
     int8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vmov_n_s16(__a) __extension__ \
  ({ \
     int16_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vmov_n_s32(__a) __extension__ \
  ({ \
     int32_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2si ((__builtin_neon_si) __ax); \
     __rv.__i; \
   })

#define vmov_n_f32(__a) __extension__ \
  ({ \
     float32_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2sf (__ax); \
     __rv.__i; \
   })

#define vmov_n_u8(__a) __extension__ \
  ({ \
     uint8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vmov_n_u16(__a) __extension__ \
  ({ \
     uint16_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vmov_n_u32(__a) __extension__ \
  ({ \
     uint32_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2si ((__builtin_neon_si) __ax); \
     __rv.__i; \
   })

#define vmov_n_p8(__a) __extension__ \
  ({ \
     poly8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vmov_n_p16(__a) __extension__ \
  ({ \
     poly16_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vmov_n_s64(__a) __extension__ \
  ({ \
     int64_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv1di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vmov_n_u64(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv1di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vmovq_n_s8(__a) __extension__ \
  ({ \
     int8_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vmovq_n_s16(__a) __extension__ \
  ({ \
     int16_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vmovq_n_s32(__a) __extension__ \
  ({ \
     int32_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4si ((__builtin_neon_si) __ax); \
     __rv.__i; \
   })

#define vmovq_n_f32(__a) __extension__ \
  ({ \
     float32_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4sf (__ax); \
     __rv.__i; \
   })

#define vmovq_n_u8(__a) __extension__ \
  ({ \
     uint8_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vmovq_n_u16(__a) __extension__ \
  ({ \
     uint16_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vmovq_n_u32(__a) __extension__ \
  ({ \
     uint32_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv4si ((__builtin_neon_si) __ax); \
     __rv.__i; \
   })

#define vmovq_n_p8(__a) __extension__ \
  ({ \
     poly8_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __ax); \
     __rv.__i; \
   })

#define vmovq_n_p16(__a) __extension__ \
  ({ \
     poly16_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __ax); \
     __rv.__i; \
   })

#define vmovq_n_s64(__a) __extension__ \
  ({ \
     int64_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vmovq_n_u64(__a) __extension__ \
  ({ \
     uint64_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_nv2di ((__builtin_neon_di) __ax); \
     __rv.__i; \
   })

#define vdup_lane_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev8qi (__ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev4hi (__ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev2si (__ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev2sf (__ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev8qi ((__neon_int8x8_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev4hi ((__neon_int16x4_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev2si ((__neon_int32x2_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev8qi ((__neon_int8x8_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_p16(__a, __b) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev4hi ((__neon_int16x4_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev1di (__ax.val, __b); \
     __rv.__i; \
   })

#define vdup_lane_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev1di ((__neon_int64x1_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev16qi (__ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev8hi (__ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev4si (__ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev4sf (__ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev16qi ((__neon_int8x8_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev8hi ((__neon_int16x4_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev4si ((__neon_int32x2_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev16qi ((__neon_int8x8_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_p16(__a, __b) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev8hi ((__neon_int16x4_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev2di (__ax.val, __b); \
     __rv.__i; \
   })

#define vdupq_lane_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vdup_lanev2di ((__neon_int64x1_t) __ax.val, __b); \
     __rv.__i; \
   })

#define vcombine_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev8qi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vcombine_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev4hi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vcombine_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev2si (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vcombine_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev1di (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vcombine_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev2sf (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vcombine_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vcombine_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val); \
     __rv.__i; \
   })

#define vcombine_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val); \
     __rv.__i; \
   })

#define vcombine_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val); \
     __rv.__i; \
   })

#define vcombine_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vcombine_p16(__a, __b) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcombinev4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val); \
     __rv.__i; \
   })

#define vget_high_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv16qi (__ax.val); \
     __rv.__i; \
   })

#define vget_high_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv8hi (__ax.val); \
     __rv.__i; \
   })

#define vget_high_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv4si (__ax.val); \
     __rv.__i; \
   })

#define vget_high_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv2di (__ax.val); \
     __rv.__i; \
   })

#define vget_high_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv4sf (__ax.val); \
     __rv.__i; \
   })

#define vget_high_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vget_high_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vget_high_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vget_high_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vget_high_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vget_high_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_highv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vget_low_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv16qi (__ax.val); \
     __rv.__i; \
   })

#define vget_low_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv8hi (__ax.val); \
     __rv.__i; \
   })

#define vget_low_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv4si (__ax.val); \
     __rv.__i; \
   })

#define vget_low_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv2di (__ax.val); \
     __rv.__i; \
   })

#define vget_low_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv4sf (__ax.val); \
     __rv.__i; \
   })

#define vget_low_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vget_low_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vget_low_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vget_low_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vget_low_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vget_low_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vget_lowv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vcvt_s32_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvtv2sf (__ax.val, 1); \
     __rv.__i; \
   })

#define vcvt_f32_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvtv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vcvt_f32_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvtv2si ((__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcvt_u32_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvtv2sf (__ax.val, 0); \
     __rv.__i; \
   })

#define vcvtq_s32_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvtv4sf (__ax.val, 1); \
     __rv.__i; \
   })

#define vcvtq_f32_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvtv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vcvtq_f32_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvtv4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vcvtq_u32_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvtv4sf (__ax.val, 0); \
     __rv.__i; \
   })

#define vcvt_n_s32_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvt_nv2sf (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vcvt_n_f32_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvt_nv2si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vcvt_n_f32_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvt_nv2si ((__neon_int32x2_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vcvt_n_u32_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvt_nv2sf (__ax.val, __b, 0); \
     __rv.__i; \
   })

#define vcvtq_n_s32_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvt_nv4sf (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vcvtq_n_f32_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvt_nv4si (__ax.val, __b, 1); \
     __rv.__i; \
   })

#define vcvtq_n_f32_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvt_nv4si ((__neon_int32x4_t) __ax.val, __b, 0); \
     __rv.__i; \
   })

#define vcvtq_n_u32_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vcvt_nv4sf (__ax.val, __b, 0); \
     __rv.__i; \
   })

#define vmovn_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovnv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vmovn_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovnv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vmovn_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovnv2di (__ax.val, 1); \
     __rv.__i; \
   })

#define vmovn_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovnv8hi ((__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmovn_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovnv4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmovn_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovnv2di ((__neon_int64x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vqmovn_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovnv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqmovn_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovnv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vqmovn_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovnv2di (__ax.val, 1); \
     __rv.__i; \
   })

#define vqmovn_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovnv8hi ((__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vqmovn_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovnv4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vqmovn_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovnv2di ((__neon_int64x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vqmovun_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovunv8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vqmovun_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovunv4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vqmovun_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqmovunv2di (__ax.val, 1); \
     __rv.__i; \
   })

#define vmovl_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovlv8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vmovl_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovlv4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vmovl_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovlv2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vmovl_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovlv8qi ((__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmovl_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovlv4hi ((__neon_int16x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vmovl_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmovlv2si ((__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vtbl1_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl1v8qi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtbl1_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl1v8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtbl1_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl1v8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtbl2_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __au = { __a }; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl2v8qi (__au.__o, __bx.val); \
     __rv.__i; \
   })

#define vtbl2_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __au = { __a }; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl2v8qi (__au.__o, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtbl2_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __au = { __a }; \
     uint8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl2v8qi (__au.__o, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtbl3_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __au = { __a }; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl3v8qi (__au.__o, __bx.val); \
     __rv.__i; \
   })

#define vtbl3_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __au = { __a }; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl3v8qi (__au.__o, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtbl3_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __au = { __a }; \
     uint8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl3v8qi (__au.__o, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtbl4_s8(__a, __b) __extension__ \
  ({ \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __au = { __a }; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl4v8qi (__au.__o, __bx.val); \
     __rv.__i; \
   })

#define vtbl4_u8(__a, __b) __extension__ \
  ({ \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __au = { __a }; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl4v8qi (__au.__o, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtbl4_p8(__a, __b) __extension__ \
  ({ \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __au = { __a }; \
     uint8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbl4v8qi (__au.__o, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtbx1_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     int8x8_t __cx = __c; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx1v8qi (__ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vtbx1_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx1v8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vtbx1_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx1v8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vtbx2_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     int8x8_t __cx = __c; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx2v8qi (__ax.val, __bu.__o, __cx.val); \
     __rv.__i; \
   })

#define vtbx2_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     uint8x8_t __cx = __c; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx2v8qi ((__neon_int8x8_t) __ax.val, __bu.__o, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vtbx2_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     uint8x8_t __cx = __c; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx2v8qi ((__neon_int8x8_t) __ax.val, __bu.__o, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vtbx3_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     int8x8_t __cx = __c; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx3v8qi (__ax.val, __bu.__o, __cx.val); \
     __rv.__i; \
   })

#define vtbx3_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     uint8x8_t __cx = __c; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx3v8qi ((__neon_int8x8_t) __ax.val, __bu.__o, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vtbx3_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     uint8x8_t __cx = __c; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx3v8qi ((__neon_int8x8_t) __ax.val, __bu.__o, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vtbx4_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     int8x8_t __cx = __c; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx4v8qi (__ax.val, __bu.__o, __cx.val); \
     __rv.__i; \
   })

#define vtbx4_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     uint8x8_t __cx = __c; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx4v8qi ((__neon_int8x8_t) __ax.val, __bu.__o, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vtbx4_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     uint8x8_t __cx = __c; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtbx4v8qi ((__neon_int8x8_t) __ax.val, __bu.__o, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vmul_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev4hi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vmul_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev2si (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vmul_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev2sf (__ax.val, __bx.val, __c, 5); \
     __rv.__i; \
   })

#define vmul_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vmul_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vmulq_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev8hi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vmulq_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev4si (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vmulq_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev4sf (__ax.val, __bx.val, __c, 5); \
     __rv.__i; \
   })

#define vmulq_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x4_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vmulq_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_lanev4si ((__neon_int32x4_t) __ax.val, (__neon_int32x2_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vmla_lane_s16(__a, __b, __c, __d) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev4hi (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmla_lane_s32(__a, __b, __c, __d) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev2si (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmla_lane_f32(__a, __b, __c, __d) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     float32x2_t __cx = __c; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev2sf (__ax.val, __bx.val, __cx.val, __d, 5); \
     __rv.__i; \
   })

#define vmla_lane_u16(__a, __b, __c, __d) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmla_lane_u32(__a, __b, __c, __d) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmlaq_lane_s16(__a, __b, __c, __d) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev8hi (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmlaq_lane_s32(__a, __b, __c, __d) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev4si (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmlaq_lane_f32(__a, __b, __c, __d) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     float32x2_t __cx = __c; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev4sf (__ax.val, __bx.val, __cx.val, __d, 5); \
     __rv.__i; \
   })

#define vmlaq_lane_u16(__a, __b, __c, __d) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__neon_int16x4_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmlaq_lane_u32(__a, __b, __c, __d) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_lanev4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, (__neon_int32x2_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmlal_lane_s16(__a, __b, __c, __d) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlal_lanev4hi (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmlal_lane_s32(__a, __b, __c, __d) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlal_lanev2si (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmlal_lane_u16(__a, __b, __c, __d) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlal_lanev4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmlal_lane_u32(__a, __b, __c, __d) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlal_lanev2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vqdmlal_lane_s16(__a, __b, __c, __d) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlal_lanev4hi (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vqdmlal_lane_s32(__a, __b, __c, __d) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlal_lanev2si (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmls_lane_s16(__a, __b, __c, __d) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev4hi (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmls_lane_s32(__a, __b, __c, __d) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev2si (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmls_lane_f32(__a, __b, __c, __d) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     float32x2_t __cx = __c; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev2sf (__ax.val, __bx.val, __cx.val, __d, 5); \
     __rv.__i; \
   })

#define vmls_lane_u16(__a, __b, __c, __d) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmls_lane_u32(__a, __b, __c, __d) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmlsq_lane_s16(__a, __b, __c, __d) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev8hi (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmlsq_lane_s32(__a, __b, __c, __d) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev4si (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmlsq_lane_f32(__a, __b, __c, __d) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     float32x2_t __cx = __c; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev4sf (__ax.val, __bx.val, __cx.val, __d, 5); \
     __rv.__i; \
   })

#define vmlsq_lane_u16(__a, __b, __c, __d) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__neon_int16x4_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmlsq_lane_u32(__a, __b, __c, __d) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_lanev4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, (__neon_int32x2_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmlsl_lane_s16(__a, __b, __c, __d) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsl_lanev4hi (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmlsl_lane_s32(__a, __b, __c, __d) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsl_lanev2si (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmlsl_lane_u16(__a, __b, __c, __d) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsl_lanev4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vmlsl_lane_u32(__a, __b, __c, __d) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsl_lanev2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val, __d, 0); \
     __rv.__i; \
   })

#define vqdmlsl_lane_s16(__a, __b, __c, __d) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlsl_lanev4hi (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vqdmlsl_lane_s32(__a, __b, __c, __d) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlsl_lanev2si (__ax.val, __bx.val, __cx.val, __d, 1); \
     __rv.__i; \
   })

#define vmull_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmull_lanev4hi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vmull_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmull_lanev2si (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vmull_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmull_lanev4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vmull_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmull_lanev2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, __c, 0); \
     __rv.__i; \
   })

#define vqdmull_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmull_lanev4hi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vqdmull_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmull_lanev2si (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vqdmulhq_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_lanev8hi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vqdmulhq_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_lanev4si (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vqdmulh_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_lanev4hi (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vqdmulh_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_lanev2si (__ax.val, __bx.val, __c, 1); \
     __rv.__i; \
   })

#define vqrdmulhq_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_lanev8hi (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vqrdmulhq_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_lanev4si (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vqrdmulh_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_lanev4hi (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vqrdmulh_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_lanev2si (__ax.val, __bx.val, __c, 3); \
     __rv.__i; \
   })

#define vmul_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv4hi (__ax.val, (__builtin_neon_hi) __bx, 1); \
     __rv.__i; \
   })

#define vmul_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv2si (__ax.val, (__builtin_neon_si) __bx, 1); \
     __rv.__i; \
   })

#define vmul_n_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv2sf (__ax.val, __bx, 5); \
     __rv.__i; \
   })

#define vmul_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv4hi ((__neon_int16x4_t) __ax.val, (__builtin_neon_hi) __bx, 0); \
     __rv.__i; \
   })

#define vmul_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv2si ((__neon_int32x2_t) __ax.val, (__builtin_neon_si) __bx, 0); \
     __rv.__i; \
   })

#define vmulq_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv8hi (__ax.val, (__builtin_neon_hi) __bx, 1); \
     __rv.__i; \
   })

#define vmulq_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv4si (__ax.val, (__builtin_neon_si) __bx, 1); \
     __rv.__i; \
   })

#define vmulq_n_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv4sf (__ax.val, __bx, 5); \
     __rv.__i; \
   })

#define vmulq_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv8hi ((__neon_int16x8_t) __ax.val, (__builtin_neon_hi) __bx, 0); \
     __rv.__i; \
   })

#define vmulq_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmul_nv4si ((__neon_int32x4_t) __ax.val, (__builtin_neon_si) __bx, 0); \
     __rv.__i; \
   })

#define vmull_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmull_nv4hi (__ax.val, (__builtin_neon_hi) __bx, 1); \
     __rv.__i; \
   })

#define vmull_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmull_nv2si (__ax.val, (__builtin_neon_si) __bx, 1); \
     __rv.__i; \
   })

#define vmull_n_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmull_nv4hi ((__neon_int16x4_t) __ax.val, (__builtin_neon_hi) __bx, 0); \
     __rv.__i; \
   })

#define vmull_n_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmull_nv2si ((__neon_int32x2_t) __ax.val, (__builtin_neon_si) __bx, 0); \
     __rv.__i; \
   })

#define vqdmull_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmull_nv4hi (__ax.val, (__builtin_neon_hi) __bx, 1); \
     __rv.__i; \
   })

#define vqdmull_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmull_nv2si (__ax.val, (__builtin_neon_si) __bx, 1); \
     __rv.__i; \
   })

#define vqdmulhq_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_nv8hi (__ax.val, (__builtin_neon_hi) __bx, 1); \
     __rv.__i; \
   })

#define vqdmulhq_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_nv4si (__ax.val, (__builtin_neon_si) __bx, 1); \
     __rv.__i; \
   })

#define vqdmulh_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_nv4hi (__ax.val, (__builtin_neon_hi) __bx, 1); \
     __rv.__i; \
   })

#define vqdmulh_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_nv2si (__ax.val, (__builtin_neon_si) __bx, 1); \
     __rv.__i; \
   })

#define vqrdmulhq_n_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_nv8hi (__ax.val, (__builtin_neon_hi) __bx, 3); \
     __rv.__i; \
   })

#define vqrdmulhq_n_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_nv4si (__ax.val, (__builtin_neon_si) __bx, 3); \
     __rv.__i; \
   })

#define vqrdmulh_n_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_nv4hi (__ax.val, (__builtin_neon_hi) __bx, 3); \
     __rv.__i; \
   })

#define vqrdmulh_n_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmulh_nv2si (__ax.val, (__builtin_neon_si) __bx, 3); \
     __rv.__i; \
   })

#define vmla_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16_t __cx = __c; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv4hi (__ax.val, __bx.val, (__builtin_neon_hi) __cx, 1); \
     __rv.__i; \
   })

#define vmla_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32_t __cx = __c; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv2si (__ax.val, __bx.val, (__builtin_neon_si) __cx, 1); \
     __rv.__i; \
   })

#define vmla_n_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     float32_t __cx = __c; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv2sf (__ax.val, __bx.val, __cx, 5); \
     __rv.__i; \
   })

#define vmla_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16_t __cx = __c; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__builtin_neon_hi) __cx, 0); \
     __rv.__i; \
   })

#define vmla_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32_t __cx = __c; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__builtin_neon_si) __cx, 0); \
     __rv.__i; \
   })

#define vmlaq_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     int16_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv8hi (__ax.val, __bx.val, (__builtin_neon_hi) __cx, 1); \
     __rv.__i; \
   })

#define vmlaq_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     int32_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv4si (__ax.val, __bx.val, (__builtin_neon_si) __cx, 1); \
     __rv.__i; \
   })

#define vmlaq_n_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     float32_t __cx = __c; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv4sf (__ax.val, __bx.val, __cx, 5); \
     __rv.__i; \
   })

#define vmlaq_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     uint16_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__builtin_neon_hi) __cx, 0); \
     __rv.__i; \
   })

#define vmlaq_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     uint32_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmla_nv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, (__builtin_neon_si) __cx, 0); \
     __rv.__i; \
   })

#define vmlal_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlal_nv4hi (__ax.val, __bx.val, (__builtin_neon_hi) __cx, 1); \
     __rv.__i; \
   })

#define vmlal_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlal_nv2si (__ax.val, __bx.val, (__builtin_neon_si) __cx, 1); \
     __rv.__i; \
   })

#define vmlal_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlal_nv4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__builtin_neon_hi) __cx, 0); \
     __rv.__i; \
   })

#define vmlal_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32_t __cx = __c; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlal_nv2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__builtin_neon_si) __cx, 0); \
     __rv.__i; \
   })

#define vqdmlal_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlal_nv4hi (__ax.val, __bx.val, (__builtin_neon_hi) __cx, 1); \
     __rv.__i; \
   })

#define vqdmlal_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlal_nv2si (__ax.val, __bx.val, (__builtin_neon_si) __cx, 1); \
     __rv.__i; \
   })

#define vmls_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16_t __cx = __c; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv4hi (__ax.val, __bx.val, (__builtin_neon_hi) __cx, 1); \
     __rv.__i; \
   })

#define vmls_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32_t __cx = __c; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv2si (__ax.val, __bx.val, (__builtin_neon_si) __cx, 1); \
     __rv.__i; \
   })

#define vmls_n_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     float32_t __cx = __c; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv2sf (__ax.val, __bx.val, __cx, 5); \
     __rv.__i; \
   })

#define vmls_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16_t __cx = __c; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__builtin_neon_hi) __cx, 0); \
     __rv.__i; \
   })

#define vmls_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32_t __cx = __c; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__builtin_neon_si) __cx, 0); \
     __rv.__i; \
   })

#define vmlsq_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     int16_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv8hi (__ax.val, __bx.val, (__builtin_neon_hi) __cx, 1); \
     __rv.__i; \
   })

#define vmlsq_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     int32_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv4si (__ax.val, __bx.val, (__builtin_neon_si) __cx, 1); \
     __rv.__i; \
   })

#define vmlsq_n_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     float32_t __cx = __c; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv4sf (__ax.val, __bx.val, __cx, 5); \
     __rv.__i; \
   })

#define vmlsq_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     uint16_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__builtin_neon_hi) __cx, 0); \
     __rv.__i; \
   })

#define vmlsq_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     uint32_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmls_nv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, (__builtin_neon_si) __cx, 0); \
     __rv.__i; \
   })

#define vmlsl_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsl_nv4hi (__ax.val, __bx.val, (__builtin_neon_hi) __cx, 1); \
     __rv.__i; \
   })

#define vmlsl_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsl_nv2si (__ax.val, __bx.val, (__builtin_neon_si) __cx, 1); \
     __rv.__i; \
   })

#define vmlsl_n_u16(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsl_nv4hi ((__neon_int32x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__builtin_neon_hi) __cx, 0); \
     __rv.__i; \
   })

#define vmlsl_n_u32(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32_t __cx = __c; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vmlsl_nv2si ((__neon_int64x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__builtin_neon_si) __cx, 0); \
     __rv.__i; \
   })

#define vqdmlsl_n_s16(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlsl_nv4hi (__ax.val, __bx.val, (__builtin_neon_hi) __cx, 1); \
     __rv.__i; \
   })

#define vqdmlsl_n_s32(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vqdmlsl_nv2si (__ax.val, __bx.val, (__builtin_neon_si) __cx, 1); \
     __rv.__i; \
   })

#define vext_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv8qi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vext_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv4hi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vext_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv2si (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vext_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv1di (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vext_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv2sf (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vext_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vext_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vext_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vext_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vext_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vext_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_s8(__a, __b, __c) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv16qi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_s16(__a, __b, __c) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv8hi (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_s32(__a, __b, __c) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv4si (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_s64(__a, __b, __c) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv2di (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_f32(__a, __b, __c) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv4sf (__ax.val, __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vextq_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     poly16x8_t __bx = __b; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vextv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vrev64_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev64_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev64_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v2si (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev64_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v2sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vrev64_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v8qi ((__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev64_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v4hi ((__neon_int16x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev64_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v2si ((__neon_int32x2_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev64_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v8qi ((__neon_int8x8_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev64_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v4hi ((__neon_int16x4_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev64q_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev64q_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev64q_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v4si (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev64q_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v4sf (__ax.val, 5); \
     __rv.__i; \
   })

#define vrev64q_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v16qi ((__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev64q_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v8hi ((__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev64q_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v4si ((__neon_int32x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev64q_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v16qi ((__neon_int8x16_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev64q_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev64v8hi ((__neon_int16x8_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev32_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev32_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v4hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev32_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v8qi ((__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev32_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v4hi ((__neon_int16x4_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev32_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v8qi ((__neon_int8x8_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev32_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v4hi ((__neon_int16x4_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev32q_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev32q_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v8hi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev32q_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v16qi ((__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev32q_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v8hi ((__neon_int16x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev32q_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v16qi ((__neon_int8x16_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev32q_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev32v8hi ((__neon_int16x8_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev16_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev16v8qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev16_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev16v8qi ((__neon_int8x8_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev16_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev16v8qi ((__neon_int8x8_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vrev16q_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev16v16qi (__ax.val, 1); \
     __rv.__i; \
   })

#define vrev16q_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev16v16qi ((__neon_int8x16_t) __ax.val, 0); \
     __rv.__i; \
   })

#define vrev16q_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vrev16v16qi ((__neon_int8x16_t) __ax.val, 4); \
     __rv.__i; \
   })

#define vbsl_s8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     int8x8_t __cx = __c; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv8qi ((__neon_int8x8_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbsl_s16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     int16x4_t __cx = __c; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv4hi ((__neon_int16x4_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbsl_s32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     int32x2_t __cx = __c; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv2si ((__neon_int32x2_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbsl_s64(__a, __b, __c) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     int64x1_t __cx = __c; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv1di ((__neon_int64x1_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbsl_f32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     float32x2_t __cx = __c; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv2sf ((__neon_int32x2_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbsl_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     uint8x8_t __cx = __c; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vbsl_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     uint16x4_t __cx = __c; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val); \
     __rv.__i; \
   })

#define vbsl_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     uint32x2_t __cx = __c; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, (__neon_int32x2_t) __cx.val); \
     __rv.__i; \
   })

#define vbsl_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     uint64x1_t __cx = __c; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, (__neon_int64x1_t) __cx.val); \
     __rv.__i; \
   })

#define vbsl_p8(__a, __b, __c) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     poly8x8_t __cx = __c; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, (__neon_int8x8_t) __cx.val); \
     __rv.__i; \
   })

#define vbsl_p16(__a, __b, __c) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     poly16x4_t __bx = __b; \
     poly16x4_t __cx = __c; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, (__neon_int16x4_t) __cx.val); \
     __rv.__i; \
   })

#define vbslq_s8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     int8x16_t __cx = __c; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv16qi ((__neon_int8x16_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbslq_s16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     int16x8_t __cx = __c; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv8hi ((__neon_int16x8_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbslq_s32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     int32x4_t __cx = __c; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv4si ((__neon_int32x4_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbslq_s64(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     int64x2_t __cx = __c; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv2di ((__neon_int64x2_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbslq_f32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     float32x4_t __cx = __c; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv4sf ((__neon_int32x4_t) __ax.val, __bx.val, __cx.val); \
     __rv.__i; \
   })

#define vbslq_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     uint8x16_t __cx = __c; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, (__neon_int8x16_t) __cx.val); \
     __rv.__i; \
   })

#define vbslq_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     uint16x8_t __cx = __c; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__neon_int16x8_t) __cx.val); \
     __rv.__i; \
   })

#define vbslq_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     uint32x4_t __cx = __c; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, (__neon_int32x4_t) __cx.val); \
     __rv.__i; \
   })

#define vbslq_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     uint64x2_t __cx = __c; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, (__neon_int64x2_t) __cx.val); \
     __rv.__i; \
   })

#define vbslq_p8(__a, __b, __c) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     poly8x16_t __cx = __c; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, (__neon_int8x16_t) __cx.val); \
     __rv.__i; \
   })

#define vbslq_p16(__a, __b, __c) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     poly16x8_t __bx = __b; \
     poly16x8_t __cx = __c; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbslv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, (__neon_int16x8_t) __cx.val); \
     __rv.__i; \
   })

#define vtrn_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8qi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtrn_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4hi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtrn_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv2si (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtrn_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv2sf (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtrn_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtrn_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val); \
     __rv.__i; \
   })

#define vtrn_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val); \
     __rv.__i; \
   })

#define vtrn_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtrn_p16(__a, __b) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val); \
     __rv.__i; \
   })

#define vtrnq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv16qi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtrnq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8hi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtrnq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4si (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtrnq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4sf (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vtrnq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val); \
     __rv.__i; \
   })

#define vtrnq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val); \
     __rv.__i; \
   })

#define vtrnq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val); \
     __rv.__i; \
   })

#define vtrnq_p8(__a, __b) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val); \
     __rv.__i; \
   })

#define vtrnq_p16(__a, __b) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     poly16x8_t __bx = __b; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vtrnv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val); \
     __rv.__i; \
   })

#define vzip_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8qi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vzip_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4hi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vzip_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv2si (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vzip_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv2sf (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vzip_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vzip_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val); \
     __rv.__i; \
   })

#define vzip_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val); \
     __rv.__i; \
   })

#define vzip_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vzip_p16(__a, __b) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val); \
     __rv.__i; \
   })

#define vzipq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv16qi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vzipq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8hi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vzipq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4si (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vzipq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4sf (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vzipq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val); \
     __rv.__i; \
   })

#define vzipq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val); \
     __rv.__i; \
   })

#define vzipq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val); \
     __rv.__i; \
   })

#define vzipq_p8(__a, __b) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val); \
     __rv.__i; \
   })

#define vzipq_p16(__a, __b) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     poly16x8_t __bx = __b; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vzipv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val); \
     __rv.__i; \
   })

#define vuzp_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8qi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vuzp_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4hi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vuzp_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv2si (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vuzp_f32(__a, __b) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv2sf (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vuzp_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vuzp_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val); \
     __rv.__i; \
   })

#define vuzp_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val); \
     __rv.__i; \
   })

#define vuzp_p8(__a, __b) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val); \
     __rv.__i; \
   })

#define vuzp_p16(__a, __b) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val); \
     __rv.__i; \
   })

#define vuzpq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv16qi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vuzpq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8hi (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vuzpq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4si (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vuzpq_f32(__a, __b) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4sf (__ax.val, __bx.val); \
     __rv.__i; \
   })

#define vuzpq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val); \
     __rv.__i; \
   })

#define vuzpq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val); \
     __rv.__i; \
   })

#define vuzpq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val); \
     __rv.__i; \
   })

#define vuzpq_p8(__a, __b) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val); \
     __rv.__i; \
   })

#define vuzpq_p16(__a, __b) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     poly16x8_t __bx = __b; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vuzpv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val); \
     __rv.__i; \
   })

#define vld1_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld1_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld1_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v2sf (__ax); \
     __rv.__i; \
   })

#define vld1_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld1_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld1_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld1q_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v2di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld1q_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v4sf (__ax); \
     __rv.__i; \
   })

#define vld1q_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld1q_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v2di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld1q_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     float32x2_t __bx = __b; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev2sf (__ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __ax), (__neon_int32x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     poly8x8_t __bx = __b; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), (__neon_int8x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     poly16x4_t __bx = __b; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), (__neon_int16x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_s64(__a, __b, __c) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev1di (__neon_ptr_cast(const __builtin_neon_di *, __ax), __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_lane_u64(__a, __b, __c) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev1di (__neon_ptr_cast(const __builtin_neon_di *, __ax), (__neon_int64x1_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     float32x4_t __bx = __b; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev4sf (__ax, __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __ax), (__neon_int32x4_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     poly8x16_t __bx = __b; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), (__neon_int8x16_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     poly16x8_t __bx = __b; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), (__neon_int16x8_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_s64(__a, __b, __c) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev2di (__neon_ptr_cast(const __builtin_neon_di *, __ax), __bx.val, __c); \
     __rv.__i; \
   })

#define vld1q_lane_u64(__a, __b, __c) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_lanev2di (__neon_ptr_cast(const __builtin_neon_di *, __ax), (__neon_int64x2_t) __bx.val, __c); \
     __rv.__i; \
   })

#define vld1_dup_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv2sf (__ax); \
     __rv.__i; \
   })

#define vld1_dup_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld1_dup_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv4sf (__ax); \
     __rv.__i; \
   })

#define vld1q_dup_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv2di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld1q_dup_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld1_dupv2di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vst1_s8(__a, __b) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     int8x8_t __bx = __b; \
     __builtin_neon_vst1v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bx.val); \
   })

#define vst1_s16(__a, __b) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     int16x4_t __bx = __b; \
     __builtin_neon_vst1v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bx.val); \
   })

#define vst1_s32(__a, __b) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     int32x2_t __bx = __b; \
     __builtin_neon_vst1v2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bx.val); \
   })

#define vst1_s64(__a, __b) __extension__ \
  ({ \
     int64_t * __ax = __a; \
     int64x1_t __bx = __b; \
     __builtin_neon_vst1v1di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bx.val); \
   })

#define vst1_f32(__a, __b) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     float32x2_t __bx = __b; \
     __builtin_neon_vst1v2sf (__ax, __bx.val); \
   })

#define vst1_u8(__a, __b) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     uint8x8_t __bx = __b; \
     __builtin_neon_vst1v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), (__neon_int8x8_t) __bx.val); \
   })

#define vst1_u16(__a, __b) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     uint16x4_t __bx = __b; \
     __builtin_neon_vst1v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), (__neon_int16x4_t) __bx.val); \
   })

#define vst1_u32(__a, __b) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     uint32x2_t __bx = __b; \
     __builtin_neon_vst1v2si (__neon_ptr_cast(__builtin_neon_si *, __ax), (__neon_int32x2_t) __bx.val); \
   })

#define vst1_u64(__a, __b) __extension__ \
  ({ \
     uint64_t * __ax = __a; \
     uint64x1_t __bx = __b; \
     __builtin_neon_vst1v1di (__neon_ptr_cast(__builtin_neon_di *, __ax), (__neon_int64x1_t) __bx.val); \
   })

#define vst1_p8(__a, __b) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     poly8x8_t __bx = __b; \
     __builtin_neon_vst1v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), (__neon_int8x8_t) __bx.val); \
   })

#define vst1_p16(__a, __b) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     poly16x4_t __bx = __b; \
     __builtin_neon_vst1v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), (__neon_int16x4_t) __bx.val); \
   })

#define vst1q_s8(__a, __b) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     int8x16_t __bx = __b; \
     __builtin_neon_vst1v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bx.val); \
   })

#define vst1q_s16(__a, __b) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     int16x8_t __bx = __b; \
     __builtin_neon_vst1v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bx.val); \
   })

#define vst1q_s32(__a, __b) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     int32x4_t __bx = __b; \
     __builtin_neon_vst1v4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bx.val); \
   })

#define vst1q_s64(__a, __b) __extension__ \
  ({ \
     int64_t * __ax = __a; \
     int64x2_t __bx = __b; \
     __builtin_neon_vst1v2di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bx.val); \
   })

#define vst1q_f32(__a, __b) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     float32x4_t __bx = __b; \
     __builtin_neon_vst1v4sf (__ax, __bx.val); \
   })

#define vst1q_u8(__a, __b) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     uint8x16_t __bx = __b; \
     __builtin_neon_vst1v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), (__neon_int8x16_t) __bx.val); \
   })

#define vst1q_u16(__a, __b) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     uint16x8_t __bx = __b; \
     __builtin_neon_vst1v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), (__neon_int16x8_t) __bx.val); \
   })

#define vst1q_u32(__a, __b) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     uint32x4_t __bx = __b; \
     __builtin_neon_vst1v4si (__neon_ptr_cast(__builtin_neon_si *, __ax), (__neon_int32x4_t) __bx.val); \
   })

#define vst1q_u64(__a, __b) __extension__ \
  ({ \
     uint64_t * __ax = __a; \
     uint64x2_t __bx = __b; \
     __builtin_neon_vst1v2di (__neon_ptr_cast(__builtin_neon_di *, __ax), (__neon_int64x2_t) __bx.val); \
   })

#define vst1q_p8(__a, __b) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     poly8x16_t __bx = __b; \
     __builtin_neon_vst1v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), (__neon_int8x16_t) __bx.val); \
   })

#define vst1q_p16(__a, __b) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     poly16x8_t __bx = __b; \
     __builtin_neon_vst1v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), (__neon_int16x8_t) __bx.val); \
   })

#define vst1_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     int8x8_t __bx = __b; \
     __builtin_neon_vst1_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bx.val, __c); \
   })

#define vst1_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     int16x4_t __bx = __b; \
     __builtin_neon_vst1_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bx.val, __c); \
   })

#define vst1_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     int32x2_t __bx = __b; \
     __builtin_neon_vst1_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bx.val, __c); \
   })

#define vst1_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     float32x2_t __bx = __b; \
     __builtin_neon_vst1_lanev2sf (__ax, __bx.val, __c); \
   })

#define vst1_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     uint8x8_t __bx = __b; \
     __builtin_neon_vst1_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), (__neon_int8x8_t) __bx.val, __c); \
   })

#define vst1_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     uint16x4_t __bx = __b; \
     __builtin_neon_vst1_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), (__neon_int16x4_t) __bx.val, __c); \
   })

#define vst1_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     uint32x2_t __bx = __b; \
     __builtin_neon_vst1_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __ax), (__neon_int32x2_t) __bx.val, __c); \
   })

#define vst1_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     poly8x8_t __bx = __b; \
     __builtin_neon_vst1_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), (__neon_int8x8_t) __bx.val, __c); \
   })

#define vst1_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     poly16x4_t __bx = __b; \
     __builtin_neon_vst1_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), (__neon_int16x4_t) __bx.val, __c); \
   })

#define vst1_lane_s64(__a, __b, __c) __extension__ \
  ({ \
     int64_t * __ax = __a; \
     int64x1_t __bx = __b; \
     __builtin_neon_vst1_lanev1di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bx.val, __c); \
   })

#define vst1_lane_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64_t * __ax = __a; \
     uint64x1_t __bx = __b; \
     __builtin_neon_vst1_lanev1di (__neon_ptr_cast(__builtin_neon_di *, __ax), (__neon_int64x1_t) __bx.val, __c); \
   })

#define vst1q_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     int8x16_t __bx = __b; \
     __builtin_neon_vst1_lanev16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bx.val, __c); \
   })

#define vst1q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     int16x8_t __bx = __b; \
     __builtin_neon_vst1_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bx.val, __c); \
   })

#define vst1q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     int32x4_t __bx = __b; \
     __builtin_neon_vst1_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bx.val, __c); \
   })

#define vst1q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     float32x4_t __bx = __b; \
     __builtin_neon_vst1_lanev4sf (__ax, __bx.val, __c); \
   })

#define vst1q_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     uint8x16_t __bx = __b; \
     __builtin_neon_vst1_lanev16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), (__neon_int8x16_t) __bx.val, __c); \
   })

#define vst1q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     uint16x8_t __bx = __b; \
     __builtin_neon_vst1_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), (__neon_int16x8_t) __bx.val, __c); \
   })

#define vst1q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     uint32x4_t __bx = __b; \
     __builtin_neon_vst1_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __ax), (__neon_int32x4_t) __bx.val, __c); \
   })

#define vst1q_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     poly8x16_t __bx = __b; \
     __builtin_neon_vst1_lanev16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), (__neon_int8x16_t) __bx.val, __c); \
   })

#define vst1q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     poly16x8_t __bx = __b; \
     __builtin_neon_vst1_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), (__neon_int16x8_t) __bx.val, __c); \
   })

#define vst1q_lane_s64(__a, __b, __c) __extension__ \
  ({ \
     int64_t * __ax = __a; \
     int64x2_t __bx = __b; \
     __builtin_neon_vst1_lanev2di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bx.val, __c); \
   })

#define vst1q_lane_u64(__a, __b, __c) __extension__ \
  ({ \
     uint64_t * __ax = __a; \
     uint64x2_t __bx = __b; \
     __builtin_neon_vst1_lanev2di (__neon_ptr_cast(__builtin_neon_di *, __ax), (__neon_int64x2_t) __bx.val, __c); \
   })

#define vld2_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld2_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v2sf (__ax); \
     __rv.__i; \
   })

#define vld2_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld2_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x1x2_t __i; __neon_int64x1x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld2_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x1x2_t __i; __neon_int64x1x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld2q_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2q_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2q_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld2q_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4sf (__ax); \
     __rv.__i; \
   })

#define vld2q_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2q_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2q_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld2q_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2q_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __bu = { __b }; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev2sf (__ax, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __bu = { __b }; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4sf (__ax, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld2_dup_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv2sf (__ax); \
     __rv.__i; \
   })

#define vld2_dup_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x1x2_t __i; __neon_int64x1x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld2_dup_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x1x2_t __i; __neon_int64x1x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld2_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vst2_s8(__a, __b) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst2_s16(__a, __b) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst2_s32(__a, __b) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst2_f32(__a, __b) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v2sf (__ax, __bu.__o); \
   })

#define vst2_u8(__a, __b) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst2_u16(__a, __b) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst2_u32(__a, __b) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst2_p8(__a, __b) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst2_p16(__a, __b) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst2_s64(__a, __b) __extension__ \
  ({ \
     int64_t * __ax = __a; \
     union { int64x1x2_t __i; __neon_int64x1x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v1di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bu.__o); \
   })

#define vst2_u64(__a, __b) __extension__ \
  ({ \
     uint64_t * __ax = __a; \
     union { uint64x1x2_t __i; __neon_int64x1x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v1di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bu.__o); \
   })

#define vst2q_s8(__a, __b) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x16x2_t __i; __neon_int8x16x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst2q_s16(__a, __b) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst2q_s32(__a, __b) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst2q_f32(__a, __b) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4sf (__ax, __bu.__o); \
   })

#define vst2q_u8(__a, __b) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x16x2_t __i; __neon_int8x16x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst2q_u16(__a, __b) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst2q_u32(__a, __b) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst2q_p8(__a, __b) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x16x2_t __i; __neon_int8x16x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst2q_p16(__a, __b) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst2_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst2_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst2_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst2_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x2x2_t __i; __neon_float32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev2sf (__ax, __bu.__o, __c); \
   })

#define vst2_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst2_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst2_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x2x2_t __i; __neon_int32x2x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst2_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x8x2_t __i; __neon_int8x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst2_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x4x2_t __i; __neon_int16x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst2q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst2q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst2q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x4x2_t __i; __neon_float32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4sf (__ax, __bu.__o, __c); \
   })

#define vst2q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst2q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x4x2_t __i; __neon_int32x4x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst2q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x8x2_t __i; __neon_int16x8x2_t __o; } __bu = { __b }; \
     __builtin_neon_vst2_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vld3_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld3_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v2sf (__ax); \
     __rv.__i; \
   })

#define vld3_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld3_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x1x3_t __i; __neon_int64x1x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld3_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x1x3_t __i; __neon_int64x1x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld3q_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x16x3_t __i; __neon_int8x16x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3q_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3q_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld3q_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4sf (__ax); \
     __rv.__i; \
   })

#define vld3q_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x16x3_t __i; __neon_int8x16x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3q_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3q_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld3q_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x16x3_t __i; __neon_int8x16x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3q_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __bu = { __b }; \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev2sf (__ax, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __bu = { __b }; \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4sf (__ax, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld3_dup_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv2sf (__ax); \
     __rv.__i; \
   })

#define vld3_dup_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x1x3_t __i; __neon_int64x1x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld3_dup_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x1x3_t __i; __neon_int64x1x3_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld3_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vst3_s8(__a, __b) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst3_s16(__a, __b) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst3_s32(__a, __b) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst3_f32(__a, __b) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v2sf (__ax, __bu.__o); \
   })

#define vst3_u8(__a, __b) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst3_u16(__a, __b) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst3_u32(__a, __b) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst3_p8(__a, __b) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst3_p16(__a, __b) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst3_s64(__a, __b) __extension__ \
  ({ \
     int64_t * __ax = __a; \
     union { int64x1x3_t __i; __neon_int64x1x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v1di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bu.__o); \
   })

#define vst3_u64(__a, __b) __extension__ \
  ({ \
     uint64_t * __ax = __a; \
     union { uint64x1x3_t __i; __neon_int64x1x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v1di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bu.__o); \
   })

#define vst3q_s8(__a, __b) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x16x3_t __i; __neon_int8x16x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst3q_s16(__a, __b) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst3q_s32(__a, __b) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst3q_f32(__a, __b) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4sf (__ax, __bu.__o); \
   })

#define vst3q_u8(__a, __b) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x16x3_t __i; __neon_int8x16x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst3q_u16(__a, __b) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst3q_u32(__a, __b) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst3q_p8(__a, __b) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x16x3_t __i; __neon_int8x16x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst3q_p16(__a, __b) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst3_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst3_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst3_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst3_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x2x3_t __i; __neon_float32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev2sf (__ax, __bu.__o, __c); \
   })

#define vst3_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst3_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst3_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x2x3_t __i; __neon_int32x2x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst3_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x8x3_t __i; __neon_int8x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst3_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x4x3_t __i; __neon_int16x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst3q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst3q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst3q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x4x3_t __i; __neon_float32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4sf (__ax, __bu.__o, __c); \
   })

#define vst3q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst3q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x4x3_t __i; __neon_int32x4x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst3q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x8x3_t __i; __neon_int16x8x3_t __o; } __bu = { __b }; \
     __builtin_neon_vst3_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vld4_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld4_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v2sf (__ax); \
     __rv.__i; \
   })

#define vld4_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld4_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x1x4_t __i; __neon_int64x1x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld4_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x1x4_t __i; __neon_int64x1x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld4q_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x16x4_t __i; __neon_int8x16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4q_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4q_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld4q_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4sf (__ax); \
     __rv.__i; \
   })

#define vld4q_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x16x4_t __i; __neon_int8x16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4q_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4q_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v4si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld4q_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x16x4_t __i; __neon_int8x16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v16qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4q_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4v8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __bu = { __b }; \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev2sf (__ax, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev2si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __bu = { __b }; \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4sf (__ax, __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev4si (__neon_ptr_cast(const __builtin_neon_si *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_lanev8hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax), __bu.__o, __c); \
     __rv.__i; \
   })

#define vld4_dup_s8(__a) __extension__ \
  ({ \
     const int8_t * __ax = __a; \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_s16(__a) __extension__ \
  ({ \
     const int16_t * __ax = __a; \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_s32(__a) __extension__ \
  ({ \
     const int32_t * __ax = __a; \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_f32(__a) __extension__ \
  ({ \
     const float32_t * __ax = __a; \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv2sf (__ax); \
     __rv.__i; \
   })

#define vld4_dup_u8(__a) __extension__ \
  ({ \
     const uint8_t * __ax = __a; \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_u16(__a) __extension__ \
  ({ \
     const uint16_t * __ax = __a; \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_u32(__a) __extension__ \
  ({ \
     const uint32_t * __ax = __a; \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv2si (__neon_ptr_cast(const __builtin_neon_si *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_p8(__a) __extension__ \
  ({ \
     const poly8_t * __ax = __a; \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv8qi (__neon_ptr_cast(const __builtin_neon_qi *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_p16(__a) __extension__ \
  ({ \
     const poly16_t * __ax = __a; \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv4hi (__neon_ptr_cast(const __builtin_neon_hi *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_s64(__a) __extension__ \
  ({ \
     const int64_t * __ax = __a; \
     union { int64x1x4_t __i; __neon_int64x1x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vld4_dup_u64(__a) __extension__ \
  ({ \
     const uint64_t * __ax = __a; \
     union { uint64x1x4_t __i; __neon_int64x1x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vld4_dupv1di (__neon_ptr_cast(const __builtin_neon_di *, __ax)); \
     __rv.__i; \
   })

#define vst4_s8(__a, __b) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst4_s16(__a, __b) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst4_s32(__a, __b) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst4_f32(__a, __b) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v2sf (__ax, __bu.__o); \
   })

#define vst4_u8(__a, __b) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst4_u16(__a, __b) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst4_u32(__a, __b) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst4_p8(__a, __b) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst4_p16(__a, __b) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst4_s64(__a, __b) __extension__ \
  ({ \
     int64_t * __ax = __a; \
     union { int64x1x4_t __i; __neon_int64x1x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v1di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bu.__o); \
   })

#define vst4_u64(__a, __b) __extension__ \
  ({ \
     uint64_t * __ax = __a; \
     union { uint64x1x4_t __i; __neon_int64x1x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v1di (__neon_ptr_cast(__builtin_neon_di *, __ax), __bu.__o); \
   })

#define vst4q_s8(__a, __b) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x16x4_t __i; __neon_int8x16x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst4q_s16(__a, __b) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst4q_s32(__a, __b) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst4q_f32(__a, __b) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4sf (__ax, __bu.__o); \
   })

#define vst4q_u8(__a, __b) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x16x4_t __i; __neon_int8x16x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst4q_u16(__a, __b) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst4q_u32(__a, __b) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o); \
   })

#define vst4q_p8(__a, __b) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x16x4_t __i; __neon_int8x16x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v16qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o); \
   })

#define vst4q_p16(__a, __b) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4v8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o); \
   })

#define vst4_lane_s8(__a, __b, __c) __extension__ \
  ({ \
     int8_t * __ax = __a; \
     union { int8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst4_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst4_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst4_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x2x4_t __i; __neon_float32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev2sf (__ax, __bu.__o, __c); \
   })

#define vst4_lane_u8(__a, __b, __c) __extension__ \
  ({ \
     uint8_t * __ax = __a; \
     union { uint8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst4_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst4_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x2x4_t __i; __neon_int32x2x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev2si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst4_lane_p8(__a, __b, __c) __extension__ \
  ({ \
     poly8_t * __ax = __a; \
     union { poly8x8x4_t __i; __neon_int8x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8qi (__neon_ptr_cast(__builtin_neon_qi *, __ax), __bu.__o, __c); \
   })

#define vst4_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x4x4_t __i; __neon_int16x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst4q_lane_s16(__a, __b, __c) __extension__ \
  ({ \
     int16_t * __ax = __a; \
     union { int16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst4q_lane_s32(__a, __b, __c) __extension__ \
  ({ \
     int32_t * __ax = __a; \
     union { int32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst4q_lane_f32(__a, __b, __c) __extension__ \
  ({ \
     float32_t * __ax = __a; \
     union { float32x4x4_t __i; __neon_float32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4sf (__ax, __bu.__o, __c); \
   })

#define vst4q_lane_u16(__a, __b, __c) __extension__ \
  ({ \
     uint16_t * __ax = __a; \
     union { uint16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vst4q_lane_u32(__a, __b, __c) __extension__ \
  ({ \
     uint32_t * __ax = __a; \
     union { uint32x4x4_t __i; __neon_int32x4x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev4si (__neon_ptr_cast(__builtin_neon_si *, __ax), __bu.__o, __c); \
   })

#define vst4q_lane_p16(__a, __b, __c) __extension__ \
  ({ \
     poly16_t * __ax = __a; \
     union { poly16x8x4_t __i; __neon_int16x8x4_t __o; } __bu = { __b }; \
     __builtin_neon_vst4_lanev8hi (__neon_ptr_cast(__builtin_neon_hi *, __ax), __bu.__o, __c); \
   })

#define vand_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vand_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vand_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vand_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vand_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vand_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vand_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vand_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vandq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vandq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vandq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vandq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vandq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vandq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vandq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vandq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vandv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorr_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorr_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorr_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorr_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorr_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorr_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorr_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorr_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorrq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorrq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorrq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorrq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorrq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorrq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorrq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorrq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vorrv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define veor_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define veor_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define veor_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define veor_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define veor_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define veor_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define veor_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define veor_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define veorq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define veorq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define veorq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define veorq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define veorq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define veorq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define veorq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define veorq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_veorv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vbic_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vbic_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vbic_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vbic_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vbic_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vbic_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vbic_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vbic_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vbicq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vbicq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vbicq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vbicq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vbicq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vbicq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vbicq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vbicq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vbicv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorn_s8(__a, __b) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     int8x8_t __bx = __b; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv8qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorn_s16(__a, __b) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     int16x4_t __bx = __b; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv4hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorn_s32(__a, __b) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     int32x2_t __bx = __b; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv2si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorn_s64(__a, __b) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     int64x1_t __bx = __b; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv1di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vorn_u8(__a, __b) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     uint8x8_t __bx = __b; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv8qi ((__neon_int8x8_t) __ax.val, (__neon_int8x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorn_u16(__a, __b) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     uint16x4_t __bx = __b; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv4hi ((__neon_int16x4_t) __ax.val, (__neon_int16x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorn_u32(__a, __b) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     uint32x2_t __bx = __b; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv2si ((__neon_int32x2_t) __ax.val, (__neon_int32x2_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vorn_u64(__a, __b) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     uint64x1_t __bx = __b; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv1di ((__neon_int64x1_t) __ax.val, (__neon_int64x1_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vornq_s8(__a, __b) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     int8x16_t __bx = __b; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv16qi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vornq_s16(__a, __b) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     int16x8_t __bx = __b; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv8hi (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vornq_s32(__a, __b) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     int32x4_t __bx = __b; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv4si (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vornq_s64(__a, __b) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     int64x2_t __bx = __b; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv2di (__ax.val, __bx.val, 1); \
     __rv.__i; \
   })

#define vornq_u8(__a, __b) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     uint8x16_t __bx = __b; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv16qi ((__neon_int8x16_t) __ax.val, (__neon_int8x16_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vornq_u16(__a, __b) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     uint16x8_t __bx = __b; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv8hi ((__neon_int16x8_t) __ax.val, (__neon_int16x8_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vornq_u32(__a, __b) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     uint32x4_t __bx = __b; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv4si ((__neon_int32x4_t) __ax.val, (__neon_int32x4_t) __bx.val, 0); \
     __rv.__i; \
   })

#define vornq_u64(__a, __b) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     uint64x2_t __bx = __b; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vornv2di ((__neon_int64x2_t) __ax.val, (__neon_int64x2_t) __bx.val, 0); \
     __rv.__i; \
   })


#define vreinterpret_p8_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p8_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { poly8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p8_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { poly8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_p16_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { poly16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_p16_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { poly16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_f32_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { float32x2_t __i; __neon_float32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2sfv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_f32_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { float32x4_t __i; __neon_float32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4sfv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s64_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { int64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s64_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { int64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u64_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { uint64x1_t __i; __neon_int64x1_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv1div4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u64_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { uint64x2_t __i; __neon_int64x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2div8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s8_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { int8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s8_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { int8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s16_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { int16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s16_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { int16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_s32_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { int32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_s32_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { int32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u8_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { uint8x8_t __i; __neon_int8x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8qiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u8_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { uint8x16_t __i; __neon_int8x16_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv16qiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_u32(__a) __extension__ \
  ({ \
     uint32x2_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv2si ((__neon_int32x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u16_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { uint16x4_t __i; __neon_int16x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4hiv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_u32(__a) __extension__ \
  ({ \
     uint32x4_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv4si ((__neon_int32x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u16_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { uint16x8_t __i; __neon_int16x8_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv8hiv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_s8(__a) __extension__ \
  ({ \
     int8x8_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv8qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_s16(__a) __extension__ \
  ({ \
     int16x4_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv4hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_s32(__a) __extension__ \
  ({ \
     int32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv2si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_s64(__a) __extension__ \
  ({ \
     int64x1_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv1di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_f32(__a) __extension__ \
  ({ \
     float32x2_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv2sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_u8(__a) __extension__ \
  ({ \
     uint8x8_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_u16(__a) __extension__ \
  ({ \
     uint16x4_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_u64(__a) __extension__ \
  ({ \
     uint64x1_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv1di ((__neon_int64x1_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_p8(__a) __extension__ \
  ({ \
     poly8x8_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv8qi ((__neon_int8x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpret_u32_p16(__a) __extension__ \
  ({ \
     poly16x4_t __ax = __a; \
     union { uint32x2_t __i; __neon_int32x2_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv2siv4hi ((__neon_int16x4_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_s8(__a) __extension__ \
  ({ \
     int8x16_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv16qi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_s16(__a) __extension__ \
  ({ \
     int16x8_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv8hi (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_s32(__a) __extension__ \
  ({ \
     int32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv4si (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_s64(__a) __extension__ \
  ({ \
     int64x2_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv2di (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_f32(__a) __extension__ \
  ({ \
     float32x4_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv4sf (__ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_u8(__a) __extension__ \
  ({ \
     uint8x16_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_u16(__a) __extension__ \
  ({ \
     uint16x8_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_u64(__a) __extension__ \
  ({ \
     uint64x2_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv2di ((__neon_int64x2_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_p8(__a) __extension__ \
  ({ \
     poly8x16_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv16qi ((__neon_int8x16_t) __ax.val); \
     __rv.__i; \
   })

#define vreinterpretq_u32_p16(__a) __extension__ \
  ({ \
     poly16x8_t __ax = __a; \
     union { uint32x4_t __i; __neon_int32x4_t __o; } __rv; \
     __rv.__o = __builtin_neon_vreinterpretv4siv8hi ((__neon_int16x8_t) __ax.val); \
     __rv.__i; \
   })

#ifdef __cplusplus
}
#endif
#endif
#endif

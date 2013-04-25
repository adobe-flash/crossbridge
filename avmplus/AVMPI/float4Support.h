/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __float4Support__
#define __float4Support__


#if defined(VMCFG_GENERIC_FLOAT4) || !defined(VMCFG_FLOAT)
/**
 * Float4 support, no intrinsics
 */
typedef 
struct float4_t {
    float x, y, z, w;
} float4_t;


/* These may be hardware-accelerated and slighly imprecise (e.g. in handling of denormal numbers)
   The implementation stays in .cpp so it can be done in the same way that the CodegenLIR.cpp does it */
float4_t f4_add(const float4_t& x1, const float4_t& x2);
float4_t f4_sub(const float4_t& x1, const float4_t& x2);
float4_t f4_mul(const float4_t& x1, const float4_t& x2);
float4_t f4_div(const float4_t& x1, const float4_t& x2);


REALLY_INLINE int32_t f4_eq_i(const float4_t& x1, const float4_t& x2)
{
    return (x1.x == x2.x) && (x1.y == x2.y) && (x1.z == x2.z) && (x1.w == x2.w);
}

// in hardware: vdupq_n_f32/ _mm_set_ps1
REALLY_INLINE float4_t f4_setall(float v) 
{ 
    float4_t retval = {v, v, v, v};
    return retval;
}

// in hardware: vdupq_n_f32, or _mm_sqrt_ps
REALLY_INLINE float4_t f4_sqrt(const float4_t& v) 
{ 
    float4_t retval = { sqrtf(v.x), sqrtf(v.y), sqrtf(v.z), sqrtf(v.w)};
    return retval;
}

REALLY_INLINE float f4_x(const float4_t& v) { return v.x; }
REALLY_INLINE float f4_y(const float4_t& v) { return v.y; }
REALLY_INLINE float f4_z(const float4_t& v) { return v.z; }
REALLY_INLINE float f4_w(const float4_t& v) { return v.w; }


/**
 * Float4 support through intrinsics starts below
 */


#else  // not VMCFG_GENERIC_FLOAT4
#define verifyEnterVECR_adapter verifyEnterVECR
#define debugEnterVECR_adapter debugEnterExitWrapperV
#define thunkEnterVECR_adapter(thunk, env, argc, argv) ((VecrMethodProc)thunk)(env,argc,argv)

#if defined VMCFG_SSE2
#include <xmmintrin.h>

typedef __m128  float4_t;

#define f4_add    _mm_add_ps
#define f4_sub    _mm_sub_ps
#define f4_mul    _mm_mul_ps
#define f4_div    _mm_div_ps
#define f4_setall _mm_set_ps1
#define f4_sqrt   _mm_sqrt_ps

REALLY_INLINE int32_t f4_eq_i(float4_t a, float4_t b)
{
    return (_mm_movemask_epi8(_mm_castps_si128(_mm_cmpneq_ps(a, b))) == 0);
}

REALLY_INLINE float f4_x(float4_t v) { return _mm_cvtss_f32(v); }
REALLY_INLINE float f4_y(float4_t v) { return _mm_cvtss_f32(_mm_shuffle_ps(v, v, 0x55)); }
REALLY_INLINE float f4_z(float4_t v) { return _mm_cvtss_f32(_mm_shuffle_ps(v, v, 0xAA)); }
REALLY_INLINE float f4_w(float4_t v) { return _mm_cvtss_f32(_mm_shuffle_ps(v, v, 0xFF)); }

#elif defined VMCFG_NEON // FIXME: nobody defines VMCFG_NEON currently... this should change.
#include <arm_neon.h>

typedef float32x4_t  float4_t;

#define f4_add       vaddq_f32
#define f4_sub       vsubq_f32
#define f4_mul       vmulq_f32
#define f4_sqrt      vsqrtq_f32
#define f4_setall       vdupq_n_f32

REALLY_INLINE int32_t f4_eq_i(float4_t a, float4_t b)
{
    uint32x2_t res = vreinterpret_u32_u16(vmovn_u32(vceqq_f32(a, b)));
    return vget_lane_u32(res, 0) == 0xffffffff && vget_lane_u32(res, 1) == 0xffffffff;
}

REALLY_INLINE float f4_x(float4_t v) { return vgetq_lane_f32(v, 0); }
REALLY_INLINE float f4_y(float4_t v) { return vgetq_lane_f32(v, 1); }
REALLY_INLINE float f4_z(float4_t v) { return vgetq_lane_f32(v, 2); }
REALLY_INLINE float f4_w(float4_t v) { return vgetq_lane_f32(v, 3); }

REALLY_INLINE float4_t f4_div(float4_t a, float4_t b)
{
    float4_t result = { f4_x(a) / f4_x(b), f4_y(a) / f4_y(b), f4_z(a) / f4_z(b), f4_w(a) / f4_w(b) };
    return result;
}
#else
#error Hardware-accellerated float4 support not implemented for the current system! The generic mode should have been activated.
#endif

#endif // VMCFG_GENERIC_FLOAT4
#endif // __float4Support__

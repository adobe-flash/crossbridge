/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include "float4Support.h"

#ifdef VMCFG_GENERIC_FLOAT4
namespace avmplus
{
/* Note: This file must be compiled with NEON turned on even for targets that don't have NEON, in order for the adapters to work.
  But VMCFG_NEON may be undefined, indicating that we don't want fast intrinsic implementation */
#ifdef VMCFG_FLOAT
 #ifdef VMCFG_ARM 
   #include <arm_neon.h>
   #define float4_ret_t float32x4_t
 #elif defined VMCFG_SSE2
   #include <xmmintrin.h>
   #define float4_ret_t __m128
 #else
   #error Unsupported platform in float4Support.cpp
 #endif
#else

#endif

typedef union {
   float4_ret_t f4_jit;
   float4_t f4;
} rvtype;

// Alignment is buggy in MSVC, do it by hand.
#define DECLARE_ALIGNED_FLOAT4_PTR(v) \
    float locals[8];   \
    uintptr_t lptr = (uintptr_t)(&locals[0]);\
    rvtype *v = reinterpret_cast<rvtype*>((lptr + 0xf) & ~0xf);

float4_ret_t verifyEnterVECR_adapter_impl(avmplus::MethodEnv* env, int32_t argc, uint32_t* ap){
    DECLARE_ALIGNED_FLOAT4_PTR(retval);
    retval->f4 = avmplus::BaseExecMgr::verifyEnterVECR(env, argc, ap);
    return retval->f4_jit;
}

float4_t thunkEnterVECR_adapter_impl(void* thunk_p, MethodEnv* env, int32_t argc, uint32_t* argv){
    typedef float4_ret_t (*VecrThunk)(avmplus::MethodEnv* env, int32_t argc, uint32_t* argv);
    DECLARE_ALIGNED_FLOAT4_PTR(retval);
    if( thunk_p)  // prevent ARM GCC from doing CSE, it crashes otherwise
        retval->f4_jit = ((VecrThunk) thunk_p)(env, argc, argv);
    return retval->f4;
}
#ifdef DEBUGGER
float4_ret_t debugEnterVECR_adapter_impl(avmplus::MethodEnv* env, int32_t argc, uint32_t* ap){
    DECLARE_ALIGNED_FLOAT4_PTR(retval);
    retval->f4 = avmplus::BaseExecMgr::debugEnterExitWrapperV(env, argc, ap);
    return retval->f4_jit;
}
const VecrMethodProc debugEnterVECR_adapter = (VecrMethodProc) debugEnterVECR_adapter_impl;
#endif

const VecrMethodProc verifyEnterVECR_adapter = (VecrMethodProc) verifyEnterVECR_adapter_impl;
const VecrThunkProc thunkEnterVECR_adapter = thunkEnterVECR_adapter_impl;

}
#endif
// TODO: leave these for "#if !defined(VMCFG_NEON) && !defined(VMCFG_SSE2)", add hardware-accellerated versions for NEON and SSE2
float4_t f4_add(const float4_t& x1, const float4_t& x2) 
{ 
    float4_t retval = { x1.x + x2.x, x1.y + x2.y, x1.z + x2.z, x1.w + x2.w };
    return retval;
}

float4_t f4_sub(const float4_t& x1, const float4_t& x2) 
{ 
    float4_t retval = { x1.x - x2.x, x1.y - x2.y, x1.z - x2.z, x1.w - x2.w };
    return retval;
}

float4_t f4_mul(const float4_t& x1, const float4_t& x2) 
{ 
    float4_t retval = { x1.x * x2.x, x1.y * x2.y, x1.z * x2.z, x1.w * x2.w };
    return retval;
}

float4_t f4_div(const float4_t& x1, const float4_t& x2) 
{ 
    float4_t retval = { x1.x / x2.x, x1.y / x2.y, x1.z / x2.z, x1.w / x2.w };
    return retval;
}

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <math.h>

#if defined(AVMPLUS_IA32)
#define X86_MATH
#endif //#if defined(AVMPLUS_IA32)

// Avoid unsafe floating-point optimizations, including replacing library calls
// with inlined x87 instructions.  We will do this explicitly with inline asm
// where appropriate.
#pragma float_control(push)
#pragma float_control(precise, on)

// warning this code is used by amd64 and arm builds

namespace avmplus
{

    REALLY_INLINE double MathUtils::abs(double value)
    {
#ifdef X86_MATH
        _asm fld [value];
        _asm fabs;
#else
        return ::fabs(value);
#endif /* X86_MATH */
    }

#ifdef X86_MATH
    REALLY_INLINE double MathUtils::atan2(double y, double x)
    {
        _asm fld [y];
        _asm fld [x];
        _asm fpatan;
    }
#elif !defined(UNDER_CE)
    REALLY_INLINE double MathUtils::atan2(double y, double x)
    {
        return ::atan2(y,x);
    }
#endif /* X86_MATH */

    REALLY_INLINE double MathUtils::acos(double value)
    {
#ifdef X86_MATH
        // atan2 and sqrt are hardware instructions on x86
        return MathUtils::atan2(MathUtils::sqrt(1.0-value*value), value);
#else
        return ::acos(value);
#endif /* X86_MATH */
    }

    REALLY_INLINE double MathUtils::asin(double value)
    {
#ifdef X86_MATH
        // atan2 and sqrt are hardware instructions on x86
        return MathUtils::atan2(value, MathUtils::sqrt(1.0-value*value));
#else
        return ::asin(value);
#endif /* X86_MATH */
    }

    REALLY_INLINE double MathUtils::atan(double value)
    {
#ifdef X86_MATH
        _asm fld [value];
        _asm fld1;
        _asm fpatan;
#else
        return ::atan(value);
#endif /* X86_MATH */
    }

#ifndef X86_MATH
    REALLY_INLINE double MathUtils::ceil(double value)
    {
        return ::ceil(value);
    }
#endif /* X86_MATH */

// The x87 trigonometric instructions produce values that differ from the
// results expected by the ECMAscript test suite for sin, cos, and tan.
// Use the standard library instead on Win32 as well as Win64. See bug 521245.

#ifndef UNDER_CE
    REALLY_INLINE double MathUtils::cos(double value)
    {
        return ::cos(value);
    }
#endif /* UNDER_CE */

#ifndef X86_MATH
    REALLY_INLINE double MathUtils::exp(double value)
    {
        return ::exp(value);
    }
#endif /* X86_MATH */

#ifndef X86_MATH
    REALLY_INLINE double MathUtils::floor(double value)
    {
        return ::floor(value);
    }
#endif /* X86_MATH */

    REALLY_INLINE double MathUtils::log(double value)
    {
#ifdef X86_MATH
        _asm fld [value];
        _asm fldln2;
        _asm fxch;
        _asm fyl2x;
#else
        return ::log(value);
#endif /* X86_MATH */
    }

#ifndef X86_MATH
#if defined(UNDER_CE) || defined(UNDER_RT)
// TODO: FIXME: _WIN8_ARM_SLOWDOWN_ here. Open ToFix
    REALLY_INLINE double MathUtils::mod(double x, double y)
    {
        if (!y) {
            return kNaN;
        }
        return ::fmod(x, y);
    }
#else //#if defined(UNDER_CE)
    extern "C" {
        // See win64setjmp.asm
        double modInternal(double x, double y);
    }

    REALLY_INLINE double MathUtils::mod(double x, double y)
    {
        if (!y) {
            return kNaN;
        }
        return modInternal(x, y);
    }
#endif //#if defined(UNDER_CE)
#endif /* X86_MATH */

#ifndef X86_MATH
    REALLY_INLINE double MathUtils::powInternal(double x, double y)
    {
        return ::pow(x, y);
    }
#endif /* X86_MATH */

// The x87 trigonometric instructions produce values that differ from the
// results expected by the ECMAscript test suite for sin, cos, and tan.
// Use the standard library instead on Win32 as well as Win64. See bug 521245.

#ifndef UNDER_CE
    REALLY_INLINE double MathUtils::sin(double value)
    {
        return ::sin(value);
    }

    REALLY_INLINE double MathUtils::tan(double value)
    {
        return ::tan(value);
    }
#endif /* UNDER_CE */

    REALLY_INLINE double MathUtils::sqrt(double value)
    {
#ifdef X86_MATH
        _asm fld [value];
        _asm fsqrt;
#else
        return ::sqrt(value);
#endif /* X86_MATH */
    }

    // TODO: Optimize the float versions, but only if benchmarking shows it to be useful to do so.

    REALLY_INLINE float MathUtils::absf(float value)
    {
        return ::fabsf(value);
    }
    
    REALLY_INLINE float MathUtils::acosf(float value)
    {
        return ::acosf(value);
    }
    
    REALLY_INLINE float MathUtils::asinf(float value)
    {
        return ::asinf(value);
    }
    
    REALLY_INLINE float MathUtils::atanf(float value)
    {
        return ::atanf(value);
    }
    
    REALLY_INLINE float MathUtils::ceilf(float value)
    {
        return ::ceilf(value);
    }
    
    REALLY_INLINE float MathUtils::cosf(float value)
    {
#if defined(VMCFG_TWEAK_SIN_COS_NONFINITE)
        if (isNaNf(value) || isInfinitef(value))
            return kFltNaN;
#endif
        return ::cosf(value);
    }
    
    REALLY_INLINE float MathUtils::expf(float value)
    {
        // MSDN docs: Infinity is not in the domain for expf().
        int32_t x = isInfinitef(value);
        if (x < 0)
            return 0;
        if (x > 0)
            return kFltInfinity;
        return ::expf(value);
    }
    
    REALLY_INLINE float MathUtils::floorf(float value)
    {
        return ::floorf(value);
    }
    
    REALLY_INLINE float MathUtils::logf(float value)
    {
        return ::logf(value);
    }
    
    REALLY_INLINE float MathUtils::modf(float x, float y)
    {
        return ::fmodf(x, y);
    }
    
    REALLY_INLINE float MathUtils::sinf(float value)
    {
#if defined(VMCFG_TWEAK_SIN_COS_NONFINITE)
        if (isNaNf(value) || isInfinitef(value))
            return kFltNaN;
#endif
        return ::sinf(value);
    }

    REALLY_INLINE float MathUtils::recipf(float value)
    {
        // FIXME (Bugzilla 704097): this must use the intrinsic
        return 1 / value;
    }

    REALLY_INLINE float MathUtils::rsqrtf(float value)
    {
        // FIXME (Bugzilla 704097): this must use the intrinsic
        return 1 / ::sqrtf(value);
    }
    
    REALLY_INLINE float MathUtils::sqrtf(float value)
    {
        return ::sqrtf(value);
    }
    
    REALLY_INLINE float MathUtils::tanf(float value)
    {
        return ::tanf(value);
    }
}

// Restore the prevailing floating-point model.
#pragma float_control(pop)

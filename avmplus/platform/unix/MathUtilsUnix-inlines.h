/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <math.h>

namespace avmplus
{
    // todo need asm versions from Player

    REALLY_INLINE double MathUtils::abs(double value)
    {
        return ::fabs(value);
    }

    REALLY_INLINE float MathUtils::absf(float value)
    {
        return ::fabsf(value);
    }
    
    REALLY_INLINE double MathUtils::acos(double value)
    {
        return ::acos(value);
    }

    REALLY_INLINE float MathUtils::acosf(float value)
    {
        return ::acosf(value);
    }
    
    REALLY_INLINE double MathUtils::asin(double value)
    {
        return ::asin(value);
    }

    REALLY_INLINE float MathUtils::asinf(float value)
    {
        return ::asinf(value);
    }
    
    REALLY_INLINE double MathUtils::atan(double value)
    {
        return ::atan(value);
    }

    REALLY_INLINE float MathUtils::atanf(float value)
    {
        return ::atanf(value);
    }
    
    REALLY_INLINE double MathUtils::atan2(double y, double x)
    {
        return ::atan2(y, x);
    }

    REALLY_INLINE float MathUtils::atan2f(float y, float x)
    {
        return ::atan2f(y, x);
    }
    
    REALLY_INLINE double MathUtils::ceil(double value)
    {
// MacOS X Intel 10.5 incorrectly make ceil(-0.5) -> 0.0 instead of -0.0
// special-case and correct.
#if defined(AVMPLUS_MAC) && (defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64))
        double r = ::ceil(value);
        if (r == 0.0 && value < 0.0) r = -0.0;
        return r;
#else
        return ::ceil(value);
#endif
    }

    REALLY_INLINE float MathUtils::ceilf(float value)
    {
        // Code copied from above without checking if this still holds.
        // MacOS X Intel 10.5 incorrectly make ceil(-0.5) -> 0.0 instead of -0.0
        // special-case and correct.
#if defined(AVMPLUS_MAC) && (defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64))
        double r = ::ceilf(value);
        if (r == 0.0 && value < 0.0) r = -0.0;
        return r;
#else
        return ::ceilf(value);
#endif
    }
    
    REALLY_INLINE double MathUtils::cos(double value)
    {
#if defined(VMCFG_TWEAK_SIN_COS_NONFINITE)
        if (isNaN(value) || isInfinite(value))
            return kNaN;
#endif
        return ::cos(value);
    }

    REALLY_INLINE float MathUtils::cosf(float value)
    {
#if defined(VMCFG_TWEAK_SIN_COS_NONFINITE)
        if (isNaNf(value) || isInfinitef(value))
            return kFltNaN;
#endif
        return ::cosf(value);
    }
    
    REALLY_INLINE double MathUtils::exp(double value)
    {
        return ::exp(value);
    }

    REALLY_INLINE float MathUtils::expf(float value)
    {
        return ::expf(value);
    }
    
    REALLY_INLINE double MathUtils::floor(double value)
    {
        return ::floor(value);
    }

    REALLY_INLINE float MathUtils::floorf(float value)
    {
        return ::floorf(value);
    }
    
    REALLY_INLINE uint64_t MathUtils::frexp(double value, int *eptr)
    {
        double fracMantissa = ::frexp(value, eptr);

        // correct mantissa and eptr to get integer values
        //  for both
        *eptr -= 53; // 52 mantissa bits + the hidden bit
        return (uint64_t)(fracMantissa * (double)(1LL << 53));
    }

    REALLY_INLINE double MathUtils::log(double value)
    {
        return ::log(value);
    }

    REALLY_INLINE float MathUtils::logf(float value)
    {
        return ::logf(value);
    }
    
    REALLY_INLINE double MathUtils::mod(double x, double y)
    {
        return ::fmod(x, y);
    }

    REALLY_INLINE float MathUtils::modf(float x, float y)
    {
        return ::fmodf(x, y);
    }
    
    REALLY_INLINE double MathUtils::powInternal(double x, double y)
    {
        return ::pow(x, y);
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

    REALLY_INLINE double MathUtils::sin(double value)
    {
#if defined(VMCFG_TWEAK_SIN_COS_NONFINITE)
        if (isNaN(value) || isInfinite(value))
            return kNaN;
#endif
        return ::sin(value);
    }

    REALLY_INLINE float MathUtils::sinf(float value)
    {
#if defined(VMCFG_TWEAK_SIN_COS_NONFINITE)
        if (isNaNf(value) || isInfinitef(value))
            return kFltNaN;
#endif
        return ::sinf(value);
    }
    
    REALLY_INLINE double MathUtils::sqrt(double value)
    {
        return ::sqrt(value);
    }

    REALLY_INLINE float MathUtils::sqrtf(float value)
    {
        return ::sqrtf(value);
    }
    
    REALLY_INLINE double MathUtils::tan(double value)
    {
        return ::tan(value);
    }

    REALLY_INLINE float MathUtils::tanf(float value)
    {
        return ::tanf(value);
    }
}

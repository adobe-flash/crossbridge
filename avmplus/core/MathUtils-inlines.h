/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_MathUtils_inlines__
#define __avmplus_MathUtils_inlines__

namespace avmplus
{
    // These methods appear to be unused
    // The implementations of max, min, maxf, and minf do not follow the spec
    // in some cases with signed zero arguments.
    // BEGIN
    REALLY_INLINE double MathUtils::infinity() { return kInfinity; }
    REALLY_INLINE double MathUtils::neg_infinity() { return kNegInfinity; }
    REALLY_INLINE double MathUtils::max(double x, double y) { return (x > y) ? x : y; }
    REALLY_INLINE double MathUtils::min(double x, double y) { return (x < y) ? x : y; }
    REALLY_INLINE double MathUtils::nan() { return kNaN; }

    REALLY_INLINE float  MathUtils::maxf(float x, float y) { return (x > y) ? x : y; }
    REALLY_INLINE float  MathUtils::minf(float x, float y) { return (x < y) ? x : y; }
    // END

#if !defined(WIN32) || !defined(AVMPLUS_IA32)
    REALLY_INLINE double MathUtils::round(double value)
    {
        return MathUtils::floor(value + 0.5);
    }
#endif // !defined(WIN32) || !defined(AVMPLUS_IA32)

    REALLY_INLINE float MathUtils::roundf(float value)
    {
        return MathUtils::floorf(value + 0.5f);
    }

    REALLY_INLINE bool MathUtils::isNaNInline(double d)
    {
#if defined(AVMPLUS_IA32) && !defined(AVMPLUS_SSE2_ALWAYS)
        double_overlay u(d);
        return ((u.bits64 & ~0x8000000000000000ULL) > 0x7ff0000000000000ULL);
#else
        return (d != d);
#endif
    }
}

#endif /* __avmplus__MathUtils__ */

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

    REALLY_INLINE double MathUtils::atan(double value)
    {
        return ::atan(value);
    }

    REALLY_INLINE double MathUtils::ceil(double value)
    {
        return ::ceil(value);
    }

    REALLY_INLINE double MathUtils::cos(double value)
    {
        return ::cos(value);
    }

    REALLY_INLINE double MathUtils::exp(double value)
    {
        return ::exp(value);
    }

    REALLY_INLINE double MathUtils::floor(double value)
    {
        return ::floor(value);
    }

    REALLY_INLINE uint64_t MathUtils::frexp(double value, int *eptr)
    {
        double fracMantissa = ::frexp(value, eptr);

        // correct mantissa and eptr to get integer values
        //  for both
        *eptr -= 53; // 52 mantissa bits + the hidden bit
        return (uint64_t)(fracMantissa * (double)(1LL << 53));
    }

    REALLY_INLINE double MathUtils::mod(double x, double y)
    {
        return ::fmod(x, y);
    }

    REALLY_INLINE double MathUtils::powInternal(double x, double y)
    {
        return ::pow(x, y);
    }

    REALLY_INLINE double MathUtils::sin(double value)
    {
        return ::sin(value);
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

    REALLY_INLINE double MathUtils::sqrt(double value)
    {
        return ::sqrt(value);
    }

    REALLY_INLINE double MathUtils::tan(double value)
    {
        return ::tan(value);
    }

}

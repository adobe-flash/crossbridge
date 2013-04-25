/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include <math.h>

#if defined(AVMPLUS_IA32)
#define X86_MATH
#endif //#if defined(AVMPLUS_IA32)

// Avoid unsafe floating-point optimizations, including replacing library calls
// with inlined x87 instructions.  We will do this explicitly with inline asm
// where appropriate.
#pragma float_control(precise, on)

// warning this code is used by amd64 and arm builds

namespace avmplus
{
    const static double PI = 3.141592653589793;
    const static double PI3_BY_4 = 3*PI/4;
    const static double PI_BY_4 = PI/4;
    const static double PI2 = 2*PI;

#ifdef UNDER_CE
    // 0=no, 1=+0, -1=-0
    static int32_t isZero(double v)
    {
        int32_t r = (MathUtils::isNegZero(v)) ? -1 : (v==0.0)? 1 : 0;
        return r;
    }
#endif

    const static float PI_f = float(PI);
    const static float PI3_BY_4_f = float(PI3_BY_4);
    const static float PI_BY_4_f = float(PI_BY_4);
    const static float PI2_f = float(PI2);

    // 0=no, 1=+0, -1=-0
    static int32_t isZerof(float v)
    {
        int32_t r = (MathUtils::isNegZerof(v)) ? -1 : (v==0.0)? 1 : 0;
        return r;
    }

    static bool isFiniteAndNonzerof(float x)
    {
        float_overlay u(x);
        return ((u.word & 0x7fffffffL) != 0x7F800000L) && ((u.word & 0x7FFFFFFFL) != 0);
    }

#ifdef UNDER_CE

    // sin, cos, tan all function incorrectly when called with really large values on windows mobile
    // they all start failing at different values, but all start failing somewhere with values
    // greater than 210 million.
    #define AVMPLUS_TRIG_FUNC_MAX 210000000

    const static bool broken_trig_funcs = MathUtils::isNaN(MathUtils::cos(250000000));

    // Helper function to adjust a value for sin, cos, or tan into an equivalent value
    // in the range that works correctly.  This works because these functions all have a period
    // of 2*PI or PI, so there are many equivalent values.
    static double adjustValueForTrigFuncs(double v)
    {
        bool negate = false;
        if( v < 0 ) {
            v = -v;
            negate = true;
        }

        int temp = (int)((v - AVMPLUS_TRIG_FUNC_MAX)/PI2);
        double offset = PI2*(temp+1);
        v -= offset;

        if( negate )
            v = -v;

        return v;
    }

#endif /* UNDER_CE */

#ifdef UNDER_CE
    double MathUtils::atan2(double y, double x)
    {
        int32_t zx = isZero(x);
        int32_t zy = isZero(y);
        if (zx==-1 && zy!=0)
            return zy*PI;  // +-0,-0 case
        else if (zy==-1 && (x==1.0 || x==-1.0))
            return -(::atan2(y,x));  // negate result

        double r = ::atan2(y, x);
        if (MathUtils::isNaN(r)) {
            int32_t s = MathUtils::isInfinite(x);
            if (s==1)
                r = MathUtils::isInfinite(y) * PI_BY_4;
            else if (s==-1)
                r = MathUtils::isInfinite(y) * PI3_BY_4;
        }
        return r;
    }
#endif /* UNDER_CE */

    float MathUtils::atan2f(float y, float x)
    {
        // MSDN docs appear to state that Infinity is in the domain for atan2f, but
        // that does not appear to be true.  So implement the whole mess here.

        // Easy cases are handled by the libc function.
        if (isFiniteAndNonzerof(y) && isFiniteAndNonzerof(x))
            return ::atan2f(y, x);

        if (isNaNf(x)) return x;
        if (isNaNf(y)) return y;
        if (isInfinitef(y))
        {
            if (!isInfinitef(x))
                return (y > 0 ? (PI_f / 2) : -(PI_f / 2));
            float r = (x > 0 ? (PI_f / 4) : (3 * PI_f / 4));
            return (y > 0 ? r : -r);
        }
        else if (y != 0)
        {
            if (x == 0)
                return (y > 0 ? (PI_f / 2) : -(PI_f / 2));
            assert(isInfinite(x));
            float r = (x > 0 ? 0.0f : PI_f);
            return (y > 0 ? r : -r); 
        }
        else // y == 0
        {
            int32_t zx = isZerof(x);
            int32_t zy = isZerof(y);
            float r = (x > 0 || zx == 1) ? 0 : PI_f;
            return (zy > 0) ? r : -r;
        }
    }

#ifdef X86_MATH
    double MathUtils::ceil(double value)
    {
        // todo avoid control word modification
        short oldcw, newcw;
        _asm fnstcw [oldcw];
        _asm mov ax, [oldcw];
        _asm and ax, 0xf3ff; // Set to round down.
        _asm or  ax, 0x800;
        _asm mov [newcw], ax;
        _asm fldcw [newcw];
        _asm fld [value];
        _asm frndint;
        _asm fldcw [oldcw];
    }
#endif /* X86_MATH */

#ifdef UNDER_CE
    double MathUtils::cos(double value)
    {
        if( broken_trig_funcs && (value > AVMPLUS_TRIG_FUNC_MAX || value < -AVMPLUS_TRIG_FUNC_MAX) )
        {
            return ::cos(adjustValueForTrigFuncs(value));
        }
        else
        {
            return ::cos(value);
        }
    }
#endif /* UNDER_CE */

#ifdef X86_MATH
    // Utility function, this module only.
    REALLY_INLINE static double expInternal(double x)
    {
        double value, exponent;
        _asm fld [x];
        _asm fldl2e;
        _asm _emit 0xD8; // fmul st(1);
        _asm _emit 0xC9;
        _asm _emit 0xDD; // fst st(1);
        _asm _emit 0xD1;
        _asm frndint;
        _asm fxch;
        _asm _emit 0xD8; // fsub st1;
        _asm _emit 0xE1;
        _asm f2xm1;
        _asm fstp [value];
        _asm fstp [exponent];

        value += 1.0;

        _asm fld [exponent];
        _asm fld [value];
        _asm fscale;

        _asm fxch;
        _asm _emit 0xDD; // fstp st(0);
        _asm _emit 0xD8;
    }

    // Inlined on other architectures
    double MathUtils::exp(double value)
    {
        switch (isInfinite(value)) {
        case 1:
            return kInfinity;
        case -1:
            return +0;
        default:
            return expInternal(value);
        }
    }
#endif /* X86_MATH */

#ifdef X86_MATH
    double MathUtils::round(double value)
    {
        value += 0.5;

        // todo avoid control word modification
        short oldcw, newcw;
        _asm fnstcw [oldcw];
        _asm mov ax, [oldcw];
        _asm and ax, 0xf3ff; // Set to round down.
        _asm or  ax, 0x400;
        _asm mov [newcw], ax;
        _asm fldcw [newcw];
        _asm fld [value];
        _asm frndint;
        _asm fldcw [oldcw];
    }
    double MathUtils::floor(double value)
    {
        // todo avoid control word modification
        short oldcw, newcw;
        _asm fnstcw [oldcw];
        _asm mov ax, [oldcw];
        _asm and ax, 0xf3ff; // Set to round down.
        _asm or  ax, 0x400;
        _asm mov [newcw], ax;
        _asm fldcw [newcw];
        _asm fld [value];
        _asm frndint;
        _asm fldcw [oldcw];
    }
#endif /* X86_MATH */

    /* @(#)s_frexp.c 5.1 93/09/24 */
    /*
     * ====================================================
     * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
     *
     * Developed at SunPro, a Sun Microsystems, Inc. business.
     * Permission to use, copy, modify, and distribute this
     * software is freely granted, provided that this notice
     * is preserved.
     * ====================================================
     */

    /*
     * for non-zero x
     *  x = frexp(arg,&exp);
     * return a double fp quantity x such that 0.5 <= |x| <1.0
     * and the corresponding binary exponent "exp". That is
     *  arg = x*2^exp.
     * If arg is inf, 0.0, or NaN, then frexp(arg,&exp) returns arg
     * with *exp=0.
     */

    /*
     * NOTE: This is little-endian, must be adjusted to work for
     * big-endian systems.
     */
#define EXTRACT_WORDS(hx, lx, x) {DWORD *ptr = (DWORD*)&x; hx=ptr[1]; lx=ptr[0];}
#define SET_HIGH_WORD(x, hx) {DWORD *ptr = (DWORD*)&x; ptr[1]=hx;}
#define GET_HIGH_WORD(hx, x) {DWORD *ptr = (DWORD*)&x; hx=ptr[1];}
    static const double two54 =  1.80143985094819840000e+16; /* 0x43500000, 0x00000000 */

    REALLY_INLINE static double ExtractFraction(double x, int *eptr)
    {
        DWORD hx, ix, lx;
        EXTRACT_WORDS(hx,lx,x);
        ix = 0x7fffffff&hx;
        *eptr = 0;
        if(ix>=0x7ff00000||((ix|lx)==0)) return x;  /* 0,inf,nan */
        if (ix<0x00100000) {        /* subnormal */
            x *= two54;
            GET_HIGH_WORD(hx,x);
            ix = hx&0x7fffffff;
            *eptr = -54;
        }
        *eptr += (ix>>20)-1022;
        hx = (hx&0x800fffff)|0x3fe00000;
        SET_HIGH_WORD(x,hx);
        return x;
    }

    uint64_t MathUtils::frexp(double x, int *eptr)
    {
        double fracMantissa = ExtractFraction(x, eptr);
        // correct mantissa and eptr to get integer values
        //  for both
        *eptr -= 53; // 52 mantissa bits + the hidden bit
        return (uint64_t)((fracMantissa) * (double)(1LL << 53));
    }

#ifdef X86_MATH
    // VC++ 2008 refuses to inline this, issues warning on _forceinline
    #pragma warning ( disable : 4740 ) // flow in or out of inline asm code suppresses global optimization
    double MathUtils::mod(double x, double y)
    {
        if (!y) {
            return kNaN;
        }
        _asm    fld [y];
        _asm    fld [x];
      ModLoop:
        _asm    fprem;
        _asm    fnstsw ax;
        _asm    sahf;
        _asm    jp ModLoop;
        _asm _emit 0xDD; // fstp st(1);
        _asm _emit 0xD9;
    }
    #pragma warning ( default : 4740 )
#endif /* X86_MATH */

#ifdef X86_MATH
    // Std. library pow()
    double MathUtils::powInternal(double x, double y)
    {
        double value, exponent;

        _asm fld1;
        _asm fld [x];
        _asm fyl2x;
        _asm fstp [value];

        _asm fld [value];
        _asm fld [y];
        _asm _emit 0xD8; // fmul st(1);
        _asm _emit 0xC9;
        _asm _emit 0xDD; // fst st(1);
        _asm _emit 0xD1;
        _asm frndint;
        _asm fxch;
        _asm _emit 0xD8; // fsub st1;
        _asm _emit 0xE1;
        _asm f2xm1;
        _asm fstp [value];
        _asm fstp [exponent];

        value += 1.0;

        _asm fld [exponent];
        _asm fld [value];
        _asm fscale;

        _asm fxch;
        _asm _emit 0xDD; // fstp st(0);
        _asm _emit 0xD8;
    }
#endif /* X86_MATH */

#ifdef UNDER_CE
    double MathUtils::sin(double value)
    {
        if( broken_trig_funcs && (value > AVMPLUS_TRIG_FUNC_MAX || value < -AVMPLUS_TRIG_FUNC_MAX) )
        {
            return ::sin(adjustValueForTrigFuncs(value));
        }
        else
        {
            return ::sin(value);
        }
    }

    double MathUtils::tan(double value)
    {
        if( broken_trig_funcs && (value > AVMPLUS_TRIG_FUNC_MAX || value < -AVMPLUS_TRIG_FUNC_MAX) )
        {
            return ::tan(adjustValueForTrigFuncs(value));
        }
        else
        {
            return ::tan(value);
        }
    }
#endif /* UNDER_CE */
}

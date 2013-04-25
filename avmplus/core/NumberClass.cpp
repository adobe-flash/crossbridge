/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    NumberClass::NumberClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        toplevel()->_numberClass = this;

        // prototype objects are always vanilla objects.
        createVanillaPrototype();
    }

    Atom NumberClass::construct(int argc, Atom* argv)
    {
        // Number called as constructor creates new Number instance
        // Note: SpiderMonkey returns 0 for new Number() with no args
        if (argc == 0)
            return zeroIntAtom;   // yep this is zero atom
        else
            return core()->numberAtom(argv[1]);
        // TODO ArgumentError if argc > 1
    }

    Stringp NumberClass::_convert(double n, int precision, int mode)
    {
        AvmCore* core = this->core();

        if (mode == MathUtils::DTOSTR_PRECISION)
        {
            if (precision < 1 || precision > 21) {
                toplevel()->throwRangeError(kInvalidPrecisionError, core->toErrorString(precision), core->toErrorString(1), core->toErrorString(21));
            }
        }
        else
        {
            if (precision < 0 || precision > 20) {
                toplevel()->throwRangeError(kInvalidPrecisionError, core->toErrorString(precision), core->toErrorString(0), core->toErrorString(20));
            }
        }

        return MathUtils::convertDoubleToString(core, n, mode, precision);
    }

    Stringp NumberClass::_numberToString(double dVal, int radix)
    {
        AvmCore* core = this->core();

        if (radix == 10 || MathUtils::isInfinite(dVal) || MathUtils::isNaN(dVal))
            return core->doubleToString(dVal);

        if (radix < 2 || radix > 36)
            toplevel()->throwRangeError(kInvalidRadixError, core->toErrorString(radix));

        // convertDoubleToStringRadix will convert the integer part of dVal
        // to a string in the specified radix, and it will handle large numbers
        // beyond the range of int/uint.  It will not handle the fractional
        // part.  To properly handle that, MathUtils::convertDoubleToString
        // would have to handle any base.  That's a lot of extra code and complexity for
        // something the ES3 spec says is implementation dependent
        // (i.e. we're not required to do it)

        return MathUtils::convertDoubleToStringRadix(core, dVal, radix);
    }

    // volatile externals to keep the constant propagator from outsmarting us.
    volatile double minDouble = 4.94e-324;
    volatile double minNormalizedDouble = 2.2250738585072014e-308;  // really needs all these digits to get it right

    double NumberClass::_minValue()
    {
        // https://bugzilla.mozilla.org/show_bug.cgi?id=555805
        //
        // The ARM architecture saves transistors by not implementing subnormal floating point numbers in
        // hardware.  Instead, those values produce a trap ("bounce") so that they can be emulated in software.
        // There is, however, a "RunFast" mode that suppresses the "bounce" and instead does a
        // "flush-to-zero" on underflow.  Some platforms operate in RunFast mode by default and while it
        // is possible for user code to turn off that mode, it seems to be in the OS's purview to install
        // the trap handler.  Long story short, on certain ARM-based platforms, values smaller than the smallest
        // normalized floating point value are snapped (flushed) to zero.
        //
        // The definitive reference URLs appear to be perishable, but the terms in quotes above are useful in searches:
        //      http://www.google.com/search?q=ARM+runfast+flush-to-zero+bounce
        //
        // The issue seems specific to ARM processors (though perhaps other mobilized cores as well), but will only occur
        // if the platform opts not to install an emulator trap.  So we dynamically find the smallest value, being wary of
        // techniques that might be optimized away by compilers (and especially cross-compilers!).

        double minValue = minDouble;
        if (minValue == 0.0)
            minValue = minNormalizedDouble;
#ifdef _DEBUG
        // It's going to be either 0x1 (~4.94e-324) if subnormals are supported or 0x0010000000000000 (~2.225e-308) if not.
        double_overlay d(minValue);
        AvmAssert((d.words.msw == 0 && d.words.lsw == 1) || (d.words.msw == 0x00100000 && d.words.lsw == 0));
#endif
        return minValue;
    }

    double NumberClass::abs(double x)
    {
        return MathUtils::abs(x);
    }
    
    double NumberClass::acos(double x)
    {
        return MathUtils::acos(x);
    }
    
    double NumberClass::asin(double x)
    {
        return MathUtils::asin(x);
    }
    
    double NumberClass::atan(double x)
    {
        return MathUtils::atan(x);
    }
    
    double NumberClass::atan2(double y, double x)
    {
        return MathUtils::atan2(y, x);
    }
    
    double NumberClass::ceil(double x)
    {
        return MathUtils::ceil(x);
    }
    
    double NumberClass::cos(double x)
    {
        return MathUtils::cos(x);
    }
    
    double NumberClass::exp(double x)
    {
        return MathUtils::exp(x);
    }
    
    double NumberClass::floor(double x)
    {
        return MathUtils::floor(x);
    }
    
    double NumberClass::log(double x)
    {
        return MathUtils::log(x);
    }
    
    double NumberClass::pow(double x, double y)
    {
        return MathUtils::pow(x, y);
    }
    
    double NumberClass::random()
    {
        // Share the RNG stream between Math and Number.
        return toplevel()->builtinClasses()->get_MathClass()->random();
    }
    
    double NumberClass::round(double x)
    {
        return MathUtils::round(x);
    }
    
    double NumberClass::sin(double x)
    {
        return MathUtils::sin(x);
    }
    
    double NumberClass::sqrt(double x)
    {
        return MathUtils::sqrt(x);
    }
    
    double NumberClass::tan(double x)
    {
        return MathUtils::tan(x);
    }
    
    double NumberClass::min(double x, double y, const Atom* argv, uint32_t argc)
    {
        return MathClass::minImpl(x,y,argv,argc);
    }

    double NumberClass::max(double x, double y, const Atom* argv, uint32_t argc)
    {
        return MathClass::maxImpl(x,y,argv,argc);
    }
}

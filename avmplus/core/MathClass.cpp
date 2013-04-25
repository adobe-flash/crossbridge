/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    Atom MathClass::construct(int /*argc*/, Atom* /*argv*/)
    {
        // according to ES3 15.8, Math cannot be used as a function or constructor.
        toplevel()->throwTypeError(kMathNotConstructorError);
        return 0;
    }

    Atom MathClass::call(int /*argc*/, Atom* /*argv*/)
    {
        // according to ES3 15.8, Math cannot be used as a function or constructor.
        toplevel()->throwTypeError(kMathNotFunctionError);
        return 0;
    }

    MathClass::MathClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        AvmAssert(traits()->getSizeOfInstance() == sizeof(MathClass));
        MathUtils::initRandom(&seed);

        // todo does ES4 Math have a prototype object?
    }

    //
    // ISSUE. ES3 15.8.2 says all these functions call ToNumber on its argument.
    // this is more forgiving than declaring the args to be Number.  We either
    // have to declare these Object and to ToNumber internally, or add some special
    // type support so compiler will have ToNumber.
    //
    // for E3 compatibility all args should be optional=NaN and rest args ignored,
    // except for min, max where rest args are processed.
    //

    double MathClass::abs(double x)
    {
        return MathUtils::abs(x);
    }

    double MathClass::acos(double x)
    {
        return MathUtils::acos(x);
    }

    double MathClass::asin(double x)
    {
        return MathUtils::asin(x);
    }

    double MathClass::atan(double x)
    {
        return MathUtils::atan(x);
    }

    double MathClass::atan2(double y, double x)
    {
        return MathUtils::atan2(y, x);
    }

    double MathClass::ceil(double x)
    {
        return MathUtils::ceil(x);
    }

    double MathClass::cos(double x)
    {
        return MathUtils::cos(x);
    }

    double MathClass::exp(double x)
    {
        return MathUtils::exp(x);
    }

    double MathClass::floor(double x)
    {
        return MathUtils::floor(x);
    }

    double MathClass::log(double x)
    {
        return MathUtils::log(x);
    }

    double MathClass::min(double x, double y, const Atom* argv, uint32_t argc)
    {
        return minImpl(x,y,argv,argc);
    }

    double MathClass::max(double x, double y, const Atom* argv, uint32_t argc)
    {
        return maxImpl(x,y,argv,argc);
    }
    
    
    double MathClass::pow(double x, double y)
    {
        return MathUtils::pow(x, y);
    }

    double MathClass::random()
    {
        return MathUtils::random(&seed);
    }

    double MathClass::round(double x)
    {
        return MathUtils::round(x);
    }

    double MathClass::sin(double x)
    {
        return MathUtils::sin(x);
    }

    double MathClass::sqrt(double x)
    {
        return MathUtils::sqrt(x);
    }

    double MathClass::tan(double x)
    {
        return MathUtils::tan(x);
    }

    // minImpl and maxImpl are shared between MathClass and NumberClass.
    /*static*/
    double MathClass::minImpl(double x, double y, const Atom* argv, uint32_t argc)
    {
        if (MathUtils::isNaN(x))
        {
            return x;
        }
        else if (MathUtils::isNaN(y))
        {
            return y;
        }
        else if (y < x)
        {
            x = y;
        }
        else
        {
            if (y == x)
                if (y == 0.0)
                    if (1.0/y < 0.0)
                        x = y;  // -0
        }
        for (uint32_t i=0; i < argc; i++)
        {
            y = AvmCore::number(argv[i]);
            if (MathUtils::isNaN(y)) return y;
            if (y < x)
            {
                x = y;
            }
            else if (y == x && y == 0.0)
            {
                /*
                 Lars: "You can tell -0 from 0 by dividing 1 by the zero, -0 gives -Infinity
                 and 0 gives Infinity, so if you know x is a zero the test for negative
                 zero is (1/x < 0)."
                 */
                if ((1.0 / y) < 0.0)
                    x = y;  // pick up negative zero when appropriate
            }
        }
        return x;
    }
    
    /*static*/
    double MathClass::maxImpl(double x, double y, const Atom* argv, uint32_t argc)
    {
        if (MathUtils::isNaN(x))
        {
            return x;
        }
        else if (MathUtils::isNaN(y))
        {
            return y;
        }
        else if (y > x)
        {
            x = y;
        }
        else
        {
            if (y == x)
                if (y == 0.0)
                    if (1.0/y > 0.0)
                        x = y;  // +0
        }
        for (uint32_t i=0; i < argc; i++)
        {
            y = AvmCore::number(argv[i]);
            if (MathUtils::isNaN(y)) return y;
            if (y > x)
            {
                x = y;
            }
            else if (y == x && y == 0)
            {
                /*
                 Lars: "You can tell -0 from 0 by dividing 1 by the zero, -0 gives -Infinity
                 and 0 gives Infinity, so if you know x is a zero the test for negative
                 zero is (1/x < 0)."
                 */
                if ((1.0 / y) > 0.0)
                    x = y;  // pick up negative zero when appropriate
            }
        }
        return x;
    }
    
    double MathClass::_min(double x, double y)
    {
        if (MathUtils::isNaN(x))
            return x;
        if (MathUtils::isNaN(y))
            return y;
        if (x < y)
            return x;
        if (x == y && x == 0.0 && 1.0/x < 0.0 && core()->currentBugCompatibility()->bugzilla551587 == 1)
            return x;  // -0
        return y;
    }

    double MathClass::_max(double x, double y)
    {
        if (MathUtils::isNaN(x))
            return x;
        if (MathUtils::isNaN(y))
            return y;
        if (x > y)
            return x;
        if (x == y && x == 0.0 && 1.0/x > 0.0 && core()->currentBugCompatibility()->bugzilla551587 == 1)
            return x; // +0
        return y;
    }

}

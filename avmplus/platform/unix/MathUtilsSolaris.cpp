/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <math.h>

#include "avmplus.h"

#ifndef PI
#define PI 3.141592653589793
#endif

namespace avmplus
{

    double MathUtils::acos(double value)
    {
        double result = ::acos(value);
        if(result == 0 && (value > 1 || value < -1)) {
            return MathUtils::nan();
        }
        return result;
    }

    double MathUtils::asin(double value)
    {
        double result = ::asin(value);
        if(result == 0 && (value > 1 || value < -1)) {
            return MathUtils::nan();
        }
        return result;
    }

    double MathUtils::atan2(double y, double x)
    {
        double result = ::atan2(y, x);
        if(result !=0 )
            return result;

        if (MathUtils::isNegZero(y) && MathUtils::isNegZero(x))
            return -PI;
        else if(y == 0 && MathUtils::isNegZero(x))
            return PI;
        else if(x == 0 && MathUtils::isNegZero(y))
            return y;
        else if(x == 0 && y == 0)
            return y;
        return result;
    }

    double MathUtils::log(double value)
    {
        if( value >= 0 )
            return ::log(value);
        else
            return MathUtils::nan();
    }

}

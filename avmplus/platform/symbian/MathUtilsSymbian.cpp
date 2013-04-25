/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <e32math.h>
#include <math.h> // no Symbian equivalent of frexp, ceil, floor.

#include "avmplus.h"

namespace avmplus
{
	double MathUtils::abs(double value)
	{
		return ::fabs(value);
	}
	
	double MathUtils::acos(double value)
	{
		double result;
		Math::ACos(result, value);
		return result;
	}
	
	double MathUtils::asin(double value)
	{
		double result;
		Math::ASin(result, value);
		return result;
	}
	
	double MathUtils::atan(double value)
	{
		double result;
		Math::ATan(result, value);
		return result;
	}

	double MathUtils::atan2(double y, double x)
	{
		bool xnan = (((int*)(&x))[1] & 0x7FF80000) == 0x7FF80000;
		bool ynan = (((int*)(&y))[1] & 0x7FF80000) == 0x7FF80000;
		
		if (xnan || ynan)
		{
			return nan();
		}

		int sx = (((int*)(&x))[1] & 0x80000000);
		int sy = (((int*)(&y))[1] & 0x80000000);
		
		if (y == 0.0)
		{
			if (sy) // y is -0
			{
				if (sx) // x is <0 or -0
				{
					return -KPi;
				}
				else // x is >0 or +0
				{
					return -0.0;
				}
			}
			else // y is +0
			{
				if (sx) // x is <0 or -0
				{
					return KPi;
				}
				else // x is >0 or +0
				{
					return +0.0;
				}
			}
		}
		double result;
		Math::ATan(result, y, x);
		return result;
	}
	
	double MathUtils::ceil(double value)
	{
		return ::ceil(value);
	}

	double MathUtils::cos(double value)
	{
		double result;
		Math::Cos(result, value);
		return result;
	}
	
	double MathUtils::exp(double value)
	{
		double result;
		Math::Exp(result, value);
		return result;
	}

	double MathUtils::floor(double value)
	{
		return ::floor(value);
	}

	uint64_t MathUtils::frexp(double value, int *eptr)
	{
		double fracMantissa = ::frexp(value, eptr);
		// correct mantissa and eptr to get integer values for both
		*eptr -= 53; // 52 mantissa bits + the hidden bit
		return (uint64_t)(fracMantissa * (double)(1LL << 53));
	}
	
	// The log function wants the natural log, not the base 10 log.
	double MathUtils::log(double value)
	{
		double result;
		Math::Ln(result, value);
		return result;
	}

	double fmod(double x, double y)
	{
		double result;
		Math::Mod(result, x, y);
		return result;
	}

	double MathUtils::mod(double x, double y)
	{
		double result;
		Math::Mod(result, x, y);
		return result;
	}

	double MathUtils::powInternal(double x, double y)
	{
		double result;
		Math::Pow(result, x, y);
		return result;
	}
	
	double MathUtils::sin(double value)
	{
		double result;
		Math::Sin(result, value);
		return result;
	}

	double MathUtils::sqrt(double value)
	{
		double result;
		Math::Sqrt(result, value);
		return result;
	}

	double MathUtils::tan(double value)
	{
		double result;
		Math::Tan(result, value);
		return result;
	}
}

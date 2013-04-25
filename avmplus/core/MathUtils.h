/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_MathUtils__
#define __avmplus_MathUtils__

namespace avmplus
{
#ifdef _MSC_VER
	#undef max
	#undef min
#endif

    /**
     * Data structure for state of fast random number generator.
     */
    struct TRandomFast
    {
        /* Random result and seed for next random result. */
        uint32_t  uValue;

        /* XOR mask for generating the next random value. */
        uint32_t  uXorMask;

        /* This is the number of values which will be generated in the
           /  sequence given the initial value of n. */
        uint32_t  uSequenceLength;
    };
    //
    // Random number generator
    //
    typedef struct TRandomFast *pTRandomFast;

    /**
     * The pure random number generator returns random numbers between zero
     * and this number.  This value is useful to know for scaling random
     * numbers to a desired range.
     */
    #define kRandomPureMax 0x7FFFFFFFL

    /**
     * The MathUtils class is a utility class which supports many
     * common mathematical operations, particularly those defined in
     * ECMAScript's Math class.
     */
    class MathUtils
    {
    public:
        static const double kNaN;
        static const double kInfinity;
        static const double kNegInfinity;
        static const float  kFltNaN;
        static const float  kFltInfinity;
        static const float  kFltNegInfinity;
    public:
        // The use of the "f" suffix on float functions is really unfortunate
        // but unavoidable, the JIT's method tables cannot deal with overloaded
        // method names.
        static double abs(double value);
        
        static float  absf(float value);
        static float  acosf(float value);
        static float  asinf(float value);
        static float  atanf(float value);
        static float  atan2f(float y, float x);
        static float  ceilf(float value);
        static float  cosf(float value);
        static bool   equalsf(float x, float y);
        static float  expf(float value);
        static float  floorf(float value);
        static int32_t isInfinitef(float value);
        static bool   isNaNf(float value);
        static bool   isNegZerof(float x);
        static float  logf(float value);
        static float  maxf(float x, float y);
        static float  minf(float x, float y);
        static float  modf(float x, float y);
        static float  recipf(float value);
        static float  roundf(float value);
        static float  rsqrtf(float value);
        static float  sinf(float value);
        static float  sqrtf(float value);
        static float  tanf(float value);
        static float  powf(float x, float y);
        static float  toIntf(float value);
        static float  as3_minf(float x, float y);
        static float  as3_maxf(float x, float y);

        static double acos(double value);
        static double asin(double value);
        static double atan(double value);
        static double atan2(double y, double x);
        static double ceil(double value);
        static double cos(double value);
        static bool   equals(double x, double y);
        static double exp(double value);
        static double floor(double value);
        static uint64_t  frexp(double x, int32_t *eptr);
        /// Return 1 if value is +Infinity, -1 if -Infinity, 0 otherwise.
        static int32_t isInfinite(double value);
        static bool   isNaNInline(double value); // only for time critical routines
        static bool   isNaN(double value);
        static bool   isNegZero(double x);
        static double log(double value);
        static double max(double x, double y);
        static double min(double x, double y);
        static double mod(double x, double y);
        static double pow(double x, double y);
        static double powInternal(double x, double y);
        static void   initRandom(TRandomFast *seed);
        static double random(TRandomFast *seed);
        static double round(double value);
        static double sin(double value);
        static double sqrt(double value);
        static double tan(double value);
        static double toInt(double value);

        static double parseInt(Stringp s, int32_t radix=10, bool strict=true);
        static int32_t nextPowerOfTwo(int32_t n);

        static double nan();
        static double infinity();
        static double neg_infinity();

        /**
         * Enumeration values for the minimum buffer size required to convert
         * a number to a string.
         */
        enum
        {
            /**
             * The buffer for a 32-bit integer, base 10, needs to be >= 12:
             * -2147483647<NUL> or 4294967294
             */
            kMinSizeForInt32_t_base10_toString  = 12,
            /**
             * The buffer for a 64-bit integer needs to be >= 65:
             * sign, up to 63 (64 w/o sign) bits if base =-=2, NUL
             */
            kMinSizeForInt64_t_toString         = 65,
            /**
             * For security reason, the buffer must be well over 347 characters:
             * Bug 192033: Number.MAX_VALUE is 1.79e+308, so this was 312.
             * Bug 230183: That wasn't big enough.  Number.MIN_VALUE.toPrecision(21)
             * needs 347.  But why be stingy?  There may be other cases that are
             * even bigger, it's hard to say.
             */
             kMinSizeForDouble_base10_toString  = 380,
             /**
              *  Buffer size for converting IEEE-754 double to string
              *  using worst-case radix(2) plus a leading '-' char.
              *  Denormalized (i.e., very small) numbers are  truncated
              *  to "0" so they don't require additional space.
              */
              kMinSizeForDouble_base2_toString  = 1025
        };

        enum UnsignedTreatment
        {
            kTreatAsSigned = 0,
            kTreatAsUnsigned = 1
        };

        /**
         * Convert an integer to a string. Since the conversion happens from
         * right to left, the string is right-aligned in the buffer, and the
         * returned pointer points somewhere into the buffer. It is NULL if
         * the base is out of range (must be between 2 and 36), or the buffer
         * is too small (the debug versions throw an assert if the buffer
         * does not fit). The buffer is NUL-terminated.
         * @param value            the value to convert
         * @param buffer           the buffer to fill
         * @param len              the buffer size; takes the number of characters filled in
         * @param radix            between 2 and 36
         * @param treatAsUnsigned  true for an unsigned conversion
         * @return                 a pointer into the buffer or NULL on errors
         */
        static char* convertIntegerToStringBuffer(intptr_t value,
                                            char *buffer,
                                            int32_t& len,
                                            int32_t radix,
                                            UnsignedTreatment treatAs);
        /**
         * Convert a 32/64-bit integer to a String instance. This method uses an internal
         * buffer of 65 characters (up to 63 bits+sign, or 64 bits, NUL) on the stack.
         * @param core              The AvmCore instance to create strings with
         * @param value             the value to convert (32 or 64 bits depending on platform)
         * @param radix             the conversion radix (2...36)
         * @param treatAsUnsigned   true for an unsigned conversion
         * @return                  the String instance
         */
        static Stringp convertIntegerToStringRadix(AvmCore* core,
                                              intptr_t value,
                                              int32_t  radix,
                                              UnsignedTreatment treatAs);
        /**
         * Convert a 32-bit integer to a String instance with a radix of 10. This
         * method uses a much smaller internal buffer of 12 characters (up to
         * 9 digits+sign or 10 digits, NUL) on the stack.
         * @param core              The AvmCore instance to create strings with
         * @param value             the value to convert (32 bits)
         * @param treatAsUnsigned   true for an unsigned conversion
         * @return                  the String instance
         */
        static Stringp convertIntegerToStringBase10(AvmCore* core,
                                              int32_t  value,
                                              UnsignedTreatment treatAs);

        /**
         * Convert a double to an integer using the given radix.
         * @param core              The AvmCore instance to create strings with
         * @param value             the value to convert
         * @param treatAsUnsigned   true for an unsigned conversion
         * @return                  the String instance
         */
        static Stringp convertDoubleToStringRadix(AvmCore *core,
                                                  double value,
                                                  int32_t radix);
        enum {
            DTOSTR_NORMAL,
            DTOSTR_FIXED,
            DTOSTR_PRECISION,
            DTOSTR_EXPONENTIAL
        };
        static Stringp convertDoubleToString(AvmCore* core,
                                             double value,
                                             int32_t mode = DTOSTR_NORMAL,
                                             int32_t precision = 15);
        static bool convertStringToDouble(Stringp inStr,
                                          double *value,
                                          bool strict=false);
        static double convertStringToNumber(Stringp inStr);
        static int32_t nextDigit(double *value);

        static int32_t doubleToBool(double d) {
            // ecma3/Boolean/e15_6_1.abc
            return d == d && d != 0;
        }
    private:
        static double powerOfTen(int32_t exponent, double value);
        static int32_t roundInt(double x);

    public:
        static void RandomFastInit(pTRandomFast pRandomFast);
        static int32_t RandomPureHasher(int32_t iSeed);
        static int32_t GenerateRandomNumber(pTRandomFast pRandomFast);
        static int32_t Random(int32_t range, pTRandomFast pRandomFast);

    };
}

#if AVMSYSTEM_WIN32
    #include "win32/MathUtilsWin-inlines.h"
#elif AVMSYSTEM_UNIX
    #ifdef SOLARIS
        #include "unix/MathUtilsSolaris-inlines.h"
    #elif AVMSYSTEM_ARM
        #include "arm/MathUtilsUnix-inlines.h"
    #else
        #include "unix/MathUtilsUnix-inlines.h"
    #endif
#elif AVMSYSTEM_MAC
    #include "unix/MathUtilsUnix-inlines.h"
#elif AVMSYSTEM_SYMBIAN
    // No inlines presently defined for the Symbian platform.
#endif
#endif /* __avmplus_MathUtils__ */

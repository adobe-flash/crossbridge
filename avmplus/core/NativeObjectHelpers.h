/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_NativeObjectHelpers__
#define __avmplus_NativeObjectHelpers__

namespace avmplus
{
    namespace NativeObjectHelpers
    {
        // CN:  ES3 compliance requires uint32_t array and string indicies.  The arguments to several
        //  array methods are integral values held in a double (to allow for negative offsets from
        //  the array length).  To deal with this, ClampIndex replaces ClampA
        // i.e. instead of doing this:

        //  int start = AvmCore::integer(argv[0]);
        //  int end   = AvmCore::integer(argv[1]]);
        //  ClampA(start,end,a->getLength());
        //
        //  do this:
        //  uint32_t start = ClampArrayIndex( AvmCore::toInteger(argv[0]), a->getLength() );
        //  uint32_t end   = ClampArrayIndex( AvmCore::toInteger(argv[1]), a->getLength() );
        //  if (end < start)
        //    end == start;
        inline uint32_t ClampIndex(double intValue, uint32_t length)
        {
            uint32_t result;
            if (intValue < 0.0)
            {
                if (intValue + length < 0.0)
                    result = 0;
                else
                    result = (uint32_t)(intValue + length);
            }
            else if (intValue > length)
                result = length;
            else if (intValue != intValue) // lookout for NaN.  It converts to a 0 int value on Win, but as 0xffffffff on Mac
                result = 0;
            else
                result = (uint32_t)intValue;

            return result;
        }

        inline uint32_t ClampIndexInt(int intValue, uint32_t length)
        {
            uint32_t result;
            if (intValue < 0)
            {
                if (intValue + int(length) < 0)
                    result = 0;
                else
                    result = (uint32_t)(intValue + length);
            }
            else if (intValue > int(length))
                result = length;
            else
                result = (uint32_t)intValue;

            return result;
        }


        inline void ClampB(double& start, double& end, int32_t length)
        {
            AvmAssert(length >= 0);

            if (end < 0)
                end = 0;

            if (end >= length)
                end = length;

            if (start < 0)
                start = 0;

            if (start >= length)
                start = length;

            if (start > end)
            {
                double swap = start;
                start = end;
                end = swap;
            }

            // Have the indexes been successfully normalized?
            // Postconditions:
            AvmAssert(start >= 0 && start <= length);
            AvmAssert(end >= 0 && end <= length);
            AvmAssert(start <= end);
        }

        inline void ClampBInt(int& start, int& end, int32_t length)
        {
            AvmAssert(length >= 0);

            if (end < 0)
                end = 0;

            if (end >= length)
                end = length;

            if (start < 0)
                start = 0;

            if (start >= length)
                start = length;

            if (start > end)
            {
                int swap = start;
                start = end;
                end = swap;
            }

            // Have the indexes been successfully normalized?
            // Postconditions:
            AvmAssert(start >= 0 && start <= length);
            AvmAssert(end >= 0 && end <= length);
            AvmAssert(start <= end);
        }

        inline void copyNarrowToWide(const char *src, wchar *dst)
        {
            while (*src) {
                *dst++ = *src++;
            }
            *dst = 0;
        }
    }
}

#endif /* __avmplus_NativeObjectHelpers__ */

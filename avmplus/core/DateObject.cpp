/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{

    Stringp DateObject::_toString(int index)
    {
        wchar buffer[256];
        int len;
        date.toString(buffer, index, len);
        return core()->newStringUTF16(buffer, len);
    }

    double DateObject::AS3_valueOf()
    {
        return date.getTime();
    }

    double DateObject::_setTime(double value)
    {
        date.setTime(value);
        return date.getTime();
    }

    double DateObject::_get(int index)
    {
        return date.getDateProperty(index);
    }

    double DateObject::_set(int index, Atom *argv, int argc)
    {
        double num[7];
        int i;

        for (i=0; i<7; i++) {
            num[i] = MathUtils::kNaN;
        }
        bool utcFlag = (index < 0);
        index = (int)MathUtils::abs(index);
        int j = index-1;

        for (i=0; i<argc; i++) {
            if (j >= 7) {
                break;
            }
            num[j++] = AvmCore::number(argv[i]);
            if (MathUtils::isNaN(num[j-1])) // actually specifying NaN results in a NaN date. Don't pass Nan, however, because we use
            {                               //  that value to denote that an optional arg was not supplied.
                date.setTime(MathUtils::kNaN);
                return date.getTime();
            }
        }

        const int minTimeSetterIndex = 4; // any setNames index >= 4 should call setTime() instead of setDate()
                                          //  setFullYear/setUTCFullYear/setMonth/setUTCMonth/setDay/setUTCDay are all in indices < 3
        if (index < minTimeSetterIndex)
        {
            date.setDate(num[0],
                            num[1],
                            num[2],
                            utcFlag);
        }
        else
        {
            date.setTime(num[3],
                            num[4],
                            num[5],
                            num[6],
                            utcFlag);
        }
        return date.getTime();
    }

#ifdef AVMPLUS_VERBOSE
    PrintWriter& DateObject::print(PrintWriter& prw) const
    {
        wchar buffer[256];
        int len;
        date.toString(buffer, Date::kToString, len);
        return prw << "<" << asUTF16(buffer, len) << ">@" << asAtomHex(atom());
    }
#endif
}

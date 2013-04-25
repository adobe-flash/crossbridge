/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    DateClass::DateClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        ScriptObject* object_prototype = toplevel()->objectClass->prototypePtr();
        setPrototypePtr(DateObject::create(core()->GetGC(), ivtable(), object_prototype));
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom DateClass::construct(int argc, Atom* argv)
    {
        AvmCore* core = this->core();
        if (argc == 1) {
            double dateAsDouble = ( core->isString(argv[1]) ?
                                    stringToDateDouble(core->string(argv[1])) :
                                    AvmCore::number(argv[1]) );

            Date   date(dateAsDouble);
            return DateObject::create(core->GetGC(), this, date)->atom();
        }
        else if (argc != 0) {
            double num[7] = { 0, 0, 1, 0, 0, 0, 0 }; // defaults
            int i;
            if (argc > 7)
                argc = 7;
            for (i=0; i<argc; i++) {
                num[i] = AvmCore::number(argv[i+1]);
            }
            Date date(num[0],
                      num[1],
                      num[2],
                      num[3],
                      num[4],
                      num[5],
                      num[6],
                      false);

            return DateObject::create(core->GetGC(), this, date)->atom();
        } else {
            Date date;
            return DateObject::create(core->GetGC(), this, date)->atom();
        }
    }

    // this = argv[0]
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom DateClass::call(int /*argc*/, Atom* /*argv*/)
    {
        // Date called as a top-level function.  Return the date
        // as a string.

        // ISSUE is it correct to ignore args here?

        Date date;
        wchar buffer[256];
        int len;
        date.toString(buffer, Date::kToString, len);
        return core()->newStringUTF16(buffer, len)->atom();
    }

    // static function UTC(year, month, date, hours, minutes, seconds, ms, ... rest)
    double DateClass::UTC(Atom year, Atom month, Atom date,
                        Atom hours, Atom minutes, Atom seconds, Atom ms,
                        Atom* /*argv*/, int /*argc*/) // rest
    {
        Date d(AvmCore::number(year),
                  AvmCore::number(month),
                  AvmCore::number(date),
                  AvmCore::number(hours),
                  AvmCore::number(minutes),
                  AvmCore::number(seconds),
                  AvmCore::number(ms),
                  true);
        return d.getTime();
    }

    double DateClass::parse(Atom input)
    {
        AvmCore* core = this->core();
        Stringp s = core->string(input);
        return stringToDateDouble(s);
    }


    /*
     * DateClass::parse() / stringToDateDouble()  support code
     *
     */


    // Identify keyword within substring of <s> and modify hour, month,
    //  or timeZoneOffset appropriately.  Return false if keyword is
    //  invalid.   Assumes that <offset> is the start of a word in <s> and
    //  <offset> + <count> marks the end of that word.
    inline bool parseDateKeyword(const StringIndexer& s, int offset, int count, int& hour,
                                 int& month, double& timeZoneOffset)
    {
        static const char kDayAndMonthKeywords[] = "JanFebMarAprMayJunJulAugSepOctNovDecSunMonTueWedThuFriSatGMTUTC";
        static const int  kKeyWordLength = 3;
        static const int  kUtcKewordPos = 12+7+1;
        bool              validKeyWord = false;

        if (count > kKeyWordLength)
            return false;

        // string case must match.  Case insensitivity is not necessary for compliance.

        char subString[kKeyWordLength+1];
        StringIndexer str_idx(s);
        for (int i = 0; i < count; i++)
        {
            uint32_t ch = str_idx[offset + i];
            // must be alphabetic
            if (ch < 'A' || ch > 'z' || (ch > 'Z' && ch < 'a'))
                return false;
            subString[i] = (char) ch;
        }
        if (count == 3)
        {
            for(int x=0; x < 7+12+2; x++)
            {
                const char *key = kDayAndMonthKeywords+(kKeyWordLength*x);
                if ( (key[0] == subString[0]) && (key[1] == subString[1]) && (key[2] == subString[2]) )
                {
                    validKeyWord = true;
                    if (x < 12)  // its a month
                        month = x;
                    else if (x == kUtcKewordPos) // UTC
                        timeZoneOffset = 0;
                    // else its a day or 'GMT'.  Ignore it: GMT is always followed by + or -, and we identify
                    //  it from there.  day must always be specified numerically, name of day is optional.

                    break;
                }
            }
        }
        else if (count == 2)
        {
            if (subString[0] == 'A' && subString[1] == 'M')
            {
                validKeyWord = (hour <= 12 && hour >=0);
                if (hour == 12)
                    hour = 0;
            }
            else if (subString[0] == 'P' && subString[1] == 'M')
            {
                validKeyWord = (hour <= 12 && hour >=0);
                if (hour != 12)
                    hour += 12;
            }
        }

        return validKeyWord;
    }

    // Parses a number out of the string and updates year/month/day/hour/min/timeZoneOffset as
    //  appropriate (based on whatever has already been parsed).  Assumes that s[i] is an integer
    //  character.  (broken out into a seperate function from stringToDateDouble() for readability)
    static inline bool parseDateNumber(const StringIndexer& s, int sLength, int &i, wchar &c, wchar prevc,
                                       double &timeZoneOffset, int &year, int &month, int &day,
                                       int &hour, int &min, int &sec)
    {
        bool numberIsValid = true;

        // first get number value
        int numVal = c - ((wchar)'0');
        while (i < sLength && (c=s[i]) >= ((wchar)'0') && c <= ((wchar)'9'))
        {
            numVal = numVal * 10 + c - ((wchar)'0');
            i++;
        }

        // Supported examples:  "Mon Jan 1 00:00:00 GMT-0800 1900",  '1/1/1999 13:30 PM'
        //                      "Mon Jan 1 00:00:00 UTC-0800 1900"

        // Check for timezone numeric info, which is the only place + or - can occur.
        //  ala:  'Sun Sept 12 11:11:11 GMT-0900 2004'
        if ((prevc == ((wchar)'+') || prevc == ((wchar)'-')))
        {
            if (numVal < 24)
                numVal = numVal * 60; //GMT-9
            else
                numVal = numVal % 100 + numVal / 100 * 60; // GMT-0900

            if (prevc == ((wchar)'+')) // plus is east of GMT
                numVal = 0-numVal;

            if (timeZoneOffset == 0 || timeZoneOffset == -1)
                timeZoneOffset = numVal;
            else
                numberIsValid = false;
            // note:  do not support ':' in timzone value ala GMT-9:00
        }

        // else check for year value
        else if (numVal >= 70  ||
                 (prevc == ((wchar)'/') && month >= 0 && day >= 0 && year < 0))
        {
            if (year >= 0)
                numberIsValid = false;
            else if (c <= ((wchar)' ') || c == ((wchar)',') || c == ((wchar)'/') || i >= sLength)
                year = numVal < 100 ? numVal + 1900 : numVal;
            else
                numberIsValid = false;
        }

        // else check for month or day
        else if (c == ((wchar)'/'))
        {
            if (month < 0)
                month = numVal-1;
            else if (day < 0)
                day = numVal;
            else
                numberIsValid = false;
        }

        // else check for time unit
        else if (c == ((wchar)':'))
        {
            if (hour < 0)
                hour = numVal;
            else if (min < 0)
                min = numVal;
            else
                numberIsValid = false;
        }

        // ensure next char is valid before allowing the final cases
        else if (i < sLength && c != ((wchar)',') && c > ((wchar)' ') && c != ((wchar)'-'))
        {
            numberIsValid = false;
        }

        // check for end of time hh:mm:sec
        else if (hour >= 0 && min < 0)
        {
            min = numVal;
        }
        else if (min >= 0 && sec < 0)
        {
            sec = numVal;
        }

        // check for end of mm/dd/yy
        else if (day < 0)
        {
            day = numVal;
        }
        else
        {
            numberIsValid = false;
        }
        return numberIsValid;
    }

    // used by both Date::parse() and the Date(String) constructor
    double DateClass::stringToDateDouble(Stringp _s)
    {
        StringIndexer s(_s);

        int year = -1;
        int month = -1;
        int day = -1;
        int hour = -1;
        int min = -1;
        int sec = -1;
        double timeZoneOffset = -1;

        //  Note:  compliance with ECMAScript 3 only requires that we can parse what our toString()
        //  method produces.  We do support some other simple formats just to pass Spidermonkey test
        //  suite, however (for instance "1/1/1999 13:30 PM"), but we don't handle timezone keywords
        //  or instance.

        int sLength = s->length();
        wchar c, prevc = 0;
        int i = 0;
        while (i < sLength)
        {
            c = s[i];
            i++;

            // skip whitespace and delimiters (and possibly garbage chars)
            if (c <= ((wchar)' ') || c == ((wchar)',') || c == ((wchar)'-')) {
                if (i < sLength) {
                    wchar nextc = s[i];
                    // if number follows '-' save c in prevc for use in parseDateNumber for detecting GMT offset
                    if ( c == ((wchar)'-') && ((wchar)'0') <= nextc && nextc <= ((wchar)'9') ) {
                        prevc = c;
                    }
                }
            }

            // remember date and time seperators and/or numeric +- modifiers.
            else if (c == ((wchar)'/') || c == ((wchar)':') || c == ((wchar)'+') || c == ((wchar)'-'))
            {
                prevc = c;
            }

            // parse numeric value.
            else if (((wchar)'0') <= c && c <= ((wchar)'9'))
            {
                if (parseDateNumber(s,sLength,i,c,prevc,timeZoneOffset,year,month,day,hour,
                                    min,sec) == false)
                    return MathUtils::kNaN;
                prevc = 0;
            }

            // parse keyword
            else
            {
                // walk forward to end of word
                int st = i - 1;
                while (i < sLength) {
                    c = s[i];
                    if (!(( ((wchar)'A') <= c && c <= ((wchar)'Z') ) || ( ((wchar)'a') <= c && c <= ((wchar)'z') )))
                        break;
                    i++;
                }
                if (i <= st + 1)
                    return MathUtils::kNaN;

                // check keyword substring against known keywords, modify hour/month/timeZoneOffset as appropriate
                if (parseDateKeyword(s, st, i-st, hour, month, timeZoneOffset) == false)
                    return MathUtils::kNaN;
                prevc = 0;
            }
        }
        if (year < 0 || month < 0 || day < 0)
            return MathUtils::kNaN;
        if (sec < 0)
            sec = 0;
        if (min < 0)
            min = 0;
        if (hour < 0)
            hour = 0;
        if (timeZoneOffset == -1) { /* no time zone specified, have to use local */
            Date date(year,
                      month,
                      day,
                      hour,
                      min,
                      sec,
                      0,
                      false);

            return date.getTime();
        }
        else
        {
            Date date(year,
                      month,
                      day,
                      hour,
                      min,
                      sec,
                      0,
                      true);
            return date.getTime() + (timeZoneOffset * 60000); // millesecondsPerMinute = 60000
        }
    }



}

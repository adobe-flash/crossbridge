/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Date__
#define __avmplus_Date__

namespace avmplus
{
    int YearFromTime(double t);

    /**
     * Date is used to provide the underpinnings for the Date class.
     * It is a layer over OS-specific date/time functionality.
     */
    class Date
    {
    public:
        enum {
            kUTCFullYear,
            kUTCMonth,
            kUTCDate,
            kUTCDay,
            kUTCHours,
            kUTCMinutes,
            kUTCSeconds,
            kUTCMilliseconds,
            kFullYear,
            kMonth,
            kDate,
            kDay,
            kHours,
            kMinutes,
            kSeconds,
            kMilliseconds,
            kTimezoneOffset,
            kTime
        };

        enum {
            kToString,
            kToDateString,
            kToTimeString,
            kToLocaleString,
            kToLocaleDateString,
            kToLocaleTimeString,
            kToUTCString
        };

#define kHalfTimeDomain   8.64e15

        static inline double TimeClip(double t)
        {
            if (MathUtils::isInfinite(t) || MathUtils::isNaN(t) || ((t < 0 ? -t : t) > kHalfTimeDomain)) {
                return MathUtils::kNaN;
            }
            return MathUtils::toInt(t) + (+0.);
        }


        Date();
        Date(const Date& toCopy) {
            m_time = toCopy.m_time;
        }
        Date& operator= (const Date& toCopy) {
            m_time = toCopy.m_time;
            return *this;
        }
        Date(double time) { m_time = TimeClip(time); }
        Date(double year,
             double month,
             double date,
             double hours,
             double min,
             double sec,
             double msec,
             bool utcFlag);
        ~Date() { m_time = 0; }
        double getDateProperty(int index);
        double getTime() const { return m_time; }
        void setDate(double year,
                     double month,
                     double date,
                     bool utcFlag);
        void setTime(double hours,
                     double min,
                     double sec,
                     double msec,
                     bool utcFlag);
        void setTime(double value);
        bool toString(wchar *buffer, int formatIndex, int &len) const;

    private:
        double m_time;
        void format(wchar *buffer,
                    const char *format, ...) const;
    };
    double GetTimezoneOffset(double t);
}

#endif /* __avmplus_Date__ */

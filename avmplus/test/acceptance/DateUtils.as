/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.adobe.test
{
    /*
     *  Date utilities for as4 tests - mostly ported from shell.as
     *
     */
    public class DateUtils
    {
        //  Originally, the test suite used a hard-coded value TZ_DIFF = -8.
        //  But that was only valid for testers in the Pacific Standard Time Zone!
        //  We calculate the proper number dynamically for any tester. We just
        //  have to be careful not to use a date subject to Daylight Savings Time...
        public static function getTimeZoneDiff()
        {
          return -((new Date(2000, 1, 1)).getTimezoneOffset())/60;
        }
        
        public static const msPerDay =          86400000;
        public static const HoursPerDay =       24;
        public static const MinutesPerHour =    60;
        public static const SecondsPerMinute =  60;
        public static const msPerSecond =       1000;
        public static const msPerMinute =       60000;      //  msPerSecond * SecondsPerMinute
        public static const msPerHour =         3600000;    //  msPerMinute * MinutesPerHour
        public static const TZ_DIFF = getTimeZoneDiff();  // offset of tester's timezone from UTC
        public static const TZ_PST = -8;  // offset of Pacific Standard Time from UTC
        public static const TZ_IST = +5.5; // offset of Indian Standard Time from UTC
        public static const IST_DIFF = TZ_DIFF - TZ_IST;  // offset of tester's timezone from IST
        public static const PST_DIFF = TZ_DIFF - TZ_PST;  // offset of tester's timezone from PST
        public static const TIME_1970    = 0;
        public static const TIME_2000    = 946684800000;
        public static const TIME_1900    = -2208988800000;
        public static const TZ_ADJUST = TZ_DIFF * msPerHour;
        public static const UTC_29_FEB_2000 = TIME_2000 + 31*msPerDay + 28*msPerDay;
        public static const UTC_1_JAN_2005 = TIME_2000 + TimeInYear(2000) + TimeInYear(2001) +
                             TimeInYear(2002) + TimeInYear(2003) + TimeInYear(2004);
        
        public static const now = new Date();
        public static const TIME_NOW = now.valueOf();
        
        // TODO: Port this from shell.as - figure out if really needed
        // function adjustResultArray(ResultArray, msMode)
        
        public static function Day( t ) {
            return ( Math.floor(t/msPerDay ) );
        }
        
        public static function DaysInYear( y ) {
            if ( y % 4 != 0 ) {
                return 365;
            }
            if ( (y % 4 == 0) && (y % 100 != 0) ) {
                return 366;
            }
            if ( (y % 100 == 0) &&  (y % 400 != 0) ) {
                return 365;
            }
            if ( (y % 400 == 0) ){
                return 366;
            } else {
                _print("ERROR: DaysInYear(" + y + ") case not covered");
                return Number.NaN; //"ERROR: DaysInYear(" + y + ") case not covered";
            }
        }
        public static function TimeInYear( y ) {
            return ( DaysInYear(y) * msPerDay );
        }
        public static function DayNumber( t ) {
            return ( Math.floor( t / msPerDay ) );
        }
        public static function TimeWithinDay( t ) {
            if ( t < 0 ) {
                return ( (t % msPerDay) + msPerDay );
            } else {
                return ( t % msPerDay );
            }
        }
        public static function YearNumber( t ) {
        }
        public static function TimeFromYear( y ) {
            return ( msPerDay * DayFromYear(y) );
        }
        public static function DayFromYear( y ) {
            return (    365*(y-1970) +
                        Math.floor((y-1969)/4) -
                        Math.floor((y-1901)/100) +
                        Math.floor((y-1601)/400) );
        }
        public static function InLeapYear( t ) {
            if ( DaysInYear(YearFromTime(t)) == 365 ) {
                return 0;
            }
            if ( DaysInYear(YearFromTime(t)) == 366 ) {
                return 1;
            } else {
                return "ERROR:  InLeapYear("+ t + ") case not covered";
            }
        }
        public static function YearFromTime( t ) {
            t = Number( t );
            var sign = ( t < 0 ) ? -1 : 1;
            var year = ( sign < 0 ) ? 1969 : 1970;
        
            for (   var timeToTimeZero = t; ;  ) {
            //  subtract the current year's time from the time that's left.
                timeToTimeZero -= sign * TimeInYear(year)
                if (isNaN(timeToTimeZero))
                    return NaN;
        
            //  if there's less than the current year's worth of time left, then break.
                if ( sign < 0 ) {
                    if ( sign * timeToTimeZero <= 0 ) {
                        break;
                    } else {
                        year += sign;
                    }
                } else {
                    if ( sign * timeToTimeZero < 0 ) {
                        break;
                    } else {
                        year += sign;
                    }
                }
            }
            return ( year );
        }
        public static function MonthFromTime( t ) {
            //  i know i could use switch but i'd rather not until it's part of ECMA
            var day = DayWithinYear( t );
            var leap = InLeapYear(t);
        
            if ( (0 <= day) && (day < 31) ) {
                return 0;
            }
            if ( (31 <= day) && (day < (59+leap)) ) {
                return 1;
            }
            if ( ((59+leap) <= day) && (day < (90+leap)) ) {
                return 2;
            }
            if ( ((90+leap) <= day) && (day < (120+leap)) ) {
                return 3;
            }
            if ( ((120+leap) <= day) && (day < (151+leap)) ) {
                return 4;
            }
            if ( ((151+leap) <= day) && (day < (181+leap)) ) {
                return 5;
            }
            if ( ((181+leap) <= day) && (day < (212+leap)) ) {
                return 6;
            }
            if ( ((212+leap) <= day) && (day < (243+leap)) ) {
                return 7;
            }
            if ( ((243+leap) <= day) && (day < (273+leap)) ) {
                return 8;
            }
            if ( ((273+leap) <= day) && (day < (304+leap)) ) {
                return 9;
            }
            if ( ((304+leap) <= day) && (day < (334+leap)) ) {
                return 10;
            }
            if ( ((334+leap) <= day) && (day < (365+leap)) ) {
                return 11;
            } else {
                return "ERROR:  MonthFromTime("+t+") not known";
            }
        }
        public static function DayWithinYear( t ) {
                return( Day(t) - DayFromYear(YearFromTime(t)));
        }
        public static function DateFromTime( t ) {
            var day = DayWithinYear(t);
            var month = MonthFromTime(t);
            if ( month == 0 ) {
                return ( day + 1 );
            }
            if ( month == 1 ) {
                return ( day - 30 );
            }
            if ( month == 2 ) {
                return ( day - 58 - InLeapYear(t) );
            }
            if ( month == 3 ) {
                return ( day - 89 - InLeapYear(t));
            }
            if ( month == 4 ) {
                return ( day - 119 - InLeapYear(t));
            }
            if ( month == 5 ) {
                return ( day - 150- InLeapYear(t));
            }
            if ( month == 6 ) {
                return ( day - 180- InLeapYear(t));
            }
            if ( month == 7 ) {
                return ( day - 211- InLeapYear(t));
            }
            if ( month == 8 ) {
                return ( day - 242- InLeapYear(t));
            }
            if ( month == 9 ) {
                return ( day - 272- InLeapYear(t));
            }
            if ( month == 10 ) {
                return ( day - 303- InLeapYear(t));
            }
            if ( month == 11 ) {
                return ( day - 333- InLeapYear(t));
            }
        
            return ("ERROR:  DateFromTime("+t+") not known" );
        }
        public static function WeekDay( t ) {
            var weekday = (Day(t)+4) % 7;
            return( weekday < 0 ? 7 + weekday : weekday );
        }
        
        // missing daylight savins time adjustment
        
        public static function HourFromTime( t ) {
            var h = Math.floor( t / msPerHour ) % HoursPerDay;
            return ( (h<0) ? HoursPerDay + h : h  );
        }
        public static function MinFromTime( t ) {
            var min = Math.floor( t / msPerMinute ) % MinutesPerHour;
            return( ( min < 0 ) ? MinutesPerHour + min : min  );
        }
        public static function SecFromTime( t ) {
            var sec = Math.floor( t / msPerSecond ) % SecondsPerMinute;
            return ( (sec < 0 ) ? SecondsPerMinute + sec : sec );
        }
        public static function msFromTime( t ) {
            var ms = t % msPerSecond;
            return ( (ms < 0 ) ? msPerSecond + ms : ms );
        }
        public static function LocalTZA() {
            return ( TZ_DIFF * msPerHour );
        }
        public static function UTC( t ) {
            return ( t - LocalTZA() - DaylightSavingTA(t - LocalTZA()) );
        }
        
        public static function DaylightSavingTA( t ) {
            // There is no Daylight saving time in India
            if (IST_DIFF == 0)
                return 0;
        
            var dst_start;
            var dst_end;
            // Windows fix for 2007 DST change made all previous years follow new DST rules
            // create a date pre-2007 when DST is enabled according to 2007 rules
            var pre_2007:Date = new Date("Mar 20 2006");
            // create a date post-2007
            var post_2007:Date = new Date("Mar 20 2008");
            // if the two dates timezoneoffset match, then this must be a windows box applying
            // post-2007 DST rules to earlier dates.
            var win_machine:Boolean = pre_2007.timezoneOffset == post_2007.timezoneOffset
        
            if (TZ_DIFF<=-4 && TZ_DIFF>=-8) {
                if (win_machine || YearFromTime(t)>=2007) {
                    dst_start = GetSecondSundayInMarch(t) + 2*msPerHour;
                    dst_end = GetFirstSundayInNovember(t) + 2*msPerHour;
                } else {
                    dst_start = GetFirstSundayInApril(t) + 2*msPerHour;
                    dst_end = GetLastSundayInOctober(t) + 2*msPerHour;
                }
            } else {
                dst_start = GetLastSundayInMarch(t) + 2*msPerHour;
                dst_end = GetLastSundayInOctober(t) + 2*msPerHour;
            }
            if ( t >= dst_start && t < dst_end ) {
                return msPerHour;
            } else {
                return 0;
                        
            }
        
            // Daylight Savings Time starts on the second Sunday    in March at 2:00AM in
            // PST.  Other time zones will need to override this function.
            _print( new Date( UTC(dst_start + LocalTZA())) );
        
            return UTC(dst_start  + LocalTZA());
        }
        public static function GetLastSundayInMarch(t) {
            var year = YearFromTime(t);
            var leap = InLeapYear(t);
            var march = TimeFromYear(year) + TimeInMonth(0,leap) +  TimeInMonth(1,leap)-LocalTZA()+2*msPerHour;
            var sunday;
            for( sunday=march;WeekDay(sunday)>0;sunday +=msPerDay ){;}
            var last_sunday;
            while (true) {
               sunday=sunday+7*msPerDay;
               if (MonthFromTime(sunday)>2)
                   break;
               last_sunday=sunday;
            }
            return last_sunday;
        }
        public static function GetSecondSundayInMarch(t ) {
            var year = YearFromTime(t);
            var leap = InLeapYear(t);
            var march = TimeFromYear(year) + TimeInMonth(0,leap) +  TimeInMonth(1,leap)-LocalTZA()+2*msPerHour;
            var first_sunday;
            for ( first_sunday = march; WeekDay(first_sunday) >0;
                first_sunday +=msPerDay )
            {
                ;
            }
            var second_sunday=first_sunday+7*msPerDay;
            return second_sunday;
        }
        
        public static function GetFirstSundayInNovember( t ) {
            var year = YearFromTime(t);
            var leap = InLeapYear(t);
                var     nov,m;
            for ( nov = TimeFromYear(year), m = 0; m < 10; m++ )    {
                nov += TimeInMonth(m, leap);
            }
            nov=nov-LocalTZA()+2*msPerHour;
            for ( var first_sunday =    nov; WeekDay(first_sunday)  > 0;
                first_sunday    += msPerDay )
            {
                ;
            }
            return first_sunday;
        }
        
        public static function GetFirstSundayInApril( t ) {
            var year = YearFromTime(t);
            var leap = InLeapYear(t);
            var     apr,m;
            for ( apr = TimeFromYear(year), m = 0; m < 3; m++ ) {
                apr += TimeInMonth(m, leap);
            }
            apr=apr-LocalTZA()+2*msPerHour;
        
            for ( var first_sunday =    apr; WeekDay(first_sunday)  > 0;
                first_sunday    += msPerDay )
            {
                ;
            }
            return first_sunday;
        }
        
        public static function GetLastSundayInOctober(t) {
            var year = YearFromTime(t);
            var leap = InLeapYear(t);
            var oct,m;
            for (oct =  TimeFromYear(year), m = 0; m < 9; m++ ) {
                oct += TimeInMonth(m, leap);
            }
            oct=oct-LocalTZA()+2*msPerHour;
            var sunday;
            for( sunday=oct;WeekDay(sunday)>0;sunday +=msPerDay ){;}
            var last_sunday;
            while (true) {
               last_sunday=sunday;
               sunday=sunday+7*msPerDay;
               if (MonthFromTime(sunday)>9)
                   break;
            }
            return last_sunday;
        }
        
        public static function LocalTime( t ) {
            return ( t + LocalTZA() + DaylightSavingTA(t) );
        }
        
        public static function MakeTime( hour, min, sec, ms ) {
            if ( isNaN( hour ) || isNaN( min ) || isNaN( sec ) || isNaN( ms ) ) {
                return Number.NaN;
            }
        
            hour = ToInteger(hour);
            min  = ToInteger( min);
            sec  = ToInteger( sec);
            ms   = ToInteger( ms );
        
            return( (hour*msPerHour) + (min*msPerMinute) +
                    (sec*msPerSecond) + ms );
        }
        
        public static function MakeDay( year, month, date ) {
            if ( isNaN(year) || isNaN(month) || isNaN(date) ) {
                return Number.NaN;
            }
            year = ToInteger(year);
            month = ToInteger(month);
            date = ToInteger(date );
        
            var sign = ( year < 1970 ) ? -1 : 1;
            var t =    ( year < 1970 ) ? 1 :  0;
            var y =    ( year < 1970 ) ? 1969 : 1970;
        
            var result5 = year + Math.floor( month/12 );
            var result6 = month % 12;
        
            if ( year < 1970 ) {
               for ( y = 1969; y >= year; y += sign ) {
                 t += sign * TimeInYear(y);
               }
            } else {
                for ( y = 1970 ; y < year; y += sign ) {
                    t += sign * TimeInYear(y);
                }
            }
        
            var leap = InLeapYear( t );
        
            for ( var m = 0; m < month; m++ ) {
                t += TimeInMonth( m, leap );
            }
        
            if ( YearFromTime(t) != result5 ) {
                return Number.NaN;
            }
            if ( MonthFromTime(t) != result6 ) {
                return Number.NaN;
            }
            if ( DateFromTime(t) != 1 ) {
                return Number.NaN;
            }
        
            return ( (Day(t)) + date - 1 );
        }
        
        public static function TimeInMonth( month, leap ) {
            // september april june november
            // jan 0  feb 1  mar 2  apr 3   may 4  june 5  jul 6
            // aug 7  sep 8  oct 9  nov 10  dec 11
        
            if ( month == 3 || month == 5 || month == 8 || month == 10 ) {
                return ( 30*msPerDay );
            }
        
            // all the rest
            if ( month == 0 || month == 2 || month == 4 || month == 6 ||
                 month == 7 || month == 9 || month == 11 ) {
                return ( 31*msPerDay );
             }
        
            // save february
            return ( (leap == 0) ? 28*msPerDay : 29*msPerDay );
        }
        
        public static function MakeDate( day, time ) {
            if (    day == Number.POSITIVE_INFINITY ||
                    day == Number.NEGATIVE_INFINITY ||
                    day == Number.NaN ) {
                return Number.NaN;
            }
            if (    time == Number.POSITIVE_INFINITY ||
                    time == Number.POSITIVE_INFINITY ||
                    day == Number.NaN) {
                return Number.NaN;
            }
            return ( day * msPerDay ) + time;
        }
        
        // Compare 2 dates, they are considered equal if the difference is less than 1 second
        public static function compareDate(d1, d2) {
            //Dates may be off by a second
            if (d1 == d2) {
                return true;
            } else if (Math.abs(new Date(d1).getTime() - new Date(d2).getTime()) <= 1000) {
                return true;
            } else {
                return false;
            }
        }
    }
}

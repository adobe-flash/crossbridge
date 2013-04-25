/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VMPI.h"

#include <Mmsystem.h>
#include <time.h>

/*
* Windows implementation of platform-dependent date and time code
*/
static const double kMsecPerDay       = 86400000;
static const double kMsecPerHour      = 3600000;
static const double kMsecPerSecond    = 1000;
static const double kMsecPerMinute    = 60000;

static TIME_ZONE_INFORMATION UpdateTimeZoneInfo()
{
    // protects gTimeZoneInfo and gGmtCache
    //   We're relying on the fact that static initialization is OK for spin locks.
    static vmpi_spin_lock_t gTimeZoneInfoLock;
    static TIME_ZONE_INFORMATION gTimeZoneInfo;
    static SYSTEMTIME gGmtCache;

    SYSTEMTIME gmt;
    GetSystemTime(&gmt);
    VMPI_lockAcquire(&gTimeZoneInfoLock);
    if ((gmt.wMinute != gGmtCache.wMinute) ||
        (gmt.wHour != gGmtCache.wHour) ||
        (gmt.wDay != gGmtCache.wDay) ||
        (gmt.wMonth != gGmtCache.wMonth) ||
        (gmt.wYear != gGmtCache.wYear)
        )
    {
        // Cache is invalid.
        GetTimeZoneInformation(&gTimeZoneInfo);
        gGmtCache = gmt;
    }
    TIME_ZONE_INFORMATION tz = gTimeZoneInfo;
    VMPI_lockRelease(&gTimeZoneInfoLock);
    return tz;
}

double VMPI_getLocalTimeOffset()
{
    TIME_ZONE_INFORMATION tz = UpdateTimeZoneInfo();
    return -tz.Bias * 60.0 * 1000.0;
}

// Bugzilla 668442, fix this evil of working around tangle of
// dependencies with local declarations (below are from Date.cpp)
namespace avmplus
{
    int WeekDay(double t);
    double MakeDate(double day, double time);
    double MakeDay(double year, double month, double date);
    double MakeTime(double hour, double min, double sec, double ms);
    int YearFromTime(double t);
}

static double ConvertWin32DST(int year, SYSTEMTIME *st)
{
    // The StandardDate and DaylightDate members of
    // TIMEZONE_INFORMATION may be specified in two ways:
    // 1. An absolute time and date
    // 2. A month, day of week and week in month, and a time of day

    if (st->wYear != 0) {
        return avmplus::MakeDate(avmplus::MakeDay(year,
            st->wMonth - 1,
            st->wDay),
            avmplus::MakeTime(st->wHour,
            st->wMinute,
            st->wSecond,
            st->wMilliseconds));
    }

    double timeValue = avmplus::MakeDate(avmplus::MakeDay(year,
        st->wMonth-1,
        1),
        avmplus::MakeTime(st->wHour,
        st->wMinute,
        st->wSecond,
        st->wMilliseconds));

    double nextMonth;
    if (st->wMonth < 12) {
        nextMonth = avmplus::MakeDate(avmplus::MakeDay(year, st->wMonth, 1), 0);
    } else {
        nextMonth = avmplus::MakeDate(avmplus::MakeDay(year + 1, 0, 1), 0);
    }

    int dayOfWeek = avmplus::WeekDay(timeValue);
    if (dayOfWeek != st->wDayOfWeek) {
        int dayDelta = st->wDayOfWeek - dayOfWeek;
        if (dayDelta < 0) {
            dayDelta += 7;
        }
        timeValue += (double)kMsecPerDay * dayDelta;
    }

    // Advance appropriate # of weeks
    timeValue += (double)kMsecPerDay * 7.0 * (st->wDay - 1);

    // Cap it at the end of the month
    while (timeValue >= nextMonth) {
        timeValue -= (double)kMsecPerDay * 7.0;
    }

    return timeValue;
}

//time is passed in as milliseconds from UTC.
double VMPI_getDaylightSavingsTA(double time)
{
    // On Windows, ask the OS what the daylight saving time bias
    // is.  If it's zero, perform no adjustment.
    TIME_ZONE_INFORMATION tz = UpdateTimeZoneInfo();
    if (tz.DaylightBias != -60 || tz.DaylightDate.wMonth == 0) {
        return 0;
    }

    // In most of the US, Daylight Saving Time begins on the
    // first Sunday of April at 2 AM.  It ends at 2 am on
    // the last Sunday of October.

    // 1. Compute year this time represents.
    int year = avmplus::YearFromTime(time);

    // 2. Compute time that daylight saving time begins
    double timeD = ConvertWin32DST(year, &tz.DaylightDate);

    // 3. Compute time that standard time begins
    double timeS = ConvertWin32DST(year, &tz.StandardDate);

    // Subtract the daylight bias from the standard transition time
    timeS -= kMsecPerHour;

    // The times we have calculated are in local time,
    // but "time" was passed in as UTC.  Convert it to local time.
    time += VMPI_getLocalTimeOffset();

    // Does time fall in the daylight saving period?

    // Where Daylight savings time is earlier in the year than standard time
    if(timeS > timeD)
    {
        if (time >= timeD && time < timeS)
            return kMsecPerHour;
        else
            return 0;
    }
    // Where Daylight savings time is later in the year than standard time
    else
    {
        if (time >= timeS && time < timeD)
            return 0;
        else
            return kMsecPerHour;
    }

}

#define FILETIME_EPOCH_BIAS ((LONGLONG)116444736000000000)
#define FILETIME_MS_FACTOR (10000.0)

static double NormalizeFileTime(FILETIME* ft)
{
    LARGE_INTEGER li;
    li.LowPart = ft->dwLowDateTime;
    li.HighPart = ft->dwHighDateTime;

    return ((double) (li.QuadPart - FILETIME_EPOCH_BIAS)) / FILETIME_MS_FACTOR;
}

static double NormalizeSystemTime(SYSTEMTIME* st)
{
    FILETIME ft;
    SystemTimeToFileTime(st, &ft);
    return NormalizeFileTime(&ft);
}

double VMPI_getDate()
{
    SYSTEMTIME stime;
    GetSystemTime(&stime);
    return NormalizeSystemTime(&stime);
}

uint64_t VMPI_getTime()
{
    return timeGetTime();
}

// On Windows, _vsnprintf isn't reliable (no NUL termination) and _vsnprintf_s
// is not compatible (throws an exception if the format is wrong).  Easy
// enough to create our own on top of _vsnprintf by adding the terminator
// and adjusting the return value.
//
// ANSI C requires vsnprintf to return the number of characters that would have
// been printed had the buffer been unrestricted.  The MSVC _vsnprintf does not
// do that.  Instead, return the number of characters written not including
// the NUL in all cases.

int VMPI_vsnprintf(char *s, size_t n, const char *format, va_list args)
{
    int ret = _vsnprintf(s, n, format, args);
    if (ret == -1)
        ret = int(n);
    if (ret == int(n)) {
        s[n-1] = 0;
        ret--;
    }
    return ret;
}

// On Windows, _snprintf isn't reliable (no NUL termination) and _snpritnf_s
// is not compatible (throws an exception if the format is wrong).  Easy
// enough to create our own on top of VMPI_vsnprintf.
//
// Return what VMPI_vsnprintf returns; see notes above.

int VMPI_snprintf(char *s, size_t n, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = VMPI_vsnprintf(s, n, format, args);
    va_end(format);
    return ret;
}

void* VMPI_alloc(size_t size)
{
    return HeapAlloc(GetProcessHeap(), 0, size);
}

void VMPI_free(void* ptr)
{
    HeapFree(GetProcessHeap(), 0, ptr);
}

size_t VMPI_size(void* ptr)
{
    return HeapSize(GetProcessHeap(), 0, ptr);
}

typedef void (*LoggingFunction)(const char*);
extern LoggingFunction GetCurrentLogFunction();

void VMPI_log(const char* message)
{
#ifndef UNDER_CE
    ::OutputDebugStringA(message);
#endif
    LoggingFunction logFunc = GetCurrentLogFunction();
    if(logFunc)
        logFunc(message);
    else {
        printf("%s",message);
        fflush(stdout);
    }
}

const char *VMPI_getenv(const char *env)
{
    const char *val = NULL;
    (void)env;
#ifndef UNDER_CE
    val = getenv(env);
#endif
    return val;
}

// Call VMPI_getPerformanceFrequency() once to initialize its cache; avoids thread safety issues.
static uint64_t unused_value = VMPI_getPerformanceFrequency();

uint64_t VMPI_getPerformanceFrequency()
{
    // *** NOTE ABOUT THREAD SAFETY ***
    //
    // This static ought to be safe because it is initialized by a call at startup
    // (see lines above this function), before any AvmCores are created.

    static uint64_t gPerformanceFrequency = 0;
    if (gPerformanceFrequency == 0) {
        QueryPerformanceFrequency((LARGE_INTEGER*)&gPerformanceFrequency);
    }
    return gPerformanceFrequency;
}

uint64_t VMPI_getPerformanceCounter()
{
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return value.QuadPart;
}


// Defined in GenericDebugUtils.cpp to prevent them from being inlined below

extern void CallWithRegistersSaved2(void (*fn)(void* stackPointer, void* arg), void* arg, void* buf);
extern void CallWithRegistersSaved3(void (*fn)(void* stackPointer, void* arg), void* arg, void* buf);

void VMPI_callWithRegistersSaved(void (*fn)(void* stackPointer, void* arg), void* arg)
{
    jmp_buf buf;
    VMPI_setjmpNoUnwind(buf);                   // Save registers
    CallWithRegistersSaved2(fn, arg, &buf);     // Computes the stack pointer, calls fn
    CallWithRegistersSaved3(fn, &arg, &buf);    // Probably prevents the previous call from being a tail call
}

static size_t computePagesize()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    return sysinfo.dwPageSize;
}

// Private to AAVMPI_getVMPageSize (but initialized here to avoid threading concerns).
// DO NOT REFERENCE THIS VARIABLE ELSEWHERE.  Always call AAVMPI_getVMPageSize.

static size_t pagesize = computePagesize();

size_t VMPI_getVMPageSize()
{
    return pagesize;
}

// Timer data, Windows specific data
struct VMPI_WinTimerData : VMPI_TimerData
{
    // Platform-specific data
    unsigned int timerId;
};

// The timer callback proc, called when the timer fires
void CALLBACK timerProc(UINT, UINT, DWORD_PTR dwUser, DWORD_PTR, DWORD_PTR)
{
    VMPI_TimerData *data = (VMPI_TimerData*)dwUser;
    data->client->tick();
}

// The constant TIME_KILL_SYNCHRONOUS is only available if WINVER >= 0x0501 (== WinXP)
static const UINT kTimeKillSynchronous = 0x0100;

// Starts a timer
uintptr_t VMPI_startTimer(uint32_t micros, VMPI_TimerClient *client)
{
    VMPI_WinTimerData *data = (VMPI_WinTimerData *) VMPI_alloc(sizeof(VMPI_WinTimerData));
    data->init(micros, client);

    data->timerId = timeSetEvent(micros / 1000, micros / 1000, (LPTIMECALLBACK)timerProc, (DWORD_PTR)data,
            TIME_PERIODIC | TIME_CALLBACK_FUNCTION
#ifndef UNDER_CE
            | kTimeKillSynchronous
#endif
            );

    return (uintptr_t)data;
}

// Stops a timer
void VMPI_stopTimer(uintptr_t data)
{
    UINT timerId = ((VMPI_WinTimerData *)data)->timerId;
    if (timerId)
        timeKillEvent((UINT)timerId);
    VMPI_free((VMPI_WinTimerData *)data);
}

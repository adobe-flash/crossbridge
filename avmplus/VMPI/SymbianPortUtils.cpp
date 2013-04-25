/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VMPI.h"

#include <sys/time.h>
#include <math.h>
#include <sys/mman.h>
#include "SymbianJITHeap.h"
#include <e32std.h>
#include <f32file.h>
#include <hal.h>

#define kMsecPerDay     86400000
#define kMsecPerHour    3600000
#define kMsecPerSecond  1000
#define kMsecPerMinute  60000

#define DIVCLOCK ( CLOCKS_PER_SEC / 1000 )

#define kMicroPerSec 1000000.0

/**
* Logging code
*/

void DebugTrace(const TDesC8& aBuffer)
{
    RFile file;
    RFs fs;

    if (fs.Connect() != KErrNone)
    {
        return;
    }

    TInt err = file.Open(fs, _L("c:\\data\\vmpi_debug.txt"), EFileWrite);
    if (err == KErrNotFound)
    {
        err = file.Create(fs, _L("c:\\data\\vmpi_debug.txt"), EFileWrite);
    }
    if (err == KErrNone)
    {
        TInt pos = 0;
        file.Seek(ESeekEnd, pos);
        file.Write(aBuffer);
        file.Close();
    }

    fs.Close();
}

_LIT8(kNewline, "\n");
_LIT8(kNullString, "NULL STRING");

void DebugTraceString(const char* aString)
{
    if (aString == NULL)
    {
        DebugTrace(kNullString);
    }
    else
    {
        TPtrC8 tstr((const TUint8*)aString);
        DebugTrace(tstr);
        DebugTrace(kNewline);
    }
}
/**
* Logging code ends
*/

double VMPI_getLocalTimeOffset()
{
    struct tm* t;
    time_t current, localSec, globalSec;

    // The win32 implementation ignores the passed in time
    // and uses current time instead, so to keep similar
    // behaviour we will do the same
    time( &current );

    t = localtime( &current );
    localSec = mktime( t );

    t = gmtime( &current );
    globalSec = mktime( t );

    return double( localSec - globalSec ) * 1000.0;
}

double VMPI_getDate()
{
    struct timeval tv;
    struct timezone tz; // Unused

    gettimeofday(&tv, &tz);
    double v = (tv.tv_sec + (tv.tv_usec/kMicroPerSec)) * kMsecPerSecond;
    double ip;
    ::modf(v, &ip); // strip fractional part
    return ip;
}

//time is passed in as milliseconds from UTC.
double VMPI_getDaylightSavingsTA(double newtime)
{
    struct tm *broken_down_time;

    //convert time from milliseconds
    newtime=newtime/kMsecPerSecond;

    time_t time_t_time=(time_t)newtime;

    //pull out a struct tm
    broken_down_time = localtime( &time_t_time );

    if (!broken_down_time)
    {
        return 0;
    }

    if (broken_down_time->tm_isdst > 0)
    {
        //daylight saving is definitely in effect.
        return kMsecPerHour;
    }

    //either daylight saving is not in effect, or we don't know (if tm_isdst is negative).
    return 0;
}

uint64_t VMPI_getTime()
{
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    uint64_t result = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    return result;
}

void* VMPI_alloc(size_t size)
{
    void* ptr = malloc(size);
    if(ptr) memset(ptr, 0, size);
    return ptr;
}

void VMPI_free(void* ptr)
{
    free(ptr);
}

size_t VMPI_size(void *ptr)
{
    return 0;
}

void VMPI_log(const char* message)
{
    DebugTraceString(message);
}

bool VMPI_isMemoryProfilingEnabled()
{
    //read the mmgc profiling option switch
    const char *env = getenv("MMGC_PROFILE");
    return (env && (VMPI_strncmp(env, "1", 1) == 0));
}

static SymbianJITHeap* symbianJITHeap = 0;

void *VMPI_allocateCodeMemory(size_t nbytes)
{
    // Our custom SymbianJITHeap class can allocate only one page a time (=4k) per design.
    // If NJ starts allocating more than 4k a time on ARM platforms, we have to change this,
    // probably just to use RHeap directly (and manually page align the memory it returns).

    // FIXME https://bugzilla.mozilla.org/show_bug.cgi?id=571407
    // This really should be a runtime check, not an assertion
    AvmAssert( nbytes == VMPI_getVMPageSize() );
    if(!symbianJITHeap)
    {
        symbianJITHeap = new SymbianJITHeap();
        if(symbianJITHeap && !symbianJITHeap->IsInitialized())
        {
            delete symbianJITHeap;
            symbianJITHeap = 0;
        }
    }

    if(symbianJITHeap)
    {
        return symbianJITHeap->Alloc();
    }
    else
    {
        // FIXME https://bugzilla.mozilla.org/show_bug.cgi?id=571407
        // The documentation for VMPI_allocateCodeMemory states specifically that the
        // function never returns NULL, so this is wrong and must be fixed.
        // Being landed temporarily to sync various code lines.
        return 0;
    }
}

void VMPI_freeCodeMemory(void* address, size_t nbytes)
{
    // FIXME https://bugzilla.mozilla.org/show_bug.cgi?id=571407
    // This really should be a runtime check, not an assertion
    AvmAssert( nbytes == VMPI_getVMPageSize() );
    if(symbianJITHeap)
    {
        symbianJITHeap->Free(address);
        if(symbianJITHeap->GetNumAllocs() == 0)
        {
            delete symbianJITHeap;
            symbianJITHeap = 0;
        }
    }
}

void VMPI_makeCodeMemoryExecutable(void *address, size_t nbytes, bool makeItSo)
{
    // See VMPI.h for documentation about the semantics of this method

    // See PosixPortUtils.cpp for many useful comments about constraints
    // that you may wish to implement in this function

    (void)address;
    (void)nbytes;
    (void)makeItSo;
    AvmAssert(!"Unimplemented: VMPI_freeCodeMemory");
}

const char *VMPI_getenv(const char *name)
{
    return getenv(name);
}

uint64_t VMPI_getPerformanceFrequency()
{
    TInt tickPeriod;
    HAL::Get(HALData::EFastCounterFrequency, tickPeriod);
    return (uint64_t)tickPeriod;
}

uint64_t VMPI_getPerformanceCounter()
{
    // Alternative way:
    /*
     TTime t;
     t.UniversalTime ();
     TInt64 lt = t.Int64 ();
     return (uint64_t) (lt & 0x7FFFFFFF);
     */
    TUint32 counter = User::FastCounter();
    return counter;
}

// Defined in GenericPortUtils.cpp to prevent them from being inlined below
extern void CallWithRegistersSaved2(void (*fn)(void* stackPointer, void* arg), void* arg, void* buf);
extern void CallWithRegistersSaved3(void (*fn)(void* stackPointer, void* arg), void* arg, void* buf);

void AVMPI_callWithRegistersSaved(void (*fn)(void* stackPointer, void* arg), void* arg)
{
    jmp_buf buf;
    VMPI_setjmpNoUnwind(buf);                   // Save registers
    CallWithRegistersSaved2(fn, arg, &buf);     // Computes the stack pointer, calls fn
    CallWithRegistersSaved3(fn, &arg, &buf);    // Probably prevents the previous call from being a tail call
}

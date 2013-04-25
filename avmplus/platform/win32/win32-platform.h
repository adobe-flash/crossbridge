/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* -*- tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_win32_platform__
#define __avmplus_win32_platform__

#include <stdarg.h>

/**
 * We have avmplus.vcproj compiled with the /W4 warning level
 * which is quite picky.  Disable warnings we don't care about.
 */
#ifdef _MSC_VER
    #pragma warning(disable:4946) // reinterpret_cast used between related classes (bugzilla 615964)
    #pragma warning(disable:4127) // conditional expression is constant - appears to be compiler noise primarily
    #pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
    #pragma warning(disable:4251) // X needs to have dll-interface to be used by clients of class Y
    #pragma warning(disable:4310) // cast truncates constant value
    #pragma warning(disable:4371) // better packing than in the previous version of the compiler
    #pragma warning(disable:4511) // can't generate copy ctor
    #pragma warning(disable:4512) // assignment operator could not be generated
    #pragma warning(disable:4611) // interaction between '_setjmp' and C++ object destruction is non-portable
    #pragma warning(disable:4725) // instruction may be inaccurate on some Pentiums
    #pragma warning(disable:4324) // padding was added at the end of a structure because you specified a __declspec(align) value.
    #pragma warning(disable:4800) // forcing value to bool 'true' or 'false' - bogus performance warning

    // enable some that are off even in /W4 mode, but are still handy
    #pragma warning(default:4242)   // 'identifier' : conversion from 'type1' to 'type2', possible loss of data
    #pragma warning(default:4263)   // 'function' : member function does not override any base class virtual member function
    #pragma warning(default:4264)   // 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
    #pragma warning(default:4265)   // 'class' : class has virtual functions, but destructor is not virtual
    #pragma warning(default:4266)   // 'function' : no override available for virtual member function from base 'type'; function is hidden
    #pragma warning(default:4296)   // expression is always true (false) (Generally, an unsigned variable was used in a comparison operation with zero.)
    #pragma warning(default:4905)   // wide string literal cast to 'LPSTR'
    #pragma warning(default:4906)   // string literal cast to 'LPWSTR'

    // some that might be useful to turn on someday, but would require too much twiddly code tweaking right now
    //  #pragma warning(error:4820) // 'bytes' bytes padding added after construct 'member_name' (MSFT system headers generate zillions of these, sadly)

/**
 * Disable PREfast analysis warnings that complain about legitimate usage.
 */
#ifdef _PREFAST_
    // These are triggered by the use of named constants for configuration purposes.
    // There is nothing wrong with using these in preference to preprocessor conditionalization.
    #pragma warning(disable:6237)   // (<zero> && <expression>) is always zero
    #pragma warning(disable:6239)   // (<non-zero constant> && <expression>) always evaluates to the result of <expression>

    // This needs to be fixed, but reflects our generally cavalier treatment of stack overflow
    #pragma warning(disable:6255)   // _alloca indicates failure by raising a stack overflow exception
#endif
#endif // _MSC_VER

#define VMPI_memcpy         ::memcpy
#define VMPI_memset         ::memset
#define VMPI_memcmp         ::memcmp
#define VMPI_memmove        ::memmove
#define VMPI_memchr         ::memchr
#define VMPI_strcmp         ::strcmp
#define VMPI_strcat         ::strcat
#define VMPI_strchr         ::strchr
#define VMPI_strrchr        ::strrchr
#define VMPI_strcpy         ::strcpy
#define VMPI_strlen         ::strlen
#define VMPI_strncat        ::strncat
#define VMPI_strncmp        ::strncmp
#define VMPI_strncpy        ::strncpy
#define VMPI_strtol         ::strtol
#define VMPI_strstr         ::strstr

#define VMPI_sprintf        ::sprintf
#define VMPI_sscanf         ::sscanf

// Print rest args into s according to format.  The size of s is n.  A NUL
// terminator is always written.  Returns the number of characters written,
// not including the NUL.  (Hence the return value is always 0..n-1.)

int VMPI_snprintf(char *s, size_t n, const char *format, ...);

// As for VMPI_snprintf but with an argument list instead of rest args.

int VMPI_vsnprintf(char *s, size_t n, const char *format, va_list args);

#define VMPI_atoi           ::atoi
// these aren't prefixed b/c off winmo/pcre problems with ? : expressions
#define VMPI_tolower        tolower
#define VMPI_islower        islower
#define VMPI_toupper        toupper
#define VMPI_isupper        isupper
#define VMPI_isdigit        isdigit
#define VMPI_isalnum        isalnum
#define VMPI_isxdigit       isxdigit
#define VMPI_isspace        isspace
#define VMPI_isgraph        isgraph
#define VMPI_isprint        isprint
#define VMPI_ispunct        ispunct
#define VMPI_iscntrl        iscntrl
#define VMPI_isalpha        isalpha
#ifdef UNDER_CE
    #define VMPI_abort() ::TerminateProcess(GetCurrentProcess(), 0)
#else
    #define VMPI_abort   ::abort
#endif

#define VMPI_exit           ::exit

#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <ctype.h>
#include <limits.h>

#include <windows.h>
#include <malloc.h>

#if defined(UNDER_CE)
// winmo complains if we try to include <new> and it complains if someone else includes new before us so...
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void* operator new(size_t, void* p) { return p; }
#endif
#else
// others want the system new
#include <new>
#endif

#ifdef _ARM_
    // Windows Mobile doesn't provide intptr_t or uintptr_t
    typedef __int32             intptr_t;
    typedef unsigned __int32    uintptr_t;
#endif

typedef void *maddr_ptr;

// Set up a jmp_buf suitable for VMPI_longjmpNoUnwind.
//#define VMPI_setjmpNoUnwind ::setjmp

// Jump to an active jmp_buf that was set up by VMPI_setjmpNoUnwind.
// Under no circumstances may C++ destructors be unwound during the
// jump (MSVC likes to do this by default).
//#define VMPI_longjmpNoUnwind ::longjmp

#ifdef UNDER_RT
    #include <setjmpex.h>
    extern "C"
    {
        void __cdecl longjmp_arm(jmp_buf jmpbuf, int arg);
    }
    #define VMPI_setjmpNoUnwind     ::setjmp
    #define VMPI_longjmpNoUnwind    ::longjmp_arm
#else
	#ifdef VMCFG_64BIT
		#include <setjmpex.h>
		extern "C"
		{
			_int64 __cdecl longjmp64(jmp_buf jmpbuf, _int64 arg);
		}
		#define VMPI_setjmpNoUnwind     ::setjmp
		#define VMPI_longjmpNoUnwind    ::longjmp64
	#else
		#include <setjmp.h>
		#define VMPI_setjmpNoUnwind     ::setjmp
		#define VMPI_longjmpNoUnwind    ::longjmp
	#endif
#endif

#if !defined(UNDER_CE)
  // Newer versions of the Windows SDK set up the intrinsics slightly differently
  // than VC8. Only include intrin.h if the SDK doesn't declare it.
  #ifndef InterlockedBitTestAndSet
    #include <intrin.h>
  #endif
  #ifndef _ARM_
  #include <emmintrin.h>
  #endif
  #ifdef VMCFG_VTUNE
    #include "JITProfiling.h"
  #endif
#endif

// Windows doesn't support inttypes.h or most C99 types directly

#if _MSC_VER > 1500 
	#include <stdint.h>
#else
// included in stdint.h
	typedef __int8              int8_t;
	typedef __int16             int16_t;
	typedef __int32             int32_t;
	typedef __int64             int64_t;
	typedef unsigned __int8     uint8_t;
	typedef unsigned __int16    uint16_t;
	typedef unsigned __int32    uint32_t;
	typedef unsigned __int64    uint64_t;
#endif

// This must come after all the include files
#if defined _MSC_VER && !defined DEBUG
    #pragma intrinsic(memcmp)
    #pragma intrinsic(memcpy)
    #pragma intrinsic(memset)
    #pragma intrinsic(strlen)
    #pragma intrinsic(strcpy)
    #pragma intrinsic(strcat)
#endif

#if _MSC_VER
    #define REALLY_INLINE __forceinline
    #define FASTCALL __fastcall
#elif __GNUC__
    #define FASTCALL __attribute__((fastcall))
#else
    #define FASTCALL
#endif

#if defined(_MSC_VER)
    #define AVMPLUS_ALIGN8(type) __declspec(align(8)) type
    #define AVMPLUS_ALIGN16(type) __declspec(align(16)) type
#elif defined(__GNUC__)
    #define AVMPLUS_ALIGN8(type) type __attribute__ ((aligned (8)))
    #define AVMPLUS_ALIGN16(type) type __attribute__ ((aligned (16)))
#else
    #error "Unrecognized compiler"
#endif

#if defined(_MSC_VER) && _MSC_VER < 1400 && defined(VMCFG_NANOJIT)
    #define NJ_NO_VARIADIC_MACROS
#endif

/**
* Type defintion for an opaque data type representing platform-defined spin lock
* @see VMPI_lockInit(), VMPI_lockAcquire()
*/
struct vmpi_spin_lock_t
{
    volatile LONG lock;
};

REALLY_INLINE void VMPI_lockInit(vmpi_spin_lock_t* lock)
{
    lock->lock = 0;
}

REALLY_INLINE void VMPI_lockDestroy(vmpi_spin_lock_t* lock)
{
    lock->lock = 0;
}

REALLY_INLINE bool VMPI_lockAcquire(vmpi_spin_lock_t *lock)
{
    int tries = 0;
    while (::InterlockedCompareExchange((LPLONG)&lock->lock, 1, 0) != 0)
    {
        ++tries;
        // We used to reset to zero if we got to 100. This resets to 0 at 64 instead, with no branch.
        tries &= 63;
        // if tries == 0, we just rolled over 64, so we Sleep(1) to give other threads a chance to run... otherwise we Sleep(0)
        ::Sleep(tries == 0);
    }
    return true;
}

REALLY_INLINE bool VMPI_lockRelease(vmpi_spin_lock_t *lock)
{
    ::InterlockedExchange((LPLONG)&lock->lock, 0);
    return true;
}

REALLY_INLINE bool VMPI_lockTestAndAcquire(vmpi_spin_lock_t *lock)
{
    return ::InterlockedCompareExchange((LPLONG)&lock->lock, 1, 0) == 0;
}

REALLY_INLINE int32_t VMPI_atomicIncAndGet32WithBarrier(volatile int32_t* value)
{
#ifdef UNDER_CE
    return InterlockedIncrement(const_cast<LONG*>((volatile LONG*)value)); // Barrier semantics are not actually documented
#else
    return InterlockedIncrement((volatile LONG*)value);
#endif
}

REALLY_INLINE int32_t VMPI_atomicIncAndGet32(volatile int32_t* value)
{
    // No barrier-less version is available
    return VMPI_atomicIncAndGet32WithBarrier(value);
}

REALLY_INLINE int32_t VMPI_atomicDecAndGet32WithBarrier(volatile int32_t* value)
{
#ifdef UNDER_CE
    return InterlockedDecrement(const_cast<LONG*>((volatile LONG*)value)); // Barrier semantics are not actually documented
#else
    return InterlockedDecrement((volatile LONG*)value);
#endif
}

REALLY_INLINE int32_t VMPI_atomicDecAndGet32(volatile int32_t* value)
{
    // No barrier-less version is available
    return VMPI_atomicDecAndGet32WithBarrier(value);
}

REALLY_INLINE bool VMPI_compareAndSwap32WithBarrier(int32_t oldValue, int32_t newValue, volatile int32_t* address)
{
#ifdef UNDER_CE
    return InterlockedCompareExchange(const_cast<LONG*>((volatile LONG*)address), newValue, oldValue) == oldValue;  // Barrier semantics are not actually documented
#else
    return InterlockedCompareExchange((volatile LONG*)address, newValue, oldValue) == oldValue;
#endif
}

REALLY_INLINE bool VMPI_compareAndSwap32(int32_t oldValue, int32_t newValue, volatile int32_t* address)
{
    return VMPI_compareAndSwap32WithBarrier(oldValue, newValue, address);
}

REALLY_INLINE void VMPI_memoryBarrier()
{
    // FIXME: bug 609820 Memory barrier (fence) for win32
    //
    // void MemoryBarrier(void) is only available >= Vista
    // This is a temp solution until we have a x86 asm version.
    volatile int32_t dummy;
    VMPI_atomicIncAndGet32WithBarrier(&dummy);
}

/**
 * Threads waiting on a conditional variable are recorded in a list
 * of WaitingThread structs.
 */
struct WaitingThread {
    WaitingThread* next;
    DWORD threadID;
    bool notified;
};

/**
 * A condition variable is implemented as a linked list of waiting threads.
 * The variable itself needs to record only the head and the tail of the list.
 */
struct vmpi_condvar_t {
    WaitingThread* head;
    WaitingThread* tail;
};

typedef DWORD                  vmpi_thread_t;
typedef CRITICAL_SECTION       vmpi_mutex_t;
typedef LPVOID                 vmpi_thread_arg_t; // Argument type for thread start function
typedef DWORD                  vmpi_thread_rtn_t; // Return type for thread start function
typedef LPTHREAD_START_ROUTINE vmpi_thread_start_t;

struct vmpi_thread_attr_t {
    size_t stackSize;
    size_t guardSize;
    int priority;
};

#define VMPI_THREAD_START_CC   WINAPI

REALLY_INLINE bool VMPI_recursiveMutexInit(vmpi_mutex_t* mutex)
{
    InitializeCriticalSection(mutex);
    return true;
}

REALLY_INLINE bool VMPI_recursiveMutexDestroy(vmpi_mutex_t* mutex)
{
    DeleteCriticalSection(mutex);
    return true;
}

REALLY_INLINE bool VMPI_recursiveMutexTryLock(vmpi_mutex_t* mutex)
{
    return TryEnterCriticalSection(mutex) != 0;
}

REALLY_INLINE void VMPI_recursiveMutexLock(vmpi_mutex_t* mutex)
{
    EnterCriticalSection(mutex);
}

REALLY_INLINE void VMPI_recursiveMutexUnlock(vmpi_mutex_t* mutex)
{
    LeaveCriticalSection(mutex);
}

REALLY_INLINE bool VMPI_condVarInit(vmpi_condvar_t* condvar)
{
    condvar->head = NULL;
    condvar->tail = NULL;
    return true;
}

REALLY_INLINE bool VMPI_condVarDestroy(vmpi_condvar_t* condvar)
{
    (void)condvar;
    return true;
}

REALLY_INLINE vmpi_thread_t VMPI_currentThread()
{
    return (vmpi_thread_t) (uintptr_t)GetCurrentThreadId();
}

REALLY_INLINE vmpi_thread_t VMPI_nullThread()
{
    return NULL;
}

REALLY_INLINE bool VMPI_tlsSetValue(uintptr_t tlsId, void* value)
{
    return TlsSetValue((DWORD)tlsId, value) == TRUE;
}

REALLY_INLINE void* VMPI_tlsGetValue(uintptr_t tlsId)
{
    return TlsGetValue((DWORD)tlsId);
}

REALLY_INLINE void VMPI_threadYield()
{
#ifdef UNDER_CE
    Sleep(1);
#else
    SwitchToThread();
#endif
}

REALLY_INLINE int VMPI_processorQtyAtBoot()
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    // dwNumberOfProcessors can be unreliable, but we know we have at least one processor
    return systemInfo.dwNumberOfProcessors < 1 ? 1 : systemInfo.dwNumberOfProcessors;
}

REALLY_INLINE void VMPI_spinloopPause()
{
#ifdef AVMPLUS_IA32
    __asm pause;
#endif
}

#endif // __avmplus_win32_platform__


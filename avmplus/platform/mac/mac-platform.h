/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_mac_platform__
#define __avmplus_mac_platform__

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
#define VMPI_snprintf       ::snprintf
#define VMPI_vsnprintf      ::vsnprintf
#define VMPI_sscanf         ::sscanf

#define VMPI_atoi   ::atoi
#define VMPI_tolower ::tolower
#define VMPI_islower ::islower
#define VMPI_toupper ::toupper
#define VMPI_isupper ::isupper
#define VMPI_isdigit ::isdigit
#define VMPI_isalnum ::isalnum
#define VMPI_isxdigit ::isxdigit
#define VMPI_isspace ::isspace
#define VMPI_isgraph ::isgraph
#define VMPI_isprint ::isprint
#define VMPI_ispunct ::ispunct
#define VMPI_iscntrl ::iscntrl
#define VMPI_isalpha ::isalpha
#define VMPI_abort   ::abort
#define VMPI_exit    ::exit

// Set up a jmp_buf suitable for VMPI_longjmpNoUnwind.
// Use the routine version with an underscore to avoid system calls
// to query the signal mask.
#define VMPI_setjmpNoUnwind ::_setjmp

// Jump to an active jmp_buf that was set up by VMPI_setjmpNoUnwind.
// Under no circumstances may C++ destructors be unwound during the
// jump (MSVC likes to do this by default).
// Use the routine version with an underscore to avoid system calls
// to query the signal mask.
#define VMPI_longjmpNoUnwind ::_longjmp

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <ctype.h>
#include <limits.h>

#include <inttypes.h>
#include <alloca.h>

// Bug 645878: must guard the include to avoid breaking AIR_IOS.
#if defined(AVMPLUS_MAC_CARBON) || defined(AVMPLUS_PPC)
    #include <CoreServices/CoreServices.h>   // for MakeDataExecutable
#endif

#include <mach/mach.h>                   // for vm_protect()
#include <AvailabilityMacros.h>

#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <new>
#include <libkern/OSAtomic.h>
#include <signal.h>

#include <sys/sysctl.h>

#ifdef DEBUG
#include <assert.h>
#endif

typedef void *maddr_ptr;
typedef pthread_t vmpi_thread_t;

#ifdef AVMPLUS_MAC_CARBON
    /**
     * On Mac Carbon, if you compile with Altivec support,
     * setjmp is redirected to __vec_setjmp and longjmp is redirected
     * to __vec_longjmp.  These routines do not gracefully degrade if
     * the CPU does not have Altivec... they just crash.
     *
     * We don't need Altivec support in the places where exceptions
     * are thrown and caught, so this code forces the setjmp/longjmp
     * implementation to be the old school, non-Altivec versions.
     */
    typedef long *jmp_buf[70];
    extern "C"
    {
        int __setjmp(jmp_buf jmpbuf);
        void longjmp(jmp_buf jmpbuf, int value);
    }

    #define setjmp __setjmp
#else
    #include <setjmp.h>
#endif

#ifdef __GNUC__
// don't force inlining when debugging, just causes pain.
#  ifdef DEBUG
#    define REALLY_INLINE inline
#  else
#    define REALLY_INLINE inline __attribute__((always_inline))
#  endif

// Bug 569361.  See notes for NO_INLINE in VMPI.h
#  define NO_INLINE __attribute__((noinline))

// only define FASTCALL for x86-32; other gcc versions will spew warnings
#  ifdef AVMPLUS_IA32
#    ifndef VMCFG_AOT // Doesn't work with llvm compiler (need a better symbol for this, but don't know one)
#      define FASTCALL __attribute__((fastcall))
#    endif
#  endif
#endif // __GNUC__

#if defined(__GNUC__)
    #define AVMPLUS_ALIGN8(type) type __attribute__ ((aligned (8)))
    #define AVMPLUS_ALIGN16(type) type __attribute__ ((aligned (16)))
#else
    #error "Unrecognized compiler"
#endif

// "verify" is a Mac thing, it gets in the way of our code
// FIXME: should clean up our code, as this #undef will leak into the AVM embedder's code

#undef verify

/**
* Type defintion for an opaque data type representing platform-defined spin lock
* @see VMPI_lockInit(), VMPI_lockAcquire()
*/
struct vmpi_spin_lock_t
{
    volatile OSSpinLock lock;
#ifdef DEBUG
    pthread_t ownerThread;
#endif
};

REALLY_INLINE void VMPI_lockInit(vmpi_spin_lock_t* lock)
{
    lock->lock = OS_SPINLOCK_INIT;
#ifdef DEBUG
    lock->ownerThread = NULL;
#endif
}

REALLY_INLINE void VMPI_lockDestroy(vmpi_spin_lock_t* lock)
{
    lock->lock = OS_SPINLOCK_INIT;
#ifdef DEBUG
    lock->ownerThread = NULL;
#endif
}

REALLY_INLINE bool VMPI_lockAcquire(vmpi_spin_lock_t* lock)
{
#ifdef DEBUG
    if(!::OSSpinLockTry((OSSpinLock*)&lock->lock)) {
        // deadlock assert
        assert(lock->ownerThread != pthread_self());
        ::OSSpinLockLock((OSSpinLock*)&lock->lock);
    }
    lock->ownerThread = pthread_self();
#else
    ::OSSpinLockLock((OSSpinLock*)&lock->lock);
#endif
    return true;
}

REALLY_INLINE bool VMPI_lockRelease(vmpi_spin_lock_t* lock)
{
#ifdef DEBUG
    lock->ownerThread = NULL;
#endif
    ::OSSpinLockUnlock((OSSpinLock*)&lock->lock);
    return true;
}

REALLY_INLINE bool VMPI_lockTestAndAcquire(vmpi_spin_lock_t* lock)
{
    if(::OSSpinLockTry((OSSpinLock*)&lock->lock))
    {
#ifdef DEBUG
        assert(lock->ownerThread == NULL);
        lock->ownerThread = pthread_self();
#endif
        return true;
    }
    return false;
}

REALLY_INLINE int32_t VMPI_atomicIncAndGet32WithBarrier(volatile int32_t* value)
{
#if MACOSX_DEPLOYMENT_TARGET == MACOSX_DEPLOYMENT_TARGET_10_4
    return OSAtomicIncrement32Barrier(const_cast<int32_t*>(value));
#else
    return OSAtomicIncrement32Barrier(value);
#endif
}

REALLY_INLINE int32_t VMPI_atomicIncAndGet32(volatile int32_t* value)
{
#if MACOSX_DEPLOYMENT_TARGET == MACOSX_DEPLOYMENT_TARGET_10_4
    return OSAtomicIncrement32(const_cast<int32_t*>(value));
#else
    return OSAtomicIncrement32(value);
#endif
}

REALLY_INLINE int32_t VMPI_atomicDecAndGet32WithBarrier(volatile int32_t* value)
{
#if MACOSX_DEPLOYMENT_TARGET == MACOSX_DEPLOYMENT_TARGET_10_4
    return OSAtomicDecrement32Barrier(const_cast<int32_t*>(value));
#else
    return OSAtomicDecrement32Barrier(value);
#endif
}

REALLY_INLINE int32_t VMPI_atomicDecAndGet32(volatile int32_t* value)
{
#if MACOSX_DEPLOYMENT_TARGET == MACOSX_DEPLOYMENT_TARGET_10_4
    return OSAtomicDecrement32(const_cast<int32_t*>(value));
#else
    return OSAtomicDecrement32(value);
#endif
}

REALLY_INLINE bool VMPI_compareAndSwap32(int32_t oldValue, int32_t newValue, volatile int32_t* address)
{
#if MACOSX_DEPLOYMENT_TARGET == MACOSX_DEPLOYMENT_TARGET_10_4
    return OSAtomicCompareAndSwap32(oldValue, newValue, const_cast<int32_t*>(address));
#else
    return OSAtomicCompareAndSwap32(oldValue, newValue, address);
#endif
}

REALLY_INLINE bool VMPI_compareAndSwap32WithBarrier(int32_t oldValue, int32_t newValue, volatile int32_t* address)
{
#if MACOSX_DEPLOYMENT_TARGET == MACOSX_DEPLOYMENT_TARGET_10_4
    return OSAtomicCompareAndSwap32Barrier(oldValue, newValue, const_cast<int32_t*>(address));
#else
    return OSAtomicCompareAndSwap32Barrier(oldValue, newValue, address);
#endif
}

REALLY_INLINE void VMPI_memoryBarrier()
{
    OSMemoryBarrier();
}

REALLY_INLINE int VMPI_processorQtyAtBoot()
{
    size_t len = 0;
    int num = 0;
    int mib[] = {CTL_HW, HW_NCPU};
    sysctl(mib, 2, NULL, &len, NULL, 0);
    sysctl(mib, 2, &num, &len, NULL, 0);
    // May be unreliable, but we know we have at least one processor
    return num < 1 ? 1 : num;
}

REALLY_INLINE void VMPI_spinloopPause()
{
#ifdef AVMPLUS_IA32
    __asm__("pause");
#endif
}

REALLY_INLINE vmpi_thread_t VMPI_nullThread()
{
    return NULL;
}

#include "../VMPI/ThreadsPosix-inlines.h"

#endif // __avmplus_mac_platform__

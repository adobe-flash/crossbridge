/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_symbian_platform__
#define __avmplus_symbian_platform__

#define VMPI_memcpy     ::memcpy
#define VMPI_memset         ::memset
#define VMPI_memcmp         ::memcmp
#define VMPI_memmove        ::memmove
#define VMPI_memchr         ::memchr
#define VMPI_strcmp         ::strcmp
#define VMPI_strcat         ::strcat
#define VMPI_strchr         ::strchr
//#define VMPI_strrchr      ::strrchr   not called by avm
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
//#define VMPI_sscanf       ::sscanf    not called by avm
//#define VMPI_atoi ::atoi              not called by avm
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
#define VMPI_setjmpNoUnwind ::setjmp

// Jump to an active jmp_buf that was set up by VMPI_setjmpNoUnwind.
// Under no circumstances may C++ destructors be unwound during the
// jump (MSVC likes to do this by default).
#define VMPI_longjmpNoUnwind ::longjmp

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <ctype.h>
#include <limits.h>

#include <inttypes.h>
#include <e32std.h>
#include <e32cmn.h>

#include <setjmp.h> // for OOM.h
#include <pthread.h>
#include <new> // for std::bad_alloc definition in GCGlobalNew.h

#define REALLY_INLINE inline

#ifdef __WINSCW__
#define DISABLE_STATIC_PCRECONTEXT
#undef _WIN32
#undef __MWERKS__
#endif

#ifdef MMGC_OVERRIDE_GLOBAL_NEW
    #error "configuration not supported"
#endif

#if defined(__ARMCC__)
    #define AVMPLUS_ALIGN8(type) type __attribute__ ((aligned (8)))
    #define AVMPLUS_ALIGN16(type) type __attribute__ ((aligned (16)))
#else
    // @todo implement me
    #define AVMPLUS_ALIGN8(type) type
    #define AVMPLUS_ALIGN16(type) type
#endif

typedef pthread_t vmpi_thread_t;

REALLY_INLINE vmpi_thread_t VMPI_nullThread()
{
    return NULL;
}

/**
* Type defintion for an opaque data type representing platform-defined spin lock
* @see VMPI_lockInit(), VMPI_lockAcquire()
*/
struct vmpi_spin_lock_t
{
    // Looks like Symbian SDK does not support pthread spinlock.
    // Using pthread_mutex for now (unfortunate since it's usually more expensive).
    volatile pthread_mutex_t lock;
};

REALLY_INLINE void VMPI_lockInit(vmpi_spin_lock_t* lock)
{
    pthread_mutex_init((pthread_mutex_t*)&lock->lock, 0);
}

REALLY_INLINE void VMPI_lockDestroy(vmpi_spin_lock_t *lock)
{
	// Avm can call VMPI_lockDestroy before calling VMPI_lockRelease.
	if(pthread_mutex_trylock((pthread_mutex_t*)&lock->lock) == EBUSY)
	{
		pthread_mutex_unlock((pthread_mutex_t*)&lock->lock);
	}
	pthread_mutex_destroy((pthread_mutex_t*)&lock->lock);
}

REALLY_INLINE bool VMPI_lockAcquire(vmpi_spin_lock_t *lock)
{
	// It's allowed that different threads try to lock the same lock.
	int ret = 0;
	int counter = 0;
	while (true)
	{
		ret = pthread_mutex_trylock( (pthread_mutex_t*)&lock->lock ); 
		if(ret == 0 || ret != EBUSY) // some bad error
			break;		
		counter = (counter++) & 63;
		if(counter == 0)
			sched_yield();
	}
	return ret == 0;
}

REALLY_INLINE bool VMPI_lockRelease(vmpi_spin_lock_t *lock)
{
    return pthread_mutex_unlock((pthread_mutex_t*)&lock->lock) == 0;
}

REALLY_INLINE bool VMPI_lockTestAndAcquire(vmpi_spin_lock_t *lock)
{
    return pthread_mutex_trylock((pthread_mutex_t*)&lock->lock) == 0;
}

REALLY_INLINE int VMPI_processorQtyAtBoot()
{
    return 1;
}

REALLY_INLINE void VMPI_spinloopPause()
{
    // No-op
}

#define EMULATE_ATOMICS_WITH_PTHREAD_MUTEX

#include "../VMPI/ThreadsPosix-inlines.h"

#endif // __avmplus_symbian_platform__

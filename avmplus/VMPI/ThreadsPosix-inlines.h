/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <assert.h>

typedef pthread_mutex_t        vmpi_mutex_t;
typedef pthread_cond_t         vmpi_condvar_t;
typedef void*                  vmpi_thread_arg_t; // Argument type for thread start function
typedef void*                  vmpi_thread_rtn_t; // Return type for thread start function
typedef vmpi_thread_rtn_t (*vmpi_thread_start_t)(vmpi_thread_arg_t);
typedef pthread_attr_t         vmpi_thread_attr_t;

#define VMPI_THREAD_START_CC

REALLY_INLINE bool VMPI_recursiveMutexInit(vmpi_mutex_t* mutex)
{
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) == 0) {
        if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0) {
            return pthread_mutex_init(mutex, &attr) == 0;
        }
    }
    return false;
}

REALLY_INLINE bool VMPI_recursiveMutexDestroy(vmpi_mutex_t* mutex)
{
    return pthread_mutex_destroy(mutex) == 0;
}

REALLY_INLINE void VMPI_recursiveMutexLock(vmpi_mutex_t* mutex)
{
    pthread_mutex_lock(mutex);
}

REALLY_INLINE bool VMPI_recursiveMutexTryLock(vmpi_mutex_t* mutex)
{
    return pthread_mutex_trylock(mutex) == 0;
}

REALLY_INLINE void VMPI_recursiveMutexUnlock(vmpi_mutex_t* mutex)
{
    pthread_mutex_unlock(mutex);
}

REALLY_INLINE bool VMPI_condVarInit(vmpi_condvar_t* condvar)
{
    return pthread_cond_init(condvar, NULL) == 0;
}

REALLY_INLINE bool VMPI_condVarDestroy(vmpi_condvar_t* condvar)
{
    return pthread_cond_destroy(condvar) == 0;
}

REALLY_INLINE vmpi_thread_t VMPI_currentThread()
{
    vmpi_thread_t rtn = pthread_self();
    // Bugzilla 656008: double-checking VMPI_nullThread; must not collide with thread id of self
    assert(rtn != VMPI_nullThread());
    return rtn;
}

REALLY_INLINE bool VMPI_tlsSetValue(uintptr_t tlsId, void* value)
{
    return pthread_setspecific((pthread_key_t)tlsId, value) == 0;
}

REALLY_INLINE void* VMPI_tlsGetValue(uintptr_t tlsId)
{
    return pthread_getspecific((pthread_key_t)tlsId);
}

REALLY_INLINE void VMPI_threadYield()
{
    sched_yield();
}

// This is clearly very slow, and should only be used for testing purposes.
#ifdef EMULATE_ATOMICS_WITH_PTHREAD_MUTEX

static pthread_mutex_t atomicsLock = PTHREAD_MUTEX_INITIALIZER;

REALLY_INLINE int32_t VMPI_atomicIncAndGet32WithBarrier(volatile int32_t* value)
{
    pthread_mutex_lock(&atomicsLock);
    int32_t result = *value + 1;
    *value = result;
    pthread_mutex_unlock(&atomicsLock);
    return result;
}

REALLY_INLINE int32_t VMPI_atomicIncAndGet32(volatile int32_t* value)
{
    return VMPI_atomicIncAndGet32WithBarrier(value);
}

REALLY_INLINE int32_t VMPI_atomicDecAndGet32WithBarrier(volatile int32_t* value)
{
    pthread_mutex_lock(&atomicsLock);
    int32_t result = *value - 1;
    *value = result;
    pthread_mutex_unlock(&atomicsLock);
    return result;
}

REALLY_INLINE int32_t VMPI_atomicDecAndGet32(volatile int32_t* value)
{
    return VMPI_atomicDecAndGet32WithBarrier(value);
}

REALLY_INLINE bool VMPI_compareAndSwap32WithBarrier(int32_t oldValue, int32_t newValue, volatile int32_t* address)
{
    bool swapped;
    pthread_mutex_lock(&atomicsLock);
    if (*address == oldValue) {
        *address = newValue;
        swapped = true;
    } else {
        swapped = false;
    }
    pthread_mutex_unlock(&atomicsLock);
    return swapped;
}

REALLY_INLINE bool VMPI_compareAndSwap32(int32_t oldValue, int32_t newValue, volatile int32_t* address)
{
    return VMPI_compareAndSwap32WithBarrier(oldValue, newValue, address);
}

REALLY_INLINE void VMPI_memoryBarrier()
{
    pthread_mutex_lock(&atomicsLock);
    pthread_mutex_unlock(&atomicsLock);
}
#endif

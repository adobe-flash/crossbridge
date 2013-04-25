/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VMAssert.h"
#include "VMPI.h"

#include <pthread.h>

bool VMPI_tlsCreate(uintptr_t* tlsId)
{
    pthread_key_t key;
    const int r = pthread_key_create(&key, NULL);

    if(r == 0)
    {
        // we expect the value to default to zero
        assert(pthread_getspecific(key) == 0);
        *tlsId = (uintptr_t) key;
        return true;
    }

    return false;
}

void VMPI_tlsDestroy(uintptr_t tlsId)
{
    pthread_key_delete((pthread_key_t)tlsId);
}

bool VMPI_threadCreate(vmpi_thread_t* thread, vmpi_thread_attr_t* attr, vmpi_thread_start_t start_fn, vmpi_thread_arg_t arg)
{
    bool rtn = pthread_create(thread, attr, start_fn, arg) == 0;
    // Bugzilla 656008: double-checking VMPI_nullThread; must not collide with id of fresh thread
    assert(*thread != VMPI_nullThread());
    return rtn;
}

bool VMPI_threadDetach(vmpi_thread_t thread)
{
    return pthread_detach(thread) == 0;
}

void VMPI_threadSleep(int32_t timeout_millis)
{
    // We emulate sleeping by performing a timed wait
    // on a stack-local mutex and condition variable.
    vmpi_mutex_t mutex;
    vmpi_condvar_t condvar;
    VMPI_recursiveMutexInit(&mutex);
    VMPI_condVarInit(&condvar);
    VMPI_recursiveMutexLock(&mutex);
    VMPI_condVarTimedWait(&condvar, &mutex, timeout_millis);
    VMPI_recursiveMutexUnlock(&mutex);
    VMPI_recursiveMutexDestroy(&mutex);
    VMPI_condVarDestroy(&condvar);
}

void VMPI_threadJoin(vmpi_thread_t thread)
{
    void* value;
    pthread_join(thread, &value);
}

void VMPI_condVarWait(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex)
{
    pthread_cond_wait(condvar, mutex);
}


bool VMPI_condVarTimedWait(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex, int32_t timeout_millis)
{
#if AVMSYSTEM_MAC
    struct timespec timeSpec;
    timeSpec.tv_sec = timeout_millis / 1000;
    timeSpec.tv_nsec = (timeout_millis % 1000) * 1000000;
    return pthread_cond_timedwait_relative_np(condvar, mutex, &timeSpec) == ETIMEDOUT;
#else
    struct timespec timeSpec;
    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);
    timeSpec.tv_sec = timeVal.tv_sec + (timeout_millis / 1000);
    timeSpec.tv_nsec = (timeVal.tv_usec * 1000) + ((timeout_millis % 1000) * 1000000);
    return pthread_cond_timedwait(condvar, mutex, &timeSpec) == ETIMEDOUT;
#endif
}

void VMPI_condVarBroadcast(vmpi_condvar_t* condvar)
{
    pthread_cond_broadcast(condvar);
}

void VMPI_condVarSignal(vmpi_condvar_t* condvar)
{
    pthread_cond_signal(condvar);
}

bool VMPI_threadAttrInit(vmpi_thread_attr_t* attr)
{
    return pthread_attr_init(attr) == 0;
}

bool VMPI_threadAttrDestroy(vmpi_thread_attr_t* attr)
{
    return pthread_attr_destroy(attr) == 0;
}

bool VMPI_threadAttrSetGuardSize(vmpi_thread_attr_t* attr, size_t size)
{
    #ifdef __CYGWIN__
        fprintf(stderr, "unimplmented: VMPI_threadAttrSetGuardSize\n");
        abort();
        (void) attr;
        (void) size;
    #else
    return pthread_attr_setguardsize(attr, size) == 0;
    #endif
}

bool VMPI_threadAttrSetStackSize(vmpi_thread_attr_t* attr, size_t size)
{
    return pthread_attr_setstacksize(attr, size) == 0;
}

void VMPI_threadAttrSetPriorityLow(vmpi_thread_attr_t* attr)
{
    (void)attr;
    // pthreads don't have priorities at the PTHREAD_SCOPE_SYSTEM scope.
}

void VMPI_threadAttrSetPriorityNormal(vmpi_thread_attr_t* attr)
{
    (void)attr;
    // pthreads don't have priorities at the PTHREAD_SCOPE_SYSTEM scope.
}

void VMPI_threadAttrSetPriorityHigh(vmpi_thread_attr_t* attr)
{
    (void)attr;
    // pthreads don't have priorities at the PTHREAD_SCOPE_SYSTEM scope.
}

size_t VMPI_threadAttrDefaultGuardSize()
{
    #ifdef __CYGWIN__
        fprintf(stderr, "unimplemented: VMPI_threadAttrDefaultGuardSize\n");
        abort();
    #else
    pthread_attr_t attr;
    size_t size;
    pthread_attr_init(&attr);
    pthread_attr_getguardsize(&attr, &size);
    return size;
    #endif
}

#ifndef __CYGWIN__
// in MMgcPortUnix.cpp for Cygwin
size_t VMPI_threadAttrDefaultStackSize()
{
    pthread_attr_t attr;
    size_t size;
    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &size);
    return size;
}
#endif

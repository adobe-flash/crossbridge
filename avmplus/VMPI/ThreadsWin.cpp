/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VMPI.h"
#include "VMAssert.h"

#ifndef TLS_OUT_OF_INDEXES
    #define TLS_OUT_OF_INDEXES (DWORD)0xFFFFFFFF
#endif

bool VMPI_tlsCreate(uintptr_t* tlsId)
{
    DWORD id = TlsAlloc();
    *tlsId = (uintptr_t)id;
    return (id != TLS_OUT_OF_INDEXES);
}

void VMPI_tlsDestroy(uintptr_t tlsId)
{
    TlsFree((DWORD)tlsId);
}

bool VMPI_threadCreate(vmpi_thread_t* thread, vmpi_thread_attr_t* attr, vmpi_thread_start_t start_fn, vmpi_thread_arg_t arg)
{
    DWORD id;
    SIZE_T stackSize = attr == NULL || attr->stackSize == VMPI_threadAttrDefaultStackSize() ? 0 : attr->stackSize;
    HANDLE threadHandle = CreateThread(NULL, stackSize, start_fn, arg, 0, &id);
    if (threadHandle) {
        *thread = id;
        CloseHandle(threadHandle);
        if (attr != NULL && attr->priority != THREAD_PRIORITY_NORMAL) {
            SetThreadPriority(threadHandle, attr->priority);
        }
        // The SetThreadStackGuarantee API is not available in all versions of Windows we care about,
        // so we can't honor any specific guard size request.
        return true;
    } else {
        return false;
    }
}

bool VMPI_threadDetach(vmpi_thread_t thread)
{
    // Not applicable
    (void)thread;
    return true;
}

void VMPI_threadSleep(int32_t timeout_millis)
{
    Sleep(timeout_millis);
}

#ifdef UNDER_CE

void VMPI_threadJoin(vmpi_thread_t thread)
{
    (void)thread;
    AvmAssertMsg(false, "Not supported under CE");
}

void VMPI_condVarWait(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex)
{
    (void)condvar;
    (void)mutex;
    AvmAssertMsg(false, "Not supported under CE");
}

bool VMPI_condVarTimedWait(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex, int32_t timeout_millis)
{
    (void)condvar;
    (void)mutex;
    (void)timeout_millis;
    AvmAssertMsg(false, "Not supported under CE");
    return false;
}

void VMPI_condVarBroadcast(vmpi_condvar_t* condvar)
{
    (void)condvar;
    AvmAssertMsg(false, "Not supported under CE");
}

void VMPI_condVarSignal(vmpi_condvar_t* condvar)
{
    (void)condvar;
    AvmAssertMsg(false, "Not supported under CE");
}

#else

void VMPI_threadJoin(vmpi_thread_t thread)
{
    HANDLE threadHandle = OpenThread(STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3FF, false, thread);
    if (threadHandle) {
        WaitForSingleObject(threadHandle, INFINITE);
        CloseHandle(threadHandle);
    }
}

bool VMPI_condVarTimedWaitInternal(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex, int32_t timeout_millis, bool infinite)
{

    // We use the same emulation technique as FP.
    // Although post-notify scheduling is not exactly fair, the algorithm is simple.
    // If we want something fairer then use Alexander Terekhov's 8a algorithm.

    // Add the calling thread to the condition variable's list of waiting threads.
    // Note that the linked list is implicitly guarded by the mutex paired with the conditional variable.
    int32_t timeout = infinite ? INFINITE : timeout_millis;
    WaitingThread thisThread;
    thisThread.threadID = GetCurrentThreadId();
    thisThread.notified = timeout == 0;
    thisThread.next = NULL;
    if (timeout != 0) {
        if (condvar->head == NULL) {
            condvar->head = &thisThread;
        } else {
            condvar->tail->next = &thisThread;
        }
        condvar->tail = &thisThread;
    }
    // Unlock the mutex and then sleep until we are notified or timeout.
    // Note that there should be *no* lost wakeups between the unlock and the sleep, as all notifies are
    // queued as APCs.
    LeaveCriticalSection(mutex);
    DWORD rtn = 0;
    if (timeout == 0) {
        SwitchToThread();
    }
    else  {
        rtn = SleepEx(timeout, true);
    }
    // As per pthread condition variable semantics, we block until we can re-acquire the mutex we released.
    EnterCriticalSection(mutex);

    if (!thisThread.notified) {
        // A spurious or time-out wake-up, so the thread must remove itself from the condvar's wait list.
        WaitingThread* waiter = condvar->head;
        WaitingThread* prev = NULL;
        do {
            if (waiter == &thisThread) {
                if (prev == NULL) {
                    condvar->head = waiter->next;
                } else {
                    prev->next = waiter->next;
                }
                if (waiter->next == NULL) {
                    condvar->tail = prev;
                }
                return rtn == 0;
            }
            prev = waiter;
            waiter = waiter->next;
        } while(waiter);
		assert(!"Current thread not found in list of waiters");
	}
    return rtn == 0;
}

void VMPI_condVarWait(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex)
{
    VMPI_condVarTimedWaitInternal(condvar, mutex, 0, true);
}

bool VMPI_condVarTimedWait(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex, int32_t timeout_millis)
{
    return VMPI_condVarTimedWaitInternal(condvar, mutex, timeout_millis, false);
}

// A private, dummy callback for the QueueUserAPC implementation of VMPI_condVarBroadcast/Signal
void WINAPI dummyAPC(ULONG_PTR) {
    // Nothing to do
}

void VMPI_condVarBroadcast(vmpi_condvar_t* condvar)
{
    // Signal the whole list.
    // Note that the list is implicitly guarded by the mutex paired with the conditional variable
    // (which the calling thread holds).
    WaitingThread* waiter = condvar->head;
    condvar->head = NULL;
    condvar->tail = NULL;
    while (waiter != NULL) {
        waiter->notified = true;
        HANDLE threadHandle = OpenThread(STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3FF, false, waiter->threadID);
        // Signal the thread by queuing a dummy APC.
        QueueUserAPC(dummyAPC, threadHandle, NULL);
        CloseHandle(threadHandle);
        waiter = waiter->next;
    }
}

void VMPI_condVarSignal(vmpi_condvar_t* condvar)
{
    // Signal the head of the list. FIFO looks like it should be fair, but it's not really.
    // Note that the list is implicitly guarded by the mutex paired with the conditional variable
    // (which the calling thread holds).
    WaitingThread* waiter = condvar->head;
    if (waiter != NULL) {
        waiter->notified = true;
        condvar->head = waiter->next;
        if (condvar->head == NULL) {
            condvar->tail = NULL;
        }
        HANDLE threadHandle = OpenThread(STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3FF, false, waiter->threadID);
        assert(threadHandle != NULL);
        // Signal the thread by queuing a dummy APC.
        DWORD result = QueueUserAPC(dummyAPC, threadHandle, NULL);
        (void)result;
        assert(result > 0);
        CloseHandle(threadHandle);
    }
}

#endif // UNDER_CE

bool VMPI_threadAttrInit(vmpi_thread_attr_t* attr)
{
    attr->stackSize = VMPI_threadAttrDefaultStackSize();
    attr->guardSize = VMPI_threadAttrDefaultGuardSize();
    attr->priority = THREAD_PRIORITY_NORMAL;
    return true;
}

bool VMPI_threadAttrDestroy(vmpi_thread_attr_t* attr)
{
    (void)attr;
    return true;
}

bool VMPI_threadAttrSetGuardSize(vmpi_thread_attr_t* attr, size_t size)
{
    attr->guardSize = size;
    return true;
}

bool VMPI_threadAttrSetStackSize(vmpi_thread_attr_t* attr, size_t size)
{
    attr->stackSize = size;
    return true;
}

void VMPI_threadAttrSetPriorityLow(vmpi_thread_attr_t* attr)
{
    attr->priority = THREAD_PRIORITY_BELOW_NORMAL;
}

void VMPI_threadAttrSetPriorityNormal(vmpi_thread_attr_t* attr)
{
    attr->priority = THREAD_PRIORITY_NORMAL;
}

void VMPI_threadAttrSetPriorityHigh(vmpi_thread_attr_t* attr)
{
    attr->priority = THREAD_PRIORITY_ABOVE_NORMAL;
}

size_t VMPI_threadAttrDefaultGuardSize()
{
    // It will be a single page.
    // The SetThreadStackGuarantee API isn't available so we can't change it
    // from the default.
    // http://msdn.microsoft.com/en-us/library/8cxs58a6%28vs.71%29.aspx
    return VMPI_getVMPageSize();
}

size_t VMPI_threadAttrDefaultStackSize()
{
    // FIXME: bug 609822 Is it possible to implement VMPI_threadAttrDefaultStackSize on win32?
    //
    // Stack size is set in the linker. The default is 1MB:
    // http://msdn.microsoft.com/en-us/library/8cxs58a6%28vs.71%29.aspx
    // But can we find this out programmatically? There doesn't appear to be an API:
    // http://msdn.microsoft.com/en-us/library/ms686774%28v=VS.85%29.aspx
    return 1024 * 1024;
}

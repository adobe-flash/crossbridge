/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace vmbase {

#ifdef DEBUG
    REALLY_INLINE RecursiveMutex::RecursiveMutex()
        : m_ownerThreadID((vmpi_thread_t) 0)
        , m_recursionCount(0)
    {
#else
        REALLY_INLINE RecursiveMutex::RecursiveMutex() {
#endif
        VMPI_recursiveMutexInit(&m_mutex);
    }

    REALLY_INLINE RecursiveMutex::~RecursiveMutex()
    {
        assert(m_recursionCount == 0);
        assert(m_ownerThreadID == 0);
        VMPI_recursiveMutexDestroy(&m_mutex);
    }

    REALLY_INLINE bool RecursiveMutex::tryLock()
    {
#ifdef DEBUG
        if (VMPI_recursiveMutexTryLock(&m_mutex)) {
            assert(m_recursionCount >= 0);
            if (m_recursionCount++ == 0) {
                m_ownerThreadID = VMPI_currentThread();
            }
            return true;
        } else {
            return false;
        }
#else
        return VMPI_recursiveMutexTryLock(&m_mutex);
#endif
    }

    REALLY_INLINE void RecursiveMutex::lock()
    {
        VMPI_recursiveMutexLock(&m_mutex);
#ifdef DEBUG
        assert(m_recursionCount >= 0);
        if (m_recursionCount++ == 0) {
            m_ownerThreadID = VMPI_currentThread();
        }
#endif
    }

    REALLY_INLINE void RecursiveMutex::unlock()
    {
#ifdef DEBUG
        assert(m_recursionCount > 0);
        if (--m_recursionCount == 0) {
            m_ownerThreadID = (vmpi_thread_t) 0;
        }
#endif
        VMPI_recursiveMutexUnlock(&m_mutex);
    }

#ifdef DEBUG
    REALLY_INLINE bool RecursiveMutex::isLockedByCurrentThread() const
    {
        return m_ownerThreadID == VMPI_currentThread();
    }
#endif

    REALLY_INLINE ConditionVariable::ConditionVariable()
    {
        VMPI_condVarInit(&m_condVar);
    }

    REALLY_INLINE ConditionVariable::~ConditionVariable()
    {
        VMPI_condVarDestroy(&m_condVar);
    }

    REALLY_INLINE void ConditionVariable::broadcast()
    {
        VMPI_condVarBroadcast(&m_condVar);
    }

    REALLY_INLINE void ConditionVariable::wait(RecursiveMutex& mutex)
    {
#ifdef DEBUG
        assert(mutex.isLockedByCurrentThread());
        assert(mutex.m_recursionCount == 1);
        mutex.m_recursionCount = 0;
        mutex.m_ownerThreadID = (vmpi_thread_t) 0;
        VMPI_condVarWait(&m_condVar, &mutex.m_mutex);
        mutex.m_ownerThreadID = VMPI_currentThread();
        mutex.m_recursionCount = 1;
#else
        VMPI_condVarWait(&m_condVar, &mutex.m_mutex);
#endif
    }

    REALLY_INLINE bool ConditionVariable::wait(RecursiveMutex& mutex, int32_t timeoutMillis)
    {
#ifdef DEBUG
        assert(mutex.isLockedByCurrentThread());
        assert(mutex.m_recursionCount == 1);
        mutex.m_recursionCount = 0;
        mutex.m_ownerThreadID = (vmpi_thread_t) 0;
        bool ret = VMPI_condVarTimedWait(&m_condVar, &mutex.m_mutex, timeoutMillis);
        mutex.m_ownerThreadID = VMPI_currentThread();
        mutex.m_recursionCount = 1;
        return ret;
#else
        return VMPI_condVarTimedWait(&m_condVar, &mutex.m_mutex, timeoutMillis);
#endif
    }

    REALLY_INLINE void ConditionVariable::signal()
    {
        VMPI_condVarSignal(&m_condVar);
    }

    REALLY_INLINE void WaitNotifyMonitor::notify()
    {
        m_condVar.signal();
    }

    REALLY_INLINE void WaitNotifyMonitor::notifyAll()
    {
        m_condVar.broadcast();
    }

#ifndef VMCFG_SAFEPOINTS // See Safepoint-inlines.h for templatized versions
    REALLY_INLINE MutexLocker::MutexLocker(RecursiveMutex& mutex)
        : m_mutex(mutex)
    {
        m_mutex.lock();
    }
    REALLY_INLINE MutexLocker::~MutexLocker()
    {
        m_mutex.unlock();
    }

    REALLY_INLINE MonitorLocker::MonitorLocker(WaitNotifyMonitor& monitor)
        : m_monitor(monitor)
    {
        m_monitor.lock();
    }

    REALLY_INLINE MonitorLocker::~MonitorLocker()
    {
        m_monitor.unlock();
    }

    REALLY_INLINE void MonitorLocker::wait()
    {
        m_monitor.wait();
    }

    REALLY_INLINE bool MonitorLocker::wait(int32_t timeout_millis)
    {
        return m_monitor.wait(timeout_millis);
    }

    REALLY_INLINE void MonitorLocker::notify()
    {
        m_monitor.notify();
    }

    REALLY_INLINE void MonitorLocker::notifyAll()
    {
        m_monitor.notifyAll();
    }
#endif

    REALLY_INLINE AtomicCounter32::AtomicCounter32(int32_t value)
        : m_value(value)
    {
    }

    REALLY_INLINE AtomicCounter32::~AtomicCounter32()
    {
    }

    REALLY_INLINE void AtomicCounter32::set(int32_t value)
    {
        m_value = value;
        MemoryBarrier::readWrite();
    }

    REALLY_INLINE int32_t AtomicCounter32::get() const
    {
        return m_value;
    }

    REALLY_INLINE void AtomicCounter32::inc()
    {
        VMPI_atomicIncAndGet32WithBarrier(&m_value);
    }

    REALLY_INLINE int32_t AtomicCounter32::incAndGet()
    {
        return VMPI_atomicIncAndGet32WithBarrier(&m_value);
    }

    REALLY_INLINE void AtomicCounter32::dec()
    {
        VMPI_atomicDecAndGet32WithBarrier(&m_value);
    }

    REALLY_INLINE int32_t AtomicCounter32::decAndGet()
    {
        return VMPI_atomicDecAndGet32WithBarrier(&m_value);
    }

    /* static */REALLY_INLINE bool AtomicOps::compareAndSwap32(int32_t oldValue, int32_t newValue, volatile int32_t* address)
    {
        return VMPI_compareAndSwap32(oldValue, newValue, address);
    }

    /* static */REALLY_INLINE bool AtomicOps::compareAndSwap32WithBarrier(int32_t oldValue, int32_t newValue, volatile int32_t* address)
    {
        return VMPI_compareAndSwap32WithBarrier(oldValue, newValue, address);
    }

    /* static */ REALLY_INLINE int32_t AtomicOps::compareAndSwap32WithBarrierPrev(int32_t expected, int32_t next, volatile int32_t* wordptr)
    {
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
        uint32_t val;
        __asm__ __volatile__("lock; cmpxchg %1, (%2);"
                             :"=a"(val)
                             :"r"(next), "r"(wordptr), "a"(expected)
                             :"memory", "cc");
        return val;
#elif defined(_MSC_VER) 
        // we know that we are dealing with only 32 bit word here and a 32 bit compiler option
        return ::InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(wordptr), next, expected);
#else
        //  THIS IS WRONG!!!!
        //  the "val" needs to be atomically obtained from "compareAndSwap32WithBuffer"
        int32_t val = *wordptr;
        if (!compareAndSwap32WithBarrier(expected, next, wordptr))
        {
            return val;
        }
        return expected;
#endif

    }

    /* static */REALLY_INLINE int32_t AtomicOps::or32(uint32_t mask, volatile int32_t* address)
    {
        int32_t oldValue;
        int32_t newValue;
        do {
            oldValue = *address;
            newValue = oldValue | mask;
        } while (!compareAndSwap32(oldValue, newValue, address));
        return newValue;
    }

    /* static */REALLY_INLINE int32_t AtomicOps::or32WithBarrier(uint32_t mask, volatile int32_t* address)
    {
        int32_t oldValue;
        int32_t newValue;
        do {
            oldValue = *address;
            newValue = oldValue | mask;
        } while (!compareAndSwap32WithBarrier(oldValue, newValue, address));
        return newValue;
    }

    /* static */REALLY_INLINE int32_t AtomicOps::and32(uint32_t mask, volatile int32_t* address)
    {
        int32_t oldValue;
        int32_t newValue;
        do {
            oldValue = *address;
            newValue = oldValue & mask;
        } while (!compareAndSwap32(oldValue, newValue, address));
        return newValue;
    }

    /* static */REALLY_INLINE int32_t AtomicOps::and32WithBarrier(uint32_t mask, volatile int32_t* address)
    {
        int32_t oldValue;
        int32_t newValue;
        do {
            oldValue = *address;
            newValue = oldValue & mask;
        } while (!compareAndSwap32WithBarrier(oldValue, newValue, address));
        return newValue;
    }

    /* static */REALLY_INLINE int32_t AtomicOps::or32Prev(uint32_t mask, volatile int32_t* address)
    {
        int32_t oldValue;
        int32_t newValue;
        do {
            oldValue = *address;
            newValue = oldValue | mask;
        } while (!compareAndSwap32(oldValue, newValue, address));
        return oldValue;
    }

    /* static */REALLY_INLINE int32_t AtomicOps::or32WithBarrierPrev(uint32_t mask, volatile int32_t* address)
    {
        int32_t oldValue;
        int32_t newValue;
        do {
            oldValue = *address;
            newValue = oldValue | mask;
        } while (!compareAndSwap32WithBarrier(oldValue, newValue, address));
        return oldValue;
    }

    /* static */REALLY_INLINE int32_t AtomicOps::and32Prev(uint32_t mask, volatile int32_t* address)
    {
        int32_t oldValue;
        int32_t newValue;
        do {
            oldValue = *address;
            newValue = oldValue & mask;
        } while (!compareAndSwap32(oldValue, newValue, address));
        return oldValue;
    }

    /* static */REALLY_INLINE int32_t AtomicOps::and32WithBarrierPrev(uint32_t mask, volatile int32_t* address)
    {
        int32_t oldValue;
        int32_t newValue;
        do {
            oldValue = *address;
            newValue = oldValue & mask;
        } while (!compareAndSwap32WithBarrier(oldValue, newValue, address));
        return oldValue;
    }

    /* static */REALLY_INLINE void MemoryBarrier::readWrite()
    {
        VMPI_memoryBarrier();
    }

    /* static */REALLY_INLINE VMThread* VMThread::currentThread()
    {
        return VMThread::m_currentThread.get();
    }

} // namespace vmbase

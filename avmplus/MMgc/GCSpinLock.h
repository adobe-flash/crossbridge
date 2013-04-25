/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCSpinLock__
#define __GCSpinLock__

namespace MMgc
{
    /**
     * GCAcquireSpinlock is a convenience class which acquires
     * the specified spinlock at construct time, then releases
     * the spinlock at desruct time.  The single statement
     *
     *    GCAcquireSpinlock acquire(spinlock);
     *
     * ... will acquire the spinlock at the top of the function
     * and release it at the end.  This makes for less error-prone
     * code than explicit acquire/release.
     */
    class GCAcquireSpinlock
    {
    public:
        REALLY_INLINE explicit GCAcquireSpinlock(vmpi_spin_lock_t *spinlock) :
            m_spinlock(spinlock)
        {
        #ifdef _DEBUG
            bool r =
        #endif
            VMPI_lockAcquire(m_spinlock);

            GCAssert(r);
        }

        REALLY_INLINE ~GCAcquireSpinlock()
        {
        #ifdef _DEBUG
            bool r =
        #endif
            VMPI_lockRelease(m_spinlock);

            GCAssert(r);
        }

    private:
        vmpi_spin_lock_t *m_spinlock;

    private: // not implemented
        GCAcquireSpinlock();
        GCAcquireSpinlock(const GCAcquireSpinlock&);
        GCAcquireSpinlock& operator=(const GCAcquireSpinlock&);
    };

    class GCAcquireSpinlockWithRecursion
    {
    public:
        REALLY_INLINE explicit GCAcquireSpinlockWithRecursion(vmpi_spin_lock_t *sl, vmpi_thread_t owner)
            : m_spinlock(sl)
        {
            if(!VMPI_lockTestAndAcquire(sl)) {
                if(VMPI_currentThread() == owner) {
                    m_spinlock = NULL;
                } else {
                    VMPI_lockAcquire(sl);
                }
            }
        }

        ~GCAcquireSpinlockWithRecursion()
        {
            if(m_spinlock)
                VMPI_lockRelease(m_spinlock);
        }

    private:
        vmpi_spin_lock_t *m_spinlock;

    private: // not implemented
        GCAcquireSpinlockWithRecursion();
        GCAcquireSpinlockWithRecursion(const GCAcquireSpinlockWithRecursion&);
        GCAcquireSpinlockWithRecursion& operator=(const GCAcquireSpinlockWithRecursion&);
    };
}

#endif /* __GCSpinLock__ */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vmbase.h"

#ifdef VMCFG_SAFEPOINTS

namespace vmbase {

// The number of spins a safepoint-task requesting thread will busy-wait
// until yielding. (Only applicable on MP machines)
#define SAFEPOINT_SPINS 128

    VMThreadLocal<SafepointRecord*> SafepointRecord::m_current;

    SafepointRecord::~SafepointRecord()
    {
        assert(m_status == SP_UNSAFE && m_manager == NULL);
    }

    SafepointManager::SafepointManager()
        : m_records(NULL)
        , m_requester((vmpi_thread_t) 0)
        , m_hardwareConcurrency(VMPI_processorQtyAtBoot())
    {
    }

    SafepointManager::~SafepointManager()
    {
        assert(m_records == NULL);
    }

    void SafepointManager::requestSafepointTask(SafepointTask& task)
    {
        assert(SafepointRecord::hasCurrent());
        assert(SafepointRecord::current()->m_manager == this);
        assert(!inSafepointTask());

        // Serialize dispatch of safepoint tasks.
        SCOPE_LOCK_SP_NAMED(locker, m_requestMutex) {
            bool restart = false;
            const volatile SafepointRecord* safepointRecord = NULL;
            do {
				SafepointRecord* myRecord = SafepointRecord::current();

				myRecord->m_status = SafepointRecord::SP_SAFE;
                // Threads polling this SafepointManager will pass through a
                // SafepointGate when they see m_requester as non-null.
                
                m_requester = VMPI_currentThread();
                
                safepointRecord = m_records;
                // Tell every core to interrupt
                const int SafepointPoll = 3;
                
                while (safepointRecord) {
                    // We do not want to check our own record's interrupt reason
					const bool cIsMyOwnRecord = (safepointRecord->m_isolateDesc == myRecord->m_isolateDesc);
					
                    if (!cIsMyOwnRecord && safepointRecord->m_status == SafepointRecord::SP_UNSAFE && 
						safepointRecord->m_interruptLocation) 
					{
                        int32_t previous = AtomicOps::compareAndSwap32WithBarrierPrev(0, SafepointPoll, safepointRecord->m_interruptLocation);
                        if (previous != 0 && previous != SafepointPoll) {
                            // We noticed an unwinding operation request. 
                            // This is very rare, because the interrupt flag will be cleared promptly.
                            // More likely the target thread will be in the process of unwinding,
                            // and will either reach a safepoint (is that possible while unwinding?) 
                            // or exit the safepoint mgr.

                            // The thread *should* unwind and utlimately exit the safepoint manager.
                            // But it'll try to grab m_requestMutex and deadlock there.
                            // Instead, wait for exit notification then redo the whole iteration,
                            // because the linked list might have changed.
                            
                            // Undo state changes.
                            myRecord->m_status = SafepointRecord::SP_UNSAFE;
                            m_requester = (vmpi_thread_t) 0;
							locker.wait();
                            restart = true;
                            break;
                        } else {
                            // Thread in safepoint, probably because another thread also requested a safepoint
                            // while this thread was sitting in locker.wait()
                        }
                    } else {
                        /*
                        fprintf(stderr, "requested safepoint thru record of worker %d safe? %d\n", safepointRecord->m_isolateDesc, 
                                safepointRecord->m_status == SafepointRecord::SP_SAFE); 
                        */
                    }
                    safepointRecord = safepointRecord->m_managerNext;

                    if (safepointRecord == NULL) {
                        restart = false;
                    }
                }
            } while (restart == true);


            // The current (requesting) thread will busy-wait until it sees all
            // SafepointRecords managed by this SafepointManager as SP_SAFE.
            safepointRecord = m_records;
            if (m_hardwareConcurrency > 1) {
                // MP-machine spin-wait
                while (safepointRecord) {
                    int spinCount = 0;
                    while (safepointRecord->m_status == SafepointRecord::SP_UNSAFE) {
                        if (++spinCount == SAFEPOINT_SPINS) {
                            VMPI_threadYield();
                            spinCount = 0;
                        }
                        VMPI_spinloopPause();
                    }
                    safepointRecord = safepointRecord->m_managerNext;
                }
            } else {
                // UP-machine spin-wait
                while (safepointRecord) {
                    while (safepointRecord->m_status == SafepointRecord::SP_UNSAFE) {
                        VMPI_threadYield();
                    }
                    safepointRecord = safepointRecord->m_managerNext;
                }
            }

            *SafepointRecord::current()->m_interruptLocation = 0;  // AvmCore::NotInterrupted: Don't interrupt oneself.
            // Run the task, now that all threads are safepointed
            task.run();
            // Cleanup.
            // The safepointed threads that trapped into a SafepointGate are blocked on
            // m_requestMutex, so they will be released after the current thread leaves
            // this critical section.
            SafepointRecord::current()->m_status = SafepointRecord::SP_UNSAFE;
            m_requester = (vmpi_thread_t) 0;
        }
    }

    void SafepointManager::enter(SafepointRecord* record)
    {
        assert(record->m_manager == NULL);
        SafepointRecord* stackPrev = SafepointRecord::current();
        if (stackPrev) {
            // This is a nested entry. Make the previous
            // stack region safe up to the new SafepointRecord.
            stackPrev->m_safeRegionEnd = record;
            MemoryBarrier::readWrite();
            stackPrev->m_status = SafepointRecord::SP_SAFE;
            record->m_stackPrev = stackPrev;
        }

        // We do not need a implicit safepoint at this lock as the
        // current thread is safe in the context of the new SafepointRecord
        // (and the previous one, if we set it to SP_SAFE above).
        SCOPE_LOCK_NO_SP_NAMED(locker, m_requestMutex) {
            record->m_managerNext = m_records;
            if (m_records != NULL) {
                m_records->m_managerPrev = record;
            }
            m_records = record;
            record->m_manager = this;
            // We modified the list, make others notice.
            locker.notifyAll();
        }
        SafepointRecord::setCurrent(record);
    }

    void SafepointManager::leave(SafepointRecord* record)
    {
        assert(record->m_manager == this);
        SafepointRecord* const stackPrev = record->m_stackPrev;
        // Remove the SafepointRecord from this SafepointManager.
        SCOPE_LOCK_SP_NAMED(locker, m_requestMutex) {
            if (m_records == record) {
                m_records = m_records->m_managerNext;
            } else {
                record->m_managerPrev->m_managerNext = record->m_managerNext;
            }
            if (record->m_managerNext) {
                record->m_managerNext->m_managerPrev = record->m_managerPrev;
            }
            record->m_manager = NULL;
            locker.notifyAll();
        }

        SafepointRecord::setCurrent(stackPrev);
        if (stackPrev) {
            // We're unwinding into the previous SafepointRecord, so
            // we have to block until no safepoint task is ongoing.
            SCOPE_LOCK_NO_SP_NAMED(locker, stackPrev->m_manager->m_requestMutex) {
                stackPrev->m_status = SafepointRecord::SP_UNSAFE;
                locker.notifyAll();
          }
        }
    }
}

#endif // VMCFG_SAFEPOINTS

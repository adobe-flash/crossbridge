// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gtest/gtest.h>
#include "vmbase.h"

using namespace vmbase;

// We use the same testing method as that for VMPI threads:
// Each construct is tested by (1) using it in the implementation of a mutator that
// modifies a counter for a fixed number of iterations, and then (2) running
// duplicates of that mutator in parallel. The final counter value ends up in
// sharedCounter, which is guarded by m_monitor (except for in CASTest).
// Each test checks that the sharedCounter ends up with a statically determined
// end value.
// (Description courtesy of Felix)

// We need a specific namespace as threads tests use the same test class names
namespace concurrencytests {

    class ThreadTestBase : public Runnable {
        public:
            ThreadTestBase(int iterations) : m_iterations(iterations), sharedCounter(0) {
            }
            virtual ~ThreadTestBase() {
            }

        protected:
            WaitNotifyMonitor m_monitor;
            const int m_iterations;

        public:
            int sharedCounter;
    };

    class TestRunner {
        public:
            struct ThreadRecord {
                VMThread* thread;
                bool startupOk;
            };

            TestRunner(int threadQty, bool doJoin = true) : m_threadQty(threadQty), m_doJoin(doJoin) {
            }
            virtual ~TestRunner() {
            }

            void runTest(ThreadTestBase& test) {

                m_threads = new ThreadRecord[m_threadQty];

                // Start up the threads
                for (int i = 0; i < m_threadQty; i++) {
                    m_threads[i].thread = new VMThread(&test);
                    m_threads[i].startupOk = m_threads[i].thread->start();
                }

                // ...then block until they all terminate
                for (int i = 0; i < m_threadQty; i++) {
                    if (m_doJoin && m_threads[i].startupOk) {
                        m_threads[i].thread->join();
                    }
                    delete m_threads[i].thread;
                }

                delete[] m_threads;
            }

        private:
            const int m_threadQty;
            ThreadRecord* m_threads;
            bool m_doJoin;
    };

    class MutexTest : public ThreadTestBase {
        public:
            MutexTest(int iterations) : ThreadTestBase(iterations) {}
            virtual ~MutexTest() {}

            virtual void run() {
                assert(m_iterations % 2 == 0);
                for (int i = 0; i < m_iterations/2; i++) {
                    SCOPE_LOCK(m_monitor) {
                        SCOPE_LOCK(m_monitor) {
                            sharedCounter++;
                        }
                    }
                }
                for (int i = 0; i < m_iterations/2; i++) {
                    SCOPE_LOCK_NAMED(locker, m_monitor) {
                        SCOPE_LOCK_NAMED(locker, m_monitor) {
                            sharedCounter++;
                        }
                    }
                }
            }
    };
    
    class ConditionTest : public ThreadTestBase {
        public:
            ConditionTest(int iterations, int threadQty) : ThreadTestBase(iterations), m_threadQty(threadQty) {}
            virtual ~ConditionTest() {}

            virtual void run() {

                assert(m_threadQty >= 2);

                for (int i = 0; i < m_iterations; i++) {
                    SCOPE_LOCK_NAMED(locker, m_monitor) {
                        sharedCounter++;
                        // If there's another thread still active then wait.
                        if (m_threadQty > 1) {
                            locker.notify();
                            locker.wait();
                        }
                        // This thread has finished, so let's wake everyone else up
                        if (i == m_iterations - 1) {
                            --m_threadQty;
                            locker.notifyAll();
                        }
                    }

                }

            }

        private:
            int m_threadQty;
    };

    class AtomicCounterTest : public ThreadTestBase {
        public:
            AtomicCounterTest(int iterations, int threadQty) : ThreadTestBase(iterations), m_threadQty(threadQty) {}
            virtual ~AtomicCounterTest() {}

            virtual void run() {

                assert(m_iterations % 4 == 0);

                for (int i = 0; i < m_iterations/4; i++) {
                    m_counter++;
                }
                for (int i = 0; i < m_iterations/4; i++) {
                    m_counter--;
                }
                for (int i = 0; i < m_iterations/4; i++) {
                    ++m_counter;
                }
                for (int i = 0; i < m_iterations/4; i++) {
                    --m_counter;
                }

                SCOPE_LOCK(m_monitor) {
                    if (--m_threadQty == 0) {
                        sharedCounter = m_counter;
                    }
                }
            }
        private:
            AtomicCounter32 m_counter;
            int m_threadQty;
    };

    class CASTest : public ThreadTestBase {
        public:
            CASTest(int iterations, bool withBarrier) : ThreadTestBase(iterations), m_withBarrier(withBarrier) {}
            virtual ~CASTest() {}

            virtual void run() {
                if (m_withBarrier) {
                    for (int i = 0; i < m_iterations; i++) {
                        int32_t current, next;
                        do {
                            current = sharedCounter;
                            next = current + 1;
                        } while (!AtomicOps::compareAndSwap32WithBarrier(current, next, &sharedCounter));
                    }
                } else {
                    for (int i = 0; i < m_iterations; i++) {
                        int32_t current, next;
                        do {
                            current = sharedCounter;
                            next = current + 1;
                        } while (!AtomicOps::compareAndSwap32(current, next, &sharedCounter));
                    }
                }
            }
        private:
            bool m_withBarrier;
    };

    /**
     * We protect a shared counter with a Dekker-style lock that has been made
     * sequentially consistent with memory barriers.
     *
     * The idea is that if the barriers are correct, then two threads can compete
     * to update the counter n times each, so that the final counter value is 2n. If
     * the final value is not 2n, then the barriers have failed to ensure sequential
     * consistency.
     *
     * FIXME: bug 609943
     * This seems way too complicated. We have to be confident in the algorithm
     * before considering the barrier implementations, and I'm not convinced as yet.
     * Is there something simpler?
     * Note that the barriers below are extremely conservative.
     *
     * This is test is not actually run. The verifyPass below just returns true.
     */
    class MemoryBarrierTest : public ThreadTestBase {
        public:
            MemoryBarrierTest(int iterations) : ThreadTestBase(iterations), m_thread0(0), m_thread1(0), m_turn(NULL) {}
            virtual ~MemoryBarrierTest() {}

            virtual void run() {

                volatile int* me;
                volatile int* other;
                volatile int* const counterp = &sharedCounter;

                SCOPE_LOCK(m_monitor) {
                    if (m_turn == NULL) {
                        me = &m_thread0;
                        other = &m_thread1;
                        m_turn = me;
                    } else {
                        me = &m_thread1;
                        other = &m_thread0;
                    }
                }

                for (int i = 0; i < m_iterations; i++) {
                    // Dekker lock
                    *me = 1;
                    MemoryBarrier::readWrite();
                    while (*other == 1) {
                        MemoryBarrier::readWrite();
                        if (m_turn == other) {
                            MemoryBarrier::readWrite();
                            *me = 0;
                            MemoryBarrier::readWrite();
                            while (m_turn == other) {
                                MemoryBarrier::readWrite();
                            }
                            MemoryBarrier::readWrite();
                            *me = 1;
                            MemoryBarrier::readWrite();
                        }
                    }
                    MemoryBarrier::readWrite();
                    (*counterp)++;
                    MemoryBarrier::readWrite();
                    m_turn = other;
                    MemoryBarrier::readWrite();
                    *me = 0;
                    MemoryBarrier::readWrite();
                }
            }
        private:
            volatile int m_thread0;
            volatile int m_thread1;
            volatile int* volatile m_turn;
    };

    class ConditionWithWaitTest : public ThreadTestBase {
        public:
            ConditionWithWaitTest(int iterations) : ThreadTestBase(iterations) {}
            virtual ~ConditionWithWaitTest() {}

            virtual void run() {
                for (int i = 0; i < m_iterations; i++) {
                    SCOPE_LOCK_NAMED(locker, m_monitor) {
                        sharedCounter++;
                        locker.wait(1);
                    }
                }
            }
    };

    class SleepTest : public ThreadTestBase {
        public:
            SleepTest(int iterations) : ThreadTestBase(iterations) {}
            virtual ~SleepTest() {}

            virtual void run() {
                for (int i = 0; i < m_iterations; i++) {
                    SCOPE_LOCK(m_monitor) {
                        sharedCounter++;
                    }
                    VMThread::sleep(1);
                }
            }
    };

    class VMThreadLocalTest : public ThreadTestBase {
        public:
            VMThreadLocalTest(int iterations) : ThreadTestBase(iterations) {}
            virtual ~VMThreadLocalTest() {}

            virtual void run() {
                for (int i = 0; i < m_iterations; i++) {
                    m_localCounter.set(m_localCounter.get() + 1);
                }
                SCOPE_LOCK(m_monitor) {
                    sharedCounter += (int)m_localCounter;
                }
            }
        private:
            VMThreadLocal<uintptr_t> m_localCounter;
    };
}

using namespace concurrencytests;

// This needs to be at least 2 for ConditionTest
#define THREAD_QTY 4
#define ITERATIONS 100000

TEST(vmbaseConcurrency, Mutex) {
    TestRunner runner(THREAD_QTY);
    MutexTest test(ITERATIONS);
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * ITERATIONS, test.sharedCounter);
}

TEST(vmbaseConcurrency, Condition) {
    TestRunner runner(THREAD_QTY);
    ConditionTest test(ITERATIONS, THREAD_QTY);
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * ITERATIONS, test.sharedCounter);
}

TEST(vmbaseConcurrency, AtomicCounter) {
    TestRunner runner(THREAD_QTY);
    AtomicCounterTest test(ITERATIONS, THREAD_QTY);
    runner.runTest(test);
    ASSERT_EQ(0, test.sharedCounter);
}

TEST(vmbaseConcurrency, CAS1) {
    TestRunner runner(THREAD_QTY);
    CASTest test(ITERATIONS, false);
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * ITERATIONS, test.sharedCounter);
}

TEST(vmbaseConcurrency, CAS2) {
    TestRunner runner(THREAD_QTY);
    CASTest test(ITERATIONS, true);
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * ITERATIONS, test.sharedCounter);
}

TEST(vmbaseConcurrency, MemoryBarrier) {
#if 0
    /* This test is failing on Windows and Mac OSX 10.4.
     * For Windows, see bug 609820.
     * For Mac, are the 10.4 APIs not reliable?
     * It could also be the test, or the compiler!
     * FIXME: bug 609943 Selftests to stress memory barriers (fences)
     */
    // Note that the memory barrier test is based on a Dekker lock, so we
    // only ever use 2 threads.
    TestRunner runner(2);
    MemoryBarrierTest test(ITERATIONS);
    runner.runTest(test);
    ASSERT_EQ(2 * ITERATIONS, test.sharedCounter);
#endif
}

TEST(vmbaseConcurrency, ConditionWithWait) {
    TestRunner runner(THREAD_QTY);
    ConditionWithWaitTest test(2000); // Use 2000 iterations with a 1 ms wait
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * 2000, test.sharedCounter);
}

TEST(vmbaseConcurrency, Sleep) {
    TestRunner runner(THREAD_QTY);
    SleepTest test(2000); // Use 2000 iterations with a 1 ms sleep
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * 2000, test.sharedCounter);
}

TEST(vmbaseConcurrency, VMThreadLocal) {
    TestRunner runner(THREAD_QTY);
    VMThreadLocalTest test(ITERATIONS);
    runner.runTest(test);
    // ASSERT What?
}

TEST(vmbaseConcurrency, Mutex2) {
    {
        VMThread vmthread;
    }
    // Run the mutex test but call the VMThread dtors without joining first
    TestRunner runner(THREAD_QTY, false);
    MutexTest test(ITERATIONS);
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * ITERATIONS, test.sharedCounter);
}

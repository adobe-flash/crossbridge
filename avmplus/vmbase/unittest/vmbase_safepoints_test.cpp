// Generated from ST_vmbase_safepoints.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gtest/gtest.h>
#include "vmbase.h"

#ifdef VMCFG_SAFEPOINTS

using namespace vmbase;

namespace safepointtests {

    /**
     * Base class for tests run by a TestRunner.
     * Sub-classes should override runInSafepointManagerContext().
     */
    class SafepointTestBase : public Runnable
    {
    public:
        SafepointTestBase(SafepointManager& safepointManager, int iterations, int threadQty)
            : m_safepointManager(safepointManager)
            , m_iterations(iterations)
            , m_threadQty(threadQty)
            , sharedCounter(0)
        {
        }

        virtual ~SafepointTestBase()
        {
        }

        virtual void run()
        {
            // Enter the safepoint context shared by the primordial and worker threads
            SafepointRecord spRecord;
            m_safepointManager.enter(&spRecord);

            runInSafepointManagerContext();

            // Cleanup
            m_safepointManager.leave(&spRecord);
        }

        // Tests should override this function
        virtual void runInSafepointManagerContext() = 0;

        int threadQty()
        {
            return m_threadQty;
        }

    protected:
        SafepointManager& m_safepointManager;
        WaitNotifyMonitor m_monitor;
        const int m_iterations;
        const int m_threadQty;

    public:
        int sharedCounter;
    };

    /**
     * Creates the given number of VMThreads, and passes each the SafepointTestBase to execute.
     * The TestRunner contains the SafepointManager with which the primordial and each worker
     * thread will register their topmost SafepointRecord.
     */
    class TestRunner
    {
    public:
        struct ThreadRecord
        {
            VMThread* thread;
            bool startupOk;
        };

        TestRunner(bool doJoin = true)
            : m_doJoin(doJoin)
        {
        }

        virtual ~TestRunner()
        {
        }

        void runTest(SafepointTestBase& test)
        {
            // Enter the safepoint context shared by the primordial and worker threads
            SafepointRecord spRecord;
            m_safepointManager.enter(&spRecord);

            m_threads = new ThreadRecord[test.threadQty()];

            // Start up the threads.
            // Each should also enter m_safepointManager
            for (int i = 0; i < test.threadQty(); i++) {
                m_threads[i].thread = new VMThread(&test);
                m_threads[i].startupOk = m_threads[i].thread->start();
            }

            // ...then block until they all terminate.
            // Whilst performing the join, this thread will
            // be implicitly safepointed.
            for (int i = 0; i < test.threadQty(); i++) {
                if (m_doJoin && m_threads[i].startupOk) {
                    m_threads[i].thread->join();
                }
                delete m_threads[i].thread;
            }

            delete[] m_threads;

            // Cleanup
            m_safepointManager.leave(&spRecord);
        }

        SafepointManager& safepointManager()
        {
            return m_safepointManager;
        }

    private:
        ThreadRecord* m_threads;
        bool m_doJoin;
        SafepointManager m_safepointManager;
    };

    /**
     * Threads increment a shared counter.
     * Thread safety is guaranteed by only performing the increment within a safepoint task.
     */
    class SimpleTest : public SafepointTestBase
    {
        class SimpleSafepointTask : public SafepointTask
        {
        public:
            SimpleSafepointTask(int& counter)
                : m_counter(counter)
            {
            }

            void run()
            {
                // We're in the safepoint task, so update
                // the shared counter;
                m_counter++;
            }

        private:
            int& m_counter;
        };

    public:
        SimpleTest(SafepointManager& safepointManager, int iterations, int threadQty)
            : SafepointTestBase(safepointManager, iterations, threadQty)
        {
        }

        virtual ~SimpleTest()
        {
        }

        // Thread entrypoint
        virtual void runInSafepointManagerContext()
        {
            for (int i = 0; i < m_iterations; i++) {
                // Increment shared counter in a safepoint task
                SimpleSafepointTask task(sharedCounter);
                m_safepointManager.requestSafepointTask(task);
            }
        }
    };

    /**
     * Producer-consumer with safepoints.
     *
     * Producer threads each increment a *private* counter in a tight
     * loop which includes a safepoint.
     *
     * A single consumer thread executes a tight loop which continually
     * requests that the producers be safepointed. In the safepoint task
     * the consumer 'steals' and then zeros the private counter of the
     * producers. Each stolen counter value is added to the consumer's
     * private counter.
     *
     * This continues until the producers have executed a fixed number of
     * iterations. At the end of this process the consumer should have
     * a deterministic private counter value.
     *
     */
    class ProducerConsumerTest : public SafepointTestBase
    {
    public:

        class ConsumerSafepointTask : public SafepointTask
        {
        public:
            ConsumerSafepointTask(int* producerCounters, int numProducers)
                : m_producerCounters(producerCounters)
                , m_numProducers(numProducers)
                , m_producerStolenCounter(0)
            {
            }

            void run()
            {
                // We're in the safepoint task, so we can steal and then zero
                // the producers' counters.
                // Note that we don't know how many producer threads are still
                // alive, m_numProducers is just the length of the array
                // we inspect.
                for (int i = 0; i < m_numProducers; i++) {
                    m_producerStolenCounter += m_producerCounters[i];
                    m_producerCounters[i] = 0;
                }
            }

            int producerStolenCounter()
            {
                return m_producerStolenCounter;
            }

        private:
            int* const m_producerCounters;
            const int m_numProducers;
            int m_producerStolenCounter;
        };

    public:
        ProducerConsumerTest(SafepointManager& safepointManager, int iterations, int threadQty)
            : SafepointTestBase(safepointManager, iterations, threadQty)
            , m_producerCounts(new int[threadQty])
        {
            VMPI_memset(m_producerCounts, 0, (threadQty-1) * sizeof m_producerCounts[0]);
        }

        virtual ~ProducerConsumerTest()
        {
        }

        // Thread entrypoint
        virtual void runInSafepointManagerContext()
        {
            // Wait for all the threads to startup to force as much
            // contention as possible.
            const int threadIndex = m_threadIn++;
            SCOPE_LOCK_SP_NAMED(locker, m_monitor) {
                locker.notifyAll();
                while (m_threadIn < m_threadQty) {
                    locker.wait();
                }
            }

            // Setup one of the threads as the consumer, the others are producers
            if (threadIndex == m_threadQty - 1) {
                // The consumer continually makes SafepointTask requests on
                // the SafepointManager, leading to safepointed producers.
                // In each SafepointTask the consumer takes each producer's
                // currently accrued, private counter, and adds it
                // to its own total count, then zeroes the producers counter.
                int consumerCount = 0;
                enum State
                {
                    RUNNING, DRAINING, DONE
                };
                State state = RUNNING;
                do {
                    ConsumerSafepointTask task(m_producerCounts, m_threadQty - 1);
                    m_safepointManager.requestSafepointTask(task);
                    consumerCount += task.producerStolenCounter();
                    if (state == DRAINING) {
                        state = DONE;
                    } else if (m_threadOut == m_threadQty - 1) {
                        state = DRAINING;
                    }
                } while (state != DONE);

                // sharedCounter is verified by the selftest runner
                sharedCounter = consumerCount + m_iterations;

                // Can't do this in the dtor, if the selftest verify fails, it will
                // throw an exception and the dtor will not run.
                delete[] m_producerCounts;
            } else {
                // The producers increment their private counter, but we make sure that they
                // reach an explicit safepoint every iteration.
                for (int i = 0; i < m_iterations; i++) {
                    SAFEPOINT_POLL_FAST(m_safepointManager);
                    m_producerCounts[threadIndex]++;
                    if (i % 100 == 0)
                        VMPI_threadYield(); // Try to allow some interesting interleavings
                }
            }
            m_threadOut++;
        }
    private:
        int* m_producerCounts;
        AtomicCounter32 m_threadIn;
        AtomicCounter32 m_threadOut;
    };

    /**
     * Producer-consumer with nested/recursive (arbitrary depth) SafepointManagers.
     *
     * This test uses the same idea as ProducerConsumerTest, but instead of
     * having only one SafepointManager which is entered by all the producer
     * threads, there are N SafepointManagers which the producers continually
     * nest in and out of. Each producer maintains a private counter for each
     * SafepointManager nesting level, which it only increments when in the
     * context of the corresponding SafepointManager.
     *
     * Similar to the producers, the consumer continually nests in and out of
     * the stack of SafepointManagers. At each nesting level the consumer
     * requests that the producers be safepointed with respect to the
     * SafepointManager at that nesting level. In the safepoint task
     * the consumer 'steals' and then zeros the private counter of the
     * producers for that level. Each stolen counter value is added to
     * the consumer's private counter.
     *
     * This continues until the producers have executed a fixed number of
     * iterations for each nesting level. At the end of this process the
     * consumer should have a deterministic private counter value.
     *
     * Note, to test both recursive and nested SafepointManager entry,
     * the SafepointManager used for the final nesting level is the
     * same as the first.
     */
    class NestedProducerConsumerTest : public SafepointTestBase
    {

    public:
        NestedProducerConsumerTest(SafepointManager& safepointManager, int iterations, int threadQty, int nestingDepth)
            : SafepointTestBase(safepointManager, iterations, threadQty)
            , m_nestingDepth(nestingDepth)
        {
            m_nestedManagers = new SafepointManager[m_nestingDepth];
            m_producerCounters = new int*[m_nestingDepth + 1];
            for (int i = 0; i < m_nestingDepth + 1; i++) {
                m_producerCounters[i] = new int[threadQty - 1];
                VMPI_memset(m_producerCounters[i], 0, (threadQty-1) * sizeof m_producerCounters[0]);
            }
        }

        virtual ~NestedProducerConsumerTest()
        {
        }

        int nestedConsume(int depth)
        {
            if (depth == -1) {
                // Fish for asserts:
                // At the deepest nesting level recurse into the oldest SafepointManager on the stack.
                SafepointRecord record;
                m_safepointManager.enter(&record);
                ProducerConsumerTest::ConsumerSafepointTask task(m_producerCounters[0], m_threadQty - 1);
                m_safepointManager.requestSafepointTask(task);
                m_safepointManager.leave(&record);
                return task.producerStolenCounter();;
            }
            SafepointRecord record;
            m_nestedManagers[depth].enter(&record);
            ProducerConsumerTest::ConsumerSafepointTask task(m_producerCounters[depth + 1], m_threadQty - 1);
            m_nestedManagers[depth].requestSafepointTask(task);
            int consumerCount = task.producerStolenCounter();

            // Next SafepointManager
            consumerCount += nestedConsume(depth - 1);

            m_nestedManagers[depth].leave(&record);
            return consumerCount;
        }

        void nestedProduce(int depth, int iterations, int threadIndex)
        {
            if (depth == -1) {
                // Fish for asserts:
                // At the deepest nesting level recurse into the oldest SafepointManager on the stack.
                SafepointRecord record;
                m_safepointManager.enter(&record);
                for (int i = 0; i < iterations; i++) {
                    SAFEPOINT_POLL_FAST(m_safepointManager);
                    m_producerCounters[0][threadIndex]++;
                }
                m_safepointManager.leave(&record);
                return;
            }
            SafepointRecord record;
            m_nestedManagers[depth].enter(&record);
            for (int i = 0; i < iterations; i++) {
                SAFEPOINT_POLL_FAST(m_nestedManagers[depth]);
                m_producerCounters[depth + 1][threadIndex]++;
            }
            VMPI_threadYield(); // Try to allow some interesting interleavings

            // Next SafepointManager
            nestedProduce(depth - 1, iterations, threadIndex);

            m_nestedManagers[depth].leave(&record);
        }

        // Thread entrypoint
        virtual void runInSafepointManagerContext()
        {
            // Wait for all the threads to startup to force as much
            // contention as possible.
            const int threadIndex = m_threadIn++;
            SCOPE_LOCK_SP_NAMED(locker, m_monitor) {
                locker.notifyAll();
                while (m_threadIn < m_threadQty) {
                    locker.wait();
                }
            }

            // Setup one of the threads as the consumer, the others are producers
            if (threadIndex == m_threadQty - 1) {
                // The consumer continually makes SafepointTask requests on
                // the SafepointManager at each nesting level, leading to
                // safepointed producers. In each SafepointTask the consumer
                // takes each producer's currently accrued, private counter at
                // the current nesting depth, and adds it to its own total count,
                // then zeroes the producers counter.
                int consumerCount = 0;
                enum State
                {
                    RUNNING, DRAINING, DONE
                };
                State state = RUNNING;
                do {
                    ProducerConsumerTest::ConsumerSafepointTask task(m_producerCounters[0], m_threadQty - 1);
                    m_safepointManager.requestSafepointTask(task);
                    consumerCount += task.producerStolenCounter();
                    consumerCount += nestedConsume(m_nestingDepth - 1);
                    if (state == DRAINING) {
                        state = DONE;
                    } else if (m_threadOut == m_threadQty - 1) {
                        state = DRAINING;
                    }
                } while (state != DONE);

                // sharedCounter is verified by the selftest runner
                sharedCounter = consumerCount + m_iterations;

                // Can't do this in the dtor, if the selftest verify fails, it will
                // throw an exception and the dtor will not run.
                for (int i = 0; i < m_nestingDepth + 1; i++) {
                    delete[] m_producerCounters[i];
                }
                delete[] m_producerCounters;
                delete[] m_nestedManagers;
            } else {
                // How many increments should we perform each time we visit
                // a nesting depth?
                const int incPerDepth = m_iterations / 1000;
                const int iterations = m_iterations / (incPerDepth * (m_nestingDepth + 2));
                // The producers increment their private counter at each nesting depth,
                // but we make sure that they reach an explicit safepoint every iteration.
                for (int i = 0; i < iterations; i++) {
                    for (int j = 0; j < incPerDepth; j++) {
                        SAFEPOINT_POLL_FAST(m_safepointManager);
                        m_producerCounters[0][threadIndex]++;
                    }
                    nestedProduce(m_nestingDepth - 1, incPerDepth, threadIndex);
                    VMPI_threadYield(); // Try to allow some interesting interleavings
                }
                const int balance = m_iterations % (incPerDepth * (m_nestingDepth + 2));
                for (int i = 0; i < balance; i++) {
                    SAFEPOINT_POLL_FAST(m_safepointManager);
                    m_producerCounters[0][threadIndex]++;
                }
            }
            m_threadOut++;
        }
    private:
        int** m_producerCounters;
        const int m_nestingDepth;
        SafepointManager* m_nestedManagers;
        AtomicCounter32 m_threadIn;
        AtomicCounter32 m_threadOut;
    };
}

using namespace safepointtests;

// Must be >=2 for ProducerConsumerTest and NestedProducerConsumerTest
#define THREAD_QTY 4

// Must be >= 1000 for NestedProducerConsumerTest
#define ITERATIONS 100000

// For NestedProducerConsumerTest
#define NESTING_DEPTH 7

TEST(VmbaseSafepoints, Simple) {
    TestRunner runner(THREAD_QTY!=0);
    SimpleTest test(runner.safepointManager(), ITERATIONS, THREAD_QTY);
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * ITERATIONS, test.sharedCounter);
}

TEST(VmbaseSafepoints, ProducerConsumer) {
    TestRunner runner(THREAD_QTY!=0);
    ProducerConsumerTest test(runner.safepointManager(), ITERATIONS, THREAD_QTY);
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * ITERATIONS, test.sharedCounter);
}

TEST(VmbaseSafepoints, NestedProducerConsumer) {
    TestRunner runner(THREAD_QTY!=0);
    NestedProducerConsumerTest test(runner.safepointManager(), ITERATIONS * 100, THREAD_QTY, NESTING_DEPTH);
    runner.runTest(test);
    ASSERT_EQ(THREAD_QTY * ITERATIONS * 100, test.sharedCounter);
}

#endif  // VMCFG_SAFEPOINTS


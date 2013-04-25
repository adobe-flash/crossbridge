/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __vmbase_VMThread__
#define __vmbase_VMThread__

/**
 * Defines the preferred style of block-scoped mutex locking, e.g:
 *
 *     void Foo::foo(int val)
 *     {
 *
 *         // Outside critical section
 *
 *         SCOPE_LOCK(m_mutex) {
 *             m_sharedState += val; // Critical section protected by m_mutex
 *         }
 *
 *         // Outside critical section
 *     }
 *
 * When VMCFG_SAFEPOINTS is defined, the SCOPE_LOCK_SP version of the macro
 * ensures that any threads blocked on the mutex are considered 'safe' with respect
 * to their current safepoint context. I.e. lock acquisition is performed within
 * the RAII scope of a SafepointGate to create an implicit safepoint.
 *
 * The SCOPE_LOCK_SP version is more expensive than SCOPE_LOCK_NO_SP only under
 * contention, however SCOPE_LOCK_SP is *always* safe to use, whereas use
 * of SCOPE_LOCK_NO_SP can lead to deadlock if used incorrectly in the presence
 * of safepoints. The default macro for clients, SCOPE_LOCK, is therefore defined
 * as SCOPE_LOCK_SP. Conservatively, SCOPE_LOCK_NO_SP should only be used in the
 * following scenarios:
 * a) For critical sections known only to be entered by threads that have
 *    no current safepointing context.
 * b) For mutexes that can never be contended by threads that have entered
 *    a common safepointing context.
 * c) For mutexes that protect only 'uninterruptible' critical sections, i.e.
 *    critical sections that have none of the following reachable from within
 *    their scope: a safepoint (either implicit or explicit), a request to a
 *    SafepointManager to execute a SafepointTask.
 *
 * Note that any type of lock in VM, GC or avm-host code that is acquired without
 * a MutexLocker will have equivalent acquire semantics to that of SCOPE_LOCK_NO_SP.
 */
#ifdef VMCFG_SAFEPOINTS
#define SCOPE_LOCK(_m_)                   SCOPE_LOCK_SP(_m_)
#define SCOPE_LOCK_NO_SP(_m_)             if (vmbase::MutexLocker<vmbase::NO_SAFEPOINT>       __locker = _m_) {} else
#define SCOPE_LOCK_SP(_m_)                if (vmbase::MutexLocker<vmbase::IMPLICIT_SAFEPOINT> __locker = _m_) {} else
#else
#define SCOPE_LOCK(_m_)                   if (vmbase::MutexLocker __locker = _m_) {} else
#define SCOPE_LOCK_NO_SP(_m_)             SCOPE_LOCK(_m_)
#define SCOPE_LOCK_SP(_m_)                SCOPE_LOCK(_m_)
#endif

/**
 * Defines the preferred style of block-scoped monitor locking. The given
 * name is bound to allow lexically scoped access to wait/notify functions.
 *
 *     void Foo::boo(int val)
 *     {
 *
 *         // Outside critical section
 *
 *         SCOPE_LOCK_NAMED(locker, m_monitor) {
 *             m_sharedState += val; // Critical section protected by m_monitor
 *             locker.notifyAll();
 *             locker.wait();
 *         }
 *
 *         // Outside critical section
 *     }
 *
 * When VMCFG_SAFEPOINTS is defined, the SCOPE_LOCK_SP_NAMED version of the macro
 * defines an implicit safepoint for threads blocked on the monitor (identical to
 * SCOPE_LOCK_SP). Additionally, SCOPE_LOCK_SP_NAMED defines an implicit safepoint
 * for threads waiting and timed-waiting on the monitor.
 * The rules for using SCOPE_LOCK_NO_SP_NAMED instead of SCOPE_LOCK_SP_NAMED are
 * the same as those for SCOPE_LOCK_NO_SP described above.
 */
#ifdef VMCFG_SAFEPOINTS
#define SCOPE_LOCK_NAMED(_name_, _m_)           SCOPE_LOCK_SP_NAMED(_name_, _m_)
#define SCOPE_LOCK_NO_SP_NAMED(_name_, _m_)     if (vmbase::MonitorLocker<vmbase::NO_SAFEPOINT>       _name_ = _m_) {} else
#define SCOPE_LOCK_SP_NAMED(_name_, _m_)        if (vmbase::MonitorLocker<vmbase::IMPLICIT_SAFEPOINT> _name_ = _m_) {} else
#else
#define SCOPE_LOCK_NAMED(_name_, _m_)           if (vmbase::MonitorLocker _name_ = _m_) {} else
#define SCOPE_LOCK_NO_SP_NAMED(_name_, _m_)     SCOPE_LOCK_NAMED(_name_, _m_)
#define SCOPE_LOCK_SP_NAMED(_name_, _m_)        SCOPE_LOCK_NAMED(_name_, _m_)
#endif

namespace vmbase {

#ifdef VMCFG_SAFEPOINTS
    enum BlockingMode
    {
        IMPLICIT_SAFEPOINT, // When blocked, a thread should be considered 'safe' with respect to its current safepoint context
        NO_SAFEPOINT        // Blocking has no effect on a thread's safepoint status
    };
#endif

    /**
     * Class wrapper for a (recursive) mutex synchronization primitive.
     * Native initialization and disposal will occur during object
     * construction and destruction respectively.
     *
     * RecursiveMutex should not normally be used directly,
     * rather, the WaitNotifyMonitor class provides an implicit condition
     * paired with a mutex.
     */
    class RecursiveMutex
    {
        friend class ConditionVariable;
        friend class WaitNotifyMonitor;
#ifdef VMCFG_SAFEPOINTS
        template<BlockingMode> friend class MutexLocker;
        template<BlockingMode> friend class MonitorLocker;
        friend class SafepointHelper_RecursiveMutex;
#else
        friend class MutexLocker;
        friend class MonitorLocker;
#endif

    public:
        RecursiveMutex();
        ~RecursiveMutex();

    private:
        bool tryLock();
        void lock();
        void unlock();

        // No copying allowed: undefined semantics
        RecursiveMutex(const RecursiveMutex& mutex);
        const RecursiveMutex& operator=(const RecursiveMutex& mutex);

    private:
        vmpi_mutex_t m_mutex;

#ifdef DEBUG
    public:
        bool isLockedByCurrentThread() const;
    private:
        vmpi_thread_t volatile m_ownerThreadID;
        int m_recursionCount;
#endif
    };

    /**
     * Class wrapper for a condition variable synchronization primitive.
     * Native initialization and disposal will occur during object
     * construction and destruction respectively.
     *
     * NOTE: ConditionVariables should not normally be used directly, rather,
     * the WaitNotifyMonitor class provides an implicit condition
     * paired with a mutex.
     *
     */
    class ConditionVariable
    {
        friend class WaitNotifyMonitor;

    // ConditionVariables should only to be used as part of a WaitNotifyMonitor, so keep everything private.
    private:
        ConditionVariable();

        /**
         * Signals a single thread waiting on this condition variable.
         * This function does not imply any fairness policy when selecting the thread to signal.
         * This function does not block.
         */
        void signal();

        /**
         * Signals all threads waiting on the given condition variable.
         * This function does not block.
         */
        void broadcast();

        /**
         * Blocks the calling thread on this condition variable.
         * The calling thread must own the given mutex or the results are undefined (asserted in debug builds).
         * When the thread is blocked, it releases its lock on the mutex.
         * The thread remains blocked until the condition variable is signaled
         * (either individually or via a broadcast), or the specified timeout period has expired.
         * Post-wait, the thread will attempt to re-acquire mutex. When the mutex is re-acquired,
         * this function will return.
         *
         * Note:
         * - A waiting thread may spuriously awaken without being signaled.
         * - Waiting on a condition variable with a recursively locked mutex results in
         *   undefined behavior (asserted in debug builds).
         * - Waiting on a single condition variable with multiple mutexes results in
         *   undefined behavior.
         *
         * @param mutex The mutex to release whilst waiting and re-acquire when signaled
         * @param timeoutMillis The maximum amount of time to wait to be signaled
         * @return true if the timeout period expired
         */
        bool wait(RecursiveMutex& mutex, int32_t timeoutMillis);

        /**
         * Identical to the timeout version except the thread will wait forever to be signaled.
         */
        void wait(RecursiveMutex& mutex);

        // No copying allowed: undefined semantics
        ConditionVariable(const ConditionVariable& condVar);
        const ConditionVariable& operator=(const ConditionVariable& condVar);

    public:
        ~ConditionVariable();

    private:
        vmpi_condvar_t m_condVar;
    };

    /**
     * Base class for objects which require monitor synchronization semantics with a single
     * implicit condition for wait/notify.
     *
     * The general abstraction is that a monitor contains two thread sets: the blocked set and
     * the wait set. Threads in the blocked set compete for the monitor's mutex. The monitor's
     * 'owner' thread (i.e. the one which owns the mutex) can move itself to the wait set by calling
     * one of the wait() functions. After an owner thread has entered the wait set the monitor's mutex
     * is released; the blocked set then compete to acquire the mutex. Threads in the wait
     * set may only continue execution by moving back to the blocked set and competing with other
     * threads to acquire the mutex. Moving from the waiting to the block set occurs with the
     * notify() and notifyAll() operations. Calling notify() on the monitor causes a single thread from
     * the wait set to be moved to the blocked set (any thread could be chosen). Calling notifyAll()
     * on the monitor causes all threads from the waiting set to be moved to the blocked set. When a
     * thread acquires the mutex after moving from the wait set to the blocked set it continues execution
     * by returning from its call to wait().
     * Note that only the owner of the monitor is permitted to call notify() or notifyAll().
     */
    class WaitNotifyMonitor : public RecursiveMutex
    {
#ifdef VMCFG_SAFEPOINTS
        template<BlockingMode> friend class MutexLocker;
        template<BlockingMode> friend class MonitorLocker;
        friend class SafepointHelper_WaitNotifyMonitor;
#else
        friend class MutexLocker;
        friend class MonitorLocker;
#endif

    public:
        WaitNotifyMonitor();
        virtual ~WaitNotifyMonitor();

    private:
        /**
         * Moves the calling thread (which must be the monitor owner) to the
         * monitor's wait set and releases the mutex.
         * The thread remains in the wait set until it is notified
         * or the specified timeout period has expired.
         * Following notification or timeout, the calling(waiting) thread
         * is moved to the monitor's blocked set to compete for its mutex.
         * Following re-acquisition of the mutex, this function will return.
         *
         * Note:
         * - A waiting thread may spuriously move to the blocked set without
         *   notification or timeout.
         * - Waiting on a recursively locked monitor results in
         *   undefined behavior (asserted in debug builds).
         *
         * @param timeoutMillis The maximum amount of time to wait before moving
         *                      to the blocked set without notification.
         * @return true if the timeout period expired
         */
        bool wait(int32_t timeoutMillis);

        /**
         * Identical to the timeout version except the thread will wait forever to be notified.
         */
        void wait();

        /**
         * Notifies a single thread waiting on this monitor. The notified thread will be moved
         * to the blocked set.
         * This function does not imply any fairness policy when selecting the thread to notify.
         * This function does not block.
         * Only the monitor's owner thread may call this function (asserted in debug builds).
         */
        void notify();

        /**
         * Notifies all threads waiting on this monitor. The threads will be moved
         * to the blocked set.
         * This function does not block.
         * Only the monitor's owner thread may call this function (asserted in debug builds).
         */
        void notifyAll();

        // No copying allowed: undefined semantics
        WaitNotifyMonitor(const WaitNotifyMonitor& monitor);
        const WaitNotifyMonitor& operator=(const WaitNotifyMonitor& monitor);

    private:
        ConditionVariable m_condVar;
    };

    /**
     * MutexLocker provides RAII-style locking for mutexes, i.e.
     * the lock/unlock of a mutex can only be performed by the
     * ctor/dtor of a stack allocated MutexLocker.
     *
     * Example:
     *
     * class Foo
     * {
     *
     * private:
     *     RecursiveMutex m_mutex;
     *     int m_sharedState;
     *
     * public:
     *     void foo(int val)
     *     {
     *         MutexLocker locker(m_mutex); // Lock m_mutex
     *         m_sharedState += val; // Access shared state
     *         // locker's dtor unlocks the mutex
     *     }
     * }
     *
     * MutexLockers are intended to be used with the
     * SCOPE_LOCK macros, to give synchronized-block
     * sugaring to their declaration and scoping. For
     * example, the above function Foo::foo would be:
     *
     *     void foo(int val)
     *     {
     *         SCOPE_LOCK(m_mutex) {
     *             m_sharedState += val;
     *         }
     *     }
     *
     * BlockingMode:
     *
     * When a MutexLocker is specialized with a BlockingMode
     * of IMPLICIT_SAFEPOINT, blocking calls made on a mutex
     * via the MutexLocker are considered implicit safepoints
     * (see Safepoint.h).
     */
#ifdef VMCFG_SAFEPOINTS
    template <BlockingMode BLOCKING_MODE>
#endif
    class MutexLocker
    {
    public:
        MutexLocker(RecursiveMutex& mutex);
        ~MutexLocker();

        operator bool () const {return false;} // For the SCOPE_LOCK* macros

    private:
        // No copying allowed: undefined semantics
#ifdef VMCFG_SAFEPOINTS
        const MutexLocker<BLOCKING_MODE>& operator=(const MutexLocker<BLOCKING_MODE>& locker);
#else
        const MutexLocker& operator=(const MutexLocker& locker);
#endif
        // Force stack allocation
        void* operator new(size_t);

    private:
        RecursiveMutex& m_mutex;
    };

    /**
     * MonitorLocker provides RAII-style locking for monitors.
     * See MutexLocker for more details.
     *
     * A MonitorLocker also provide access to a locked monitor's
     * wait and notify functions, providing some guarantee that
     * only the monitor's owner thread will perform these operations.
     * For example:
     *
     * class Bar
     * {
     *
     * private:
     *     WaitNotifyMonitor m_monitor;
     *     int m_sharedState;
     *
     * public:
     *     void bar(int val)
     *     {
     *         MonitorLocker locker(m_monitor); // Lock m_monitor
     *         while (m_sharedState < val) {
     *             m_sharedState += val;
     *             locker.notifyAll(); // Notify all waiters on m_monitor
     *             locker.wait(); // Wait on m_monitor
     *         }
     *     }
     * }
     *
     * or, using the preferred SCOPE_LOCK_NAMED macro:
     *
     *     void bar(int val)
     *     {
     *         SCOPE_LOCK_NAMED(locker, m_monitor) { // Must give a name if the locker will be referenced
     *             while (m_sharedState < val) {
     *                 m_sharedState += val;
     *                 locker.notifyAll(); // Notify all waiters on locked m_monitor
     *                 locker.wait(); // Wait on locked m_monitor
     *             }
     *         }
     *     }
     *
     * BlockingMode:
     *
     * When a MonitorLocker is specialized with a BlockingMode
     * of IMPLICIT_SAFEPOINT, blocking calls (including waits)
     * made on a monitor via the MonitorLocker are considered
     * implicit safepoints (see Safepoint.h).
     */
#ifdef VMCFG_SAFEPOINTS
    template <BlockingMode BLOCKING_MODE>
#endif
    class MonitorLocker
    {
    public:
        MonitorLocker(WaitNotifyMonitor& monitor);
        ~MonitorLocker();

        // These functions just delegate to those of the locked WaitNotifyMonitor.
        // See WaitNotifyMonitor for their documentation.
        void wait();
        bool wait(int32_t timeout_millis);
        void notify();
        void notifyAll();
        operator bool () const {return false;} // For the SCOPE_LOCK* macros

    private:
        // No copying allowed: undefined semantics
#ifdef VMCFG_SAFEPOINTS
        const MonitorLocker<BLOCKING_MODE>& operator=(const MonitorLocker<BLOCKING_MODE>& locker);
#else
        const MonitorLocker& operator=(const MonitorLocker& locker);
#endif
        // Force stack allocation
        void* operator new(size_t);

    private:
        WaitNotifyMonitor& m_monitor;
    };

    /**
     * A collection of memory barrier (fence) operations.
     *
     * For a memory barrier overview see Mac OS X's developer docs:
     * http://developer.apple.com/library/mac/#documentation/
     *      Cocoa/Conceptual/Multithreading/ThreadSafety/
     *      ThreadSafety.html#//apple_ref/doc/uid/10000057i-CH8-SW1
     * Note that for all platforms supported by Tamarin,
     * vmbase::MemoryBarrier::readWrite() has equivalent semantics
     * to that of OS X's OSMemoryBarrier().
     *
     * For a more hardware-oriented discussion of why memory barriers
     * are required, see:
     * 'Memory Barriers: a Hardware View for Software Hackers' (2009)
     * by Paul E. Mckenney.
     * http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.152.5245
     *
     * An older, but seminal report on memory models for shared memory
     * multi-processing (and their implications for programmers and
     * compilers):
     * 'Shared memory consistency models: A tutorial' (1996)
     * by Sarita V. Adve and Kourosh Gharachorloo.
     * http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.106.5742
     *
     */
    class MemoryBarrier
    {
    public:
        /**
         * Inserts the strongest hardware read/write memory barrier provided by the platform.
         * The minimum guarantee provided by this barrier will be:
         *   - All load and store operations executed before the barrier will appear to
         *     complete before all load and store operations after the barrier.
         *
         * Note that this function inserts an equivalent compiler memory-barrier in addition
         * to the hardware barrier.
         */
        static void readWrite();
    };

    /**
     * A collection of atomic operations.
     */
    class AtomicOps
    {
    public:
        /**
         * Performs an atomic Compare-And-Swap operation.
         * If the contents at 'address' are equal to 'oldValue', then they
         * are replaced with 'newValue'.
         *
         * The 'WithBarrier' version includes a memory barrier with
         * the ordering guarantees of MemoryBarrier::readWrite().
         *
         * @param oldValue The value to compare
         * @param newValue The value to swap-in, if oldValue is the current value
         * @param address The address of the value to update
         * @return true if the update was successful
         */
        static bool compareAndSwap32(int32_t oldValue, int32_t newValue, volatile int32_t* address);
        static bool compareAndSwap32WithBarrier(int32_t oldValue, int32_t newValue, volatile int32_t* address);


        static int32_t compareAndSwap32WithBarrierPrev(int32_t oldValue, int32_t newValue, volatile int32_t* address);

        /**
         * Performs an atomic in-place bitwise OR.
         *
         * The 'WithBarrier' versions include a memory barrier with
         * the ordering guarantees of MemoryBarrier::readWrite().
         *
         * @param mask The bit mask to apply
         * @param address The address of the value to be OR'ed
         * @return the updated value following the OR. The 'Prev' suffixed versions return the value before the OR.
         */
        static int32_t or32(uint32_t mask, volatile int32_t* address);
        static int32_t or32WithBarrier(uint32_t mask, volatile int32_t* address);
        static int32_t or32Prev(uint32_t mask, volatile int32_t* address);
        static int32_t or32WithBarrierPrev(uint32_t mask, volatile int32_t* address);

        /**
         * Performs an atomic in-place bitwise AND.
         *
         * The 'WithBarrier' versions include a memory barrier with
         * the ordering guarantees of MemoryBarrier::readWrite().
         *
         * @param mask The bit mask to apply
         * @param address The address of the value to be AND'ed
         * @return the updated value following the AND. The 'Prev' suffixed versions return the value before the AND.
         */
        static int32_t and32(uint32_t mask, volatile int32_t* address);
        static int32_t and32WithBarrier(uint32_t mask, volatile int32_t* address);
        static int32_t and32Prev(uint32_t mask, volatile int32_t* address);
        static int32_t and32WithBarrierPrev(uint32_t mask, volatile int32_t* address);
    };

    /**
     * An atomic 32-bit integer counter.
     * The set, inc, dec, incAndGet and decAndGet operations include a memory
     * barrier with the ordering guarantees of MemoryBarrier::readWrite().
     */
    class AtomicCounter32
    {
    public:
        AtomicCounter32(int32_t value = 0);
        ~AtomicCounter32();

        void set(int32_t value);
        int32_t get() const;
        void inc();
        int32_t incAndGet();
        void dec();
        int32_t decAndGet();

        int32_t operator++() {return incAndGet();}
        int32_t operator--() {return decAndGet();}
        int32_t operator++(int32_t) {return incAndGet() - 1;}
        int32_t operator--(int32_t) {return decAndGet() + 1;}
        operator int32_t() const {return get();}

    private:
        volatile int32_t m_value;
    };

    /**
     * Class-wrapper for the platform thread-local storage implementation.
     * The type T must cast to and from void*.
     * The default per-thread value of a VMThreadLocal is 0.
     */
    template <typename T>
    class VMThreadLocal
    {
    public:
        VMThreadLocal()
            : m_isInitialized(false)
        {
            m_isInitialized = VMPI_tlsCreate(&m_tlsID);
            assert(m_isInitialized);
            set(0);
            assert(get() == 0);
        }

        ~VMThreadLocal()
        {
            if (m_isInitialized) {
                m_isInitialized = false;
                VMPI_tlsDestroy(m_tlsID);
            }
        }

        bool isInitialized()
        {
            return m_isInitialized;
        }

        REALLY_INLINE void set(T value)
        {
            assert(m_isInitialized);
            VMPI_tlsSetValue(m_tlsID, (void*) (value));
        }

        REALLY_INLINE T get() const
        {
            assert(m_isInitialized);
            return (T) (VMPI_tlsGetValue(m_tlsID));
        }

        REALLY_INLINE T operator =(T t)
        {
            set(t);
            return t;
        }

        REALLY_INLINE operator T() const
        {
            return get();
        }

        REALLY_INLINE T operator->() const
        {
            return get();
        }

    private:
        VMThreadLocal(const VMThreadLocal& threadLocal);
        const VMThreadLocal& operator=(const VMThreadLocal& threadLocal);

    private:
        uintptr_t m_tlsID;
        bool m_isInitialized;
    };

    /**
     * Base class for closures/functors executed by VMThreads.
     */
    class Runnable
    {
    public:
        virtual void run() = 0;
        virtual ~Runnable();
    };

    /**
     * VMThread builds on VMPI to provide a simple, object-oriented
     * thread abstraction.
     *
     * Calling start() on a VMThread instance creates a new native thread.
     * The thread begins execution from the run() method of a Runnable object,
     * which can be passed to the VMThread constructor. If no Runnable is passed
     * in the constructor then the VMThread calls its own run() method, which can
     * be overridden.
     *
     * VMThread provides an interface for joining with other VMThreads,
     * putting VMThreads to sleep, and querying the identity of the running VMThread.
     *
     * Note that a VMThread instance does not depend on the presence of, or have
     * any affinity with, any AvmCore instance.
     *
     */
    class VMThread : public Runnable
    {
    private:
        enum ThreadState
        {
            NOT_STARTED, // The VMThread has been constructed, but it does not yet represent a running native thread.
            RUNNABLE,    // The VMThread's native thread is running, waiting, blocked or sleeping
            TERMINATED,  // The VMThread's native thread has exited
        };

    public:
        enum ThreadPriority
        {
            LOW_PRIORITY,
            NORMAL_PRIORITY,
            HIGH_PRIORITY
        };

        /**
         * Creates a VMThread with a system-assigned name.
         * When started, the VMThread will call its own run() method.
         */
        VMThread();

        /**
         * Creates a VMThread with the given name.
         * When started, the VMThread will call its own run() method.
         *
         * @param name An identifier for the VMThread (makes own copy of the string)
         */
        VMThread(const char* name);

        /**
         * Creates a VMThread with a system-assigned name.
         * When started, the VMThread will call the given Runnable's run() method.
         *
         * @param runnable The Runnable to call when the thread starts-up
         */
        VMThread(Runnable* runnable);

        /**
         * Creates a VMThread with the given name.
         * When started, the VMThread will call the given Runnable's run() method.
         *
         * @param name An identifier for the VMThread  (makes own copy of the string)
         * @param runnable The Runnable to call when the thread starts-up
         */
        VMThread(const char* name, Runnable* runnable);

        /**
         * Currently a VMThread's native thread cannot free the VMThread.
         * The free must be performed by another thread which knows how
         * the thread was allocated and when it is safe to free.
         * Clearly 'safe to free' means that the freeing thread and all
         * other threads will not reference the VMThread again.
         * Hence the dtor provides some guarantees of safety by first waiting
         * for all threads joining this VMThread to have exited the join
         * operation before continuing.
         */
        virtual ~VMThread();

        /**
         * Creates a new native thread to begin execution at this VMThread's Runnable.
         * Calling VMThread::currentThread() from the new thread will return this VMThread.
         *
         * @return true If the thread successfully began execution
         */
        bool start();

        /**
         * Identical to start(), but starts the thread with the given priority.
         * Note that if the platform does not support thread priorities then
         * the argument will be ignored.
         * As of Nov' 2010, the POSIX implementations do not honor this argument.
         *
         * @param priority The thread's priority (low/normal/high)
         * @return true If the thread successfully began execution
         */
        bool start(ThreadPriority priority);

        /**
         * Identical to start(ThreadPriority priority), but starts the thread with the given
         * stack and guard area sizes.
         * Note that if the platform does not support either parameter then any supplied
         * arguments will be ignored.
         * As of Nov' 2010, support should be:
         *               POSIX     win32 (XP version APIs)
         * priority        no         yes
         * stack size      yes        yes
         * guard size      yes        no
         *
         * @param priority The thread's priority (low/normal/high)
         * @param stackSize The thread's stack size
         * @param guardSize The thread's guard area size
         * @return true If the thread successfully began execution
         */
        bool start(ThreadPriority priority, size_t stackSize, size_t guardSize);

        /**
         * Causes the calling thread to block until this VMThread's native thread has exited.
         * This function is thread safe, i.e. multiple thread can concurrently join a thread.
         *
         * If this VMThread's native thread has already exited, then the function immediately returns.
         */
        void join();

        /**
         * The default run() implementation immediately returns.
         */
        virtual void run();

        const char* getName() const;

        /**
         * Causes the calling thread to sleep for the given number of milliseconds.
         * Note that the thread may spuriously awaken before the timeout.
         *
         * @param timeout The length of time to sleep (in milliseconds)
         */
        static void sleep(int32_t timeout);

        /**
         * Returns the calling thread's VMThread.
         *
         * If the calling thread was not created via VMThread::start() then the
         * returned value will be NULL.
         *
         * @return The calling thread's VMThread, or NULL if the calling thread
         *         was not started with VMThread::start()
         */
        static VMThread* currentThread();

    private:
        void setNameFrom(const char* name);
        static vmpi_thread_rtn_t VMPI_THREAD_START_CC startInternal(vmpi_thread_arg_t args);
        bool start(vmpi_thread_attr_t* attr);

        // No copying allowed: undefined semantics
        VMThread(const VMThread& thread);
        const VMThread& operator=(const VMThread& thread);

    private:
        Runnable* m_runnable;
        const char* m_name;
        vmpi_thread_t m_threadID;
        ThreadState m_state;
        WaitNotifyMonitor m_joinMonitor;
        int m_joinerQty;

        static AtomicCounter32 m_nextNameSuffix;
        static VMThreadLocal<VMThread*> m_currentThread; // Keep each thread's VMThread in TLS.
    };

}
#endif /* __vmbase_VMThread__ */

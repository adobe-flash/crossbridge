/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __Safepoint__
#define __Safepoint__

/**
 * Tamarin Safepoints
 * =======================================================================
 *
 * Safepoints enable a co-operative synchronization pattern used in
 * multi-threaded VMs. Specifically, given some thread that must promptly
 * execute a VM-level task, safepoints allow the thread to request that
 * all potentially conflicting threads first make themselves 'safe' with
 * respect to the task, and remain so throughout the task's execution. An
 * 'explicit safepoint' is a code location where a thread can
 * co-operatively suspend its own execution in response to such a request.
 * An 'implicit safepoint' is a code location that makes a thread 'safe' by
 * some other means; for example, a thread could be 'safe' by remaining in
 * library code for the task's duration. Implicit safepoints must be used
 * when a thread cannot be guaranteed to promptly (or ever) reach an
 * explicit safepoint location. A thread that is either suspended at an
 * explicit safepoint or 'safe' via an implicit safepoint is 'safepointed'.
 * The task that is run with threads safepointed is the 'safepoint task'.
 *
 * An example use of safepointing is in the implementation of a
 * stop-the-world GC. A 'safe' mutator with respect to collection safepoint
 * tasks could be: a) the roots of the mutator are known, b) the mutator
 * cannot update the object graph, and c), the mutator is not safepointed
 * in memory management code.
 *
 * As of December 2010, Tamarin safepoints are used within the asymmetric
 * (multi-mutator) version of MMgc and the background compiler.
 *
 *
 * General Model
 * =======================================================================
 *
 * Tamarin Safepoints are implemented by 4 main classes:
 *
 * - vmbase::SafepointTask
 *
 * Safepoint tasks must be implemented as callable objects. SafepointTask
 * provides the interface that they must implement.
 *
 * - vmbase::SafepointManager
 *
 * Provides a safepoint context for threads to enter and exit as their
 * context changes in the host process. Many SafepointManager instances may
 * exist per host-process. SafepointTask's are submitted to a
 * SafepointManager for execution (the SafepointTask is dispatched on the
 * submitting thread); before a SafepointTask begins execution, the
 * submitting thread is blocked by the SafepointManager until all threads
 * that are within the SafepointManager's context are 'safe' with respect
 * to the SafepointTask. No threads are allowed to enter or leave a
 * SafepointManager's context while it has a pending or ongoing
 * SafepointTask. SafepointTasks are dispatched serially per
 * SafepointManager; SafepointTasks submitted to different
 * SafepointManagers are dispatched concurrently. SafepointManagers support
 * nested and recursive thread entry via SafepointRecord (see next bullet).
 *
 * - vmbase::SafepointRecord
 *
 * Before entering a SafepointManager, a thread must allocate a
 * SafepointRecord on its stack. A SafepointRecord records safepoint
 * information for a thread while it is in the context of a particular
 * SafepointManager. If threads t0 and t1 are running in the context of
 * SafepointManager m0, then their respective stacks should be as below:
 *
 *   Thread t0 stack               Thread t1 stack
 * +-------------------+        +-------------------+
 * | SafepointRecord0  |        | SafepointRecord1  |
 * | status: SP_UNSAFE |        | status: SP_UNSAFE |
 * |    manager: m0    |        |    manager: m0    |
 * +-------------------+        +-------------------+
 * |  C++ & AS Frames  |        |  C++ & AS Frames  |
 * |                   |        |                   |
 * |                   |
 * |                   |
 *
 * Note that as both threads are running, the SafepointRecords of each have
 * a status of SP_UNSAFE. If a third thread, t2, which has also entered m0
 * now requests a SafepointTask be executed, then m0 will block t2 until
 * both SafepointRecord0 and SafepointRecord1 have a status of SP_SAFE
 * (i.e. the threads are *safepointed* with respect to m0). The typical
 * mechanism by which threads t0 and t1 will set their respective
 * SafepointRecords to SP_SAFE is by passing through a SafepointGate (see
 * below).
 *
 * Nested and recursive SafepointManager entry requires a SafepointRecord
 * per entry. In the example below, thread t0 has recursed into
 * SafepointManager m0, and thread t1 has nested into SafepointManager m1.
 *
 *   Thread t0 stack               Thread t1 stack
 * +-------------------+        +-------------------+ <--+
 * | SafepointRecord0  |        | SafepointRecord1  |    |
 * |  status: SP_SAFE  |        |  status: SP_SAFE  |    |  Stack region
 * |    manager: m0    |        |    manager: m0    |    |  described by
 * +-------------------+        +-------------------+    |  SafepointRecord1
 * |  C++ & AS Frames  |        |  C++ & AS Frames  |    |
 * |                   |        |                   |    |
 * |                   |        +-------------------+ <--+
 * |                   |        | SafepointRecord3  |
 * +-------------------+        | status: SP_UNSAFE |
 * | SafepointRecord2  |        |    manager: m1    |
 * | status: SP_UNSAFE |        +-------------------+
 * |    manager: m0    |        |  C++ & AS Frames  |
 * +-------------------+        |                   |
 * |                   |        |                   |
 * |  C++ & AS Frames  |
 * |                   |
 * |
 *
 * Note that SafepointRecords that are not top-most on a thread's stack are
 * necessarily always SP_SAFE (this is enforced by the entry/exit routines
 * of SafepointManagers).
 * Considering the above example, if a thread t2 now submits a SafepointTask
 * to SafepointManager m0, then m0 will block t2 until all SafepointRecords
 * known to it are SP_SAFE, and as stated above, no threads are allowed to
 * enter or leave a SafepointManager's context (i.e. push or pop a
 * SafepointRecord) until no SafepointTasks are pending on the
 * SafepointManager. So in the example above, m0 need only wait for thread t0
 * to make SafepointRecord2 SP_SAFE, then all necessary SafepointRecords
 * are SP_SAFE to allow t2 to execute its SafepointTask (all threads are
 * safepointed with respect to m0).
 *
 * - vmbase::SafepointGate
 *
 * A SafepointGate is the mechanism by which a thread makes itself 'safe'
 * (in all typical situations, other than when attempting to enter or leave
 * a SafepointManager's context, which is described above). SafepointGate
 * implements a RAII pattern for setting a thread's top-most
 * SafepointRecord (which will be SP_UNSAFE) to SP_SAFE in its constructor,
 * and then back to SP_UNSAFE in its destructor. The destructor will block
 * the thread until it can safely change the SafepointRecord back to
 * SP_UNSAFE (i.e. when no SafepointTask is pending or ongoing).
 * SafepointGate provides any requisite memory barriers to ensure that a
 * safepointed thread's stores to memory are visible to a SafepointTask.
 * SafepointGates are used for both explicit and implicit safepoints.
 *
 *
 * Explicit safepoints and location policy
 * =======================================================================
 *
 * An 'explicit safepoint' is a code location where a thread can
 * co-operatively suspend its own execution to allow a SafepointTask to
 * run. This is achieved by a thread *polling* the SafepointManager of its
 * current context (top-most SafepointRecord) to check if a SafepointTask
 * is pending, if so, the thread will 'pass through' a SafepointGate (i.e.
 * use the RAII patterned SafepointGate to block if a SafepointTask is
 * pending). The SAFEPOINT_POLL_FAST and SAFEPOINT_POLL macros are provided
 * to mark explicit safepoint locations from C++. For convenience, these
 * macros ensure that a thread flushes its registers to the stack before
 * blocking in a SafepointGate (as we expect that 'safe' with respect to
 * any SafepointTask includes exposing all of the thread's GC roots).
 *
 * So where should explicit safepoints be inserted into code?
 *
 * Candidate safepoint locations must satisfy the following rules: a) if a
 * thread is safepointed at the location, then it must be 'safe' for *all*
 * defined SafepointTasks (this restriction can be lifted at a later point
 * if it becomes impractical), b) the continuation of a thread following the
 * safepoint is robust to any changes of runtime state made by a
 * SafepointTask.
 *
 * However, not all candidate locations should be explicit safepoints. The
 * main challenge here is to optimize the trade-off between the promptness
 * of reaching an explicit safepoint, and the overhead they bring to normal
 * execution. Specifically, in most cases it will not be acceptable to have
 * a SafepointTask-submitting thread wait for extended periods for all
 * threads to be SP_SAFE in the context of the submitting thread's
 * SafepointManager (similarly, it is not acceptable for SP_SAFE threads to
 * wait for SP_UNSAFE laggards). The other side to this trade-off, overhead,
 * is a factor of how hard we have to work at each explicit safepoint
 * location to determine: a) if a SafepointTask is queued for execution, and
 * b), if the constraints hold for the dynamic execution state of the thread
 * to be 'safe' for the SafepointTask. To simplify this trade-off, item b)
 * is typically removed from the equation; no dynamic properties of the
 * thread are inspected, it is simply the static code location of the
 * safepoint that enforces the necessary constraints. To further simplify
 * the tradeoff, explicit safepoints are not conditional based on the
 * SafepointTask instance that is queued to run, other than both the
 * task-submitting thread and the safepointed thread are running within the
 * context of the same SafepointManager. In other words, any SafepointTask
 * could run when a thread is suspended at an explicit safepoint (leading to
 * the two rules of safepoint locations, above).
 *
 *
 * Implicit safepoints and location policy
 * =======================================================================
 *
 * Implicit safepoints must be used when a thread cannot be guaranteed to
 * promptly (or ever) reach an explicit safepoint location. Consider a
 * low-priority thread which wakes every ten seconds, does one second of
 * work, and then sleeps again. If an explicit safepoint is inserted in its
 * work loop, then every SafepointTask submitted to the thread's current
 * SafepointManager by other threads will take on average five seconds to
 * begin execution. The solution here is to prepare the sleeping thread to
 * appear as 'safe' in the context of its current SafepointManager before
 * it goes to sleep. This is achieved through the RAII pattern of a
 * SafepointGate, i.e. the sleep function is called between its constructor
 * and destructor. This ensures that the thread's current SafepointRecord
 * is SP_SAFE during its sleep, and if a SafepointTask is ongoing when it
 * wakes up, it will block in the SafepointGate's destructor until the task
 * is complete.
 *
 * Whereas implicit safepoints that guard long-running library calls are
 * always a good idea to ensure prompt execution of SafepointTasks,
 * implicit safepoints that guard *blocking* library calls (or any type of
 * locking construct) are absolutely essential to prevent deadlock. For
 * example, consider a thread T1 which acquires lock L1, and is later
 * safepointed whilst still holding L1. If a thread, T2, which is running
 * in the same context as T1's SafepointManager attempts to acquire L1
 * before reaching an explicit safepoint, then via transitive closure with
 * the SafepointTask submitting thread, T1 and T2 are deadlocked. This is
 * prevented by performing the acquire of L1 within the scope of
 * SafepointGate, hence, T2's SafepointRecord will be SP_SAFE while
 * blocked. However, as the operation of a SafepointGate has some overhead,
 * not all *potentially* blocking calls will warranty an implicit
 * safepoint, specifically, those where it is known that deadlock with a
 * safepointed (or SafepointTask-submitting thread) cannot occur.
 * Conservatively, the following two scenarios *do not* require implicit
 * safepoints around blocking calls:
 *   a) Where the calling thread has no current safepoint context (it has
 *      not entered a SafepointManager).
 *   b) For blocking calls that do not base their blocking decisions on
 *      the progress of any thread that shares a safepointing context with
 *      the calling thread.
 *
 *
 * SafepointTasks and locks
 * =======================================================================
 *
 * There are no restrictions on the work performed by SafepointTasks,
 * except an important consideration with regard to locks. Specifically, to
 * avoid deadlock, a safepointed thread cannot hold any lock that will be
 * acquired within a SafepointTask. This constraint is typically enforced
 * by having a SafepointTask-submitting thread acquire all of the locks
 * it will require for the SafepointTask before it is submitted to the
 * SafepointManager.
 * =======================================================================
 */

#ifdef VMCFG_SAFEPOINTS

/**
 * Explicitly marks the code location as a safepoint for the given
 * SafepointManager.
 * Threads reaching this macro will poll the given SafepointManager
 * to check for a pending safepoint task request. If a task is pending,
 * the thread will have its registers flushed to the stack before
 * passing through a SafepointGate.
 * The thread's current (top-most) SafepointRecord must be managed
 * by the given SafepointManager.
 */
#define SAFEPOINT_POLL_FAST(_spManager_) \
    assert(vmbase::SafepointRecord::hasCurrent() && \
              vmbase::SafepointRecord::current()->manager() == &_spManager_); \
    do { \
        if (_spManager_.hasRequest()) \
            vmbase::SafepointGate::gateWithRegistersSaved(); \
    } while (0)

/**
 * Explicitly marks the code location as a safepoint.
 * Threads reaching this macro will poll the SafepointManager of their
 * current SafepointRecord to check for a pending safepoint task request.
 * If a task is pending, the thread will have its registers flushed to
 * the stack before passing through a SafepointGate.
 * SAFEPOINT_POLL_FAST should be used in preference to SAFEPOINT_POLL.
 */
#define SAFEPOINT_POLL() \
    do { \
        if (vmbase::SafepointRecord::current()->manager()->hasRequest()) \
            vmbase::SafepointGate::gateWithRegistersSaved(); \
    } while (0)

namespace vmbase {

    typedef Runnable SafepointTask;

    /**
     * Provides a safepointing context for threads to enter and exit as their
     * context changes in the host process.
     * Safepoint task-requests and polling are made on SafepointManager instances.
     *
     * See the above overview for more information.
     */
    class SafepointManager
    {
        friend class SafepointGate;
    public:
        SafepointManager();
        ~SafepointManager();

        /**
         * Blocks the calling thread until all other threads are
         * 'safe' with respect to this SafepointManager; the
         * given SafepointTask will then be dispatched on the
         * calling thread. All threads will remain 'safe'
         * until the task has completed execution.
         *
         * For those threads with a current safepoint context of
         * this SafepointManager, 'safe' is achieved via explicit or
         * implicit safepoints. All other threads are by definition
         * 'safe' with respect to this SafepointManager.
         *
         * The calling thread's current SafepointRecord must be
         * managed by this SafepointManager.
         */
        void requestSafepointTask(SafepointTask& task);

        /**
         * Checks if a SafepointTask is pending on this SafepointManager.
         */
        bool hasRequest();

        /**
         * Tests if this thread is currently executing a SafepointTask
         * as the requester of the task.
         */
        bool inSafepointTask();

        /**
         * Notifies the SafepointManager that the calling thread is
         * entering its context. The given SafepointRecord is set
         * as the thread's current SafepointRecord.
         * If the thread is nesting or recursing into the
         * SafepointManager then the thread's previous
         * SafepointRecord will be made SP_SAFE.
         *
         * This function will block until SafepointManager entry
         * can be performed with no SafepointTask pending or ongoing.
         */
        void enter(SafepointRecord* record);

        /**
         * Notifies the SafepointManager that the calling thread is
         * leaving its context. The given SafepointRecord will no longer
         * be required to be SP_SAFE before the SafepointManager allows
         * execution of a SafepointTask.
         *
         * This function will block until SafepointManager exit
         * can be performed with no SafepointTask pending or ongoing.
         *
         * The thread's previous (stack-wise) SafepointRecord will be set
         * as its current SafepointRecord if this thread is unwinding into
         * a SafepointManager context. The thread will block until
         * it can set the previous SafepointRecord as SP_UNSAFE with no
         * SafepointTask pending or ongoing on the SafepointManager
         * into which it is unwinding.
         */
        void leave(SafepointRecord* record);

    public:
        /**
         * Iterates over a SafepointManager's SafepointRecords.
         * The iterator is not threadsafe outside of a safepoint
         * task.
         */
        class RecordIterator
        {
        public:
            RecordIterator(SafepointManager& manager);
            ~RecordIterator();
            const SafepointRecord* next();
        private:
            SafepointRecord* m_next;
        };

    private:
        SafepointRecord* m_records;
        WaitNotifyMonitor m_requestMutex;
        vmpi_thread_t volatile m_requester;
        const int m_hardwareConcurrency;
    };

    /**
     * Implements a RAII pattern for setting a thread's top-most
     * SafepointRecord (which must be SP_UNSAFE) to SP_SAFE in its
     * ctor, and then back to SP_UNSAFE in its dtor. The dtor will
     * block the thread until it can safely change the
     * SafepointRecord back to SP_UNSAFE (i.e. when no SafepointTask
     * is pending or ongoing). SafepointGate also provides any
     * requisite memory barriers to ensure that a safepointed
     * thread's stores to memory are visible to a SafepointTask.
     *
     * See the above overview for example usage.
     */
    class SafepointGate
    {
    public:
        SafepointGate(void* stackPointer);
        ~SafepointGate();
        static void gateWithRegistersSaved();
    private:
        static void gate(void* stackPointer, void*);
        SafepointRecord* const m_safepointRecord;
    };

    /**
     * Holds safepointing information for a region of a thread's stack.
     * See the above overview for more information.
     */
    class SafepointRecord
    {

        friend class SafepointManager;
        friend class SafepointGate;
        friend class SafepointManager::RecordIterator;
        template<BlockingMode> friend class MutexLocker;
        template<BlockingMode> friend class MonitorLocker;

    public:
        SafepointRecord();
        ~SafepointRecord();

        /**
         * Returns true if the calling thread has at
         * least one SafepointRecord describing its stack.
         */
        static bool hasCurrent();

        /**
         * Returns the calling thread's top-most
         * SafepointRecord.
         */
        static SafepointRecord* current();

        /**
         * Returns the stack pointer that this
         * SafepointRecord's thread recorded as its
         * top-of-stack before entering a safepoint.
         *
         * This function is not thread-safe when
         * called outside of a SafepointTask.
         */
        const void* safeRegionEnd() const;

        /**
         * Returns if this SafepointRecord is safe
         */
        bool isSafe() const;

        /**
         * Returns the SafepointManager with which
         * this SafepointRecord is registered.
         */
        SafepointManager* manager() const;

		/**
		 * Sets the interruptLocation and isolateDesc for this record.
		 */

		void setLocationAndDesc (int32_t* location, int desc);

    public:
        /**
         * Sets the calling thread's topmost
         * SafepointRecord to NULL after OOM.
         */
		static void cleanupAfterOOM(){SafepointRecord::setCurrent(NULL);}

    private:
        /**
         * Sets the calling thread's topmost
         * SafepointRecord as the given record.
         */
        static void setCurrent(SafepointRecord* record);

    private:
        // Force stack allocation
        void* operator new(size_t);

    private:
        enum Status {SP_UNSAFE, SP_SAFE};
        volatile Status m_status;
        void* m_safeRegionEnd;

        SafepointRecord* m_stackPrev;     // The previous SafepointRecord on the thread's stack
        SafepointRecord* m_managerPrev;   // The previous SafepointRecord on the SafepointManager's linked-list
        SafepointRecord* m_managerNext;   // The next SafepointRecord on the SafepointManager's linked-list
        SafepointManager* m_manager;      // The SafepointManager to which this SafepointRecord is registered

        volatile int32_t*			m_interruptLocation;
        int							m_isolateDesc;

    private:
        static VMThreadLocal<SafepointRecord*> m_current;
    };
}

#else // VMCFG_SAFEPOINTS

#define SAFEPOINT_POLL_FAST(_spManager_)
#define SAFEPOINT_POLL()

#endif // VMCFG_SAFEPOINTS

#endif /* __Safepoint__ */

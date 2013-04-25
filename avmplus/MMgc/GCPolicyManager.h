/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MMgc_GCPolicyManager__
#define __MMgc_GCPolicyManager__

#ifdef VMCFG_SELFTEST
namespace avmplus
{
    namespace ST_mmgc_dependent { class ST_mmgc_dependent; }
}
#endif

namespace MMgc
{
    /**
     * Configuration options for GC and GCPolicyManager construction.
     */
    class GCConfig
    {
    public:
        /* Positive value denotes heap size, in blocks, below which we
         * do not collect; non-positive denotes "choose default value
         * for threshold."
         */
        uint32_t collectionThreshold;

        /* Defaults to unlimited.  Set it to limit the mark stack in terms
         * of the number of blocks used (the smallest you can choose is 1).
         * The GC ignores this flag unless MMGC_MARKSTACK_ALLOWANCE has been
         * defined in MMgc.h.
         */
        int32_t markstackAllowance;

        /* Defaults to true.  Set it to false to disable exact tracing.
         * The GC ignores this flag unless AVMTWEAK_SELECTABLE_EXACT_TRACING 
         * has been enabled.
         */
        bool exactTracing;

        /* Defaults to true. Set to false to disable DRC. */
        bool drc;
        
        /* Defaults to false. Turn on to test that zero count objects being
         * deleted aren't reachable. */
        bool validateDRC;
        
        /* Defaults to false.  Validate incremental marking. */
        bool incrementalValidation;

        /**
         * Garbage collection mode.  The GC is configured at creation in one of
         * these (it would be pointlessly hairy to allow the mode to be changed
         * at run-time).
         */
        enum GCMode
        {
            kDisableGC=1,       // never collect
            kGreedyGC,          // stop-the-world collection at every allocation
            kIncrementalGC,     // incremental collection
            kNonincrementalGC   // nonincremental collection
        };        
        
        /* Default to kIncrementalGC. */
        GCMode mode;
        
        /* selects defaults for all configuration parameters. */
        GCConfig();
    private: // unimplemented ctors/methods.
        GCConfig(GCConfig&);
        GCConfig& operator=(GCConfig&);
    };


    /**
     * A policy manager for garbage collection.
     *
     * The policy manager centralizes policy decisions about how and when to run garbage
     * collection.  It is queried from the collector code, and collector code signals
     * to the policy manager at the occurrence of certain events.
     *
     * Typically the GCPolicyManager is simply embedded inside a GC instance, and manages
     * policy for that GC.
     *
     * ----
     *
     * Notes, 2009-06-23 / lhansen:
     *
     * This is the second cut, and it implements an allocation-driven policy that is
     * documented extensively in GC.cpp and in doc/mmgc/policy.pdf.  The policy improves
     * on the first-cut policy by running the GC less often and having lower pause times.
     *
     * ZCT reaping times are not bounded, so ZCT reaping may violate any kind of
     * incrementality guarantee.  This will be fixed by and by.
     */
    class GCPolicyManager {
    public:
#ifdef VMCFG_SELFTEST
        friend class avmplus::ST_mmgc_dependent::ST_mmgc_dependent;
#endif
        GCPolicyManager(GC* gc, GCHeap* heap, GCConfig& config);

        /**
         * Clean up and print any final statistics.  Should be called from the very
         * start of the GC's destructor.
         */
        void shutdown();

        /**
         * Situation: the GC is about to run the incremental marker.
         *
         * @return the desired length of the next incremental mark quantum.
         * @note the result can vary from call to call; the function should
         *       be called as an incremental mark is about to start and the
         *       result should not be cached.
         */
        uint32_t incrementalMarkMilliseconds();

        /**
         * @return the number of blocks owned by this GC, as accounted for by calls to
         * signalBlockAllocation and signalBlockDeallocation.
         */
        size_t blocksOwnedByGC();
        
        /**
         * @return the number of objects reported marked by signalMarkWork, since startup.
         */
        uint64_t objectsMarked();

        /**
         * @return the number of bytes reported marked by signalMarkWork, since startup.
         */
        uint64_t bytesMarked();

        /**
         * Compute a ZCT growth budget (in blocks) based on its current size.
         *
         * @return the growth budget
         */
        uint32_t queryZCTBudget(uint32_t zctBlocksUsed);

        /**
         * Set the lower limit beyond which we try not to run the garbage collector.
         * The value is specified in 4k blocks, thus 256 == 1MB.
         */
        void setLowerLimitCollectionThreshold(uint32_t blocks);

        enum PolicyEvent
        {
            NO_EVENT,
            START_StartIncrementalMark,     // also, start of garbage collection
            END_StartIncrementalMark,
            START_IncrementalMark,
            END_IncrementalMark,
            START_FinalRootAndStackScan,
            END_FinalRootAndStackScan,
            START_FinalizeAndSweep,
            END_FinalizeAndSweep,           // also, end of garbage collection
            END_FinalizeAndSweepNoShrink,   // also, end of garbage collection
            START_ReapZCT,
            END_ReapZCT
        };

           
        /**
         * Situation: a GC event corresponding to one of the PolicyEvent values occurs.
         * Tell the policy manager about it.
         */
        void signal(PolicyEvent ev);

        /**
         * Situation: the memory status of the system changes, and a message is being
         * broadcast to make this known.
         */
        void signalMemoryStatusChange(MemoryStatus from, MemoryStatus to);

        /**
         * Situation: 'numblocks' blocks have just been obtained by this GC from the GCHeap.
         */
        void signalBlockAllocation(size_t numblocks);

        /**
         * Situation: 'numblocks' blocks have just been returned from this GC to the GCHeap.
         */
        void signalBlockDeallocation(size_t numblocks);

        /* The mark work signaling methods may only be called between the signals
         * START_StartIncrementalMark and END_FinalizeAndSweep, and mark work signaled
         * after a START event may not be reflected in the values returned by
         * objectsMarked() and bytesMarked() until after the corresponding END event
         * has been signaled.
         */
        /**
         * Situation: signal that one pointer-containing object, whose size is nbytes,
         * has been traced precisely by the garbage collector.
         */
        void signalExactMarkWork(size_t nbytes);
        
        /**
         * Situation: signal that one pointer-containing object, whose size is nbytes,
         * has been traced conservatively by the garbage collector.
         */
        void signalConservativeMarkWork(size_t nbytes);
        
        /**
         * Situation: signal that one pointer-free object, whose size is nbytes,
         * has been traced by the garbage collector.
         */
        void signalPointerfreeMarkWork(size_t nbytes);

        /**
         * Situation: signal that some number of bytes have just been successfully
         * allocated and are about to be returned to the caller of the allocator.
         *
         * @return true if collection work should be triggered because the allocation
         * budget has been exhausted.
         */
        /*REALLY_INLINE*/ bool signalAllocWork(size_t nbytes);

        /**
         * Situation: signal that some number of bytes have just been successfully
         * freed.
         */
        /*REALLY_INLINE*/ void signalFreeWork(size_t nbytes);

        /**
         * Situation: external code has allocated memory whose lifetime is tied to a
         * GC object.  This memory is to be accounted for in GC policy in the normal
         * way (ie incorporated into the computation of H).
         */
        void signalDependentAllocation(size_t nbytes);

        /**
         * Situation: external code has deallocated memory whose lifetime was tied to a
         * GC object; the memory must previously have been accounted for through a call
         * to signalDependentAllocation().
         */
        void signalDependentDeallocation(size_t nbytes);

        /**
         * Situation: the incremental marker has been started, and we need to know whether
         * to run another mark increment or push the conclusion to a finish (because the
         * total allocation budget for the collection cycle has been exhausted).  This
         * predicate returns true in the latter case.
         */
        bool queryEndOfCollectionCycle();

#ifdef MMGC_POLICY_PROFILING
        /**
         * Situation: signal that one write has been examined by the write barrier and made
         * it to the given stage of the barrier.   See the barrier code to find out what the
         * stages are.
         */
        /*REALLY_INLINE*/ void signalWriteBarrierWork(int stage);

        /**
         * Situation: signal that the ZCT reaper has run and performed some work.
         */
        void signalReapWork(uint32_t objects_reaped, uint32_t bytes_reaped, uint32_t objects_pinned);
#endif
#ifdef MMGC_POINTINESS_PROFILING
        /**
         * Situation: signal that 'words' words have been scanned; that 'could_be_pointer'
         * number of these words passed the initial heap range checks; and that
         * 'actually_is_pointer' number of these words were conservatively found to
         * be pointers to heap objects.
         */
        /*inline*/ void signalDemographics(size_t words, size_t could_be_pointer, size_t actually_is_pointer);
#endif
#ifdef MMGC_REFCOUNT_PROFILING
        /**
         * Situation: signal that IncrementRef has been called on an object.
         */
        /*REALLY_INLINE*/ void signalIncrementRef();

        /**
         * Situation: signal that DecrementRef has been called on an object.
         */
        /*REALLY_INLINE*/ void signalDecrementRef();

        /**
         * Situation: signal that one reference is being added to the ZCT.  If 'initial'
         * is true then the object is freshly allocated and the adding is from RCObject's
         * constructor.  'population' is the number of elements in the table at the time
         * ZCT::Add is entered.
         */
        /*REALLY_INLINE*/ void signalZCTAdd(bool initial, uint32_t population);

        /**
         * Situation: signal that one reference is being removed from the ZCT.  If 'final'
         * is true then the object is being deleted and the removal is from RCObject's
         * destructor.
         */
        /*REALLY_INLINE*/ void signalZCTRemove(bool final);
#endif

        /**
         * The collector 'gc' (which is not the collector for this manager) has started
         * a garbage collection, indicating perhaps some memory pressure in that heap.
         */
        void signalStartCollection(GC* gc);

        /**
         * The collctor 'gc' (which is not the collector for this manager) has completed
         * a garbage collection.
         */
        void signalEndCollection(GC* gc);

        /**
         * Request a full collection happen at the next GC edge (enter/exit)
         */
        void queueFullCollection() { fullCollectionQueued = true; }

        /**
         * called after a full collection is done
         */
        void fullCollectionComplete() { fullCollectionQueued = false; }

        /**
         * called to find out if a full collection has been requested
         */
        bool queryFullCollectionQueued() { return fullCollectionQueued; }

        /**
         * Called by the owner when the owner is notified of an imminent abort
         */
        void SignalImminentAbort();

        /**
         * Compute the load factor is for a given heap size (in bytes).
         */
        double queryLoadForHeapsize(double H);

        /**
         * Return percentage of bytes scanned exactly. 
         */
        uint32_t queryExactPercentage();

        /**
         * Return the fraction [0,1] of allocation budget used.
         */
        double queryAllocationBudgetFractionUsed();

        // ----- Public data --------------------------------------

        // Elapsed time (in ticks) for various collection phases, and the maximum phase time
        // for each phase.  They are updated when an END event is signaled.  Client code must
        // not modify these variables.

        // The total time for various collection phases across the run
        uint64_t timeStartIncrementalMark;
        uint64_t timeIncrementalMark;
        uint64_t timeFinalRootAndStackScan;
        uint64_t timeFinalizeAndSweep;
        uint64_t timeReapZCT;

        // The total time doing collection work (sum of the variables above except the ZCT reap
        // time) and the elapsed time from the start of StartIncrementalMark to the end of
        // FinalizeAndSweep.  Together they provide a crude approximation to a measure of
        // pause clustering.
        uint64_t timeInLastCollection;
        uint64_t timeEndToEndLastCollection;

        // Time for ZCT reaping during the last collection cycle (end of one FinalizeAndSweep
        // to the end of the next one).
        uint64_t timeReapZCTLastCollection;

        // The maximum latceny for various collection phases across the run
        uint64_t timeMaxStartIncrementalMark;
        uint64_t timeMaxIncrementalMark;
        uint64_t timeMaxFinalRootAndStackScan;
        uint64_t timeMaxFinalizeAndSweep;
        uint64_t timeMaxReapZCT;

        // The maximum latcency for various collection phases during the previous collection cycle
        uint64_t timeMaxStartIncrementalMarkLastCollection;
        uint64_t timeMaxIncrementalMarkLastCollection;
        uint64_t timeMaxFinalRootAndStackScanLastCollection;
        uint64_t timeMaxFinalizeAndSweepLastCollection;
        uint64_t timeMaxReapZCTLastCollection;

        // The total number of times each phase was run
        uint64_t countStartIncrementalMark;
        uint64_t countIncrementalMark;
        uint64_t countFinalRootAndStackScan;
        uint64_t countFinalizeAndSweep;
        uint64_t countReapZCT;

    private:
        // The following parameters can vary not just from machine to machine and
        // run to run on the same machine, but within a run in response to memory
        // pressures and other feedback.

        // The lower limit beyond which we try not to run the garbage collector.
        uint32_t lowerLimitCollectionThreshold();

        // Get the current time (in ticks).
        uint64_t now();

#ifdef MMGC_POLICY_PROFILING
        // Convert ticks to milliseconds, as a double (used for printing)
        double ticksToMillis(uint64_t ticks);

        // @return true if we should print policy data after every GC and at the end of the run
        bool summarizeGCBehavior();

        // @return true if we should print policy data at the end of the run
        bool summarizeGCBehaviorAtEnd();
        
        // Print policy data.  At shutdown, afterCollection will be passed as false.
        void PrintGCBehaviorStats(bool afterCollection=true);
#endif

        // Various private methods for the GC policy follow.  See comment in GC.cpp for details.

        // Amount of GC work to perform (bytes to scan) per byte allocated while the GC is active
        double W();

        // Amount of allocation to allow between two invocations of IncrementalMark
        double A();

        // Called from the policy event handler to start computing adjustments to R: before any
        // mark work is performed.
        void startAdjustingR();

        // Called from the policy event handler to finish computing adjustments to R: after all
        // mark work has been performed.
        void endAdjustingR();

        // Called from adjustPolicyForNextMajorCycle to compute the effective L for the next
        // collection cycle
        void adjustL(double H);

        // Called from the policy manager's constructor
        void adjustPolicyInitially();

        // Called from the policy event handler to compute the GC policy for the next
        // major collection cycle (from the end of one FinishIncrementalMark to the start
        // of the next one)
        void adjustPolicyForNextMajorCycle(bool okToShrinkHeap);

        // Called from the policy event handler to compute the GC policy for the next
        // minor collection cycle (from the end of one IncrementalMark to the start of the
        // next one)
        void adjustPolicyForNextMinorCycle();

        // ----- Private data --------------------------------------

        GC * const gc;
        GCHeap * const heap;

        // The time recorded the last time we received signalEndOfIncrementalMark
        uint64_t timeEndOfLastIncrementalMark;

        // The time recorded the last time we received signalStartOfCollection
        uint64_t timeStartOfLastCollection;

        // The time recorded the last time we received signalEndOfCollection
        uint64_t timeEndOfLastCollection;

        // The total number of blocks owned by GC, and the maximum such number
        size_t blocksOwned;
        size_t maxBlocksOwned;

        // Dependent allocation for this GC
        size_t dependentAllocation;

        // The number of objects scanned during the last collection cycle.
        uint32_t objectsScannedExactlyLastCollection;
        uint32_t objectsScannedConservativelyLastCollection;
        uint32_t objectsScannedPointerfreeLastCollection;

        // The number of bytes scanned during the last collection cycle.
        uint32_t bytesScannedExactlyLastCollection;
        uint32_t bytesScannedConservativelyLastCollection;
        uint32_t bytesScannedPointerfreeLastCollection;

        // The number of objects scanned since startup, less the number scanned
        // during the last collection cycle.
        uint64_t objectsScannedExactlyTotal;
        uint64_t objectsScannedConservativelyTotal;
        uint64_t objectsScannedPointerfreeTotal;
        
        // The number of bytes scanned since startup less the ones scanned during the
        // last collection cycle.
        uint64_t bytesScannedExactlyTotal;
        uint64_t bytesScannedConservativelyTotal;
        uint64_t bytesScannedPointerfreeTotal;

        // Temporaries for holding the start time / start event until the end event arrives
        uint64_t start_time;
        PolicyEvent start_event;

        // Value returned by lowerLimitCollectionThreshold() and set by setLowerLimitCollectionThreshold():
        // the heap size, in blocks, below which we do not collect.
        uint32_t collectionThreshold;

        // true if a forced garbage collection has been requested
        bool fullCollectionQueued;

        // true if the ZCT statistics for the current GC cycle should be cleared;
        // this is required because the ZCT stats may need to be updated before
        // StartIncrementalMark but can't be cleared at the end of FinalizeAndSweep.
        bool pendingClearZCTStats;

#ifdef MMGC_POLICY_PROFILING
        // Records the heap population before we sweep.  Unit is blocks except where noted.
        size_t heapAllocatedBeforeSweep;
        size_t heapUsedBeforeSweep;
        size_t gcAllocatedBeforeSweep;
        size_t gcBytesUsedBeforeSweep;

        // Barrier stages hit in InlineWriteBarrier: examine mark, and hit the barrier.
        uint64_t barrierStageTotal[2];
        uint32_t barrierStageLastCollection[2];

        // Reap work, overall
        uint64_t objectsReaped;
        uint64_t bytesReaped;
        uint64_t objectsPinned;

        // Allocation work, overall
        uint64_t objectsAllocated;
        uint64_t bytesAllocated;
#endif
#ifdef MMGC_POINTINESS_PROFILING
        // Track the number of scannable words, the number that passes the initial range
        // check, and the number that turn out to be (conservative) pointers.
        //
        // These are cleared at the start of each GC so stats are only valid for one
        // GC cycle.
        uint64_t candidateWords;
        uint64_t couldBePointer;
        uint64_t actuallyIsPointer;
#endif
#ifdef MMGC_REFCOUNT_PROFILING
        uint64_t incrementRefTotal;
        uint64_t incrementRefLastCollection;
        uint64_t decrementRefTotal;
        uint64_t decrementRefLastCollection;
        uint32_t zctPeakSize;
        uint64_t addZCTTotal;
        uint64_t addZCTLastCollection;
        uint64_t removeZCTTotal;
        uint64_t removeZCTLastCollection;
        uint64_t addZCTInitialTotal;
        uint64_t removeZCTFinalTotal;
#endif

        // Various policy parameters.  For more documentation, see comments in GC.cpp.

        // max pause time in seconds
        double P;

        // approximate mark rate in bytes/sec, [1M,infty)
        double R;

        // requested inverse load factors (1,infty)
        double* L_ideal;

        // upper heap size (MB) at which the corresponding L_ideal applies
        double* L_cutoff;

        // adjusted inverse load factor (adjusted for heap pressure, growth, etc)
        double L_actual;

        // gc trigger as fraction of allocation budget to use before triggering GC [0,1]
        double T;

        // ratio of gc work to mutator work while the gc is running
        double G;

        // largest multiple of a selected L_ideal to which L_actual can grow (unless 0, which means unlimited)
        double X;

        // Value of the heap size, H, as computed at the beginning of the last major cycle
        double H_previous;

        // The allocation budget for the major GC cycle.  This variable remains constant throughout
        // the major cycle.
        double majorAllocationBudget;

        // the remaining allocation budget for the major GC cycle.  (This can go negative
        // and the variable must accomodate negative values.  It's not frequently accessed.)
        double remainingMajorAllocationBudget;

        // the allocation budget for the minor GC cycle.  This variable remains constant
        // throughout the minor cycle, is measured in bytes, and must be positive.
        int32_t minorAllocationBudget;

        // the remaining allocation budget for the minor GC cycle.  Initially this has the
        // value of minorAllocationBudget; the allocation request size is subtracted for
        // every allocation.  This variable can go negative because we can overshoot the
        // budget.
        int32_t remainingMinorAllocationBudget;

        // Temporaries used to compute R
        uint64_t adjustR_startTime;
        uint64_t adjustR_totalTime;
    };
}

#endif // __MMgc_GCPolicyManager__

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

namespace MMgc
{
#ifndef max
    inline uint64_t max(uint64_t a, uint64_t b) { return a > b ? a : b; }
#endif
    
    // Scanning rates (bytes/sec).
    //
    // Less than R_LOWER_LIMIT is not credible and is probably a measurement error / measurement
    // noise.  A too low value will tend to throw policy out of whack.
    //
    // 10MB/sec is the typical observed rate on the HTC Fuze, a mid-range 2009 smartphone.
    // 400MB/sec is the typical observed rate on a 2.6GHz MacBook Pro.
    // 100MB/sec seems like a reasonable approximation to let the application get off the ground.

#define R_LOWER_LIMIT (10*1024*1024)
#define R_INITIAL_VALUE (10*R_LOWER_LIMIT)
#define GREEDY_TRIGGER (-(INT_MAX/2))               // must be <= 0 but should never go positive as a result of a single alloc action or multiple free actions

    GCConfig::GCConfig()
        : collectionThreshold(256) // 4KB blocks, that is, 1MB
        , markstackAllowance(0)
        , exactTracing(true)
        , drc(true)
        , validateDRC(false)
        , incrementalValidation(false)
        , mode(kIncrementalGC)
    {}

    GCPolicyManager::GCPolicyManager(GC* gc, GCHeap* heap, GCConfig& config)
        // public
        : timeStartIncrementalMark(0)
        , timeIncrementalMark(0)
        , timeFinalRootAndStackScan(0)
        , timeFinalizeAndSweep(0)
        , timeReapZCT(0)
        , timeInLastCollection(0)
        , timeEndToEndLastCollection(0)
        , timeReapZCTLastCollection(0)
        , timeMaxStartIncrementalMark(0)
        , timeMaxIncrementalMark(0)
        , timeMaxFinalRootAndStackScan(0)
        , timeMaxFinalizeAndSweep(0)
        , timeMaxReapZCT(0)
        , timeMaxStartIncrementalMarkLastCollection(0)
        , timeMaxIncrementalMarkLastCollection(0)
        , timeMaxFinalRootAndStackScanLastCollection(0)
        , timeMaxFinalizeAndSweepLastCollection(0)
        , timeMaxReapZCTLastCollection(0)
        , countStartIncrementalMark(0)
        , countIncrementalMark(0)
        , countFinalRootAndStackScan(0)
        , countFinalizeAndSweep(0)
        , countReapZCT(0)
        // private
        , gc(gc)
        , heap(heap)
        , timeEndOfLastIncrementalMark(0)
        , timeStartOfLastCollection(0)
        , timeEndOfLastCollection(0)
        , blocksOwned(0)
        , maxBlocksOwned(0)
        , dependentAllocation(0)
        , objectsScannedExactlyLastCollection(0)
        , objectsScannedConservativelyLastCollection(0)
        , objectsScannedPointerfreeLastCollection(0)
        , bytesScannedExactlyLastCollection(0)
        , bytesScannedConservativelyLastCollection(0)
        , bytesScannedPointerfreeLastCollection(0)
        , objectsScannedExactlyTotal(0)
        , objectsScannedConservativelyTotal(0)
        , objectsScannedPointerfreeTotal(0)
        , bytesScannedExactlyTotal(0)
        , bytesScannedConservativelyTotal(0)
        , bytesScannedPointerfreeTotal(0)
        , start_time(0)
        , start_event(NO_EVENT)
        , collectionThreshold(config.collectionThreshold)
        , fullCollectionQueued(false)
        , pendingClearZCTStats(false)
#ifdef MMGC_POLICY_PROFILING
        , heapAllocatedBeforeSweep(0)
        , heapUsedBeforeSweep(0)
        , gcAllocatedBeforeSweep(0)
        , gcBytesUsedBeforeSweep(0)
        , objectsReaped(0)
        , bytesReaped(0)
        , objectsPinned(0)
        , objectsAllocated(0)
        , bytesAllocated(0)
#endif
#ifdef MMGC_POINTINESS_PROFILING
        , candidateWords(0)
        , couldBePointer(0)
        , actuallyIsPointer(0)
#endif
#ifdef MMGC_REFCOUNT_PROFILING
        , incrementRefTotal(0)
        , incrementRefLastCollection(0)
        , decrementRefTotal(0)
        , decrementRefLastCollection(0)
        , zctPeakSize(0)
        , addZCTTotal(0)
        , addZCTLastCollection(0)
        , removeZCTTotal(0)
        , removeZCTLastCollection(0)
        , addZCTInitialTotal(0)
        , removeZCTFinalTotal(0)
#endif
        , P(0.005)              // seconds; 5ms.  The marker /will/ overshoot this significantly
        , R(R_INITIAL_VALUE)    // bytes/second; will be updated on-line
        , L_ideal(heap->Config().gcLoad)
        , L_cutoff(heap->Config().gcLoadCutoff)
        , L_actual(L_ideal[0])
        , T(1.0-(1.0/L_actual))
        , G(heap->Config().gcEfficiency)
        , X(heap->Config().gcLoadCeiling)
        , H_previous(0)
        , majorAllocationBudget(0)
        , remainingMajorAllocationBudget(0)
        , minorAllocationBudget(0)
        , remainingMinorAllocationBudget(0)
        , adjustR_startTime(0)
        , adjustR_totalTime(0)
    {
#ifdef MMGC_POLICY_PROFILING
        for ( size_t i=0 ; i < ARRAY_SIZE(barrierStageTotal) ; i++ ) {
            barrierStageTotal[i] = 0;
            barrierStageLastCollection[i] = 0;
        }
#endif
        adjustPolicyInitially();
    }

    void GCPolicyManager::shutdown()
    {
#ifdef MMGC_POLICY_PROFILING
        if (summarizeGCBehaviorAtEnd())
            PrintGCBehaviorStats(false);
#endif
    }

    // Garbage collection policy.
    //
    // Garbage collection is incremental and driven by allocation, heap pressure, and a
    // number of parameter settings.  The policy is documented and analyzed in doc/mmgc/policy.pdf,
    // this is a recap.
    //
    // Parameters fixed in the code:
    //
    // P (max pause)  can be tuned but within a limited range, but we treat it as
    //   constant.  Right now we use it to limit pauses in incremental marking only;
    //   it does not control ZCT reaping, final root and stack scan, or finalize
    //   and sweep.  (Those are all bugs.)  On a desktop system the marker sticks
    //   to P pretty well; on phones it has trouble with that, either because of
    //   clock resolution (P=5ms normally) or because of its recursive behavior
    //   that means the timeout is not checked sufficiently often, or because large
    //   objects take a long time to scan.
    // F (floor)  size of heap below which we don't collect, can be tuned but in limited ways,
    //   host code actually changes it.  Returned by lowerLimitCollectionThreshold().
    // R (rate of marking) is given by the hardware mainly (MB/s), updated on-line
    // V (voracity of new block allocation) is given by the program, ratio of blocks
    //   gcheap allocates from OS to ratio it allocates from already committed memory
    // H (heap)  size of heap at the end of one collection cycle
    // X (multiplier)  largest multiple of L to which the effective L should be allowed to
    //   grow to meet G.
    // T (allocation trigger) fraction of allocation budget to use before triggering
    //   the first mark increment (range 0..1).  With T=1 we have nonincremental GC;
    //   with T=0 the GC is always running.  A low T is desirable except that it puts
    //   more pressure on the write barrier.  A good T is (1-(1/L)) where L is the
    //   initial or given L.
    //
    // Tunable parameters:
    //
    // L (load factor)  ratio of heap size just before finishing a collection
    //   (before any reclamation) to the heap size just after finishing the previous
    //   collection (following any reclamation) (range 1..infty).  Effectively expresses
    //   the ratio of memory used to live program data.
    //
    // G (max GC fraction of time)  ceiling on ratio of GC time to total time in the
    //   interval when the incremental GC is running.  This controls pause clustering
    //   to some extent by recomputing a new and larger L if the GC time does not
    //   stay below the ceiling.  (It /could/ be used to reduce T if L can't be
    //   increased because it is up against its own ceiling, but experiments have
    //   not shown that to be fruitful so we don't do it yet.)
    //
    // We wish the heap size just before finishing a collection cycle to be HL, and
    // the allocation budget following a collection is H(L-1), and we work to make
    // that true in the steady state.
    //
    // Facts:
    //
    // W = L/((1-T)(L-1)) : bytes to be scanned per byte allocated when gc is active
    // M = PR : max mark work per mark increment
    // A = M/W : max bytes allocated between mark increments (because W work is
    //   needed for every allocated byte); if allocation proceeds faster than this then
    //   the marker cannot keep up to honor both L and P in the steady state
    //
    // How these constrain each other:
    //
    // If T is too high or L is too low then sticking to P may be futile as we will have
    // excessive pause clustering, which is no better.
    //
    // G is meant to constrain L: if G can't be met then L is effectively raised so
    // that G can be met.  This is typically the case during system warmup, where a lot
    // of allocation in a smallish heap leads to a lot of GC activity with little gain.
    // The computation is like this:
    //    a = t(end of FinishIncrementalMark) - t(start of StartIncrementalMark)
    //    b = sum of time spent in StartIncrementalMark, IncrementalMark, FinishIncrementalMark
    // If b > aG then we must throttle GC activity for the next cycle.  G controls clustering.

    // Open issues / to do:
    //  - precise semantics of V, specifically, what is the interval over which it's computed
    //  - incorporate V in the computations

    double GCPolicyManager::W()
    {
        return L_actual / ((1 - T) * (L_actual - 1));
    }

    // Warning: GIGO applies with full force: if R is badly mispredicted (based on too little data)
    // or the marker is too far off from P, then the value of A can be bad.  In particular A may be
    // very low, which will tend to increase GC overhead because the marker will run too often.

    double GCPolicyManager::A() {
        return P * R / W();
    }

    // Called when an incremental mark starts

    void GCPolicyManager::startAdjustingR()
    {
        adjustR_startTime = now();
    }

    // Called when an incremental mark ends

    void GCPolicyManager::endAdjustingR()
    {
        adjustR_totalTime += now() - adjustR_startTime;
        R = (double(bytesScannedExactlyTotal) + double(bytesScannedConservativelyTotal) + double(bytesScannedPointerfreeTotal)) / (double(adjustR_totalTime) / double(VMPI_getPerformanceFrequency()));
        if (R < R_LOWER_LIMIT)
            R = R_LOWER_LIMIT;
    }

    double GCPolicyManager::queryLoadForHeapsize(double H)
    {
        int i=0;
        while (H / (1024*1024) >= L_cutoff[i])
            i++;
        return L_ideal[i];
    }

    // The throttles here guard against excessive growth.

    void GCPolicyManager::adjustL(double H)
    {
        double L_selected = queryLoadForHeapsize(H);
        double a = double(timeEndToEndLastCollection);
        double b = double(timeInLastCollection);

        if (b > a*G) {
            double growth = (L_actual - 1) * (1 + timeInLastCollection/timeEndToEndLastCollection);
            if (growth > 1)
                growth = 1;
            L_actual = L_actual + growth;
            if (X != 0 && L_actual > X*L_selected)
                L_actual = X*L_selected;
        }
        else
            L_actual = (L_actual + L_selected) / 2;
    }

    // Called at the start
    void GCPolicyManager::adjustPolicyInitially()
    {
        remainingMajorAllocationBudget = majorAllocationBudget = double(lowerLimitCollectionThreshold()) * double(GCHeap::kBlockSize);

        if (gc->incremental)
            remainingMinorAllocationBudget = minorAllocationBudget = int32_t(max(1,int32_t(remainingMajorAllocationBudget * T)));
        else
            remainingMinorAllocationBudget = int32_t(remainingMajorAllocationBudget);

        remainingMajorAllocationBudget -= remainingMinorAllocationBudget;
        if (gc->greedy)
            remainingMinorAllocationBudget = GREEDY_TRIGGER;
    }

    // Called when a collection ends
    void GCPolicyManager::adjustPolicyForNextMajorCycle(bool okToShrinkHeap)
    {
        double H = double(gc->GetBytesInUse() + dependentAllocation);

        if (!okToShrinkHeap && H < H_previous)
            H = H_previous;
        
        H_previous = H;

        // Compute L_actual, which takes into account how much time we spent in GC
        // during the last cycle
        adjustL(H);

        // The budget is H(L-1), with a floor
        double remainingBeforeGC = double(lowerLimitCollectionThreshold()) * double(GCHeap::kBlockSize) - H;
        remainingMajorAllocationBudget = majorAllocationBudget = H * (L_actual - 1.0);
        if (remainingMajorAllocationBudget < remainingBeforeGC)
            remainingMajorAllocationBudget = remainingBeforeGC;

        if (gc->incremental) {
            remainingMinorAllocationBudget = minorAllocationBudget = int32_t(max(1,int32_t(remainingMajorAllocationBudget * T)));
        } else {
            remainingMinorAllocationBudget = int32_t(remainingMajorAllocationBudget);
        }

#ifdef MMGC_POLICY_PROFILING
        if (summarizeGCBehavior())
            GCLog("[gcbehavior] policy: mark-rate=%.2f (MB/sec) adjusted-L=%.2f kbytes-live=%.0f kbytes-target=%.0f\n",
                  R / (1024*1024),
                  L_actual,
                  H / 1024.0,
                  (H+remainingMajorAllocationBudget) / 1024.0);
#endif
        remainingMajorAllocationBudget -= remainingMinorAllocationBudget;

        if (gc->greedy)
            remainingMinorAllocationBudget = GREEDY_TRIGGER;
    }

    // Called when an incremental mark ends
    void GCPolicyManager::adjustPolicyForNextMinorCycle()
    {
        if (remainingMinorAllocationBudget < 0) {
            // We overshot the allocation budget.  Sometimes this overshoot can be
            // much greater than A, if a single large object is allocated, so the
            // clean solution is to adjust the major budget by the overshoot first.
            remainingMajorAllocationBudget -= -remainingMinorAllocationBudget;
            remainingMinorAllocationBudget = 0;
        }

        double aval = A();
        if(aval > INT_MAX)
            aval = INT_MAX;
        else if (aval < 1)
            aval = 1;
        remainingMinorAllocationBudget = int32_t(aval);
        minorAllocationBudget = remainingMinorAllocationBudget;
        GCAssert(minorAllocationBudget > 0);
        remainingMajorAllocationBudget -= remainingMinorAllocationBudget;

        if (gc->greedy)
            remainingMinorAllocationBudget = GREEDY_TRIGGER;
    }

    // Called when an incremental mark is about to start.  The premise is that if the
    // application stays within the budget then the value returned here will correspond
    // to the desired time slice.  But if the application allocates some huge block that
    // blows the budget then we up the mark work here, /even if/ that means violating
    // the budget.  The idea is that that won't happen very often, and that it's more
    // important not to overshoot the total heap size target than to stick to the
    // time slice always.  The other alternative is to adjust the amount of allocation
    // allowed before the next mark downward, but as soon as we do that aggressively
    // we get into pause clustering issues and it will seem like one long GC pause anyway.

    uint32_t GCPolicyManager::incrementalMarkMilliseconds() {
        // Nonsensical to call this in non-incremental mode
        GCAssert(gc->incremental);
        // Bad to divide by 0 here.
        GCAssert(minorAllocationBudget != 0);
        return uint32_t(P * 1000.0 * double(minorAllocationBudget - remainingMinorAllocationBudget) / double(minorAllocationBudget));
    }

    bool GCPolicyManager::queryEndOfCollectionCycle() {
        return remainingMajorAllocationBudget <= 0;
    }

    void GCPolicyManager::setLowerLimitCollectionThreshold(uint32_t blocks) {
        collectionThreshold = blocks;
    }

    size_t GCPolicyManager::blocksOwnedByGC() {
        return blocksOwned;
    }

    uint64_t GCPolicyManager::bytesMarked() {
        return ((bytesScannedExactlyTotal + bytesScannedConservativelyTotal + bytesScannedPointerfreeTotal) + 
                (bytesScannedExactlyLastCollection + bytesScannedConservativelyLastCollection + bytesScannedPointerfreeLastCollection));
    }

    uint64_t GCPolicyManager::objectsMarked() {
        return ((objectsScannedExactlyTotal + objectsScannedConservativelyTotal + objectsScannedPointerfreeTotal) +
                (objectsScannedExactlyLastCollection + objectsScannedConservativelyLastCollection + objectsScannedPointerfreeLastCollection));
    }

    uint32_t GCPolicyManager::queryZCTBudget(uint32_t zctSizeBlocks) {
        (void)zctSizeBlocks;
        return 1;
    }
    
    void GCPolicyManager::signalBlockAllocation(size_t blocks) {
        blocksOwned += blocks;
        if (blocksOwned > maxBlocksOwned)
            maxBlocksOwned = blocksOwned;
    }
    
    void GCPolicyManager::signalBlockDeallocation(size_t blocks) {
        blocksOwned -= blocks;
    }
    
    void GCPolicyManager::signalDependentAllocation(size_t nbytes) {
        dependentAllocation += nbytes;
    }
    
    void GCPolicyManager::signalDependentDeallocation(size_t nbytes) {
        dependentAllocation -= nbytes;
    }
    
    void GCPolicyManager::signalMemoryStatusChange(MemoryStatus from, MemoryStatus to) {
        (void)from;
        (void)to;
        // do nothing for the moment
    }
    
    void GCPolicyManager::signalStartCollection(GC* gc) {
        (void)gc;
        // do nothing for the moment
    }
    
    void GCPolicyManager::signalEndCollection(GC* gc) {
        (void)gc;
        // do nothing for the moment
    }
    
    void GCPolicyManager::SignalImminentAbort() {
        start_event = NO_EVENT;
    }

    void GCPolicyManager::signal(PolicyEvent ev) {
        switch (ev) {
            case START_StartIncrementalMark:
                heap->gcManager.signalStartCollection(gc);
                timeStartOfLastCollection = now();
                timeInLastCollection = 0;
                timeMaxStartIncrementalMarkLastCollection = 0;
                timeMaxIncrementalMarkLastCollection = 0;
                timeMaxFinalRootAndStackScanLastCollection = 0;
                timeMaxFinalizeAndSweepLastCollection = 0;
                objectsScannedExactlyTotal += objectsScannedExactlyLastCollection;
                objectsScannedConservativelyTotal += objectsScannedConservativelyLastCollection;
                objectsScannedPointerfreeTotal += objectsScannedPointerfreeLastCollection;
                objectsScannedExactlyLastCollection = 0;
                objectsScannedConservativelyLastCollection = 0;
                objectsScannedPointerfreeLastCollection = 0;
                bytesScannedExactlyTotal += bytesScannedExactlyLastCollection;
                bytesScannedConservativelyTotal += bytesScannedConservativelyLastCollection;
                bytesScannedPointerfreeTotal += bytesScannedPointerfreeLastCollection;
                bytesScannedExactlyLastCollection = 0;
                bytesScannedConservativelyLastCollection = 0;
                bytesScannedPointerfreeLastCollection = 0;
#ifdef MMGC_POINTINESS_PROFILING
                candidateWords = 0;
                couldBePointer = 0;
                actuallyIsPointer = 0;
#endif
                startAdjustingR();
                goto clear_zct_stats;
            case START_IncrementalMark:
                startAdjustingR();
                goto common_actions;
            case START_FinalRootAndStackScan:
                startAdjustingR();
                goto common_actions;
            case START_FinalizeAndSweep:
#ifdef MMGC_POLICY_PROFILING
                heapAllocatedBeforeSweep = heap->GetTotalHeapSize();
                heapUsedBeforeSweep = (heap->GetTotalHeapSize() - heap->GetFreeHeapSize());
                gcAllocatedBeforeSweep = gc->GetNumBlocks();
                gcBytesUsedBeforeSweep = gc->GetBytesInUse();
#endif
                goto common_actions;
            case START_ReapZCT:
            clear_zct_stats:
                if (pendingClearZCTStats) {
                    pendingClearZCTStats = false;
                    timeReapZCTLastCollection = 0;
                    timeMaxReapZCTLastCollection = 0;
                }
                /*FALLTHROUGH*/
            common_actions:
#ifdef DEBUG
                if (start_event != NO_EVENT)
                    GCDebugMsg(true, "Should not have a start_event but had start_event=%d, ev=%d\n", (int)start_event, (int)ev);
#endif
                start_time = now();
                start_event = ev;
                return; // to circumvent resetting of start_event below
        }

#ifdef DEBUG
        {
            PolicyEvent ev0 = ev;
            if (ev0 == END_FinalizeAndSweepNoShrink)
                ev0 = END_FinalizeAndSweep;
            if (start_event != (PolicyEvent)(ev0 - 1))
                GCDebugMsg(true, "Should have had a matching start_event but instead had start_event=%d, ev=%d\n", (int)start_event, (int)ev0);
        }
#endif
        start_event = NO_EVENT;

        uint64_t t = now();
        uint64_t elapsed = t - start_time;

        switch (ev) {
            case END_StartIncrementalMark:
                countStartIncrementalMark++;
                timeStartIncrementalMark += elapsed;
                timeMaxStartIncrementalMark = max(timeMaxStartIncrementalMark, elapsed);
                timeMaxStartIncrementalMarkLastCollection = max(timeMaxStartIncrementalMarkLastCollection, elapsed);
                endAdjustingR();
                break;
            case END_FinalRootAndStackScan:
                countFinalRootAndStackScan++;
                timeFinalRootAndStackScan += elapsed;
                timeMaxFinalRootAndStackScan = max(timeMaxFinalRootAndStackScan, elapsed);
                timeMaxFinalRootAndStackScanLastCollection = max(timeMaxFinalRootAndStackScanLastCollection, elapsed);
                endAdjustingR();
                break;
            case END_ReapZCT:
                countReapZCT++;
                timeReapZCT += elapsed;
                timeReapZCTLastCollection += elapsed;
                timeMaxReapZCT = max(timeMaxReapZCT, elapsed);
                timeMaxReapZCTLastCollection = max(timeMaxReapZCTLastCollection, elapsed);
                break;
            case END_IncrementalMark:
                countIncrementalMark++;
                timeIncrementalMark += elapsed;
                timeMaxIncrementalMark = max(timeMaxIncrementalMark, elapsed);
                timeMaxIncrementalMarkLastCollection = max(timeMaxIncrementalMarkLastCollection, elapsed);
                timeEndOfLastIncrementalMark = t;
                endAdjustingR();
                break;
            case END_FinalizeAndSweep:
            case END_FinalizeAndSweepNoShrink:
                countFinalizeAndSweep++;
                timeFinalizeAndSweep += elapsed;
                timeMaxFinalizeAndSweep = max(timeMaxFinalizeAndSweep, elapsed);
                timeMaxFinalizeAndSweepLastCollection = max(timeMaxFinalizeAndSweepLastCollection, elapsed);
                timeEndOfLastCollection = t;
                timeEndToEndLastCollection = timeEndOfLastCollection - timeStartOfLastCollection;
                pendingClearZCTStats = true;
                heap->gcManager.signalEndCollection(gc);
                break;
        }
        if (ev != END_ReapZCT)
            timeInLastCollection += elapsed;

#ifdef MMGC_POLICY_PROFILING
        bool endOfCollection = (ev == END_FinalizeAndSweep || ev == END_FinalizeAndSweepNoShrink);
        if (summarizeGCBehavior() && endOfCollection)
            PrintGCBehaviorStats();
#endif // MMGC_POLICY_PROFILING
#ifdef MMGC_POLICY_PROFILING
        // Need to clear these before any writes can occur, so that means right here: if earlier,
        // we'd not have them for reporting.
        if (endOfCollection) {
            for ( size_t i=0 ; i < ARRAY_SIZE(barrierStageTotal) ; i++ ) {
                barrierStageTotal[i] += barrierStageLastCollection[i];
                barrierStageLastCollection[i] = 0;
            }
        }
#endif
#ifdef MMGC_REFCOUNT_PROFILING
        // Need to clear these before any refcount operations can occur, so that means right here:
        // if earlier, we'd not have them for reporting.
        incrementRefTotal += incrementRefLastCollection;
        incrementRefLastCollection = 0;
        decrementRefTotal += decrementRefLastCollection;
        decrementRefTotal = 0;
        addZCTTotal += addZCTLastCollection;
        addZCTLastCollection = 0;
        removeZCTTotal += removeZCTLastCollection;
        removeZCTLastCollection = 0;
#endif
        switch (ev) {
            case END_IncrementalMark:
                adjustPolicyForNextMinorCycle();
                break;
            case END_FinalizeAndSweep:
                adjustPolicyForNextMajorCycle(true);
                break;
            case END_FinalizeAndSweepNoShrink:
                adjustPolicyForNextMajorCycle(false);
                break;
        }
    }

#ifdef MMGC_POLICY_PROFILING
    bool GCPolicyManager::summarizeGCBehavior() {
        return GCHeap::GetGCHeap()->Config().gcbehavior >= 2;
    }
    
    bool GCPolicyManager::summarizeGCBehaviorAtEnd() {
        return GCHeap::GetGCHeap()->Config().gcbehavior >= 1;
    }
    
    double GCPolicyManager::ticksToMillis(uint64_t ticks) {
        return double(ticks) * 1000.0 / double(VMPI_getPerformanceFrequency());
    }

    /**
     * Note, final=1 is special and means statistics dumped at the end of the run,
     * just as the GC is about to shut down.
     *
     * Note that a lot of the stats for the last GC cycle do make sense even if
     * afterCollection is false, because they would be stats for an unfinished
     * incremental collection.  But they only make sense if gc->IncrementalMarking()
     * is true.
     *
     * TODO: alloc/free volumes might be helpful.
     */
    void GCPolicyManager::PrintGCBehaviorStats(bool afterCollection)
    {
        size_t bytesInUse = gc->GetBytesInUse();
        size_t heapAllocated = heap->GetTotalHeapSize();
        char buf[256];
        uint32_t utotal;
        double dtotal;

        GCLog("--------------------\n");
        GCLog("[gcbehavior] tag: gc=%p final=%d gccount=%u incremental-marks=%u\n",
              (void*)gc,
              (int)!afterCollection,
              (unsigned)countFinalizeAndSweep, 
              (unsigned)countIncrementalMark);

        GCLog("[gcbehavior] allocation-work: objects=%.0f bytes=%.0f\n",
              double(objectsAllocated),
              double(bytesAllocated));
        if (afterCollection)
        {
            GCLog("[gcbehavior] occupancy-before: blocks-heap-allocated=%u blocks-heap-used=%u blocks-gc-allocated=%u blocks-gc-used=%u\n",
                  (unsigned)heapAllocatedBeforeSweep, (unsigned)heapUsedBeforeSweep,
                  (unsigned)gcAllocatedBeforeSweep, (unsigned)(gcBytesUsedBeforeSweep + GCHeap::kBlockSize - 1)/GCHeap::kBlockSize);
            GCLog("[gcbehavior] occupancy-after: blocks-heap-allocated=%u blocks-heap-used=%u blocks-gc-allocated=%u blocks-gc-used=%u\n",
                  (unsigned)heapAllocated, (unsigned)(heapAllocated - heap->GetFreeHeapSize()),
                  (unsigned)gc->GetNumBlocks(), (unsigned)(bytesInUse + GCHeap::kBlockSize - 1)/GCHeap::kBlockSize);
            GCLog("[gcbehavior] user-data: kbytes-before-sweep=%u kbytes-after-sweep=%u ratio=%.2f\n",
                  unsigned(gcBytesUsedBeforeSweep/1024),
                  unsigned(bytesInUse/1024),
                  double(gcBytesUsedBeforeSweep)/double(bytesInUse));
        }
        else
        {
            size_t maxHeapBlocks;
            size_t maxPrivateBlocks;

            heap->GetMaxTotalHeapSize(maxHeapBlocks, maxPrivateBlocks);
            GCLog("[gcbehavior] peak-occupancy: blocks-heap-allocated=%u blocks-gc-allocated=%u blocks-private=%u\n",
                  (unsigned)maxHeapBlocks,
                  (unsigned)maxBlocksOwned,
                  (unsigned)maxPrivateBlocks);
            GCLog("[gcbehavior] occupancy: blocks-heap-allocated=%u blocks-heap-used=%u blocks-gc-allocated=%u blocks-gc-used=%u, blocks-gc-peak=%u\n",
                  (unsigned)heapAllocated,
                  (unsigned)(heapAllocated - heap->GetFreeHeapSize()),
                  (unsigned)gc->GetNumBlocks(),
                  (unsigned)(bytesInUse + GCHeap::kBlockSize - 1)/GCHeap::kBlockSize,
                  (unsigned)maxBlocksOwned);
            GCLog("[gcbehavior] user-data: kbytes=%u\n",
                  unsigned(bytesInUse/1024));
        }
#ifdef MMGC_POINTINESS_PROFILING
        GCLog("[gcbehavior] pointiness: candidates=%u inrange=%u actual=%u\n",
              unsigned(candidateWords),
              unsigned(couldBePointer),
              unsigned(actuallyIsPointer));
#endif
#ifdef MMGC_MARKSTACK_DEPTH
        GCLog("[gcbehavior] mark-stack: stack-max-depth=%u barrier-max-depth=%u\n",
              gc->m_incrementalWork.MaxCount(),
              gc->m_barrierWork.MaxCount());
#endif
        if (afterCollection || gc->IncrementalMarking())
        {
            GCLog("[gcbehavior] markitem-last-gc-objects: exactly=%u conservatively=%u pointerfree=%u\n",
                  unsigned(objectsScannedExactlyLastCollection),
                  unsigned(objectsScannedConservativelyLastCollection),
                  unsigned(objectsScannedPointerfreeLastCollection));
            GCLog("[gcbehavior] markitem-last-gc-bytes: exactly=%u conservatively=%u pointerfree=%u\n",
                  unsigned(bytesScannedExactlyLastCollection),
                  unsigned(bytesScannedConservativelyLastCollection),
                  unsigned(bytesScannedPointerfreeLastCollection));
        }
        GCLog("[gcbehavior] markitem-all-gc-objects: exactly=%llu conservatively=%llu pointerfree=%llu\n",
              (unsigned long long)(objectsScannedExactlyLastCollection + objectsScannedExactlyTotal),
              (unsigned long long)(objectsScannedConservativelyLastCollection + objectsScannedConservativelyTotal),
              (unsigned long long)(objectsScannedPointerfreeLastCollection + objectsScannedPointerfreeTotal));
        GCLog("[gcbehavior] markitem-all-gc-bytes: exactly=%llu conservatively=%llu pointerfree=%llu\n",
              (unsigned long long)(bytesScannedExactlyLastCollection + bytesScannedExactlyTotal),
              (unsigned long long)(bytesScannedConservativelyLastCollection + bytesScannedConservativelyTotal),
              (unsigned long long)(bytesScannedPointerfreeLastCollection + bytesScannedPointerfreeTotal));

        size_t blimit = ARRAY_SIZE(barrierStageLastCollection);
        utotal = 0;
        VMPI_sprintf(buf, "[gcbehavior] barrier-last-gc:");
        for ( size_t i=0 ; i < blimit ; i++ )
            utotal += barrierStageLastCollection[i];
        VMPI_sprintf(buf + strlen(buf), " total=%u", unsigned(utotal));
        for ( size_t i=0 ; i < blimit ; i++ )
            VMPI_sprintf(buf + strlen(buf), " stage%d=%u", unsigned(i), unsigned(barrierStageLastCollection[i]));
        VMPI_sprintf(buf + strlen(buf), " hit-ratio=%.2f\n",
                     double(barrierStageLastCollection[blimit-1])/double(utotal));
        GCLog(buf);

        dtotal = 0;
        VMPI_sprintf(buf, "[gcbehavior] barrier-all-gc: ");
        for ( size_t i=0 ; i < blimit ; i++ )
            dtotal += double(barrierStageLastCollection[i] + barrierStageTotal[i]);
        VMPI_sprintf(buf + strlen(buf), " total=%.0f", dtotal);
        for ( size_t i=0 ; i < blimit ; i++ )
            VMPI_sprintf(buf + strlen(buf), " stage%d=%.0f", unsigned(i), double(barrierStageLastCollection[i] + barrierStageTotal[i]));
        VMPI_sprintf(buf + strlen(buf), " hit-ratio=%.2f\n",
                     double(barrierStageLastCollection[blimit-1] + barrierStageTotal[blimit-1])/double(dtotal));
        GCLog(buf);

#ifdef MMGC_REFCOUNT_PROFILING
        GCLog("[gcbehavior] refcount-last-gc: increment=%.0f decrement=%.0f\n",
              double(incrementRefLastCollection),
              double(decrementRefLastCollection));
        GCLog("[gcbehavior] refcount-all-gc: increment=%.0f decrement=%.0f\n",
              double(incrementRefLastCollection + incrementRefTotal),
              double(decrementRefLastCollection + decrementRefTotal));
        GCLog("[gcbehavior] zct-traffic-last-gc: add=%.0f remove=%.0f\n",
              double(addZCTLastCollection),
              double(removeZCTLastCollection));
        GCLog("[gcbehavior] zct-traffic-all-gc: peak=%u add=%.0f add-initial-ratio=%.3f remove=%.0f remove-final-ratio=%.3f\n",
              unsigned(zctPeakSize),
              double(addZCTLastCollection + addZCTTotal),
              double(addZCTInitialTotal) / double(addZCTLastCollection + addZCTTotal),
              double(removeZCTLastCollection + removeZCTTotal),
              double(removeZCTFinalTotal) / double(removeZCTLastCollection + removeZCTTotal));
#endif

        GCLog("[gcbehavior] time-zct-reap: last-cycle=%.1f total=%.1f\n",
              ticksToMillis(timeReapZCTLastCollection),
              ticksToMillis(timeReapZCT));
        GCLog("[gcbehavior] work-zct-reap: reaps=%u objects-reaped=%.0f kbytes-reaped=%.0f objects-pinned=%.0f reaped-to-pinned=%.2f objects-reaped-per-ms=%.2f kbytes-reaped-per-ms=%.1f\n",
              unsigned(countReapZCT),
              double(objectsReaped),
              double(bytesReaped)/1024,
              double(objectsPinned),
              countReapZCT > 0 ? double(objectsReaped)/double(objectsPinned) : 0,
              countReapZCT > 0 ? double(objectsReaped)/ticksToMillis(timeReapZCT) : 0,
              countReapZCT > 0 ? (double(bytesReaped)/1024)/ticksToMillis(timeReapZCT) : 0);
        GCLog("[gcbehavior] pause-zct-reap: last-cycle=%.1f overall=%.1f\n",
              pendingClearZCTStats ? 0.0 : ticksToMillis(timeMaxReapZCTLastCollection),
              ticksToMillis(timeMaxReapZCT));
        if (afterCollection)
        {
            GCLog("[gcbehavior] time-last-gc: in-gc=%.1f end2end=%.1f mutator-efficiency=%.2f%%\n",
                  ticksToMillis(timeInLastCollection),
                  ticksToMillis(timeEndToEndLastCollection),
                  (timeEndToEndLastCollection == 0.0 ?  // edge cases
                   100.0 :
                   double(timeEndToEndLastCollection - timeInLastCollection) * 100.0 / double(timeEndToEndLastCollection)));
        }
        else
        {
            GCLog("[gcbehavior] time-last-gc: in-gc=%.1f\n", ticksToMillis(timeInLastCollection));
        }
        GCLog("[gcbehavior] time-all-gc: total=%.1f start-incremental-mark=%.1f incremental-mark=%.1f final-root-stack-scan=%.1f finalize-sweep=%.1f\n",
              ticksToMillis(timeStartIncrementalMark + timeIncrementalMark + timeFinalRootAndStackScan + timeFinalizeAndSweep),
              ticksToMillis(timeStartIncrementalMark),
              ticksToMillis(timeIncrementalMark),
              ticksToMillis(timeFinalRootAndStackScan),
              ticksToMillis(timeFinalizeAndSweep));
        if (afterCollection || gc->IncrementalMarking()) {
            GCLog("[gcbehavior] pause-last-gc: start-incremental-mark=%.1f incremental-mark=%.1f final-root-stack-scan=%.1f finalize-sweep=%.1f\n",
                  ticksToMillis(timeMaxStartIncrementalMarkLastCollection),
                  ticksToMillis(timeMaxIncrementalMarkLastCollection),
                  ticksToMillis(timeMaxFinalRootAndStackScanLastCollection),
                  ticksToMillis(timeMaxFinalizeAndSweepLastCollection));
        }
        GCLog("[gcbehavior] pause-all-gc: start-incremental-mark=%.1f incremental-mark=%.1f final-root-stack-scan=%.1f finalize-sweep=%.1f\n",
              ticksToMillis(timeMaxStartIncrementalMark),
              ticksToMillis(timeMaxIncrementalMark),
              ticksToMillis(timeMaxFinalRootAndStackScan),
              ticksToMillis(timeMaxFinalizeAndSweep));
    }
#endif // MMGC_POLICY_PROFILING
#ifdef MMGC_POLICY_PROFILING
    void GCPolicyManager::signalReapWork(uint32_t objects_reaped, uint32_t bytes_reaped, uint32_t objects_pinned)
    {
        objectsReaped += objects_reaped;
        bytesReaped += bytes_reaped;
        objectsPinned += objects_pinned;
    }
#endif

    uint32_t GCPolicyManager::queryExactPercentage()
    {
        uint64_t conserv = bytesScannedConservativelyTotal + bytesScannedConservativelyLastCollection;
        uint64_t exact = bytesScannedExactlyTotal + bytesScannedExactlyLastCollection;
        if(exact == 0)
            return 0;
        return uint32_t(exact * 100 / (exact+conserv));
    }
    
    double GCPolicyManager::queryAllocationBudgetFractionUsed()
    {
        double d = (remainingMajorAllocationBudget + remainingMinorAllocationBudget) / majorAllocationBudget;
        if (d < 0) d = 0;
        if (d > 1) d = 1;
        return 1 - d;
    }
}

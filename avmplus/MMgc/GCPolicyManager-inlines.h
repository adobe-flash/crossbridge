/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MMgc_GCPolicyManager_inlines__
#define __MMgc_GCPolicyManager_inlines__

namespace MMgc
{
    // GCPolicyManager
    // A number of the inline functions for this class are currently in GC.cpp

#ifdef MMGC_POLICY_PROFILING
    REALLY_INLINE void GCPolicyManager::signalWriteBarrierWork(int stage)
    {
        GCAssert(ARRAY_SIZE(barrierStageLastCollection) > size_t(stage));
        barrierStageLastCollection[stage]++;
    }
#endif

#ifdef MMGC_REFCOUNT_PROFILING
    REALLY_INLINE void GCPolicyManager::signalIncrementRef()
    {
        incrementRefLastCollection++;
    }

    REALLY_INLINE void GCPolicyManager::signalDecrementRef()
    {
        decrementRefLastCollection++;
    }

    REALLY_INLINE void GCPolicyManager::signalZCTAdd(bool initial, uint32_t population)
    {
        addZCTLastCollection++;
        if (initial)
            addZCTInitialTotal++;
        if (population > zctPeakSize)
            zctPeakSize = population;
    }

    REALLY_INLINE void GCPolicyManager::signalZCTRemove(bool final)
    {
        removeZCTLastCollection++;
        if (final)
            removeZCTFinalTotal++;
    }
#endif

#ifdef MMGC_POINTINESS_PROFILING
    REALLY_INLINE void GCPolicyManager::signalDemographics(size_t candidate_words, size_t could_be_pointer, size_t actually_is_pointer)
    {
        candidateWords += candidate_words;
        couldBePointer += could_be_pointer;
        actuallyIsPointer += actually_is_pointer;
    }
#endif
    
    REALLY_INLINE bool GCPolicyManager::signalAllocWork(size_t nbytes)
    {
#ifdef MMGC_POLICY_PROFILING
        objectsAllocated++;
        bytesAllocated += nbytes;
#endif
        remainingMinorAllocationBudget -= int32_t(nbytes);

        // Important to use < 0 not <= 0: in greedy mode we set the budget to
        // exactly the object size we're about to allocate.  Using <= 0 would
        // cause the allocation never to succeed.
        return remainingMinorAllocationBudget < 0;
    }

    // OPTIMIZEME: the byte counts are needed for policy, but the object counts are only for
    // unspecified profiling and we could in principle enable object counting only in a
    // profiling mode.

    REALLY_INLINE void GCPolicyManager::signalExactMarkWork(size_t nbytes)
    {
        objectsScannedExactlyLastCollection++;
        bytesScannedExactlyLastCollection += uint32_t(nbytes);
    }

    REALLY_INLINE void GCPolicyManager::signalConservativeMarkWork(size_t nbytes)
    {
        objectsScannedConservativelyLastCollection++;
        bytesScannedConservativelyLastCollection += uint32_t(nbytes);
    }

    REALLY_INLINE void GCPolicyManager::signalPointerfreeMarkWork(size_t nbytes)
    {
        objectsScannedPointerfreeLastCollection++;
        bytesScannedPointerfreeLastCollection += uint32_t(nbytes);
    }

    REALLY_INLINE void GCPolicyManager::signalFreeWork(size_t nbytes)
    {
        remainingMinorAllocationBudget += int32_t(nbytes);
    }
    
    REALLY_INLINE uint32_t GCPolicyManager::lowerLimitCollectionThreshold() {
        return collectionThreshold;
    }
    
    REALLY_INLINE uint64_t GCPolicyManager::now() {
        return VMPI_getPerformanceCounter();
    }
}

#endif // __MMgc_GCPolicyManager_inlines__

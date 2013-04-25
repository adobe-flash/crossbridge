/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FixedMalloc_inlines__
#define __FixedMalloc_inlines__

// Inline function definitions for FixedMalloc.

namespace MMgc
{
    /*static*/
    REALLY_INLINE FixedMalloc* FixedMalloc::GetInstance() { return instance; }

    /*static*/
    REALLY_INLINE FixedMalloc* FixedMalloc::GetFixedMalloc() { return instance; }

    REALLY_INLINE void* FixedMalloc::Alloc(size_t size, FixedMallocOpts flags)
    {
        // Observe that no size overflow check is needed for small allocations;
        // the large-object allocator performs the necessary checking in that case.
#ifdef DEBUG
        m_heap->CheckForOOMAbortAllocation();
#endif

        if (size <= (size_t)kLargestAlloc)
            return FindAllocatorForSize(size)->Alloc(size, flags);
        else
            return LargeAlloc(size, flags);
    }

    REALLY_INLINE void *FixedMalloc::PleaseAlloc(size_t size)
    {
        return OutOfLineAlloc(size, kCanFail);
    }

    REALLY_INLINE void FixedMalloc::Free(void *item)
    {
        if(item == 0)
            return;

#ifdef DEBUG
        EnsureFixedMallocMemory(item);
#endif

        if(IsLargeAlloc(item))
            LargeFree(item);
        else
            FixedAllocSafe::GetFixedAllocSafe(item)->Free(item);
    }

    REALLY_INLINE size_t FixedMalloc::Size(const void *item)
    {
        size_t size;
        if(IsLargeAlloc(item))
            size = LargeSize(item);
        else
            size = FixedAlloc::Size(item);
#ifdef MMGC_MEMORY_INFO
        size -= DebugSize();
#endif
        return size;
    }

    REALLY_INLINE size_t FixedMalloc::GetBytesInUse()
    {
        size_t totalAskSize, totalAllocated;
        GetUsageInfo(totalAskSize, totalAllocated);
        return totalAllocated;
    }

    /*static*/
    REALLY_INLINE bool FixedMalloc::IsLargeAlloc(const void *item)
    {
        // Small objects are never allocated on the 4K boundary since the block
        // header structure is stored there; but large objects are always
        // allocated on the 4K boundary since the objects themselves are headerless.
        //
        // Account for a debugging header, though.

        item = GetRealPointer(item);
        return ((uintptr_t) item & GCHeap::kOffsetMask) == 0;
    }

    REALLY_INLINE FixedAllocSafe* FixedMalloc::FindAllocatorForSize(size_t size)
    {
        GCAssertMsg(size > 0, "cannot allocate a 0 sized block");

#ifdef DEBUG
        uint32_t const size8 = (uint32_t)((size+7)&~7); // Round up to multiple of 8
        GCAssert((size8 >> 3) < kMaxSizeClassIndex);
        GCAssert(size8 <= (uint32_t)kLargestAlloc);
#endif

        // 'index' is (conceptually) "(size8>>3)" but the following
        // optimization allows us to skip the &~7 that is redundant
        // for non-debug builds.
#ifdef MMGC_64BIT
        unsigned const index = kSizeClassIndex[((size+7)>>3)];
#else
        // The first bucket is 4 on 32-bit systems, so special case that rather
        // than double the size-class-index table.
        unsigned const index = (size <= 4) ? 0 : kSizeClassIndex[((size+7)>>3)];
#endif

        // Assert that I fit.
        GCAssert(size <= m_allocs[index].GetItemSize());

        // Assert that I don't fit (makes sure we don't waste space.
        GCAssert(index == 0 || size > m_allocs[index-1].GetItemSize());

        return &m_allocs[index];
    }

    REALLY_INLINE size_t FixedMalloc::GetNumLargeBlocks()
    {
        MMGC_LOCK(m_largeAllocInfoLock);
        return numLargeBlocks;
    }

    REALLY_INLINE void FixedMalloc::UpdateLargeAllocStats(void* item, size_t blocksNeeded)
    {
        (void)item;
        MMGC_LOCK(m_largeAllocInfoLock);
        numLargeBlocks += blocksNeeded;

#if defined MMGC_HOOKS && defined MMGC_MEMORY_PROFILER
        if(m_heap->HooksEnabled() && m_heap->GetProfiler() != NULL)
            totalAskSizeLargeAllocs += m_heap->GetProfiler()->GetAskSize(item);
#endif
    }

    REALLY_INLINE void FixedMalloc::UpdateLargeFreeStats(void* item, size_t blocksFreed)
    {
        (void)item;
        MMGC_LOCK(m_largeAllocInfoLock);
#if defined MMGC_HOOKS && defined MMGC_MEMORY_PROFILER
        if(m_heap->HooksEnabled() && m_heap->GetProfiler() != NULL)
            totalAskSizeLargeAllocs -= m_heap->GetProfiler()->GetAskSize(item);
#endif
        numLargeBlocks -= blocksFreed;
    }

    REALLY_INLINE
    void FixedMalloc::FindBeginningRootsCache::Stash(const void *addr,
                                                     size_t size)
    {
        MMGC_LOCK(m_lock);
        m_objBegin = addr;
        m_objSize = size;
    }

    REALLY_INLINE
    void FixedMalloc::FindBeginningRootsCache::Clear(void *addr)
    {
        // Bugzilla 681388: Clear cache until overwritten with fresh entry.
        if (addr == m_objBegin) {
            MMGC_LOCK(m_lock); // Lock only when clear is necessary.

            // If overwrite occurred after test above but before lock,
            // then need not clear entry.
            if (addr == m_objBegin) {
                m_objSize = 0;
                m_objBegin = NULL;
            }
        }
    }
}

#endif /* __FixedMalloc_inlines__ */

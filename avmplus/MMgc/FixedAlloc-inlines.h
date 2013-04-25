/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FixedAlloc_inlines__
#define __FixedAlloc_inlines__

// Inline methods for FixedAlloc, FixedAllocSafe, and FastAllocator.

namespace MMgc
{
    /*static*/
    REALLY_INLINE FixedAlloc *FixedAlloc::GetFixedAlloc(void *item)
    {
        return GetFixedBlock(item)->alloc;
    }
    
    /*static*/
    REALLY_INLINE FixedAlloc::FixedBlock* FixedAlloc::GetFixedBlock(const void *item)
    {
        return (FixedBlock*) ((uintptr_t)item & GCHeap::kBlockMask);
    }
    
    /*static*/
    REALLY_INLINE size_t FixedAlloc::Size(const void *item)
    {
        return GetFixedBlock(item)->size;
    }
    
    REALLY_INLINE size_t FixedAlloc::GetBytesInUse() const
    {
        size_t totalAskSize, totalAllocated;
        GetUsageInfo(totalAskSize, totalAllocated);
        return totalAllocated;
    }

    REALLY_INLINE size_t FixedAlloc::GetMaxAlloc() const
    {
        return m_numBlocks * m_itemsPerBlock;
    }

    REALLY_INLINE size_t FixedAlloc::GetNumBlocks() const
    {
        return m_numBlocks;
    }

    REALLY_INLINE bool FixedAlloc::IsFull(FixedBlock *b) const
    {
        return b->numAlloc == m_itemsPerBlock;
    }

    REALLY_INLINE size_t FixedAlloc::GetItemSize() const
    {
        return m_itemSize - DebugSize();
    }

#ifdef MMGC_HOOKS

    REALLY_INLINE void FixedAlloc::InlineAllocHook(size_t size, void *item)
    {
        if(m_heap->HooksEnabled() && item != NULL) {
            FixedBlock *b = FixedAlloc::GetFixedBlock(item);
            m_heap->AllocHook(item, size, b->size - DebugSize(),/*managed=*/false);
        }
    }

    /*static*/
    REALLY_INLINE void FixedAlloc::InlineFreeHook(void *item MMGC_MEMORY_PROFILER_ARG(size_t& askSize))
    {
        FixedBlock *b = FixedAlloc::GetFixedBlock(item);
        GCHeap *heap = b->alloc->m_heap;

        if(heap->HooksEnabled()) {
#ifdef MMGC_MEMORY_PROFILER
            if(heap->GetProfiler())
                askSize = heap->GetProfiler()->GetAskSize(item);
#endif
            heap->FinalizeHook(item, b->size - DebugSize());
            heap->FreeHook(item, b->size - DebugSize(), uint8_t(GCHeap::FXFreedPoison));
        }
    }

#endif // MMGC_HOOKS

    REALLY_INLINE void* FixedAllocSafe::Alloc(size_t size, FixedMallocOpts flags)
    {
        void *item;
        {
            MMGC_LOCK(m_spinlock);
            item = FixedAlloc::InlineAllocSansHook(size, flags);
            GCAssertMsg(item != NULL || (flags&kCanFail), "NULL is only valid when kCanFail is set");
        }
#ifdef MMGC_HOOKS
        InlineAllocHook(size, item);
#endif
        return item;
    }

    REALLY_INLINE void FixedAllocSafe::Free(void *ptr)
    {
#ifdef MMGC_MEMORY_PROFILER
        size_t askSize = 0;
#endif
#ifdef MMGC_HOOKS
        InlineFreeHook(ptr MMGC_MEMORY_PROFILER_ARG(askSize));
#endif
        {
            MMGC_LOCK(m_spinlock);
            FixedAlloc::InlineFreeSansHook(ptr MMGC_MEMORY_PROFILER_ARG(askSize));
        }
    }

    REALLY_INLINE void* FixedAlloc::InlineAllocSansHook(size_t size, FixedMallocOpts opts)
    {
        (void)size;
        GCAssertMsg(m_heap->IsStackEntered() || (opts&kCanFail) != 0, "MMGC_ENTER must be on the stack");
        GCAssertMsg(((size_t)m_itemSize >= size), "allocator itemsize too small");

        // Obtain a non-full block if there isn't one.
        if(!m_firstFree) {
            bool canFail = (opts & kCanFail) != 0;
            CreateChunk(canFail);
            if(!m_firstFree) {
                if (!canFail) {
                    GCAssertMsg(0, "Memory allocation failed to abort properly");
                    GCHeap::SignalInconsistentHeapState("Failed to abort");
                    /*NOTREACHED*/
                }
                return NULL;
            }
        }

        FixedBlock* b = m_firstFree;
        GCAssert(b && !IsFull(b));

        b->numAlloc++;

        // Take the object from the free list if it is not empty
        void *item = NULL;
        if (b->firstFree) {
            item = FLPop(b->firstFree);
            // Assert that the freelist hasn't been tampered with (by writing to the first 4 bytes).
            GCAssert(b->firstFree == NULL ||
                    (b->firstFree >= b->items &&
                    (((uintptr_t)b->firstFree - (uintptr_t)b->items) % b->size) == 0 &&
                     (uintptr_t) b->firstFree < ((uintptr_t)b & GCHeap::kBlockMask) + GCHeap::kBlockSize));
#ifdef MMGC_MEMORY_INFO
            // Check for writes on deleted memory.
            VerifyFreeBlockIntegrity(item, b->size);
#endif
        }
        else {
            // Otherwise take the object from the end of the block.
            item = b->nextItem;
            GCAssert(item != 0);
            if(!IsFull(b)) // There are more items at the end of the block
                b->nextItem = (void *) ((uintptr_t)item+m_itemSize);
            else
                b->nextItem = 0;
        }

        // If the block has no more free items, be sure to remove it from the list of
        // blocks with free items.
        if (IsFull(b)) {
            // Crash fast in case of heap corruption, aka safe unlinking.
            if ( ((b->prevFree && (b->prevFree->nextFree!=b))) ||
                 ((b->nextFree && (b->nextFree->prevFree!=b))) )
                VMPI_abort();

            m_firstFree = b->nextFree;
            b->nextFree = NULL;
            GCAssert(b->prevFree == NULL);

            if (m_firstFree)
                m_firstFree->prevFree = 0;
        }

        item = GetUserPointer(item);

#ifdef DEBUG
        // Fresh memory poisoning.
        if ((opts & kZero) == 0 && !RUNNING_ON_VALGRIND)
            memset(item, uint8_t(GCHeap::FXFreshPoison), b->size - DebugSize());
#endif

        // Note that we'd like to use the requested size and not
        // b->size but clients will use the slop after calling Size().
        // Using the requested size and expanding to b->size via
        // VALGRIND_MEMPOOL_CHANGE in Size() doesn't work because the
        // scanner scans the full size (see bug 594756).
        VALGRIND_MEMPOOL_ALLOC(b, item, b->size);

        if((opts & kZero) != 0)
            memset(item, 0, b->size - DebugSize());

#ifdef MMGC_MEMORY_PROFILER
        if(m_heap->HooksEnabled())
            m_totalAskSize += size;
#endif

        return item;
    }

    /*static*/
    REALLY_INLINE void FixedAlloc::InlineFreeSansHook(void *item MMGC_MEMORY_PROFILER_ARG(size_t askSize))
    {
        FixedBlock *b = (FixedBlock*) FixedAlloc::GetFixedBlock(item);

        // IsAddressInHeap acquires the heap lock which we can't do safely here
        //      GCAssertMsg(b->alloc->m_heap->IsAddressInHeap(item), "Bogus pointer passed to free");

#ifdef MMGC_MEMORY_PROFILER
        GCHeap *heap = b->alloc->m_heap;
        if(heap->GetProfiler())
            b->alloc->m_totalAskSize -= askSize;
#endif

        item = GetRealPointer(item);

        FLPush(b->firstFree, item);

        VALGRIND_MEMPOOL_FREE(b, GetUserPointer(item));
        
        // 'b' was full but now it has a free spot, add it to the free block list.
        if (b->numAlloc == b->alloc->m_itemsPerBlock)
        {
            GCAssert(!b->nextFree && !b->prevFree);
            b->nextFree = b->alloc->m_firstFree;
            if (b->alloc->m_firstFree)
                b->alloc->m_firstFree->prevFree = b;
            b->alloc->m_firstFree = b;
        }
#ifdef DEBUG
        else // The item should already be on b's free list
        {
            GCAssert ((b == b->alloc->m_firstFree) || b->prevFree);
        }
#endif

        b->numAlloc--;

        if(b->numAlloc == 0)
            b->alloc->FreeChunk(b);
    }

    /*static*/
    REALLY_INLINE FixedAllocSafe* FixedAllocSafe::GetFixedAllocSafe(void *item)
    {
        return (FixedAllocSafe*) FixedAlloc::GetFixedAlloc(item);
    }

    /*static*/
    REALLY_INLINE void* FastAllocator::operator new(size_t size, FixedAlloc *alloc)
    {
        return alloc->Alloc(size);
    }

    /*static*/
    REALLY_INLINE void FastAllocator::operator delete(void *item)
    {
        FixedAlloc::Free(item);
    }

    REALLY_INLINE bool FixedAllocSafe::QueryOwnsObject(const void* item)
    {
        MMGC_LOCK(m_spinlock);
        return FixedAlloc::QueryOwnsObject(item);
    }
}

#endif /* __FixedAlloc_inlines__ */

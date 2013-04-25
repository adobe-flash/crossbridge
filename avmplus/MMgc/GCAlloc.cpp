/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "MMgc.h"

/* Notes on the quick list.
 *
 * Some programs have a high churn rate where objects are allocated and freed
 * rapidly, and often freed explicitly - the AS2 interpreter is an example
 * of this, another is AS3 reference counting.  These programs benefit from
 * an allocator that performs limited book-keeping during times of high
 * allocation and deallocation and only occasionally reconciles information
 * about allocated and freed objects into a convenient state.  The quick list in
 * GCAlloc provides such a facility within the current (Dec 2009) MMgc API.
 *
 * (The old allocator essentially performed eager coalescing of free objects:
 * it would check whether a block was full, whether it needed sweeping,
 * and so on.  The old allocator also had reluctant allocation: objects
 * would be preferred off a block's free list, and the tail end of a block
 * would not be split into individual objects until the free objects had
 * been exhausted: this added more tests, and made optimization harder.
 * The quick list effectively implements deferred coalescing and eager
 * allocation, and adds a few other optimizations.)
 *
 * The fast path for Alloc picks an object off the free list, sets up the
 * header bits, and returns the object.  The fast path for Free places the
 * object on the quick list.  Slow paths are triggered when the quick list
 * is empty (in Alloc) or there's something about the object's block that
 * requires special action (in Free).  The amount of testing on the fast
 * paths is kept to an absolute minimum.  For example, when the collector
 * starts sweeping it clears all the quick lists, and only the slow allocation
 * path needs to test whether the object needs to be allocated pre-marked.
 * Similarly, blocks that may contain objects that are weakly held or which must
 * be swept are marked as such using a single, common flag, so the Free code only
 * has one test on the fast path; only the slow path needs to test whether
 * an object's weak reference must be cleared /or/ the block must actually
 * be swept.
 *
 * Central to this optimization is that all free storage in a block is on
 * the free list, and CreateChunk now explodes the block onto the free list
 * in a tight loop.  This gets rid of a test and branch in Alloc and lost
 * optimizations resulting from that test.
 *
 * Generally we also try to open up for tail-call optimization throughout.
 *
 * Code that relies on the GC for storage reclamation benefits from these
 * optimizations as well, as Alloc is substantially faster than before.
 * The cost of sweeping is however about the same cost as before the quick
 * list.
 *
 * So where's the cost?
 *
 * Primarily, each quick list will need to be coalesced into the free lists
 * in the blocks whose free objects are on the quick list before sweeping
 * (ie at the start and end of each GC cycle) and whenever there is an excess
 * of free storage on the quick lists globally.  Since the quick list
 * coalescing has economy of scale where the old immediate coalescing did
 * not we do it quicker than before, and in fast-allocating programs we will
 * usually do it for vastly fewer objects, but it's not free.
 *
 * We also risk losing some incrementality during coalescing if the quick lists
 * become very long.  There's one quick list per allocator; for some very popular
 * object types that are freed explicitly there's a chance that the quick lists
 * could become long.  We already have bad incrementality at most points where
 * the quick lists must be coalesced, however, so it's doubtful that coalescing
 * will make much of a difference.
 *
 * Another minor cost is a slight difference in accounting, since all objects
 * in a block are on a free list - in the past, accounting did not count the
 * free space at the end of a block, but now there's no such memory.  In practice
 * this is not expected to be significant, as there should be very little such
 * free space globally in the system except in minuscule heaps.
 *
 * The final cost is that we must ensure that a large population of free objects
 * does not build up and thus increase fragmentation system-wide.  To counter
 * this, the GC has a global quick list budget which is a constant number of
 * bytes that it will allow on quick lists for all its allocators.  Each allocator
 * has a budget - initially one block's worth of data - and will ask to increase
 * it when its quick list grows longer; that request will cause the GC to flush
 * some quick lists if the requested increase is not available.  Thus the total
 * amount of fragmentation in the system resulting from the quick lists is always
 * bounded by a constant (which is better than the amount of fragmentation
 * resulting from the ordinary free lists - it is not bounded).  The cost of
 * this accounting is a per-allocator counter that is incremented/decremented
 * at each allocation/deallocation and compared with zero.  Regrettably this
 * accounting is on the hot path and I've not yet found a way to coalesce it
 * with the ordinary alloc/free accounting.
 *
 *
 * Notes.
 *
 * An optimization that was tried but which is not yet in this code is propagating
 * the value of the object index from Free/CreateChunk/Sweep to Alloc: Free must
 * compute it, and both CreateChunk and Sweep can compute the index much more cheaply
 * than Alloc can, so it makes sense to do so.  Two facts stand in the way.  One,
 * on 64-bit system the object may only be one word long.  Two, there's a system-wide
 * invariant that free RC objects have a 'composite' field (the second word) whose
 * value is zero (or some poison value, in DEBUG builds), and reference counting
 * operations test the value of that field to ignore operations on objects that are
 * in the process of being deleted.  This protocol makes finalization work in the
 * current system.  See comments about this invariant in GCObject.h.
 */

namespace MMgc
{
#ifdef MMGC_FASTBITS
    static uint32_t log2(uint32_t n)
    {
        uint32_t result = 0;
        while (n > 1) {
            result += 1;
            n >>= 1;
        }
        return result;
    }
#endif

    /*virtual*/
    GCAllocBase::~GCAllocBase()
    {
    }

    GCAlloc::GCAlloc(GC* _gc, int _itemSize, bool _containsPointers, bool _isRC, bool _isFinalized, int _sizeClassIndex, uint8_t _bibopTag) :
        m_firstBlock(NULL),
        m_lastBlock(NULL),
        m_firstFree(NULL),
        m_needsSweeping(NULL),
        m_qList(NULL),
        m_qBudget(0),
        m_qBudgetObtained(0),
        m_itemSize((_itemSize+7)&~7), // Round itemSize to the nearest boundary of 8
        m_itemsPerBlock((kBlockSize - sizeof(GCBlock)) / m_itemSize),
        m_totalAllocatedBytes(0),
    #ifdef MMGC_FASTBITS
        m_bitsShift(log2(m_itemSize)),
        m_numBitmapBytes(kBlockSize / (1 << m_bitsShift)),
    #else
        m_numBitmapBytes(((m_itemsPerBlock * sizeof(gcbits_t))+3)&~3), // round up to 4 bytes so we can go through the bits several items at a time
    #endif
        m_sizeClassIndex(_sizeClassIndex),
    #ifdef MMGC_MEMORY_PROFILER
        m_totalAskSize(0),
    #endif
        m_bitsInPage(_containsPointers && kBlockSize - int(m_itemsPerBlock * m_itemSize + sizeof(GCBlock)) >= m_numBitmapBytes),
        m_bibopTag(_bibopTag),
        multiple(ComputeMultiply((uint16_t)m_itemSize)),
        shift(ComputeShift((uint16_t)m_itemSize)),
        containsPointers(_containsPointers),
        containsRCObjects(_isRC),
        containsFinalizedObjects(_isFinalized),
        m_finalized(false),
        m_gc(_gc)
    {
#ifdef DEBUG
        int usedSpace = m_itemsPerBlock * m_itemSize + sizeof(GCBlock);
#endif
        GCAssert((unsigned)kBlockSize == GCHeap::kBlockSize);
        GCAssert(usedSpace <= kBlockSize);
        GCAssert(kBlockSize - usedSpace < (int)m_itemSize);
        GCAssert(m_itemSize < GCHeap::kBlockSize);
        GCAssert(!_isRC || _isFinalized);
        m_gc->ObtainQuickListBudget(m_itemSize*m_itemsPerBlock);
        m_qBudget = m_qBudgetObtained = m_itemsPerBlock;
    }

    GCAlloc::~GCAlloc()
    {
        CoalesceQuickList();

        // Free all of the blocks
        GCAssertMsg(GetNumAlloc() == 0, "You have leaks");

        while (m_firstBlock) {
#ifdef MMGC_MEMORY_INFO
            //check where any item within this block wasn't written to after being poisoned
            VerifyFreeBlockIntegrity(m_firstBlock->firstFree, m_firstBlock->size);
#endif //MMGC_MEMORY_INFO

            GCBlock *b = m_firstBlock;
            UnlinkChunk(b);
            FreeChunk(b);
        }
    }

    GCAlloc::GCBlock* GCAlloc::CreateChunk(int flags)
    {
        // Too many definitions of kBlockSize, make sure they're at least in sync.

        GCAssert(uint32_t(kBlockSize) == GCHeap::kBlockSize);

        // Get bitmap space; this may trigger OOM handling.

        gcbits_t* bits = m_bitsInPage ? NULL : (gcbits_t*)m_gc->AllocBits(m_numBitmapBytes, m_sizeClassIndex);

        // Allocate a new block; this may trigger OOM handling (though that
        // won't affect the bitmap space, which is not GC'd individually).

        GCBlock* b = (GCBlock*) m_gc->AllocBlock(1, PageMap::kGCAllocPage, /*zero*/true,  (flags&GC::kCanFail) != 0);

        if (b)
        {
            VALGRIND_CREATE_MEMPOOL(b, 0/*redZoneSize*/, 1/*zeroed*/);

            // treat block header as a separate allocation
            VALGRIND_MEMPOOL_ALLOC(b, b, sizeof(GCBlock));


            b->gc = m_gc;
            b->alloc = this;
            b->size = m_itemSize;
            b->slowFlags = 0;
            if(m_gc->collecting && m_finalized)
                b->finalizeState = m_gc->finalizedValue;
            else
                b->finalizeState = !m_gc->finalizedValue;

            b->bibopTag = m_bibopTag;

#ifdef MMGC_FASTBITS
            b->bitsShift = (uint8_t) m_bitsShift;
#endif
            b->containsPointers = ContainsPointers();
            b->rcobject = ContainsRCObjects();

            if (m_bitsInPage)
                b->bits = (gcbits_t*)b + sizeof(GCBlock);
            else
                b->bits = bits;

            // ditto for in page bits
            if (m_bitsInPage) {
                VALGRIND_MEMPOOL_ALLOC(b, b->bits, m_numBitmapBytes);
            }

            // Link the block at the end of the list
            b->prev = m_lastBlock;
            b->next = 0;

            if (m_lastBlock) {
                m_lastBlock->next = b;
            }
            if (!m_firstBlock) {
                m_firstBlock = b;
            }
            m_lastBlock = b;

            // Add our new ChunkBlock to the firstFree list (which should be empty)
            if (m_firstFree)
            {
                GCAssert(m_firstFree->prevFree == 0);
                m_firstFree->prevFree = b;
            }
            b->nextFree = m_firstFree;
            b->prevFree = 0;
            m_firstFree = b;

            // calculate back from end (better alignment, no dead space at end)
            b->items = (char*)b+GCHeap::kBlockSize - m_itemsPerBlock * m_itemSize;
            b->numFree = (short)m_itemsPerBlock;

            // explode the new block onto its free list
            //
            // We must make the object look free, which means poisoning it properly and setting
            // the mark bits correctly.

            b->firstFree = b->items;
            void** p = (void**)(void*)b->items;
            int limit = m_itemsPerBlock-1;
#ifdef MMGC_HOOKS
            GCHeap* heap = GCHeap::GetGCHeap();
#endif
            for ( int i=0 ; i < limit ; i++ ) {
#ifdef MMGC_HOOKS
#ifdef MMGC_MEMORY_INFO // DebugSize is 0 if MEMORY_INFO is off, so we get an "obviously true" warning from GCC.
                GCAssert(m_itemSize >= DebugSize());
#endif
                if(heap->HooksEnabled())
                    heap->PseudoFreeHook(GetUserPointer(p), m_itemSize - DebugSize(), uint8_t(GCHeap::GCSweptPoison));
#endif
                p = FLSeed(p, (char*)p + m_itemSize);
            }
#ifdef MMGC_HOOKS
            if(heap->HooksEnabled())
                heap->PseudoFreeHook(GetUserPointer(p), m_itemSize - DebugSize(), uint8_t(GCHeap::GCSweptPoison));
#endif
            p[0] = NULL;

            // Set all the mark bits to 'free'
            
            GCAssert(sizeof(gcbits_t) == 1);
            GCAssert(kFreelist == 3);
            GCAssert(m_numBitmapBytes % 4 == 0);
            
            uint32_t *pbits = (uint32_t*)(void *)b->bits;
            for(int i=0, n=m_numBitmapBytes>>2; i < n; i++)
                pbits[i] = 0x03030303;

#ifdef MMGC_MEMORY_INFO
            VerifyFreeBlockIntegrity(b->firstFree, m_itemSize);
#endif
        }
        else {
            if (bits)
                m_gc->FreeBits((uint32_t*)(void *)bits, m_sizeClassIndex);
        }

        return b;
    }

    void GCAlloc::UnlinkChunk(GCBlock *b)
    {
        GCAssert(!b->needsSweeping());
        if ( ((b->prevFree && (b->prevFree->nextFree!=b))) ||
            ((b->nextFree && (b->nextFree->prevFree!=b))) )
            VMPI_abort();

        // Unlink the block from the list
        if (b == m_firstBlock) {
            m_firstBlock = Next(b);
        } else {
            b->prev->next = Next(b);
        }

        if (b == m_lastBlock) {
            m_lastBlock = b->prev;
        } else {
            Next(b)->prev = b->prev;
        }

        m_totalAllocatedBytes -= (m_itemsPerBlock - b->numFree) * m_itemSize;
        if(b->nextFree || b->prevFree || b == m_firstFree) {
            RemoveFromFreeList(b);
        }
#ifdef _DEBUG
        b->next = b->prev = NULL;
        b->nextFree = b->prevFree = NULL;
#endif
    }

    void GCAlloc::FreeChunk(GCBlock* b)
    {
        GCAssert(b->numFree == m_itemsPerBlock);
        if(!m_bitsInPage) {
            VMPI_memset(b->bits, 0, m_numBitmapBytes);
            m_gc->FreeBits((uint32_t*)(void *)b->bits, m_sizeClassIndex);
            b->bits = NULL;
        } else {
            // Only free bits if they were in page, see CreateChunk.
            VALGRIND_MEMPOOL_FREE(b, b->bits);
        }
        
        VALGRIND_MEMPOOL_FREE(b, b);

        // Free the memory
        m_gc->FreeBlock(b, 1);

        VALGRIND_DESTROY_MEMPOOL(b);
    }

#if defined DEBUG || defined MMGC_MEMORY_PROFILER
    void* GCAlloc::Alloc(size_t askSize, int flags)
#else
    void* GCAlloc::Alloc(int flags)
#endif
    {
#ifdef DEBUG
        m_gc->heap->CheckForOOMAbortAllocation();
#endif

        // Allocation must be signalled before we allocate because no GC work must be allowed to
        // come between an allocation and an initialization - if it does, we may crash, as
        // GCFinalizedObject subclasses may not have a valid vtable, but the GC depends on them
        // having it.  In principle we could signal allocation late but only set the object
        // flags after signaling, but we might still cause trouble for the profiler, which also
        // depends on non-interruptibility.

        m_gc->SignalAllocWork(m_itemSize);

        GCAssertMsg(((size_t)m_itemSize >= askSize), "allocator itemsize too small");
        GCAssert(!m_gc->collecting || m_qList == NULL);

#ifdef MMGC_MEMORY_INFO
        if (m_qList != NULL) {
            //check for writes on deleted memory
            //the quick list can be long so limit the check to the first 20 to
            //avoid slowing down debug builds too much.
            VerifyFreeBlockIntegrity(m_qList, m_itemSize, 20);
        }
#endif

        m_totalAllocatedBytes += m_itemSize;
        if (m_qList == NULL) {
#if defined DEBUG || defined MMGC_MEMORY_PROFILER
            return AllocSlow(askSize, flags);
#else
            return AllocSlow(flags);
#endif
        }

#if defined DEBUG || defined MMGC_MEMORY_PROFILER
        return AllocFromQuickList(askSize, flags);
#else
        return AllocFromQuickList(flags);
#endif
    }

#if defined DEBUG || defined MMGC_MEMORY_PROFILER
    REALLY_INLINE void* GCAlloc::AllocFromQuickList(size_t askSize, int flags)
#else
    REALLY_INLINE void* GCAlloc::AllocFromQuickList(int flags)
#endif
    {
        // This is absurd.
#if defined DEBUG || defined MMGC_MEMORY_PROFILER
        (void)askSize;
#endif
        void *item = FLPopAndZero(m_qList);
        GCBlock* b = GetBlock(item);

        GCAssert(!b->needsSweeping());

        // Code below uses these optimizations
        GCAssert((unsigned long)GC::kFinalize == (unsigned long)kFinalizable);
        GCAssert((unsigned long)GC::kInternalExact == (unsigned long)kVirtualGCTrace);
        GCAssert((flags & GC::kFinalize) == 0 || containsFinalizedObjects);

#if defined VMCFG_EXACT_TRACING
        b->bits[GetBitsIndex(b, item)] = (flags & (GC::kFinalize|GC::kInternalExact));
#elif defined VMCFG_SELECTABLE_EXACT_TRACING
        b->bits[GetBitsIndex(b, item)] = (flags & (GC::kFinalize|m_gc->runtimeSelectableExactnessFlag));  // 0 or GC::kInternalExact
#else
        b->bits[GetBitsIndex(b, item)] = (flags & GC::kFinalize);
#endif

#ifdef MMGC_HOOKS
        GCHeap* heap = GCHeap::GetGCHeap();
        if(heap->HooksEnabled()) {
            size_t userSize = m_itemSize - DebugSize();
#ifdef MMGC_MEMORY_PROFILER
            // Only add if the profiler is installed since we can only
            // subtract if the profiler is installed.
            if(heap->GetProfiler())
                m_totalAskSize += askSize;
            heap->AllocHook(GetUserPointer(item), askSize, userSize, /*managed=*/true);
#else
            heap->AllocHook(GetUserPointer(item), 0, userSize, /*managed=*/true);
#endif
        }
#endif // MMGC_HOOKS

        m_qBudget++;

        VALGRIND_MEMPOOL_ALLOC(b, item, m_itemSize);

        return item;
    }

#if defined DEBUG || defined MMGC_MEMORY_PROFILER
    void *GCAlloc::AllocSlow(size_t askSize, int flags)
#else
    void *GCAlloc::AllocSlow(int flags)
#endif
    {
        GCBlock* b = m_firstFree;

        while (b == NULL) {
            if (m_needsSweeping && !m_gc->collecting) {
                Sweep(m_needsSweeping);
                b = m_firstFree;
                if (b != NULL)
                    break;
            }
            else {
                bool canFail = (flags & GC::kCanFail) != 0;
                CreateChunk(canFail);
                b = m_firstFree;
                if (b != NULL)
                    break;
                GCAssert(canFail);
                return NULL;
            }
        }

        GCAssert(!b->needsSweeping());
        GCAssert(b == m_firstFree);
        GCAssert(b->firstFree != NULL);

        if (!m_gc->collecting && !m_gc->greedy)
        {
            // Fast path: fill the quick list from b, then tail-call AllocFromQuickList()
            FillQuickList(b);
            GCAssert(m_qList != NULL);
#if defined DEBUG || defined MMGC_MEMORY_PROFILER
            return AllocFromQuickList(askSize, flags);
#else
            return AllocFromQuickList(flags);
#endif
        }
        else
        {
            // Slow path: We basically don't care how expensive this is, so
            // punt: fill the quick list, alloc, set the bits, then coalesce
            // to clear the list and establish the situation that triggers
            // slow-path allocation.

            FillQuickList(b);
            GCAssert(m_qList != NULL);
#if defined DEBUG || defined MMGC_MEMORY_PROFILER
            void *item = AllocFromQuickList(askSize, flags);
#else
            void *item = AllocFromQuickList(flags);
#endif
            if(m_gc->collecting && b->finalizeState != m_gc->finalizedValue) {
                b->bits[GetBitsIndex(b, item)] |= kMark;
            }
            CoalesceQuickList();
            GCAssert(m_qList == NULL);

            return item;
        }
    }

    void GCAlloc::FillQuickList(GCBlock* b)
    {
        GCAssert(m_qList == NULL);

        // Check the budget first to avoid freeing the quick list if this allocator
        // is the next victim.

        if (m_qBudget < b->numFree) {
            m_gc->ObtainQuickListBudget(m_itemsPerBlock*m_itemSize);
            m_qBudget += m_itemsPerBlock;
            m_qBudgetObtained += m_itemsPerBlock;
        }

        m_qList = (void**)b->firstFree;
        m_qBudget -= b->numFree;
        b->numFree = 0;
        b->firstFree = NULL;
        RemoveFromFreeList(b);
    }

    /* virtual */
    void GCAlloc::Free(const void *item)    // item is realPtr
    {
        GCBlock *b = GetBlock(item);
        int bitsindex = GetBitsIndex(b, item);

        // We can't allow free'ing something during sweeping - it messes up
        // the per-block statistics - or anything that's on a mark queue.

        GCAssert(m_gc->collecting == false || m_gc->marking == true);
        if (m_gc->marking && (m_gc->collecting || b->bits[bitsindex] & kQueued)) {
            m_gc->AbortFree(GetUserPointer(item));
            return;
        }

#ifdef _DEBUG
        VerifyNotFree(b, item);

        // RCObject have contract that they must clean themselves, since they
        // have to scan themselves to decrement other RCObjects they might as well
        // clean themselves too, better than suffering a memset later
        if(b->rcobject)
            m_gc->RCObjectZeroCheck((RCObject*)GetUserPointer(item));
#endif

#ifdef MMGC_HOOKS
        GCHeap* heap = GCHeap::GetGCHeap();
        if(heap->HooksEnabled())
        {
            const void* p = GetUserPointer(item);
            size_t userSize = GC::Size(p);
#ifdef MMGC_MEMORY_PROFILER
            if(heap->GetProfiler())
                m_totalAskSize -= heap->GetProfiler()->GetAskSize(p);
#endif
            heap->FinalizeHook(p, userSize);
            heap->FreeHook(p, userSize, uint8_t(GCHeap::GCFreedPoison));
        }
#endif

        // We must set the kFreelist bit here.
        //
        // Sweeping and finalization depend on kFreelist being set on free objects:
        // both skip items on the free list.  For sweeping we could set it in the already
        // expensive must-be-swept logic.  For finalization we could  move the setting
        // into CoalesceQuickList, where it would be set only for objects flushed to
        // the regular free lists.  It would be a savings overall if the quick list is
        // busy, as we expect it to be.
        //
        // However, when an object is freed explicitly, or freed by the reaper, there
        // may still be pointers to that object in the GC'd heap (these could be
        // uncleared pointers, untracked pointers, or misidentified pointers).  If
        // the object is not marked as free the dead object may be be marked, which
        // would mean that it may end up on both a free list and on the mark stack, and
        // that would be bad; it could also mean that objects reachable from the
        // dead object would be marked in turn and would be retained for a GC cycle.

        b->bits[bitsindex] |= kFreelist;    // Don't clear the weak ref bit, FreeSlow may inspect it
        m_totalAllocatedBytes -= m_itemSize;
        if (b->slowFlags)   // needs sweeping, or may have weak refs
        {
            FreeSlow(b, bitsindex, item);
            return;
        }

        GCAssert(!b->needsSweeping());
        GCAssert(!(b->bits[bitsindex] & kHasWeakRef));

#ifndef _DEBUG
        ClearNonRCObject((void*)item, b->size);
#endif

        FLPush(m_qList, item);

        VALGRIND_MEMPOOL_FREE(b, item);

        m_gc->SignalFreeWork(m_itemSize);
        if (--m_qBudget <= 0)
            QuickListBudgetExhausted();
    }

    void GCAlloc::FreeSlow(GCBlock* b, int bitsindex, const void* item)
    {
        if(b->bits[bitsindex] & kHasWeakRef)
            b->gc->ClearWeakRef(GetUserPointer(item));

#ifndef _DEBUG
        ClearNonRCObject((void*)item, b->size);
#endif
        bool blockSwept = false;

        if (b->needsSweeping()) {
            // See comment in GCAlloc::Free
            //b->bits[bitsindex] = kFreelist;

            // We know that the quick list does not have any items from the block b,
            // so we can push the object onto the block's free list and sweep the block.
            // Make the quick list NULL while we do that.
            void* qList = m_qList;
            m_qList = NULL;

            FLPush(b->firstFree, item);
            b->numFree++;

            blockSwept = Sweep(b);

            m_qList = qList;
        }
        else {
            *(void**)item = m_qList;
            m_qList = (void**)item;

            if (--m_qBudget <= 0)
                QuickListBudgetExhausted();
        }
        if (!blockSwept)
            VALGRIND_MEMPOOL_FREE(b, item);
    }

    REALLY_INLINE void GCAlloc::ClearNonRCObject(void* item, size_t size)
    {
        // memset rest of item not including free list pointer, in _DEBUG
        // we poison the memory (and clear in Alloc)
        //
        // BTW, experiments show that clearing on alloc instead of on free
        // benefits microbenchmark that do massive amounts of double-boxing,
        // but nothing else enough to worry about it.  (The trick is that
        // no clearing on alloc is needed when carving objects off the end
        // of a block, whereas every object is cleared on free even if the
        // page is subsequently emptied out and returned to the block manager.
        // Massively boxing programs have alloc/free patterns that are biased
        // toward non-RC objects carved off the ends of blocks.)
        //
        // As it is, we 'clear' in CreateChunk so it's good for all objects
        // on the free list to be cleared uniformly, even if it adds an
        // additional test to the hot path in Free().  We could in principle
        // merge that test with b->slowFlags if we think RCObjects are likely
        // to predominate on the path for GCAlloc::Free - not obvious, but
        // credible.
        if(!ContainsRCObjects())
            VMPI_memset((char*)item, 0, size);
    }

    // True about objects on the quick list:
    //
    //  - The numFree counter in their blocks does not account for the free objects
    //    on the quick list
    //  - They never belong to a block that must be swept - Free() checks whether
    //    a page needs sweeping
    //  - They all have kFreelist set

    void GCAlloc::CoalesceQuickList()
    {
        void* item = m_qList;

        m_qList = NULL;

        while (item != NULL) {
            void *next = FLNext(item);
            GCBlock *b = GetBlock(item);

            GCAssert(!b->needsSweeping());
            if (b->numFree == 0)
                AddToFreeList(b);

            b->numFree++;
            // See comment in GCAlloc::Free
            //int bitsindex = GetBitsIndex(b, item);
            //b->bits[bitsindex] = kFreelist;

            // The object was cleared in Free() or will be cleared in Alloc(), but
            // we need to link it onto the block's free list.

#ifdef _DEBUG
            VerifyNotFree(b, item);
#endif

            FLPush(b->firstFree, item);
            item = next;
        }

        if (m_qBudgetObtained > m_itemsPerBlock) {
            m_gc->RelinquishQuickListBudget((m_qBudgetObtained - m_itemsPerBlock)*m_itemSize);
            m_qBudgetObtained = m_itemsPerBlock;
        }
        m_qBudget = m_qBudgetObtained;

        // Faster to do this check once per block than once per object.

        GCBlock *b = m_firstFree;
        while (b != NULL) {
            GCBlock* next = Next(b);
            if(b->numFree == m_itemsPerBlock && !b->needsSweeping()) {
                UnlinkChunk(b);
                FreeChunk(b);
            }
            b = next;
        }
    }

    void GCAlloc::QuickListBudgetExhausted()
    {
        m_gc->ObtainQuickListBudget(m_itemsPerBlock*m_itemSize);
        m_qBudgetObtained += m_itemsPerBlock;
        m_qBudget += m_itemsPerBlock;
    }

#ifdef _DEBUG
    void GCAlloc::VerifyNotFree(GCBlock* b, const void* item)
    {
        for ( void *free = m_qList ; free != NULL ; free = FLNext(free) )
            GCAssert(free != item);

        for ( void *free = b->firstFree ; free != NULL ; free = FLNext(free) )
            GCAssert(free != item);
    }
#endif

    void GCAlloc::Finalize()
    {
        m_finalized = true;
        if (containsFinalizedObjects)
            FinalizationPass();
        else
            LazySweepPass();
    }

    void GCAlloc::FinalizationPass()
    {
        // Go through every item of every block.  Look for items
        // that are in use but not marked as reachable, and delete
        // them.

        GCBlock *next = NULL;
        for (GCBlock* b = m_firstBlock; b != NULL; b = next)
        {
            // we can unlink block below
            next = Next(b);

            GCAssert(!b->needsSweeping());

            // remove from freelist to avoid mutator destructor allocations
            // from using this block
            bool putOnFreeList = false;
            if(m_firstFree == b || b->prevFree != NULL || b->nextFree != NULL) {
                putOnFreeList = true;
                RemoveFromFreeList(b);
            }

            GCAssert(kMark == 0x1 && kFinalizable == 0x4 && kHasWeakRef == 0x8);

            int numMarkedItems = 0;

            gcbits_t* blockbits = b->bits;
            for ( char *item = b->items, *limit = b->items + m_itemSize * b->GetCount() ; item < limit ; item += m_itemSize )
            {
                gcbits_t& marks = blockbits[GetBitsIndex(b,item)];
                int mq = marks & kFreelist;
                if(mq == kFreelist)
                    continue;

                if(mq == kMark) {
                    numMarkedItems++;
                    continue;
                }

                GCAssertMsg(mq != kQueued, "No queued objects should exist when finalizing");

                // GC::Finalize calls GC::MarkOrClearWeakRefs before calling GCAlloc::Finalize,
                // ergo there should be no unmarked objects with weak refs.
                
                GCAssertMsg(!(marks & kHasWeakRef), "No unmarked object should have a weak ref at this point");
                
#ifdef MMGC_HOOKS
                if(m_gc->heap->HooksEnabled())
                {
                #ifdef MMGC_MEMORY_PROFILER
                    if(m_gc->heap->GetProfiler())
                        m_totalAskSize -= m_gc->heap->GetProfiler()->GetAskSize(GetUserPointer(item));
                #endif

                    m_gc->heap->FinalizeHook(GetUserPointer(item), m_itemSize - DebugSize());
                }
#endif

                if (marks & kFinalizable)
                {
                    GCFinalizedObject *obj = (GCFinalizedObject*)(void *)GetUserPointer(item);
                    marks &= ~kFinalizable;     // Clear bits first so we won't get second finalization if finalizer longjmps out

                    /* See https://bugzilla.mozilla.org/show_bug.cgi?id=573737 for the case where the object might remain in
                     * uninitialized state and thus crash occurs while calling the dtor below. 
                     */
                    if(*(intptr_t*)obj != 0)
                        obj->~GCFinalizedObject();

#if defined(_DEBUG)
                    if(((GCAlloc*)b->alloc)->ContainsRCObjects()) {
                        m_gc->RCObjectZeroCheck((RCObject*)obj);
                    }
#endif
                }

                // GC::GetWeakRef will not allow a weak reference to be created to an object that
                // is ready for destruction.

                GCAssertMsg(!(marks & kHasWeakRef), "No unmarked object should have a weak ref at this point");
            }

            // 3 outcomes:
            // 1) empty, put on list of empty pages
            // 2) no freed items, partially empty or full, return to free if partially empty
            // 3) some freed item add to the to be swept list
            if(numMarkedItems == 0) {
                // add to list of block to be returned to the Heap after finalization
                // we don't do this during finalization b/c we want finalizers to be able
                // to reference the memory of other objects being finalized
                UnlinkChunk(b);
                b->gc->AddToSmallEmptyBlockList(b);
                putOnFreeList = false;
            } else if(numMarkedItems == (m_itemsPerBlock - b->numFree)) {
                // nothing changed on this page, clear marks
                // note there will be at least one free item on the page (otherwise it
                // would not have been scanned) so the page just stays on the freelist
                ClearMarks(b);
            } else if(!b->needsSweeping()) {
                // Removed the block from the free list earlier, check again
                GCAssert(!(b->nextFree || b->prevFree || b == m_firstFree));
                AddToSweepList(b);
                putOnFreeList = false;
            }
            b->finalizeState = m_gc->finalizedValue;
            if(putOnFreeList)
                AddToFreeList(b);
        }
    }

    // For each block that is not already on the sweep list, add it to the sweep list for lazy sweeping.
    // We don't know if the block will have any free objects, or if it will have all free objects, so the
    // sweeper must take that into account.  

    void GCAlloc::LazySweepPass()
    {
        GCBlock *next = NULL;
        for (GCBlock* b = m_firstBlock; b != NULL; b = next)
        {
            // we might unlink block below
            next = Next(b);

            // Bugzilla 725955: eagerly search for all-free blocks to
            // tighten allocated memory (and thus reduce estimated
            // live storage fed into load calcuation).

            GCAssert(kMark == 0x1 && kFinalizable == 0x4 && kHasWeakRef == 0x8);

            bool anyMarkedItems = false;
            gcbits_t* blockbits = b->bits;

            for ( char *item = b->items, *limit = b->items + m_itemSize * b->GetCount() ; item < limit ; item += m_itemSize )
            {
                gcbits_t& marks = blockbits[GetBitsIndex(b,item)];
                int mq = marks & kFreelist;
                if(mq == kFreelist)
                    continue;

                if(mq == kMark) {
                    anyMarkedItems = true;
                    break;
                }
                GCAssertMsg(!(marks & kHasWeakRef),
                            "No unmarked object should have a weak ref at this point");
                GCAssertMsg(!(marks & kFinalizable),
                            "No LazySweep candidate can have kFinalizable set.");
            }

            if (!anyMarkedItems) {
                // Bugzilla 725955: add to list of block to be
                // returned to the Heap.  We could probably eagerly
                // free block here (rather than putting on the small
                // empty block list); for short-term, approximating
                // prior heap dynamics by matching prior control-flow.
                if(b->nextFree || b->prevFree || b == m_firstFree)
                    RemoveFromFreeList(b);
                UnlinkChunk(b);
                b->gc->AddToSmallEmptyBlockList(b);
            }
            else if (!b->needsSweeping())
            {
                if(b->nextFree || b->prevFree || b == m_firstFree)
                    RemoveFromFreeList(b);
                AddToSweepList(b);
            }
            b->finalizeState = m_gc->finalizedValue;
        }
    }

    // OPTIMIZEME: There are several opportunities for micro-optimizations here.

    void GCAlloc::SweepGuts(GCBlock *b)
    {
        gcbits_t* blockbits = b->bits;
        for ( char *item = b->items, *limit = b->items + m_itemSize * b->GetCount() ; item < limit ; item += m_itemSize )
        {
            uint32_t bitsindex = GetBitsIndex(b, item);
            gcbits_t& marks = blockbits[bitsindex];

            int mq = marks & kFreelist;
            if(mq == kMark || mq == kQueued)    // Sweeping is lazy; don't sweep objects on the mark stack
            {
                // live item, clear bits
                marks &= ~kFreelist;
                continue;
            }

             if(mq == kFreelist)
                 continue; // freelist item, ignore

            // garbage, freelist it
#ifdef MMGC_HOOKS
            if(m_gc->heap->HooksEnabled())
                m_gc->heap->FreeHook(GetUserPointer(item), b->size - DebugSize(), uint8_t(GCHeap::GCSweptPoison));
#endif
            b->FreeSweptItem(item, bitsindex);
        }
    }

    // Incrementality: Note that there is the possibility for an unbounded pause here,
    // though it can be no worse than before the lazy sweeping.  And we can fix it, as 
    // shown below.
    //
    // AllocSlow calls Sweep, which will sweep the block but (a) may return the block 
    // to GCHeap rather than making it available to AllocSlow, if it is entirely empty, 
    // or (b) may find that the block is entirely full and keep it off the free list.
    // In either case the result may be that no memory is made available.
    //
    // AllocSlow handles the situation gracefully by trying another block, but there's no
    // bound on how many blocks it may have to try.  The alternative would be to bound
    // the number of attempts, so that a fully free block is not returned to GCHeap after
    // some have been freed, or a fresh block is allocated from GCHeap after some full
    // blocks have unsuccessfully been swept.  In either case that can be implemented in
    // AllocSlow, by AllocSlow just going to GCHeap after failing to obtain memory from
    // the free list after some attempts.

    bool GCAlloc::Sweep(GCBlock *b)
    {
        int oldNumFree;
        GCAssert(b->needsSweeping());
        GCAssert(m_qList == NULL);
        RemoveFromSweepList(b);

        oldNumFree = b->numFree;
        SweepGuts(b);
        m_totalAllocatedBytes -= (b->numFree - oldNumFree) * m_itemSize;
        if(b->numFree == m_itemsPerBlock)
        {
            UnlinkChunk(b);
            FreeChunk(b);
            return true;
        }

        if (b->numFree > 0)
            AddToFreeList(b);

        return false;
    }

    void GCAlloc::SweepNeedsSweeping()
    {
        GCBlock* next;
        for (GCBlock* b = m_needsSweeping; b != NULL; b = next)
        {
            next = b->nextFree;
            Sweep(b);
        }
        GCAssert(m_needsSweeping == NULL);
    }
    
    void GCAlloc::ClearMarks(GCAlloc::GCBlock* block)
    {
        GCAssert(m_qList == NULL);
        // Clear all the mark bits
        GCAssert(sizeof(gcbits_t) == 1);
        GCAssert(kFreelist == 3);
        GCAssert(m_numBitmapBytes % 4 == 0);
        uint32_t *pbits = (uint32_t*)(void *)block->bits;
        uint32_t mq32 = ~uint32_t(0x03030303U);

        // Clear the marked and queued bits
        // TODO: MMX version for IA32
        for(int i=0, n=m_numBitmapBytes>>2; i < n; i++) {
            pbits[i] &= mq32;
        }

        void *item = block->firstFree;
        while(item != NULL) {
            // Set freelist bit pattern.  If it's free it's free so clear all
            // the other bits.
            block->bits[GetBitsIndex(block, item)] = kFreelist;
            item = FLNext(item);
        }
    }

    void GCAlloc::ClearMarks()
    {
        for ( GCBlock *block=m_firstBlock, *next ; block ; block=next ) {
            next = Next(block);

            if (block->needsSweeping() && Sweep(block))
                continue;

            ClearMarks(block);
        }
    }

#ifdef _DEBUG
    void GCAlloc::CheckMarks()
    {
        GCBlock *b = m_firstBlock;

        while (b) {
            GCBlock *next = Next(b);
            GCAssertMsg(!b->needsSweeping(), "All needsSweeping should have been swept at this point.");

            for ( char *item = b->items, *limit = b->items + m_itemSize * b->GetCount() ; item < limit ; item += m_itemSize ) {
                gcbits_t m = GC::GetGCBits(item) & kFreelist;
                GCAssertMsg(m == 0 || m == kFreelist, "All items should be free or clear, nothing should be marked or queued.");
            }

            // Advance to next block
            b = next;
        }
    }

    /*static*/
    int GCAlloc::ConservativeGetMark(const void *item, bool bogusPointerReturnValue)
    {
        GCBlock *block = GetBlock(item);

#ifdef MMGC_MEMORY_INFO
        item = GetRealPointer(item);
#endif

        // guard against bogus pointers to the block header
        if (item < block->items)
            return bogusPointerReturnValue;

        // floor value to start of item
        int itemNum = GetObjectIndex(block, item);

        // skip pointers into dead space at end of block
        // FIXME: not clear there is any dead space with current design
        if (itemNum > ((GCAlloc*)block->alloc)->m_itemsPerBlock - 1)
            return bogusPointerReturnValue;

        // skip pointers into objects
        if(block->items + itemNum * block->size != item)
            return bogusPointerReturnValue;

        return GC::GetMark(item);
    }

    void GCAlloc::CheckFreelist()
    {
        GCBlock *b = m_firstFree;
        while(b)
        {
            void *freelist = b->firstFree;
            while(freelist)
            {
                // b->firstFree should be either 0 end of free list or a pointer into b, otherwise, someone
                // wrote to freed memory and hosed our freelist
                GCAssert(freelist == 0 || ((uintptr_t) freelist >= (uintptr_t) b->items && (uintptr_t) freelist < (uintptr_t) b + GCHeap::kBlockSize));
                freelist = FLNext(freelist);
            }
            b = b->nextFree;
        }
    }

#endif // _DEBUG

    // allows us to avoid division in GetItemIndex, kudos to Tinic
    static void ComputeMultiplyShift(uint16_t d, uint16_t &muli, uint16_t &shft)
    {
        uint32_t s = 0;
        uint32_t n = 0;
        uint32_t m = 0;
        for ( ; n < ( 1 << 13 ) ; s++) {
            m = n;
            n = ( ( 1 << ( s + 1 ) ) / d ) + 1;
        }
        shft = (uint16_t) s - 1;
        muli = (uint16_t) m;
    }

    uint16_t GCAlloc::ComputeMultiply(uint16_t d)
    {
        uint16_t m, s;
        ComputeMultiplyShift(d, m, s);
        return m;
    }

    uint16_t GCAlloc::ComputeShift(uint16_t d)
    {
        uint16_t m, s;
        ComputeMultiplyShift(d, m, s);
        return s;
    }

    REALLY_INLINE void GCAlloc::GCBlock::FreeSweptItem(const void *item, int bitsindex)
    {
        GCAlloc* alloc = (GCAlloc*)this->alloc;
#ifdef _DEBUG
        // Check that we're not freeing something on the mark stack
        GCAssert((bits[bitsindex] & kQueued) == 0);
        alloc->VerifyNotFree(this, item);
#endif

        numFree++;
        bits[bitsindex] = kFreelist;

#ifndef _DEBUG
        alloc->ClearNonRCObject((void*)item, size);
#endif
        FLPush(firstFree, item);
        VALGRIND_MEMPOOL_FREE(this, item);
    }

    void GCAlloc::GetUsageInfo(size_t& totalAskSize, size_t& totalAllocated)
    {
        int numAlloc = GetNumAlloc();
        totalAllocated = numAlloc * m_itemSize;
#ifdef MMGC_MEMORY_PROFILER
        totalAskSize = m_totalAskSize;
#else
        totalAskSize = 0;
#endif
    }

#ifdef MMGC_MEMORY_INFO

    /* static */
    void GCAlloc::VerifyFreeBlockIntegrity(void* item, uint32_t size, uint32_t limit)
    {
        // go through every item on the free list and make sure it wasn't written to
        // after being poisoned.
        while(item)
        {
            if (--limit == 0)
                break;
#ifdef MMGC_64BIT
            int n = (size >> 2) - 3;
#else
            int n = (size >> 2) - 1;
#endif

            int startIndex = (int)((uint32_t*)item - (uint32_t*)GetRealPointer(item));

            for(int i=startIndex; i<n; i++)
            {
                uint32_t data = ((uint32_t*)item)[i];
                if(data != uint32_t(GCHeap::GCSweptPoison) && data != uint32_t(GCHeap::GCFreedPoison))
                {
                    ReportDeletedMemoryWrite(item);
                    break;
                }
            }
            // next free item
            item = FLNext(item);
        }
    }

#endif //MMGC_MEMORY_INFO

    void GCAlloc::GetAllocStats(int& numAlloc, int& maxAlloc) const
    {
        numAlloc = 0;
        maxAlloc = 0;
        GCBlock *b=m_firstBlock;
        while (b)
        {
            maxAlloc += m_itemsPerBlock;
            numAlloc += (m_itemsPerBlock - b->numFree);
            b = Next(b);
        }
        numAlloc -= (m_qBudgetObtained - m_qBudget);
    }
    
#ifdef _DEBUG
    bool GCAlloc::IsOnEitherList(GCBlock *b)
    {
        return b->nextFree != NULL || b->prevFree != NULL || b == m_firstFree || b == m_needsSweeping;
    }
    
    /*static*/
    bool GCAlloc::IsPointerIntoGCObject(const void *item)
    {
        // Contorted code allows for debugging, don't "optimize" this
        GCBlock *block = GetBlock(item);
        bool retval = false;
        if(item >= block->items)
            retval = (GC::GetGCBits(block->items + GetObjectIndex(block, item) * ((GCAlloc*)block->alloc)->m_itemSize) & kFreelist) != kFreelist;
        return retval;
    }
    
    /*static*/
    bool GCAlloc::IsWhite(const void *item)
    {
        GCBlock *block = GetBlock(item);
        // not a real item
        if(item < block->items)
            return false;
        
        if(FindBeginning(item) != item)
            return false;
        
        return (GC::GetGCBits(item) & (kMark|kQueued)) == 0;
    }
#endif // _DEBUG
}

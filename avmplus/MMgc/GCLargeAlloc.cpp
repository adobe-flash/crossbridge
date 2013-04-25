/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

namespace MMgc
{
    GCLargeAlloc::GCLargeAlloc(GC* gc) : m_totalAllocatedBytes(0), m_gc(gc)
    {
        m_blocks = NULL;
        m_startedFinalize = false;
#ifdef MMGC_MEMORY_PROFILER
        m_totalAskSize = 0;
#endif
    }

#if defined DEBUG || defined MMGC_MEMORY_PROFILER
    void* GCLargeAlloc::Alloc(size_t originalSize, size_t requestSize, int flags)
#else
    void* GCLargeAlloc::Alloc(size_t requestSize, int flags)
#endif
    {
#ifdef DEBUG
        m_gc->heap->CheckForOOMAbortAllocation();
#endif
        GCHeap::CheckForAllocSizeOverflow(requestSize, sizeof(LargeBlock)+GCHeap::kBlockSize);

        int blocks = (int)((requestSize+sizeof(LargeBlock)+GCHeap::kBlockSize-1) / GCHeap::kBlockSize);
        uint32_t computedSize = blocks*GCHeap::kBlockSize - sizeof(LargeBlock);

        // Allocation must be signalled before we allocate because no GC work must be allowed to
        // come between an allocation and an initialization - if it does, we may crash, as
        // GCFinalizedObject subclasses may not have a valid vtable, but the GC depends on them
        // having it.  In principle we could signal allocation late but only set the object
        // flags after signaling, but we might still cause trouble for the profiler, which also
        // depends on non-interruptibility.

        m_gc->SignalAllocWork(computedSize);

        // Pointer containing memory is always zeroed (see bug 594533).
        if((flags&GC::kContainsPointers) != 0)
            flags |= GC::kZero;

        LargeBlock *block = (LargeBlock*) m_gc->AllocBlock(blocks, PageMap::kGCLargeAllocPageFirst,
                                                           (flags&GC::kZero) != 0, (flags&GC::kCanFail) != 0);
        void *item = NULL;

        if (block)
        {
            // Code below uses these optimizations
            GCAssert((unsigned long)GC::kFinalize == (unsigned long)kFinalizable);
            GCAssert((unsigned long)GC::kInternalExact == (unsigned long)kVirtualGCTrace);
            
            gcbits_t flagbits0 = 0;
            gcbits_t flagbits1 = 0;

#if defined VMCFG_EXACT_TRACING
            flagbits0 = (flags & (GC::kFinalize|GC::kInternalExact));
#elif defined VMCFG_SELECTABLE_EXACT_TRACING
            flagbits0 = (flags & (GC::kFinalize|m_gc->runtimeSelectableExactnessFlag));  // 0 or GC::kInternalExact
#else
            flagbits0 = (flags & GC::kFinalize);
#endif

            VALGRIND_CREATE_MEMPOOL(block, /*rdzone*/0, (flags&GC::kZero) != 0);
            VALGRIND_MEMPOOL_ALLOC(block, block, sizeof(LargeBlock));

            block->gc = this->m_gc;
            block->alloc= this;
            block->next = m_blocks;
            block->size = computedSize;
            block->bibopTag = 0;
#ifdef MMGC_FASTBITS
            block->bitsShift = 12;     // Always use bits[0]
#endif
            block->containsPointers = ((flags&GC::kContainsPointers) != 0) ? 1 : 0;
            block->rcobject = ((flags&GC::kRCObject) != 0) ? 1 : 0;
            block->bits = block->flags;
            m_blocks = block;

            item = block->GetObject();

            if(m_gc->collecting && !m_startedFinalize)
                flagbits0 |= kMark;

            block->flags[0] = flagbits0;
            block->flags[1] = flagbits1;
#ifdef _DEBUG
            (void)originalSize;
            if (flags & GC::kZero)
            {
                if (!RUNNING_ON_VALGRIND)
                {
                    // AllocBlock should take care of this
                    for(int i=0, n=(int)(requestSize/sizeof(int)); i<n; i++) {
                        if(((int*)item)[i] != 0)
                            GCAssert(false);
                    }
                }
            }
#endif

            // see comments in GCAlloc about using full size instead of ask size
            VALGRIND_MEMPOOL_ALLOC(block, item, computedSize);

#ifdef MMGC_HOOKS
            GCHeap* heap = GCHeap::GetGCHeap();
            if(heap->HooksEnabled()) {
                size_t userSize = block->size - DebugSize();
#ifdef MMGC_MEMORY_PROFILER
                m_totalAskSize += originalSize;
                heap->AllocHook(GetUserPointer(item), originalSize, userSize, /*managed=*/true);
#else
                heap->AllocHook(GetUserPointer(item), 0, userSize, /*managed=*/true);
#endif
            }
#endif
            m_totalAllocatedBytes += computedSize;
        }
        return item;
    }


    void GCLargeAlloc::Free(const void *item)
    {
        LargeBlock *b = GetLargeBlock(item);

#ifdef _DEBUG
        // RCObject have contract that they must clean themselves, since they
        // have to scan themselves to decrement other RCObjects they might as well
        // clean themselves too, better than suffering a memset later
        if(b->rcobject)
            m_gc->RCObjectZeroCheck((RCObject*)GetUserPointer(item));
#endif


        // We can't allow free'ing something during Sweeping, otherwise alloc counters
        // get decremented twice and destructors will be called twice.
        GCAssert(m_gc->collecting == false || m_gc->marking == true);
        if (m_gc->marking && (m_gc->collecting || IsProtectedAgainstFree(b))) {
            m_gc->AbortFree(GetUserPointer(item));
            return;
        }

        m_gc->policy.signalFreeWork(b->size);

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

        if(b->flags[0] & kHasWeakRef)
            m_gc->ClearWeakRef(GetUserPointer(item));

        LargeBlock **prev = &m_blocks;
        while(*prev)
        {
            if(b == *prev)
            {
                *prev = Next(b);
                size_t numBlocks = b->GetNumBlocks();
                m_totalAllocatedBytes -= b->size;
                VALGRIND_MEMPOOL_FREE(b, b);
                VALGRIND_MEMPOOL_FREE(b, item);
                VALGRIND_DESTROY_MEMPOOL(b);
                m_gc->FreeBlock(b, (uint32_t)numBlocks);
                return;
            }
            prev = (LargeBlock**)(&(*prev)->next);
        }
        GCAssertMsg(false, "Bad free!");
    }

    void GCLargeAlloc::ClearMarks()
    {
        LargeBlock *block = m_blocks;
        while (block) {
            block->flags[0] &= ~(kMark|kQueued);
            block = Next(block);
        }
    }

    void GCLargeAlloc::Finalize()
    {
        m_startedFinalize = true;
        LargeBlock **prev = &m_blocks;
        while (*prev) {
            LargeBlock *b = *prev;
            if ((b->flags[0] & kMark) == 0)
            {
                GCAssert((b->flags[0] & kQueued) == 0);
                GC* gc = b->gc;

                // GC::Finalize calls GC::MarkOrClearWeakRefs before calling GCAlloc::Finalize,
                // ergo there should be no unmarked objects with weak refs.
                
                GCAssertMsg((b->flags[0] & kHasWeakRef) == 0, "No unmarked object should have a weak ref at this point");
                
                // Large blocks may be allocated by finalizers for large blocks, creating contention
                // for the block list.  Yet the block list must be live, since eg GetUsageInfo may be
                // called by the finalizers (or their callees).
                //
                // Unlink the block from the list early to avoid contention.

                *prev = Next(b);
                b->next = NULL;

                void *item = b+1;
                if (b->flags[0] & kFinalizable)
                {
                    GCFinalizedObject *obj = (GCFinalizedObject *) item;
                    obj = (GCFinalizedObject *) GetUserPointer(obj);
                    obj->~GCFinalizedObject();
#if defined(_DEBUG)
                    if(b->rcobject) {
                        gc->RCObjectZeroCheck((RCObject*)obj);
                    }
#endif
                }
                
                // GC::GetWeakRef will not allow a weak reference to be created to an object that
                // is ready for destruction.
                
                GCAssertMsg((b->flags[0] & kHasWeakRef) == 0, "No unmarked object should have a weak ref at this point");

#ifdef MMGC_HOOKS
                if(m_gc->heap->HooksEnabled())
                {
                #ifdef MMGC_MEMORY_PROFILER
                    if(GCHeap::GetGCHeap()->GetProfiler())
                        m_totalAskSize -= GCHeap::GetGCHeap()->GetProfiler()->GetAskSize(GetUserPointer(item));
                #endif

                    m_gc->heap->FinalizeHook(GetUserPointer(item), b->size - DebugSize());
                }
#endif

                // The block is not empty until now, so now add it.
                gc->AddToLargeEmptyBlockList(b);
                m_totalAllocatedBytes -= b->size;
                continue;
            }
            // clear marks
            b->flags[0] &= ~(kMark|kQueued);
            prev = (LargeBlock**)(&b->next);
        }
        m_startedFinalize = false;
    }

    GCLargeAlloc::~GCLargeAlloc()
    {
        GCAssert(!m_blocks);
    }

#ifdef _DEBUG
    /* static */
    bool GCLargeAlloc::ConservativeGetMark(const void *item, bool bogusPointerReturnValue)
    {
        if(!IsLargeBlock(item))
            return bogusPointerReturnValue;
        return (GetLargeBlock(item)->flags[0] & kMark) != 0;
    }

    /*static*/
    bool GCLargeAlloc::IsWhite(const void *item)
    {
        if(!IsLargeBlock(item))
            return false;
        return (GetLargeBlock(item)->flags[0] & (kMark|kQueued)) == 0;
    }
#endif

    void GCLargeAlloc::GetUsageInfo(size_t& totalAskSize, size_t& totalAllocated)
    {
        totalAskSize = 0;
        totalAllocated = 0;

        LargeBlock *block = m_blocks;
        while (block) {
            totalAllocated += block->size;
            block = Next(block);
        }

#ifdef MMGC_MEMORY_PROFILER
        totalAskSize += m_totalAskSize;
#endif
    }
}

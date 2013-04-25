/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "MMgc.h"

namespace MMgc
{
    // kSizeClassIndex[] is an array that lets us quickly determine the allocator
    // to use for a given size, without division.  A given allocation is rounded
    // up to the nearest multiple of 8, then downshifted 3 bits, and the index
    // tells us which allocator to use.  (A special case is made for <= 4 bytes on
    // 32-bit systems in FindAllocatorForSize to keep the table small.)

    // Code to generate the table:
    //
    //      const int kMaxSizeClassIndex = (kLargestAlloc>>3)+1;
    //      uint8_t kSizeClassIndex[kMaxSizeClassIndex];
    //      printf("static const unsigned kMaxSizeClassIndex = %d;\n",kMaxSizeClassIndex);
    //      printf("static const uint8_t kSizeClassIndex[kMaxSizeClassIndex] = {\n");
    //      for (int size = 0; size <= kLargestAlloc; size += 8)
    //      {
    //          int i = 0;
    //          while (kSizeClasses[i] < size)
    //          {
    //              ++i;
    //              AvmAssert(i < kNumSizeClasses);
    //          }
    //          AvmAssert((size>>3) < kMaxSizeClassIndex);
    //          kSizeClassIndex[(size>>3)] = i;
    //          if (size > 0) printf(",");
    //          if (size % (16*8) == 0) printf("\n");
    //          printf(" %d",i);
    //      }
    //      printf("};\n");

#ifdef MMGC_64BIT
    const int16_t FixedMalloc::kSizeClasses[kNumSizeClasses] = {
        8, 16, 24, 32, 40, 48, 56, 64, 72, 80, //0-9
        88, 96, 104, 112, 120, 128, 136, 144, 152, 160, //10-19
        168, 176, 192, 208, 224, 232, 248, 264, 288, 304, //20-29
        336, 360, 400, 448, 504, 576, 672, 800, 1008, 1344, //30-39
        2016, //40
    };

    /*static*/ const uint8_t FixedMalloc::kSizeClassIndex[kMaxSizeClassIndex] = {
         0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
         15, 16, 17, 18, 19, 20, 21, 22, 22, 23, 23, 24, 24, 25, 26, 26,
         27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32,
         32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34,
         35, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36,
         36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
         37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
         38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 39,
         39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
         39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
         39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 40,
         40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
         40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
         40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
         40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
         40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40
    };

#else
    const int16_t FixedMalloc::kSizeClasses[kNumSizeClasses] = {
        4, 8, 16, 24, 32, 40, 48, 56, 64, 72, //0-9
        80, 88, 96, 104, 112, 120, 128, 144, 160, 176, //10-19
        184, 192, 200, 208, 224, 232, 248, 264, 288, 312, //20-29
        336, 368, 400, 448, 504, 576, 672, 808, 1016, 1352, //30-39
        2032, //40
    };

    /*static*/ const uint8_t FixedMalloc::kSizeClassIndex[kMaxSizeClassIndex] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 17, 18, 18, 19, 19, 20, 21, 22, 23, 24, 24, 25, 26, 26,
        27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 31, 32,
        32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36,
        36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
        37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
        38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
        39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
        39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
        39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40,
        40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
        40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
        40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
        40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
        40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40
    };

#endif

#if defined DEBUG
    // For debugging we track live large objects in a list.  If there are a lot
    // of large objects then a list may slow down debug builds too much; in that
    // case we can move to a tree or similar structure.  (It's useful to avoid using
    // large objects in this data structure.)
    struct FixedMalloc::LargeObject
    {
        const void *item;       // Start of a block
        LargeObject* next;      // Next object
    };
#endif
    
    /*static*/
    FixedMalloc *FixedMalloc::instance;

    void FixedMalloc::InitInstance(GCHeap* heap)
    {
        // The size tables above are derived based on a block size of 4096; this
        // assert keeps us honest.  Talk to Lars if you get into trouble here.
        GCAssert(GCHeap::kBlockSize == 4096);
        
        m_heap = heap;
        numLargeBlocks = 0;
        VMPI_lockInit(&m_largeAllocInfoLock);
    #ifdef MMGC_MEMORY_PROFILER
        totalAskSizeLargeAllocs = 0;
    #endif
    #if defined DEBUG && !defined AVMPLUS_SAMPLER
        largeObjects = NULL;
        VMPI_lockInit(&m_largeObjectLock);
    #endif

        for (int i=0; i<kNumSizeClasses; i++)
            m_allocs[i].Init((uint32_t)kSizeClasses[i], heap);

        m_rootFindCache.Init();

        FixedMalloc::instance = this;
    }

    void FixedMalloc::DestroyInstance()
    {
        for (int i=0; i<kNumSizeClasses; i++)
            m_allocs[i].Destroy();

        VMPI_lockDestroy(&m_largeAllocInfoLock);
    #if defined DEBUG && !defined AVMPLUS_SAMPLER
        VMPI_lockDestroy(&m_largeObjectLock);
    #endif
        m_rootFindCache.Destroy();

        FixedMalloc::instance = NULL;
    }

    void FixedMalloc::FindBeginningRootsCache::Init()
    {
        VMPI_lockInit(&m_lock);
        m_objBegin = NULL;
        m_objSize = 0;
    }

    void FixedMalloc::FindBeginningRootsCache::Destroy()
    {
        VMPI_lockDestroy(&m_lock);
    }

    void* FASTCALL FixedMalloc::OutOfLineAlloc(size_t size, FixedMallocOpts flags)
    {
        return Alloc(size, flags);
    }

    void FASTCALL FixedMalloc::OutOfLineFree(void* p)
    {
        Free(p);
    }

    void FixedMalloc::GetUsageInfo(size_t& totalAskSize, size_t& totalAllocated)
    {
        totalAskSize = 0;
        totalAllocated = 0;
        for (int i=0; i<kNumSizeClasses; i++) {
            size_t allocated = 0;
            size_t ask = 0;
            m_allocs[i].GetUsageInfo(ask, allocated);
            totalAskSize += ask;
            totalAllocated += allocated;
        }

#ifdef MMGC_MEMORY_PROFILER
        {
            MMGC_LOCK(m_largeAllocInfoLock);
            totalAskSize += totalAskSizeLargeAllocs;
        }
#endif

        // Not entirely accurate, assumes large allocations using all of
        // the last block (large ask size not stored).
        totalAllocated += (GetNumLargeBlocks() * GCHeap::kBlockSize);
    }

    void *FixedMalloc::LargeAlloc(size_t size, FixedMallocOpts flags)
    {
        GCHeap::CheckForAllocSizeOverflow(size, GCHeap::kBlockSize+DebugSize());

        size += DebugSize();

        int blocksNeeded = (int)GCHeap::SizeToBlocks(size);
        uint32_t gcheap_flags = GCHeap::kExpand;

        if((flags & kCanFail) != 0)
            gcheap_flags |= GCHeap::kCanFail;
        if((flags & kZero) != 0)
            gcheap_flags |= GCHeap::kZero;

        void *item = m_heap->Alloc(blocksNeeded, gcheap_flags);
        if(item)
        {
            VALGRIND_CREATE_MEMPOOL(item, 0,  (flags & kZero) != 0);

            item = GetUserPointer(item);
#ifdef MMGC_HOOKS
            if(m_heap->HooksEnabled())
                m_heap->AllocHook(item, size - DebugSize(), Size(item), /*managed=*/false);
#endif // MMGC_HOOKS

            UpdateLargeAllocStats(item, blocksNeeded);

#ifdef DEBUG
            // Fresh memory poisoning
            if ((flags & kZero) == 0 && !RUNNING_ON_VALGRIND)
                memset(item, uint8_t(GCHeap::FXFreshPoison), size - DebugSize());

#ifndef AVMPLUS_SAMPLER
            // Enregister the large object
            AddToLargeObjectTracker(item);
#endif
#endif // DEBUG

            VALGRIND_MEMPOOL_ALLOC(GetRealPointer(item), item, Size(item));
        }
        return item;
    }

    void FixedMalloc::LargeFree(void *item)
    {
#if defined DEBUG && !defined AVMPLUS_SAMPLER
        RemoveFromLargeObjectTracker(item);
#endif
        UpdateLargeFreeStats(item, GCHeap::SizeToBlocks(LargeSize(item)));

#ifdef MMGC_HOOKS
        if(m_heap->HooksEnabled())
        {
            m_heap->FinalizeHook(item, Size(item));
            m_heap->FreeHook(item, Size(item), uint8_t(GCHeap::FXFreedPoison));
        }
#endif
        m_heap->FreeNoProfile(GetRealPointer(item));
        VALGRIND_MEMPOOL_FREE(GetRealPointer(item), item);
        VALGRIND_DESTROY_MEMPOOL(GetRealPointer(item));
    }

    size_t FixedMalloc::LargeSize(const void *item)
    {
        return m_heap->Size(GetRealPointer(item)) * GCHeap::kBlockSize;
    }

    void *FixedMalloc::Calloc(size_t count, size_t elsize, FixedMallocOpts opts)
    {
        return Alloc(GCHeap::CheckForCallocSizeOverflow(count, elsize), opts);
    }

    size_t FixedMalloc::GetTotalSize()
    {
        size_t total = GetNumLargeBlocks();
        for (int i=0; i<kNumSizeClasses; i++)
            total += m_allocs[i].GetNumBlocks();
        return total;
    }

#ifdef MMGC_MEMORY_PROFILER
    void FixedMalloc::DumpMemoryInfo()
    {
        size_t inUse, ask;
        GetUsageInfo(ask, inUse);
        GCLog("[mem] FixedMalloc total %d pages inuse %d bytes ask %d bytes\n", GetTotalSize(), inUse, ask);
        for (int i=0; i<kNumSizeClasses; i++) {
            m_allocs[i].GetUsageInfo(ask, inUse);
            if( m_allocs[i].GetNumBlocks() > 0)
                GCLog("[mem] FixedMalloc[%d] total %d pages inuse %d bytes ask %d bytes\n", kSizeClasses[i], m_allocs[i].GetNumBlocks(), inUse, ask);
        }
        GCLog("[mem] FixedMalloc[large] total %d pages\n", GetNumLargeBlocks());
    }
#endif

    const void* FixedMalloc::FindBeginning(const void *addr)
    {
        const void* begin_recv = NULL;
        size_t size_recv;
        if (FindBeginningAndSize(addr, begin_recv, size_recv))
            return begin_recv;
        else
            return NULL;
    }

    REALLY_INLINE
    bool FixedMalloc::FindBeginningRootsCache::Lookup(const void* addr,
                                                      const void* &begin_recv,
                                                      size_t &size_recv)
    {
        MMGC_LOCK(m_lock);
        const char* caddr = (const char*)addr;
        const char* cached = (const char*)m_objBegin;

        if (cached <= caddr && caddr < cached + m_objSize)
        {
            begin_recv = m_objBegin;
            size_recv  = m_objSize;
            return true;
        }

        return false;
    }

    bool FixedMalloc::FindBeginningAndSize(const void* addr,
                                           const void* &begin_recv,
                                           size_t &size_recv)
    {
        const void* obj;
        size_t sz;

        if (m_rootFindCache.Lookup(addr, begin_recv, size_recv))
            return true;

        // Only reliable way to identify small objects is to traverse
        // the m_allocs array (see Bugzilla 663386).
        for (int i=0; i<kNumSizeClasses; i++) {
            if (m_allocs[i].QueryOwnsObject(addr)) {
                obj = FixedAlloc::FindBeginning(addr);
                GCAssert(obj != NULL);
                size_recv = m_allocs[i].GetItemSize();

                // The below assertion currently fails in Debugger
                // builds because FixedAlloc::Size does not discount
                // out DebugSize() while GetItemSize does.  What
                // *should* FixedAlloc::Size do?  How is it used?
                //GCAssert(size_recv == FixedAlloc::Size(obj));

                begin_recv = obj;
                return true;
            }
        }

        // if its not small, then it must be large (if its any object at all).
        GCHeap::HeapBlock *b = m_heap->InteriorAddrToBlock(addr);
        if (b && b->inUse()) {
            GCAssert(b->size >= 1);
            obj = GetUserPointer(b->baseAddr);
            sz = LargeSize(obj);
            sz = sz - DebugSize();

            size_recv = sz;
            begin_recv = obj;
            return true;
        }

        return false;
    }

#ifdef DEBUG
    // If EnsureFixedMallocMemory returns and fixed-memory checking has not
    // been disabled then item was definitely allocated by an allocator owned
    // by this FixedMalloc.  Large objects must be handled one of two ways
    // depending on whether the sampler is operating: if it is, we can't
    // allocate storage to track large objects (see bugzilla 533954),
    // so fall back on a less accurate method.

    void FixedMalloc::EnsureFixedMallocMemory(const void* item)
    {
        // For a discussion of this flag, see bugzilla 564878.
        if (!m_heap->config.checkFixedMemory())
            return;
        
        for (int i=0; i<kNumSizeClasses; i++)
            if (m_allocs[i].QueryOwnsObject(item))
                return;

#ifdef AVMPLUS_SAMPLER
        if (m_heap->SafeSize(GetRealPointer(item)) != (size_t)-1)
            return;
#else
        {
            MMGC_LOCK(m_largeObjectLock);
            for ( LargeObject* lo=largeObjects; lo != NULL ; lo=lo->next)
                if (lo->item == item)
                    return;
        }
#endif

        GCAssertMsg(false, "Trying to delete an object with FixedMalloc::Free that was not allocated with FixedMalloc::Alloc");
    }

#ifndef AVMPLUS_SAMPLER
    void FixedMalloc::AddToLargeObjectTracker(const void* item)
    {
        if (!m_heap->config.checkFixedMemory())
            return;
        
        LargeObject* lo = (LargeObject*)Alloc(sizeof(LargeObject));
        lo->item = item;
        MMGC_LOCK(m_largeObjectLock);
        lo->next = largeObjects;
        largeObjects = lo;
    }

    void FixedMalloc::RemoveFromLargeObjectTracker(const void* item)
    {
        if (!m_heap->config.checkFixedMemory())
            return;
        
        void *loToFree=NULL;
        {
            MMGC_LOCK(m_largeObjectLock);
            LargeObject *lo, *prev;
            for ( prev=NULL, lo=largeObjects ; lo != NULL ; prev=lo, lo=lo->next ) {
                if (lo->item == item) {
                    if (prev != NULL)
                        prev->next = lo->next;
                    else
                        largeObjects = lo->next;
                    loToFree = lo;
                    break;
                }
            }
        }
        if(loToFree)
            Free(loToFree);
    }
#endif // !AVMPLUS_SAMPLER
#endif // DEBUG
}


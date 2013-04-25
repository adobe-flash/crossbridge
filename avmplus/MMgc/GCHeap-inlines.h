/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCHeap_inlines__
#define __GCHeap_inlines__

// Inline functions for GCHeap

namespace MMgc
{
    REALLY_INLINE bool GCHeapConfig::checkFixedMemory()
    {
        return _checkFixedMemory;
    }

    REALLY_INLINE void GCHeapConfig::clearCheckFixedMemory()
    {
        _checkFixedMemory = false;
    }

    REALLY_INLINE bool GCManager::tryAddGC(GC* gc)
    {
        return collectors.Add(gc);
    }

    REALLY_INLINE void GCManager::removeGC(GC* gc)
    {
        collectors.Remove(gc);
    }

    REALLY_INLINE BasicList<GC*>& GCManager::gcs()
    {
        return collectors;
    }

    /* legacy API */
    /* static */
    REALLY_INLINE void GCHeap::Init(GCMallocFuncPtr malloc, GCFreeFuncPtr free, int initialSize)
    {
        (void)malloc;
        (void)free;
        GCHeapConfig props;
        props.initialSize = initialSize;
        Init(props);
    }


    /* static */
    REALLY_INLINE GCHeap *GCHeap::GetGCHeap()
    {
        // when OOM occurs the last EnterFrame macro destroys the
        // heap so this can be NULL, clients should detect NULL
        // and re-create the heap if desired
        return instance;
    }

    /* static */
    REALLY_INLINE bool GCHeap::EnterLock()
    {
        GCAssert(instanceEnterLockInitialized);
        return VMPI_lockAcquire(&instanceEnterLock);
    }

    /* static */
    REALLY_INLINE bool GCHeap::EnterRelease()
    {
        GCAssert(instanceEnterLockInitialized);
        return VMPI_lockRelease(&instanceEnterLock);
    }

    REALLY_INLINE FixedMalloc* GCHeap::GetFixedMalloc()
    {
        return FixedMalloc::GetFixedMalloc();
    }

    REALLY_INLINE void *GCHeap::AllocNoOOM(size_t size, uint32_t flags)
    {
        return Alloc(size, flags|kNoOOMHandling);
    }

    REALLY_INLINE void GCHeap::Free(void *item)
    {
        FreeInternal(item, true, true);
    }

    REALLY_INLINE void GCHeap::FreeNoOOM(void* item)
    {
        FreeInternal(item, true, false);
    }

    REALLY_INLINE void GCHeap::FreeNoProfile(void *item)
    {
        FreeInternal(item, false, true);
    }

    REALLY_INLINE void GCHeap::SignalCodeMemoryDeallocated(size_t size, bool gcheap_memory)
    {
        if (gcheap_memory)
            gcheapCodeMemory -= size;
        else
            externalCodeMemory -= size;
    }

    REALLY_INLINE void GCHeap::Free(void *item, size_t /*ignore*/)
    {
        FreeInternal(item, true, true);
    }

    REALLY_INLINE size_t GCHeap::GetUsedHeapSize() const
    {
        return numAlloc;
    }

    REALLY_INLINE size_t GCHeap::GetFreeHeapSize() const
    {
        return GetTotalHeapSize()-numAlloc;
    }

    REALLY_INLINE size_t GCHeap::GetTotalCodeSize() const
    {
        return gcheapCodeMemory + externalCodeMemory;
    }

    REALLY_INLINE size_t GCHeap::GetTotalGCHeapAllocatedCodeSize() const
    {
        return gcheapCodeMemory;
    }
#ifdef MMGC_POLICY_PROFILING
    REALLY_INLINE void GCHeap::GetMaxTotalHeapSize(size_t& heapBlocks, size_t& privateBlocks)
    {
        heapBlocks = maxTotalHeapSize / kBlockSize;
        privateBlocks = maxPrivateMemory / kBlockSize;
    }
#endif

    REALLY_INLINE size_t GCHeap::GetTotalHeapSize() const
    {
        return blocksLen - numDecommitted + largeAllocs;
    }

    REALLY_INLINE void GCHeap::PreventDestruct()
    {
        preventDestruct++;
    }

    REALLY_INLINE void GCHeap::AllowDestruct()
    {
        GCAssert(preventDestruct > 0);
        preventDestruct--;
    }

    /* static */
    REALLY_INLINE size_t GCHeap::SizeToBlocks(size_t bytes)
    {
        return ((bytes + kBlockSize - 1) & size_t(kBlockMask)) / kBlockSize;
    }

#ifdef MMGC_HOOKS
    REALLY_INLINE void GCHeap::EnableHooks()
    {
        hooksEnabled = true;
    }

    REALLY_INLINE bool GCHeap::HooksEnabled() const
    {
        return hooksEnabled;
    }
#endif

#ifdef MMGC_MEMORY_PROFILER
    REALLY_INLINE MemoryProfiler *GCHeap::GetProfiler()
    {
        return profiler;
    }

    REALLY_INLINE void GCHeap::DumpFatties()
    {
        profiler->DumpFatties();
    }
#endif

    REALLY_INLINE MemoryStatus GCHeap::GetStatus()
    {
        return status;
    }

    /* static */
    REALLY_INLINE size_t GCHeap::CheckForAllocSizeOverflow(size_t size, size_t extra)
    {
        //calculate the total requested size
        uint64_t total = (uint64_t)size + (uint64_t)extra;

        //check if request size exceeds kMaxObjectSize
        // or for sizeof(size_t) = 8 bytes check for wraparound on the total value
#ifdef MMGC_64BIT
        if ((total > (uint64_t)kMaxObjectSize) || (total < size) || (total < extra))
            SignalObjectTooLarge();
#else
        // This is the 32-bit implementation, it avoids unnecessary checks for overflow.
        if (total > (uint64_t)kMaxObjectSize)
            SignalObjectTooLarge();
#endif
        return size_t(total);
    }

    /* static */
    REALLY_INLINE size_t GCHeap::CheckForCallocSizeOverflow(size_t count, size_t elsize)
    {
        //If either of the size of requested bytes
        //or the number of requested size
        //or if their product exceeds kMaxObjectSize
        //we treat that as overflow and abort
        uint64_t total = (uint64_t)elsize * (uint64_t)count;
#ifdef MMGC_64BIT
        if(   elsize > kMaxObjectSize
              || count >= kMaxObjectSize
              || total > (uint64_t)kMaxObjectSize)
            SignalObjectTooLarge();
#else
        if(total > (uint64_t)kMaxObjectSize)
            SignalObjectTooLarge();
#endif
        return size_t(total);
    }

    /* static */
    REALLY_INLINE size_t GCHeap::CheckForNewSizeOverflow(size_t count, size_t elsize, bool canFail)
    {
        //If either of the size of requested bytes
        //or the number of requested size
        //or if their product exceeds kMaxObjectSize
        //we treat that as overflow and abort
        uint64_t total = (uint64_t)elsize * (uint64_t)count;
#ifdef MMGC_64BIT
        if(   elsize > MMgc::GCHeap::kMaxObjectSize
            || count >= MMgc::GCHeap::kMaxObjectSize
            || total > (uint64_t)MMgc::GCHeap::kMaxObjectSize)
#else
        if(total > (uint64_t)MMgc::GCHeap::kMaxObjectSize)
#endif
        {
            if (canFail)
                return 0;
            MMgc::GCHeap::SignalObjectTooLarge();
        }
        return count;
    }

    REALLY_INLINE GCHeapConfig &GCHeap::Config()
    {
        return config;
    }

    REALLY_INLINE void *GCHeap::GetStackEntryAddress()
    {
        return (void*)GetEnterFrame();
    }

    REALLY_INLINE EnterFrame *GCHeap::GetEnterFrame()
    {
        return enterFrame;
    }

    REALLY_INLINE GC* GCHeap::SetActiveGC(GC* gc)
    {
        GCAssertMsg(IsStackEntered(), "A MMGC_ENTER macro must exist to use GCHeap memory");
        return GetEnterFrame()->SetActiveGC(gc);
    }

    REALLY_INLINE bool GCHeap::IsStackEntered()
    {
        return !entryChecksEnabled || GetEnterFrame() != NULL;
    }

    // remove this and make them always enabled once its possible
    REALLY_INLINE void GCHeap::SetEntryChecks(bool to)
    {
        entryChecksEnabled = to;
    }

#ifdef MMGC_MEMORY_PROFILER
    /* static */
    REALLY_INLINE bool GCHeap::IsProfilerInitialized()
    {
        return profiler != (MemoryProfiler*)-1;
    }
#endif

    REALLY_INLINE bool GCHeap::HeapBlock::inUse() const
    {
        return prev == NULL;
    }

    REALLY_INLINE bool GCHeap::HeapBlock::isSentinel() const
    {
        return baseAddr == 0;
    }

    REALLY_INLINE char *GCHeap::HeapBlock::endAddr() const
    {
        return baseAddr + size*kBlockSize;
    }

    REALLY_INLINE void GCHeap::HeapBlock::Clear()
    {
        baseAddr = NULL;
        size = 0;
        sizePrevious = 0;
        prev = NULL;
        next = NULL;
        committed = false;
        dirty = false;
#if defined(MMGC_MEMORY_PROFILER) && defined(MMGC_MEMORY_INFO)
        allocTrace = 0;
        freeTrace = 0;
#endif
    }

    REALLY_INLINE void GCHeap::HeapBlock::Init(char *baseAddr, size_t size, bool dirty)
    {
        Clear();
        this->baseAddr     = baseAddr;
        this->size         = size;
        committed = true;
        this->dirty = dirty;
    }

    REALLY_INLINE void GCHeap::HeapBlock::FreelistInit()
    {
        Clear();
        prev         = this;
        next         = this;
        committed    = true;
        dirty       = true;
    }

#ifndef _DEBUG
    // debug only freelist consistency checks
    REALLY_INLINE void GCHeap::CheckFreelist()
    {
        // nothing
    }
#endif

    REALLY_INLINE size_t GCHeap::LargeAllocSize(const void *item)
    {
        Region *r = AddrToRegion(item);
        // Note: we can't use r->baseAddr or r->reserveTop b/c
        // alignment might have shifted the start down and we may
        // not have committed the whole region
        return (r->commitTop - (char*)item) / kBlockSize;
    }

    REALLY_INLINE size_t GCHeap::Size(const void *item)
    {
        MMGC_LOCK(m_spinlock);
        GCAssert((uintptr_t(item) & (kBlockSize-1)) == 0);
        HeapBlock *block = BaseAddrToBlock(item);
        if(block)
            return block->size;
        else if(AddrToRegion(item))
            return LargeAllocSize((void*)item);
        return 0;
    }

    REALLY_INLINE void GCHeap::RemoveFromList(HeapBlock *block)
    {
        GCAssert(!block->inUse());
        block->prev->next = block->next;
        block->next->prev = block->prev;
        block->next = block->prev = 0;
    }

    REALLY_INLINE uint32_t GCHeap::GetFreeListIndex(size_t size)
    {
        if (size <= kUniqueThreshold) {
            return (uint32_t)size-1;
        } else if (size >= kHugeThreshold) {
            return kNumFreeLists-1;
        } else {
            return (uint32_t) ((size-kUniqueThreshold)/kFreeListCompression+kUniqueThreshold-1);
        }
    }

    REALLY_INLINE bool GCHeap::statusNotificationBeingSent()
    {
        return m_notificationBeingSent;
    }

    REALLY_INLINE bool GCHeap::statusNotNormalOrAbort()
    {
        return status != kMemNormal && status != kMemAbort;
    }

    REALLY_INLINE size_t GCHeap::numHeapBlocksToNumBlocks(size_t numBlocks)
    {
        size_t bytes = numBlocks * sizeof(HeapBlock);
        // round up to nearest block
        bytes = (bytes + kBlockSize - 1) & size_t(kBlockMask);
        return bytes / kBlockSize;
    }

    REALLY_INLINE bool GCHeap::HaveFreeRegion() const
    {
        return nextRegion != NULL || freeRegion != NULL;
    }

    REALLY_INLINE bool GCHeap::newPagesDirty()
    {
        return config.useVirtualMemory ? AVMPI_areNewPagesDirty() : true;
    }
}

#endif /* __GCHeap_inlines__ */

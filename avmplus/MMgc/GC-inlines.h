/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GC_inlines__
#define __GC_inlines__

// Inline functions for GCRoot, GC, GC::AllocaAutoPtr, GCWorkItem, Cleaner
// Inline functions for the write barrier are in WriteBarrier.h for now.

namespace MMgc
{
    
    // GCRoot

    REALLY_INLINE void *GCRoot::operator new(size_t size)
    {
        FixedMalloc *fm = FixedMalloc::GetFixedMalloc();
        void *retval = fm->OutOfLineAlloc(size, MMgc::kZero);
        // Bugzilla 664137, 681388: speed up imminent FindBeginning invocation.
        fm->m_rootFindCache.Stash(retval, size);
        return retval;
    }

    REALLY_INLINE void GCRoot::operator delete (void *object)
    {
        FixedMalloc* fm = FixedMalloc::GetFixedMalloc();
        fm->m_rootFindCache.Clear(object);
        fm->OutOfLineFree(object);
    }

#ifndef VMCFG_AOT
    REALLY_INLINE GC *GCRoot::GetGC() const { return gc; }
#endif

    // GC

    REALLY_INLINE void *GC::GetGCContextVariable(int var) const
    {
        return m_contextVars[var];
    }

    REALLY_INLINE void GC::SetGCContextVariable(int var, void *val)
    {
        m_contextVars[var] = val;
    }

    REALLY_INLINE avmplus::AvmCore *GC::core() const
    {
        return (avmplus::AvmCore*)GetGCContextVariable(GCV_AVMCORE);
    }

    REALLY_INLINE GC* GC::GetActiveGC() 
    {
        GCHeap *heap = GCHeap::GetGCHeap();
        if(heap && heap->GetEnterFrame())
            return heap->GetEnterFrame()->GetActiveGC();
        return NULL;
    }

    REALLY_INLINE void GC::QueueCollection()
    {
        policy.queueFullCollection();
    }

    REALLY_INLINE void GC::SignalAllocWork(size_t size)
    {
        if (policy.signalAllocWork(size))
            CollectionWork();
    }

    REALLY_INLINE void GC::SignalFreeWork(size_t size)
    {
        policy.signalFreeWork(size);
    }

    REALLY_INLINE void *GC::PleaseAlloc(size_t size, int flags)
    {
        return Alloc(size, flags | kCanFail);
    }

    // Normally extra will not be zero (overloaded 'new' operators take care of that)
    // so the overflow check is not actually redundant.

    REALLY_INLINE void *GC::AllocExtra(size_t size, size_t extra, int flags)
    {
        return Alloc(GCHeap::CheckForAllocSizeOverflow(size, extra), flags);
    }

    REALLY_INLINE void *GC::Calloc(size_t count, size_t elsize, int flags)
    {
        return Alloc(GCHeap::CheckForCallocSizeOverflow(count, elsize), flags);
    }

#if defined _DEBUG || defined MMGC_MEMORY_PROFILER
    #define SIZEARG size ,
#else
    #define SIZEARG
#endif

    // See comments around GC::Alloc that explain why the guard and table lookup for the
    // small-allocator cases are correct.

    REALLY_INLINE void *GC::AllocPtrZero(size_t size)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if (size <= kLargestAlloc)
            return GetUserPointer(containsPointersNonfinalizedAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero));
#endif
        return Alloc(size, GC::kContainsPointers|GC::kZero);
    }

    REALLY_INLINE void *GC::AllocPtrZeroExact(size_t size)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if (size <= kLargestAlloc)
            return GetUserPointer(containsPointersNonfinalizedAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kInternalExact));
#endif
        return Alloc(size, GC::kContainsPointers|GC::kZero|GC::kInternalExact);
    }
    
    REALLY_INLINE void *GC::AllocPtrZeroFinalized(size_t size)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if (size <= kLargestAlloc)
            return GetUserPointer(containsPointersFinalizedAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kFinalize));
#endif
        return Alloc(size, GC::kContainsPointers|GC::kZero|GC::kFinalize);
    }

    REALLY_INLINE void *GC::AllocPtrZeroFinalizedExact(size_t size)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if (size <= kLargestAlloc)
            return GetUserPointer(containsPointersFinalizedAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kFinalize|GC::kInternalExact));
#endif
        return Alloc(size, GC::kContainsPointers|GC::kZero|GC::kFinalize|GC::kInternalExact);
    }
    
    REALLY_INLINE void *GC::AllocRCObject(size_t size)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if (size <= kLargestAlloc)
            return GetUserPointer(containsPointersRCAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize));
#endif
        return Alloc(size, GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize);
    }

    REALLY_INLINE void *GC::AllocRCObjectExact(size_t size)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if (size <= kLargestAlloc)
            return GetUserPointer(containsPointersRCAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize|GC::kInternalExact));
#endif
        return Alloc(size, GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize|GC::kInternalExact);
    }
    
    REALLY_INLINE void* GC::AllocDouble()
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER && !defined MMGC_MEMORY_PROFILER
        return GetUserPointer(noPointersNonfinalizedAllocs[0]->Alloc(/*flags*/0));
#else
        return Alloc(8,0);
#endif
    }

    REALLY_INLINE void* GC::AllocBibop(GCAlloc* bibopAlloc)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER && !defined MMGC_MEMORY_PROFILER
        return GetUserPointer(bibopAlloc->Alloc(/*flags*/0));
#else
        return AllocSlow(bibopAlloc);
#endif
    }

    template<>
    REALLY_INLINE void* GC::AllocBibopType<avmplus::AtomConstants::kBibopFloatType>()
    {
        return AllocBibop(bibopAllocFloat);
    }

    template<>
    REALLY_INLINE void* GC::AllocBibopType<avmplus::AtomConstants::kBibopFloat4Type>()
    {
        void* p = AllocBibop(bibopAllocFloat4);
#if defined VMCFG_FLOAT && (defined DEBUG || 0) /* Change 0 to 1 to enable the test in release builds */
        if (uintptr_t(p) & 15)
            GCHeap::GetGCHeap()->Abort();
#endif
        return p;
    }

    // For AllocExtra the trick is that we can compute (size|extra) quickly without risk of overflow
    // and compare it to half the maximum small-alloc size (rounded down to 8 bytes), and if the guard
    // passes then we can definitely take the quick path.  Most allocations are small.
    //
    // As 'extra' won't usually be known at compile time the fallback case won't usually compile away,
    // though, so we risk bloating the code slightly here.

    REALLY_INLINE void *GC::AllocExtraPtrZero(size_t size, size_t extra)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if ((size|extra) <= (kLargestAlloc/2 & ~7)) {
            size += extra;
            return GetUserPointer(containsPointersNonfinalizedAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero));
        }
#endif
        return OutOfLineAllocExtra(size, extra, GC::kContainsPointers|GC::kZero);
    }

    REALLY_INLINE void *GC::AllocExtraPtrZeroExact(size_t size, size_t extra)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if ((size|extra) <= (kLargestAlloc/2 & ~7)) {
            size += extra;
            return GetUserPointer(containsPointersNonfinalizedAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kInternalExact));
        }
#endif
        return OutOfLineAllocExtra(size, extra, GC::kContainsPointers|GC::kZero|GC::kInternalExact);
    }
    
    REALLY_INLINE void *GC::AllocExtraPtrZeroFinalized(size_t size, size_t extra)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if ((size|extra) <= (kLargestAlloc/2 & ~7)) {
            size += extra;
            return GetUserPointer(containsPointersFinalizedAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kFinalize));
        }
#endif
        return OutOfLineAllocExtra(size, extra, GC::kContainsPointers|GC::kZero|GC::kFinalize);
    }

    REALLY_INLINE void *GC::AllocExtraPtrZeroFinalizedExact(size_t size, size_t extra)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if ((size|extra) <= (kLargestAlloc/2 & ~7)) {
            size += extra;
            return GetUserPointer(containsPointersFinalizedAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kFinalize|GC::kInternalExact));
        }
#endif
        return OutOfLineAllocExtra(size, extra, GC::kContainsPointers|GC::kZero|GC::kFinalize|GC::kInternalExact);
    }
    
    REALLY_INLINE void *GC::AllocExtraRCObject(size_t size, size_t extra)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if ((size|extra) <= kLargestAlloc/2) {
            size += extra;
            return GetUserPointer(containsPointersRCAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize));
        }
#endif
        return OutOfLineAllocExtra(size, extra, GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize);
    }

    REALLY_INLINE void *GC::AllocExtraRCObjectExact(size_t size, size_t extra)
    {
#if !defined _DEBUG && !defined AVMPLUS_SAMPLER
        if ((size|extra) <= kLargestAlloc/2) {
            size += extra;
            return GetUserPointer(containsPointersRCAllocs[sizeClassIndex[(size-1)>>3]]->Alloc(SIZEARG GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize|GC::kInternalExact));
        }
#endif
        return OutOfLineAllocExtra(size, extra, GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize|GC::kInternalExact);
    }
    
#undef SIZEARG

    REALLY_INLINE void GC::Free(const void *item)
    {
        if(item == NULL)
            return;
        FreeNotNull(item);
    }

    REALLY_INLINE void GC::FreeNotNull(const void *item)
    {
        GCAssert(item != NULL);
        GCAssertMsg(onThread(), "GC called from a different thread or not associated with a thread, missing MMGC_GCENTER macro perhaps.");
        //GCAssertMsg(!IsFinalized(item), "Finalizable storage must not be deleted with GC::Free; the destructor will not be called.");
        GetBlockHeader(item)->alloc->Free(GetRealPointer(item));
    }

    REALLY_INLINE void GC::FreeFromGCNotNull(const void *item)
    {
        GCAssert(item != NULL);
        GCAssertMsg(onThread(), "GC called from a different thread or not associated with a thread, missing MMGC_GCENTER macro perhaps.");
        GetBlockHeader(item)->alloc->Free(GetRealPointer(item));
    }
    
    REALLY_INLINE void GC::FreeFromDelete(const void *item)
    {
        if(item) {
            GCAssertMsg(onThread(), "GC called from a different thread or not associated with a thread, missing MMGC_GCENTER macro perhaps.");
#ifdef MMGC_DELETION_PROFILER
            ProfileExplicitDeletion(item);
#endif
            GetBlockHeader(item)->alloc->Free(GetRealPointer(item));
        }
    }
    
    REALLY_INLINE void GC::AddRCRootSegment(RCRootSegment *segment)
    {
        segment->next = rcRootSegments;
        if (rcRootSegments)
            rcRootSegments->prev = segment;
        rcRootSegments = segment;
    }

    REALLY_INLINE void GC::RemoveRCRootSegment(RCRootSegment *segment)
    {
        if (segment->next != NULL)
            segment->next->prev = segment->prev;
        if (segment->prev != NULL)
            segment->prev->next = segment->next;
        else
            rcRootSegments = segment->next;
    }

    /*static*/
    REALLY_INLINE size_t GC::Size(const void *ptr)
    {
        return GetBlockHeader(ptr)->size - DebugSize();
    }

    /*static*/
    REALLY_INLINE GC* GC::GetGC(const void *item)
    {
        GC *gc = GetBlockHeader(item)->gc;
        // we don't want to rely on the gcheap thread local but it makes a good
        // sanity check against misuse of this function
//      GCAssert(gc == GCHeap::GetGCHeap()->GetActiveGC());
        return gc;
    }

#ifdef MMGC_FASTBITS
    /*static*/
    REALLY_INLINE gcbits_t& GC::GetGCBits(const void *realptr)
    {
        GCBlockHeader* block = GetBlockHeader(realptr);
        return block->bits[(uintptr_t(realptr)& 0xFFF) >> block->bitsShift];
    }
#else
    /*static*/
    REALLY_INLINE gcbits_t& GC::GetGCBits(const void *realptr)
    {
        if (GCLargeAlloc::IsLargeBlock(realptr))
            return GCLargeAlloc::GetGCBits(realptr);
        else
            return GCAlloc::GetGCBits(realptr);
    }
#endif

    /*static*/
    REALLY_INLINE bool GC::ContainsPointers(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(userptr)->IsPointerToGCObject(realptr));
        return GetBlockHeader(realptr)->containsPointers != 0;
    }
    
    /*static*/
    REALLY_INLINE bool GC::IsRCObject(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(userptr)->IsPointerToGCObject(realptr));
        return GetBlockHeader(realptr)->rcobject != 0;
    }
    
    /*static*/
    REALLY_INLINE int GC::GetMark(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(realptr)->IsPointerToGCObject(realptr));
        return GetGCBits(realptr) & kMark;
    }

    /*static*/
    REALLY_INLINE void GC::SetMark(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(realptr)->IsPointerToGCObject(realptr));
        gcbits_t& bits = GetGCBits(realptr);
        bits = (bits | kMark) & ~kQueued;
    }

    REALLY_INLINE bool GC::TraceObjectGuard(const void* userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(realptr)->IsPointerToGCObject(realptr));
        gcbits_t& bits = GetGCBits(realptr);
        if ((bits & (kMark|kQueued)) == 0) {
            bits |= kMark;
            return true;
        }
        return false;
    }
    
    REALLY_INLINE int GC::GetQueued(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(realptr)->IsPointerToGCObject(realptr));
        return GetGCBits(realptr) & kQueued;
    }

    REALLY_INLINE void GC::ClearQueued(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(IsPointerToGCObject(realptr));
        GetGCBits(realptr) &= ~kQueued;
    }

    /*static*/
    REALLY_INLINE void GC::ClearFinalized(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(realptr)->IsPointerToGCObject(realptr));
        GetGCBits(realptr) &= ~kFinalizable;
    }

    /*static*/
    REALLY_INLINE int GC::IsFinalized(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(realptr)->IsPointerToGCObject(realptr));
        return GetGCBits(realptr) & kFinalizable;
    }

    /*static*/
    REALLY_INLINE int GC::IsExactlyTraced(const void *userptr)
    {
#if defined VMCFG_EXACT_TRACING || defined VMCFG_SELECTABLE_EXACT_TRACING
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(realptr)->IsPointerToGCObject(realptr));
        return GetGCBits(realptr) & kVirtualGCTrace;
#else
        (void)userptr;
        return 0;
#endif
    }
    
    /*static*/
    REALLY_INLINE int GC::HasWeakRef(const void *userptr)
    {
        const void *realptr = GetRealPointer(userptr);
        GCAssert(GetGC(realptr)->IsPointerToGCObject(realptr));
        return GetGCBits(realptr) & kHasWeakRef;
    }

    REALLY_INLINE GCHeap *GC::GetGCHeap() const
    {
        return heap;
    }

    REALLY_INLINE void GC::ReapZCT(bool scanStack)
    {
        zct.Reap(scanStack);
    }

    REALLY_INLINE bool GC::Reaping()
    {
        return zct.IsReaping();
    }

    REALLY_INLINE bool GC::IncrementalMarking()
    {
        return marking;
    }

    REALLY_INLINE bool GC::BarrierActive()
    {
        // Note, this will not always have the same value as IncrementalMarking
        // in the future.
        return marking;
    }
    
    REALLY_INLINE bool GC::Collecting()
    {
        return collecting;
    }

    REALLY_INLINE bool GC::Presweeping()
    {
        return presweeping;
    }

    REALLY_INLINE void *GC::FindBeginning(const void *gcItem)
    {
        return FindBeginningGuarded(gcItem);
    }

    REALLY_INLINE void *GC::FindBeginningFast(const void *gcItem)
    {
        PageMap::PageType bits = GetPageMapValue((uintptr_t)gcItem);
        if (bits == PageMap::kGCAllocPage)
            return GetUserPointer(GCAlloc::FindBeginning(gcItem));
        while (bits == PageMap::kGCLargeAllocPageRest)
        {
            gcItem = (void*) ((uintptr_t)gcItem - GCHeap::kBlockSize);
            bits = GetPageMapValue((uintptr_t)gcItem);
        }
        return GetUserPointer(GCLargeAlloc::FindBeginning(gcItem));
    }

    REALLY_INLINE bool GC::IsPointerToGCPage(const void *item)
    {
        return GetPageMapValueGuarded((uintptr_t)item) != 0;
    }

    REALLY_INLINE bool GC::IsPointerToGCObject(const void *realPtr)
    {
        return GetRealPointer(FindBeginningGuarded(realPtr, true)) == realPtr;
    }

    /*static*/
    REALLY_INLINE double GC::duration(uint64_t start)
    {
        return (double(VMPI_getPerformanceCounter() - start) * 1000) / VMPI_getPerformanceFrequency();
    }

    /*static*/
    REALLY_INLINE uint64_t GC::ticksToMicros(uint64_t ticks)
    {
        return (ticks*1000000)/VMPI_getPerformanceFrequency();
    }

    /*static*/
    REALLY_INLINE uint64_t GC::ticksToMillis(uint64_t ticks)
    {
        return (ticks*1000)/VMPI_getPerformanceFrequency();
    }

    REALLY_INLINE uint64_t GC::bytesMarked()
    {
        return policy.bytesMarked();
    }

    REALLY_INLINE uint64_t GC::markTicks()
    {
        return policy.timeStartIncrementalMark + policy.timeIncrementalMark;
    }

    REALLY_INLINE uint32_t GC::markIncrements()
    {
        return (uint32_t)policy.countIncrementalMark;
    }

    REALLY_INLINE bool GC::Destroying()
    {
        return destroying;
    }

    REALLY_INLINE uintptr_t GC::GetStackTop() const
    {

        // Bugzilla 754281: stackEnter is going to be assumed untrustworthy
        // until we can to prove otherwise, i.e. by switching to a better
        // protocol or by verifying on target-by-target basis.
        // Note: It is entirely possible that we would first remove the need
        // for stackEnter entirely.

#ifdef MMGC_HAS_TRUSTWORTHY_GET_STACK_ENTER
        // temporary crutch until we're moved over to the MMGC_GCENTER system
        if(stackEnter == NULL)
            return AVMPI_getThreadStackBase();
        return GetStackEnter();
#else
        return AVMPI_getThreadStackBase();
#endif // MMGC_HAS_TRUSTWORTHY_GET_STACK_ENTER
    }

    REALLY_INLINE uintptr_t GC::GetStackEnter() const
    {
        return (uintptr_t)stackEnter;
    }

    REALLY_INLINE GCAutoEnter *GC::GetAutoEnter()
    {
        return stackEnter;
    }

    REALLY_INLINE bool GC::onThread()
    {
        return VMPI_currentThread() == m_gcThread;
    }

    REALLY_INLINE void GC::FreeBits(uint32_t *bits, int sizeClass)
    {
#ifdef _DEBUG
        for(int i=0, n=noPointersNonfinalizedAllocs[sizeClass]->m_numBitmapBytes; i<n;i++)
            GCAssert(((uint8_t*)bits)[i] == 0);
#endif
        *(uint32_t**)bits = m_bitsFreelists[sizeClass];
        m_bitsFreelists[sizeClass] = bits;
    }

    REALLY_INLINE bool GC::IsMarkedThenMakeQueued(const void* userptr)
    {
        const void* realptr = GetRealPointer(userptr);
        gcbits_t& bits = GetGCBits(realptr);
        if (bits & kMark) {
            bits ^= (kMark|kQueued);
            return true;
        }
        return false;
    }

    REALLY_INLINE bool GC::IsQueued(const void* userptr)
    {
        const void* realptr = GetRealPointer(userptr);
        return (GetGCBits(realptr) & kQueued) != 0;
    }

    template<class T>
    REALLY_INLINE void GC::TraceLocation(T* const * loc)
    {
        TracePointer((void*)*loc HEAP_GRAPH_ARG((uintptr_t*)loc));
    }

    REALLY_INLINE void GC::TraceLocation(uintptr_t* loc)
    {
        TracePointer((void*)(*loc & ~7) HEAP_GRAPH_ARG(loc));
    }

    REALLY_INLINE void GC::TraceAtom(avmplus::Atom* loc)
    {
        TraceAtomValue(*loc HEAP_GRAPH_ARG(loc));
    }

    template <class T>
    REALLY_INLINE void GC::TraceLocation(MMgc::WriteBarrier<T> const * loc)
    {
        TracePointer((void*)loc->value() HEAP_GRAPH_ARG((uintptr_t*)loc->location()));
    }

    template <>
    REALLY_INLINE void GC::TraceLocation(MMgc::WriteBarrier<uintptr_t> const * loc)
    {
        TracePointer((void*)(loc->value() & ~7) HEAP_GRAPH_ARG(loc->location()));
    }
    
    template <class T>
    REALLY_INLINE void GC::TraceLocation(MMgc::WriteBarrierRC<T> const * loc)
    {
        TracePointer((void*)loc->value() HEAP_GRAPH_ARG((uintptr_t*)loc->location()));
    }

    template <>
    REALLY_INLINE void GC::TraceLocation(MMgc::WriteBarrierRC<uintptr_t> const * loc)
    {
        TracePointer((void*)(loc->value() & ~7) HEAP_GRAPH_ARG(loc->location()));
    }

    template <class T>
    REALLY_INLINE void GC::TraceLocation(MMgc::GCMemberBase<T> const * loc)
    {
        TracePointer((void*)loc->value() HEAP_GRAPH_ARG((uintptr_t*)loc->location()));
    }
    
    REALLY_INLINE void GC::TraceAtom(AtomWBCore* loc)
    {
        TraceAtomValue(loc->value() HEAP_GRAPH_ARG(loc->location()));
    }
   
    template<class T>
    REALLY_INLINE void GC::TraceLocations(MMgc::GCMemberBase<T>* p, size_t numobjects)
    {
        for ( size_t i=0 ; i < numobjects ; i++ )
            TracePointer((void*)(p+i)->value() HEAP_GRAPH_ARG((uintptr_t*)(p+i)->location()));
    }
 
    template<class T>
    REALLY_INLINE void GC::TraceLocations(T** p, size_t numobjects)
    {
        for ( size_t i=0 ; i < numobjects ; i++ )
            TracePointer(p[i] HEAP_GRAPH_ARG((uintptr_t*)(p+i)));
    }

    REALLY_INLINE void GC::TraceLocations(uintptr_t* p, size_t numobjects)
    {
        for ( size_t i=0 ; i < numobjects ; i++ )
            TracePointer((void*)(p[i] & ~7) HEAP_GRAPH_ARG(p+i));
    }
    
    REALLY_INLINE void GC::TraceAtoms(avmplus::Atom* p, size_t numobjects)
    {
        for ( size_t i=0 ; i < numobjects ; i++ )
            TraceAtomValue(p[i] HEAP_GRAPH_ARG(p+i));
    }

    REALLY_INLINE void GC::TraceConservativeLocation(uintptr_t* loc)
    {
        TraceConservativePointer(*loc, false HEAP_GRAPH_ARG(loc));
    }

    REALLY_INLINE PageMap::PageType GC::GetPageMapValue(uintptr_t addr) const
    {
        GCAssert(pageMap.AddrIsMappable(addr));
        return pageMap.AddrToVal(addr);
    }

    REALLY_INLINE PageMap::PageType GC::GetPageMapValueGuarded(uintptr_t addr)
    {
        if (pageMap.AddrIsMappable(addr))
            return GetPageMapValue(addr);
        MMGC_STATIC_ASSERT(PageMap::kNonGC == 0);
        return PageMap::kNonGC;
    }

    REALLY_INLINE void GC::AddToSmallEmptyBlockList(GCAlloc::GCBlock *b)
    {
        b->next = smallEmptyPageList;
        smallEmptyPageList = b;
    }

    REALLY_INLINE void GC::AddToLargeEmptyBlockList(GCLargeAlloc::LargeBlock *lb)
    {
        lb->next = largeEmptyPageList;
        largeEmptyPageList = lb;
    }

    REALLY_INLINE void GC::DoPreSweepCallbacks()
    {
        for ( GCCallback *cb = m_callbacks; cb ; cb = cb->nextCB ) {
            cb->presweep();
        }
    }

    REALLY_INLINE void GC::DoPostSweepCallbacks()
    {
        for ( GCCallback *cb = m_callbacks; cb ; cb = cb->nextCB ) {
            cb->postsweep();
        }
    }

    REALLY_INLINE void GC::DoPreReapCallbacks()
    {
        for ( GCCallback *cb = m_callbacks; cb ; cb = cb->nextCB ) {
            cb->prereap();
        }
    }

    REALLY_INLINE void GC::DoPreReapCallbacks(void* rcObject)
    {
        for ( GCCallback *cb = m_callbacks; cb ; cb = cb->nextCB ) {
            cb->prereap(rcObject);
        }
    }

    REALLY_INLINE void GC::DoPostReapCallbacks()
    {
        for ( GCCallback *cb = m_callbacks; cb ; cb = cb->nextCB ) {
            cb->postreap();
        }
    }

#ifdef MMGC_REFCOUNT_PROFILING
    REALLY_INLINE void GC::AddToZCT(RCObject *obj, bool initial)
    {
        zct.Add(obj, initial);
    }

    REALLY_INLINE void GC::RemoveFromZCT(RCObject *obj, bool final)
    {
        zct.Remove(obj, final);
    }
#else
    REALLY_INLINE void GC::AddToZCT(RCObject *obj)
    {
        zct.Add(obj);
    }

    REALLY_INLINE void GC::RemoveFromZCT(RCObject *obj)
    {
        zct.Remove(obj);
    }
#endif

    REALLY_INLINE void GC::PreventImmediateReaping(RCObject* obj)
    {
        if (obj->InZCT())
            zct.Remove(obj);
    }

    /*static*/
    REALLY_INLINE const void *GC::Pointer(const void *p)
    {
        return (const void*)(((uintptr_t)p)&~7);
    }

    REALLY_INLINE size_t GC::GetNumBlocks()
    {
        return policy.blocksOwnedByGC();
    }

    REALLY_INLINE void* GC::allocaTop()
    {
        return stacktop;
    }

    REALLY_INLINE void GC::allocaPopTo(void* top)
    {
        if (top >= top_segment->start && top <= top_segment->limit)
            stacktop = top;
        else
            allocaPopToSlow(top);
    }

#ifdef DEBUGGER
    REALLY_INLINE void* GC::GetAttachedSampler()
    {
        return m_sampler;
    }

    REALLY_INLINE void GC::SetAttachedSampler(void *sampler)
    {
        m_sampler = sampler;
    }
#endif

#ifdef VMCFG_TELEMETRY
    REALLY_INLINE size_t GC::getDependentMemory(DependentMemoryType memType)
    {
        GCAssert(memType < typeCount);
        if(memType < typeCount)
            return m_dependentMemory[memType];
        else
            return 0;
    }

    REALLY_INLINE void GC::UpdateDependentAllocation(uint32_t bytes, DependentMemoryType memType)
    {
        GCAssert(memType < typeCount);
        if(memType < typeCount)
            m_dependentMemory[memType] += bytes;
    }
    
    REALLY_INLINE void GC::UpdateDependentDeallocation(uint32_t bytes, DependentMemoryType memType)
    {
        GCAssert(memType < typeCount);
        if(memType < typeCount)
            m_dependentMemory[memType] -= bytes;
    }
    
    REALLY_INLINE telemetry::ITelemetry* GC::getTelemetry()
    {
        return m_telemetry;
    }

    REALLY_INLINE void GC::setTelemetry(telemetry::ITelemetry* telemetry)
    {
        m_telemetry = telemetry;
    }
#endif

    REALLY_INLINE GC::AllocaAutoPtr::AllocaAutoPtr()
        : gc(NULL)
        , unwindPtr(NULL)
    {
    }

    REALLY_INLINE GC::AllocaAutoPtr::~AllocaAutoPtr()
    {
        if (unwindPtr)
            gc->allocaPopTo(unwindPtr);
    }

    REALLY_INLINE Cleaner::Cleaner(): v(0), size(0)
    {
    }

    REALLY_INLINE Cleaner::~Cleaner()
    {
        if(v)
            VMPI_memset(v, 0, size);
        v = 0;
        size = 0;
    }

    // Disable copying
    REALLY_INLINE Cleaner& Cleaner::operator=(const Cleaner& /*rhs*/)
    {
        return *this;
    }

    REALLY_INLINE void Cleaner::set(const void * _v, size_t _size)
    {
        this->v = (int*)_v;
        this->size = _size;
    }

    REALLY_INLINE bool GCAutoEnter::Entered()
    {
        return m_gc != NULL;
    }
}

#endif /* __GC_inlines__ */

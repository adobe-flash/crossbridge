/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"
#include "StaticAssert.h"

#ifdef AVMPLUS_SAMPLER
 //sampling support
#include "avmplus.h"
#else
#define SAMPLE_FRAME(_x, _s)
#define SAMPLE_CHECK()
#endif

#include "ITelemetry.h"

//#define ZCT_TESTING                   // Test the handling of a failure to extend the ZCT

namespace MMgc
{
    /* The ZCT is implemented as a two-level table.  Given a ZCT index, the
     * first-level table (indexed by the high bits of the index) yields a
     * second-level table (indexed by the low bits) that contains a pointer to the
     * entry.  The entry is an RCObject*.  The RCObject has a header field,
     * ZCT_INDEX, which is the index at which the pointer is found, and a flag
     * stating whether that index is valid (ie whether the object is in the ZCT).
     *
     * The ZCT_INDEX field is 20 bits wide; thus it may be possible that some
     * objects cannot be entered into the ZCT.  This is OK, as the garbage
     * collector will reclaim any unreachable objects eventually anyway.  (Some
     * test programs in the acceptance tests actually run into this problem.)
     *
     * RCObjects whose reference counts are zero are added to the ZCT by being
     * passed to ZCT::Add(); they are removed by being passed to ZCT::Remove() if
     * their reference counts transition from 0 to 1, and when the object
     * is destroyed.  This is all taken care of in RCObject's constructor,
     * destructor, and reference counting operations.  See GCObject.h.
     *
     * Every RCObject is added to the ZCT on creation; 10%-25% are subsequently
     * removed as their reference counts transition from 0 to 1.  Very few are
     * removed as objects are destroyed; the bulk are removed by the reaper because
     * the objects are not pinned.  (Based on profiling some Flash apps, July 2009.)
     *
     * ZCT::Add() has a fast path that can be in-lined: it checks a pointer against
     * a limit, stores the object in the table, bumps the pointer, stores the
     * index in the object and sets the object's ZCT flag.
     *
     * ZCT::Remove() only has a fast path: it clears the table entry and clears the
     * ZCT flag in the object.  NULL entries in the table are recovered during
     * reaping.
     *
     *
     * Useful invariants:
     *
     * - gc->collecting and zct.reaping are never both true at the same time.  This
     *   is ensured by GC::FinishIncrementalMark returning immediately if zct.reaping
     *   is true, and by ZCT::Reap returning immediately if gc->collecting is true.
     *
     * - There are never any free blocks beyond the 'current' block (the one pointed
     *   into by top or slowTop) in the ZCT.  During reaping, once the ZCT is popped
     *   below a block the block is removed from the ZCT and added to an empty blocks
     *   pool.
     *
     * - The ZCT will honor calls to Pin() from prereap() but not necessarily any
     *   calls to Pin() earlier than that.  When an object is added to the ZCT its
     *   pinned flag is cleared.  (This is consistent with the old ZCT code.)
     */

#ifdef ZCT_TESTING
    // Max number less 1 of blocks the ZCT may use for the second level of the block table
    // as well as the pinned table during reaping.
    static uint32_t zct_allowance = 0;
#endif

    ZCT::ZCT()
        : gc(NULL)
        , blocktable(NULL)
        , blocktop(NULL)
        , reaping(false)
        , budget(0)
        , bottom(NULL)
        , top(NULL)
        , limit(NULL)
        , topIndex(0)
        , slowState(false)
        , slowBottom(NULL)
        , slowTop(NULL)
        , slowLimit(NULL)
        , slowTopIndex(0)
        , pinTop(NULL)
        , pinLimit(NULL)
        , pinIndex(0)
        , pinList(0)
        , pinLast(0)
        , freeList(0)
    {
    }

    void ZCT::SetGC(GC *gc)
    {
        this->gc = gc;

        // The size of the block table is limited by the field in the RCObject header
        // that accomodates the ZCT index.  This field is currently 20 bits, so
        // the max number of entries in the ZCT is 1M.  On a 64-bit system each block
        // holds 512 elements so the block table needs 2K entries, occupying
        // four blocks.  On a 32-bit system each block holds 1K elements so the block
        // table needs 1K entries, occupying a single block.  Instead of messing with
        // growing the block table later, just allocate full tables here.  The
        // pointed-to blocks are still allocated on demand.

        // The value of ZCT_CAPACITY is derived from on a block size of 4096; this
        // assert keeps us honest.  Talk to Lars if you get into trouble here.
        GCAssert(GCHeap::kBlockSize == 4096);

        // This invariant is stronger than we need; we only need for the ZCT capacity
        // to divide evenly into blocks on both 32-bit and 64-bit systems.
        GCAssert(RCObject::ZCT_CAPACITY == 0x100000U);

        const uint32_t numblocks = RCObject::ZCT_CAPACITY / CAPACITY(RCObject**) / CAPACITY(RCObject***);

        blocktable = (RCObject***) GCHeap::GetGCHeap()->Alloc(numblocks);   // must succeed, so use default flags
        for ( uint32_t i=0 ; i < CAPACITY(RCObject**)*numblocks ; i++ )
            blocktable[i] = NULL;
        blocktable[0] = (RCObject**) GCHeap::GetGCHeap()->Alloc(1);         // must succeed, so use default flags
        blocktop = blocktable + 1;

        budget = 0;
        bottom = blocktable[0];
        top = blocktable[0];
        limit = blocktable[0] + CAPACITY(RCObject*);
        topIndex = 0;

        // if disable force slow path where we check this
        if(!gc->drcEnabled)
        {
            limit = top;
#ifdef DEBUG
            GCLog("*** DRC has been disabled for GC=%p ***\n", (void*)gc);
#endif
        }
    }

    void ZCT::Destroy()
    {
        ClearBlockTable();
        ClearFreeList();
        GCHeap::GetGCHeap()->Free(blocktable);
    }

    void ZCT::StartCollecting()
    {
        GCAssert(!slowState);

        // Transfer state to slow-path variables
        slowState = true;
        slowBottom = bottom;
        slowTop = top;
        slowLimit = limit;
        slowTopIndex = topIndex;

        // Create a state that triggers the slow path
        top = limit;
    }

    void ZCT::EndCollecting()
    {
        GCAssert(slowState);

        // Transfer the state from the slow-path variables
        bottom = slowBottom;
        top = slowTop;
        limit = slowLimit;
        topIndex = slowTopIndex;
        slowState = false;
    }

    // The problem here is when a prereap(), prereap(obj), or postreap()
    // call gets into a situation where a longjmp is made across the GC,
    // or if the GC aborts while slowState is true (because this leaves us
    // with broken invariants when the heap is later swept).

    void ZCT::SignalImminentAbort()
    {
        // It's not necessary to unpin objects; pinned garbage will be
        // reclaimed by the garbage collector eventually.

        // No particular reason to clear the ZCT, the objects in it are
        // valid.

        if (slowState) {
            EndCollecting();
            ClearPinningMemory();
        }

        if (reaping)
            reaping = false;
    }

    void ZCT::AddSlow(RCObject *obj)
    {
        GCAssert(top == limit);
        GCAssert(gc->collecting + reaping < 2);

        if(!gc->drcEnabled)
            return;

        if(gc->collecting)
        {
            // This is a vestige from FP8 to fix bug 165100, it has the effect of delaying
            // the deletion of some objects; this causes the site to work.
            if(gc->dontAddToZCTDuringCollection)
                return;

            // Unmarked objects are gonna get swept anyways.
            if(!GC::GetMark(obj))
                return;
        }

        if (slowState && slowTop < slowLimit) {
            *slowTop++ = obj;
            obj->setZCTIndexAndMaybeUnpin(slowTopIndex++, KeepPinned());
            return;
        }

        // Overflow.
        // Expand or reap?  Sometimes we must grow even if the budget has been exhausted.

        bool shouldGrow = false;
        if (reaping)
            shouldGrow = true;
        else if (budget > 0 && CanGrow())
            shouldGrow = true;
        else {
            // 'obj' will not be reaped as it's on the stack; we'll add it to the ZCT below.
            Reap();
            uint32_t avail = AvailableInCurrentSegment();
            budget = gc->policy.queryZCTBudget(uint32_t(blocktop - blocktable));
            if (avail == 0)
                shouldGrow = true;
        }

        if (shouldGrow) {
            GCAssert(AvailableInCurrentSegment() == 0);
            if (!CanGrow() || !Grow())
                return;     // c'est la vie
            if (budget > 0)
                budget--;
            // Grow() does not set up the state for Add(), so do that.
            if (slowState) {
                slowBottom = blocktop[-1];
                slowTop = slowBottom;
                slowLimit = slowBottom + CAPACITY(RCObject*);
                GCAssert(slowTopIndex % CAPACITY(RCObject*) == 0);
            }
            else {
                bottom = blocktop[-1];
                limit = bottom + CAPACITY(RCObject*);
                top = bottom;
                GCAssert(topIndex % CAPACITY(RCObject*) == 0);
            }
        }

        GCAssert(AvailableInCurrentSegment() > 0);

        Add(obj);           // won't fail
    }

    uint32_t ZCT::AvailableInCurrentSegment()
    {
        return slowState ? uint32_t(slowLimit - slowTop) : uint32_t(limit - top);
    }

    bool ZCT::CanGrow()
    {
        return (slowState ? slowTopIndex : topIndex) + CAPACITY(RCObject*) <= RCObject::ZCT_CAPACITY;
    }

    void ZCT::Reap(bool scanStack)
    {
        if(gc->collecting)
            return;

        GCAssert(!slowState);

        // Do not reap if already reaping or if the ZCT is empty (waste of time).
        if (reaping || topIndex == 0)
            return;

#ifdef _DEBUG
        if (gc->validateDefRef)
            gc->DRCValidationTrace(scanStack);
#endif

        reaping = true;
        TELEMETRY_METHOD(gc->getTelemetry(), ".gc.Reap");
        gc->policy.signal(GCPolicyManager::START_ReapZCT);
        SAMPLE_FRAME("[reap]", gc->core());

        uint64_t start = VMPI_getPerformanceCounter();
#ifdef MMGC_POLICY_PROFILING
        uint32_t objects_pinned = 0;
#endif
        uint32_t objects_reaped = 0;
        size_t bytes_reaped = 0;
        size_t blocks_before = gc->GetNumBlocks();

        // Note that we must pin from root segments even if scanStack is false, because the
        // MMGC_GC_ROOT_THREAD creates one AutoRCRootSegment that is not managed by avmStackAlloc.
        // The root segment list should be very short if scanStack==false so performance-wise
        // this is not a big deal.
        //
        // It is not necessary to pin from the mark and barrier stacks because there is a
        // test in GC::Free that prevents queued objects from being deleted; we have to pay
        // for that check in any case and can depend on it here.
        //
        // For some generally difficult problems around pinning see bugzilla #506644.

        if (scanStack) {
#ifdef DEBUG
            // During DRC validation stack scanning happened already.
            // See GC::DRCValidationTrace().
            if(!gc->validateDefRef)
#endif
                VMPI_callWithRegistersSaved(ZCT::DoPinProgramStack, this);
        }
        PinRootSegments();

        // Invoke prereap on all callbacks
        gc->DoPreReapCallbacks();

        // We perform depth-first reaping using the ZCT as a stack.
        //
        // Popping an element off the end of the ZCT, it is either NULL, pinned, or unpinned.
        //  - If it's NULL it's ignored.
        //  - If it's pinned, it's shifted into a list of new blocks that will replace
        //    the blocks in the ZCT.  The index of the object is updated.
        //  - If it's not pinned, it's reaped (which runs its finalizer, which may add
        //    more elements ot the end of the ZCT).
        //
        // Depth-first processing is desirable because object graphs will tend to be wider
        // than they are deep; going depth-first reduces ZCT growth during reaping.
        //
        // Memory use is optimal to within a constant: space occupied by a pointer to a
        // reaped object is released immediately, and empty segments popped off the ZCT
        // are used for the list of replacement blocks.

        SetupPinningMemory();
        for (;;) {
            SAMPLE_CHECK();

            // Pop an element off the ZCT
            GCAssert(bottom <= top);
            GCAssert(top <= limit);

            if (top == bottom) {
                if (topIndex == 0)
                    break;
                PopFastSegment();
            }
            RCObject *rcobj = *--top;
            --topIndex;

            // Process the element
            if (rcobj == NULL)
                ;
            else if (rcobj->IsPinned()) {
#ifdef MMGC_POLICY_PROFILING
                objects_pinned++;
#endif
                PinObject(rcobj);
            }
            else {
                objects_reaped++;
                bytes_reaped += GC::Size(rcobj);
                ReapObject(rcobj);
            }
        }
        UsePinningMemory();

#ifdef DEBUG
        if(gc->validateDefRef)
            gc->AbortInProgressMarking();
#endif

        // Invoke postreap on all callbacks
        gc->DoPostReapCallbacks();

        if(gc->heap->Config().gcstats && objects_reaped > 0) {
            size_t blocks_after = gc->GetNumBlocks();
            gc->gclog("[mem] DRC reaped %u objects (%u kb) freeing %u pages (%u kb) in %.2f millis (%.4f s)\n",
                      objects_reaped,
                      unsigned(bytes_reaped/1024),
                      unsigned(blocks_before - blocks_after),
                      unsigned(blocks_after * GCHeap::kBlockSize / 1024),
                      GC::duration(start),
                      GC::duration(gc->t0)/1000);
        }

        reaping = false;

#ifdef _DEBUG
        for ( uint32_t i=0 ; i < topIndex ; i++ ) {
            // The first element of each block is usually NULL because it has
            // been used as a link for pinList.
            if (Get(i) != NULL) {
                GCAssert(Get(i)->getZCTIndex() == i);
                GCAssert(!Get(i)->IsPinned());
            }
        }
#endif

#ifdef MMGC_POLICY_PROFILING
        gc->policy.signalReapWork(objects_reaped, uint32_t(bytes_reaped), objects_pinned);
#endif
        gc->policy.signal(GCPolicyManager::END_ReapZCT);
    }

    void ZCT::PopFastSegment()
    {
        GCAssert(!slowState);
        GCAssert(blocktop-1 > blocktable);  // Can't pop the first segment
        blocktop--;
        FreeBlock(*blocktop);
        *blocktop = NULL;
        RCObject** block = blocktop[-1];
        bottom = block;
        top = block + CAPACITY(RCObject**);
        limit = top;
    }

    void ZCT::SetupPinningMemory()
    {
        GCAssert(pinList == NULL);
        GCAssert(pinLast == NULL);
        pinTop = NULL;
        pinLimit = NULL;
        pinIndex = 0;
    }

    bool ZCT::GrowPinningMemory()
    {
        GCAssert(pinTop == pinLimit);
        GCAssert(pinIndex % CAPACITY(RCObject*) == 0);

        RCObject** block = PleaseAllocBlock();
        if (block == NULL)
            return false;
        // Use the first element of the block as a 'next' pointer, we don't
        // want to use an auxiliary dynamic data structure that might fail
        // here.
        if (pinLast == NULL)
            pinList = block;
        else
            pinLast[0] = (RCObject*)block;
        pinLast = block;
        block[0] = NULL;
        pinTop = block + 1;
        pinIndex++;
        pinLimit = block + CAPACITY(RCObject*);
        return true;
    }

    // Transfer blocks from pinList into the ZCT, replacing the ZCT blocks.

    void ZCT::UsePinningMemory()
    {
        // ZCT must be empty when we do this
        GCAssert(!slowState);
        GCAssert(top == bottom);
        GCAssert(topIndex == 0);

        if (pinTop != NULL) {
            // Nuke the ZCT contents (there should only be one block in it)
            ClearBlockTable();
            GCAssert(blocktop == blocktable);
            GCAssert(*blocktop == NULL);

            // Copy block pointers into the ZCT (typically very few)
            while (pinList != NULL) {
                RCObject** block = pinList;
                pinList = (RCObject**)block[0];
                block[0] = NULL;
                *blocktop++ = block;
            }

            pinLast = NULL;

            bottom = blocktop[-1];
            top = pinTop;
            limit = pinLimit;
            topIndex = pinIndex;
        }
    }

    void ZCT::ClearPinningMemory()
    {
        while (pinList != NULL)
        {
            RCObject** block = pinList;
            pinList = (RCObject**)block[0];
            FreeBlock(block);
        }
        pinLast = NULL;
    }

    REALLY_INLINE void ZCT::PinObject(RCObject* obj)
    {
        if (pinTop == pinLimit) {
            if (!GrowPinningMemory()) {
                obj->ClearZCTFlag();
                return;
            }
        }
        *pinTop++ = obj;
        obj->setZCTIndexAndUnpin(pinIndex++);
    }

    REALLY_INLINE void ZCT::ReapObject(RCObject* obj)
    {
        obj->ClearZCTFlag();
#ifdef DEBUG
        if (gc->validateDefRef)
            gc->DefRefValidate(obj);
#endif
        // Invoke prereap on all callbacks.
        // FIXME: This is fairly wasteful and it would be good to be rid of it.
        gc->DoPreReapCallbacks(obj);

        GCAssert(*(intptr_t*)obj != 0);         // That's the vtable normally
        GCAssert(gc->IsFinalized(obj));
        ((GCFinalizedObject*)obj)->~GCFinalizedObject();
        gc->FreeFromGCNotNull(obj);

        GCAssert(gc->weakRefs.get(obj) == NULL);
    }

    /*static*/
    void ZCT::DoPinProgramStack(void* stackPointer, void* arg)
    {
        ZCT* zct = (ZCT*)arg;
        GC* gc = zct->gc;
        char* stackBase = (char*)gc->GetStackTop();
        zct->PinStackObjects(stackPointer, stackBase - (char*)stackPointer);
    }

    void ZCT::PinRootSegments()
    {
        GC::RCRootSegment* segment = gc->rcRootSegments;
        while(segment)
        {
            PinStackObjects(segment->mem, segment->size);
            segment = segment->next;
        }
    }

    void ZCT::PinStackObjects(const void *start, size_t len)
    {
        RCObject **p = (RCObject**)start;
        RCObject **end = p + len/sizeof(RCObject*);

        const void *_memStart = (const void*)gc->pageMap.MemStart();
        const void *_memEnd = (const void*)gc->pageMap.MemEnd();

        while(p < end) {
            const void *val = GC::Pointer(*p++);

#ifdef MMGC_VALGRIND
            if (end == (void*)gc->GetStackTop())
                VALGRIND_MAKE_MEM_DEFINED(&val, sizeof(val));
#endif // MMGC_VALGRIND

            if(val < _memStart || val >= _memEnd)
                continue;

            // Any pointer into the object pins the object.
            val = gc->FindBeginningGuarded(val,true);

            if(val && GC::IsRCObject(val)) {
                // We must pin all objects that are reachable from the stack whether they're in
                // the ZCT or not, because destroying an object not in the ZCT may push additional
                // references onto the ZCT, and if those are reachable from the stack they must
                // be pinned.  (Ergo adding objects during reaping must not clear the ZCT flag.)

                RCObject *obj = (RCObject*)val;
                obj->Pin();
            }
        }
    }

    bool ZCT::Grow()
    {
        GCAssert(CanGrow());
        GCAssert(*blocktop == NULL);

        // Allocate one more block
        *blocktop = PleaseAllocBlock();
        if (*blocktop == NULL)
            return false;
        blocktop++;

        return true;
    }

    void ZCT::Prune()
    {
        ClearFreeList();
    }

    void ZCT::ClearBlockTable()
    {
        while (blocktop > blocktable) {
            blocktop--;
            FreeBlock(*blocktop);
            *blocktop = NULL;
        }
    }

    void ZCT::ClearFreeList()
    {
        while (freeList != NULL) {
            void* item = (void*)freeList;
            freeList = (void**)*freeList;
            GCHeap::GetGCHeap()->FreeNoOOM(item);
        }
    }

    RCObject** ZCT::PleaseAllocBlock()
    {
#ifdef ZCT_TESTING
        if (zct_allowance == 0)
            return false;
#endif
        RCObject** block = NULL;
        if (freeList != NULL) {
            block = (RCObject**)freeList;
            freeList = (void**)*freeList;
        }
        else
            block = (RCObject**)GCHeap::GetGCHeap()->AllocNoOOM(1, GCHeap::flags_Alloc|GCHeap::kCanFail);
#ifdef ZCT_TESTING
        if (block != NULL)
            --zct_allowance;
#endif
        return block;
    }

    void ZCT::FreeBlock(RCObject** block)
    {
#ifdef ZCT_TESTING
        zct_allowance++;
#endif
        *(void**)block = (void*)freeList;
        freeList = (void**)block;
    }
}

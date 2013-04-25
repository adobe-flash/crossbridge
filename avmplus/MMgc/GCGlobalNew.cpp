/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

namespace MMgc
{
#ifdef MMGC_USE_SYSTEM_MALLOC

    void *SystemNew(size_t size, FixedMallocOpts opts)
    {
        void *space = VMPI_alloc(size);
        if (space == NULL)
        {
            if (opts & MMgc::kCanFail)
                return NULL;

            int attempt = 0;
            do {
                GCHeap::GetGCHeap()->SystemOOMEvent(size, attempt++);
                space = VMPI_alloc(size);
            } while (space == NULL);
        }
#ifdef MMGC_MEMORY_PROFILER
        GCHeap* heap = GCHeap::GetGCHeap();
        if (heap)
            heap->TrackSystemAlloc(space, size);
#endif
        if (opts & MMgc::kZero)
            VMPI_memset(space, 0, size);
        return space;
    }

    void SystemDelete(void *p)
    {
#ifdef MMGC_MEMORY_PROFILER
        if (p) {
            // heap can be NULL during OOM shutdown
            GCHeap* heap = GCHeap::GetGCHeap();
            if (heap)
                heap->TrackSystemFree(p);
        }
#endif
        VMPI_free(p);
    }

#endif // MMGC_USE_SYSTEM_MALLOC

    void *AllocCall(size_t s, FixedMallocOpts opts)
    {
        return AllocCallInline(s, opts);
    }

    void DeleteCall( void* p )
    {
        DeleteCallInline(p);
    }
};

#ifdef MMGC_OVERRIDE_GLOBAL_NEW

// Nothing; it's all inline in GCGlobalNew.h

#else

void* operator new(size_t size, MMgc::NewDummyOperand /*ignored*/) MMGC_NEW_THROWS_CLAUSE
{
    return MMgc::NewTaggedScalar(size);
}

void* operator new(size_t size, MMgc::NewDummyOperand /*ignored*/, MMgc::FixedMallocOpts opts) MMGC_NEW_THROWS_CLAUSE
{
    return MMgc::NewTaggedScalar(size, opts);
}

namespace MMgc
{
    // Return NULL iff ((opts & kCanFail) != 0)
    REALLY_INLINE void *TaggedAlloc(size_t size, FixedMallocOpts opts, uint32_t guard)
    {
        (void)guard;

#ifdef MMGC_DELETE_DEBUGGING
        // Store a guard cookie preceding the object so that we can see if it is
        // released with a proper delete (scalar/array)
        size = GCHeap::CheckForAllocSizeOverflow(size, MMGC_GUARDCOOKIE_SIZE);
#endif //MMGC_DELETE_DEBUGGING

        char* mem = (char*)AllocCallInline(size, opts);

#ifdef MMGC_DELETE_DEBUGGING
        if (mem != NULL)
        {
            *(uint32_t*)(void*)mem = guard;
            mem += MMGC_GUARDCOOKIE_SIZE;
        }
#endif // MMGC_DELETE_DEBUGGING

        return mem;
    }

    REALLY_INLINE void* NewTaggedScalar(size_t size, FixedMallocOpts opts)
    {
        GCAssertMsg(GCHeap::GetGCHeap()->IsStackEntered() || (opts&kCanFail) != 0, "MMGC_ENTER macro must exist on the stack");

        return TaggedAlloc(size, opts, GCHeap::MMScalarTag);
    }

    void* NewTaggedArray(size_t count, size_t elsize, FixedMallocOpts opts, bool isPrimitive)
    {
        GCAssertMsg(GCHeap::GetGCHeap()->IsStackEntered() || (opts&kCanFail) != 0, "MMGC_ENTER macro must exist on the stack");

        size_t size = GCHeap::CheckForCallocSizeOverflow(count, elsize);
        if(!isPrimitive)
            size = GCHeap::CheckForAllocSizeOverflow(size, MMGC_ARRAYHEADER_SIZE);

        void *p = TaggedAlloc(size, opts, GCHeap::MMNormalArrayTag + uint32_t(isPrimitive));

        if (!isPrimitive && p != NULL)
        {
            *(size_t*)p = count;
            p = (char*)p + MMGC_ARRAYHEADER_SIZE;
        }

        return p;
    }

#ifdef MMGC_DELETE_DEBUGGING
    // Helper functions to check the guard.
    // The guard is an uin32_t stored in locations preceding the object.

    // The solaris compiler does not allow these to be both static and REALLY_INLINE,
    // so choose the latter over the former.

    REALLY_INLINE bool CheckForAllocationGuard(void* mem, uint32_t guard)
    {
        return (*(uint32_t*)(void *)((char*)mem - MMGC_GUARDCOOKIE_SIZE) == guard);
    }

    REALLY_INLINE bool IsScalarAllocation(void* p)
    {
        return CheckForAllocationGuard(p, GCHeap::MMScalarTag);
    }

    REALLY_INLINE bool IsArrayAllocation(void* p, bool primitive)
    {
        // Check if we have array guard right before the pointer.
        uint32_t guard = GCHeap::MMNormalArrayTag + uint32_t(primitive);
        return CheckForAllocationGuard(p, guard)                                    // simple array
            || CheckForAllocationGuard((char*)p - MMGC_ARRAYHEADER_SIZE, guard);    // array with header
    }

    REALLY_INLINE bool IsGCHeapAllocation(void* p)
    {
        return (GCHeap::GetGCHeap() && GCHeap::GetGCHeap()->IsAddressInHeap(p));
    }

    void VerifyTaggedScalar(void* p)
    {
        if (!IsScalarAllocation(p))
        {
            if (IsArrayAllocation(p, true) || IsArrayAllocation(p, false))
            {
                GCAssertMsg(0, "Trying to release array pointer with scalar destructor! Check the allocation and free calls for this object!");
            }
            else if (!IsGCHeapAllocation(p))
            {
                GCAssertMsg(0, "Trying to release system memory with scalar deletefunc! Check the allocation and free calls for this object!");
            }
            else
            {
                GCAssertMsg(0, "Trying to release funky memory with scalar deletefunc! Check the allocation and free calls for this object!");
            }
        }
    }

    void VerifyTaggedArray(void* p, bool primitive)
    {
        if (!IsArrayAllocation(p, primitive))
        {
            if (IsArrayAllocation(p, !primitive))
            {
                GCAssertMsg(0, "Trying to release array pointer with different type destructor! Check the allocation and free calls for this object!");
            }
            if (IsScalarAllocation(p))
            {
                GCAssertMsg(0, "Trying to release scalar pointer with vector destructor! Check the allocation and free calls for this object!");
            }
            else if (!IsGCHeapAllocation(p))
            {
                GCAssertMsg(0, "Trying to release system pointer with vector deletefunc! Check the allocation and free calls for this object!");
            }
            else
            {
                GCAssertMsg(0, "Trying to release funky memory with vector deletefunc! Check the allocation and free calls for this object!");
            }
        }
    }

    // Functions to actually release the memory through FixedMalloc.
    // Non-debug versions of these functions are always inlined.

    void DeleteTaggedScalar( void* p )
    {
#ifdef MMGC_DELETE_DEBUGGING
        // we need to adjust the pointer to release also the guard.
        p = (char*)p - MMGC_GUARDCOOKIE_SIZE;
#endif //MMGC_DELETE_DEBUGGING

        DeleteCallInline(p);
    }

    void DeleteTaggedArrayWithHeader( void* p )
    {
        if( p )
        {
#ifdef MMGC_DELETE_DEBUGGING
            p = ((char*)p - (MMGC_ARRAYHEADER_SIZE + MMGC_GUARDCOOKIE_SIZE));
#else
            p = ((char*)p - MMGC_ARRAYHEADER_SIZE);
#endif //MMGC_DELETE_DEBUGGING
            DeleteCallInline(p);
        }
    }

    void DeleteTaggedScalarChecked(void* p)
    {
        if( p )
        {
#ifdef MMGC_DELETE_DEBUGGING
            VerifyTaggedScalar( p );
#endif
            DeleteTaggedScalar( p );
        }
    }

    void DeleteTaggedArrayWithHeaderChecked(void* p, bool primitive)
    {
        if (p)
        {
#ifdef MMGC_DELETE_DEBUGGING
            VerifyTaggedArray(p, primitive);
#endif
            if (primitive)
                DeleteTaggedScalar(p);
            else
                DeleteTaggedArrayWithHeader(p);
        }
    }

#endif //MMGC_DELETE_DEBUGGING


} // namespace MMgc

#endif // MMGC_OVERRIDE_GLOBAL_NEW

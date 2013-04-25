// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 663508: Add FixedMalloc::FindBeginning

%%component mmgc
%%category fixedmalloc_findbeginning

%%prefix
using namespace MMgc;

%%decls

    MMgc::FixedMalloc *fm;

    // Allocates object of size sz and tests FixedMalloc::FindBeginning
    // at various addresses within the allocated object.  The sampled
    // addresses are meant to cover the following interesting edge
    // cases: at or near the object's start, the middle of the object,
    // and at or near the object's end.
    //
    // If all the tests successively identify the object's start, then
    // returns 0.  Otherwise returns a numeric code identifying which
    // tests failed.
    int allocateVerifyAndFree(size_t sz)
    {
        int retval = 0;
        (void)sz;

        char *obj = (char*)fm->Alloc(sz);

        retval |= checkLookups(obj, obj, sz);
        retval = retval << 2;
        if (sz > 1) retval |= checkLookups(obj+1, obj, sz);
        retval = retval << 2;
        if (sz > 3) retval |= checkLookups(obj+3, obj, sz);
        retval = retval << 2;
        retval |= checkLookups(obj+sz/2, obj, sz);
        retval = retval << 2;
        retval |= checkLookups(obj+sz-1, obj, sz);
        retval = retval << 2;

        fm->Free(obj);

        return retval;
    }

    // Returns 0 if both test of FixedMalloc::FindBeginning and
    // FixedMalloc::FindBeginningAndSize pass.
    // Returns 1 if the first test (of FindBeginning) fails.
    // Returns 2 if the second test (of FindBeginningAndSize) fails.
    int checkLookups(const void* probe, const void* realStart, size_t sz)
    {
        int retval = 0;
        const void* begin_recv;
        size_t size_recv;
        size_t roundup_actual_size;

        if (sz <= (size_t)FixedMalloc::kLargestAlloc)
            roundup_actual_size = fm->FindAllocatorForSize(sz)->GetItemSize();
        else
            roundup_actual_size =
                roundUp(sz + DebugSize(), GCHeap::kBlockSize) - DebugSize();

        if (fm->FindBeginning(probe) != realStart)
            retval |= 1;
        if (!fm->FindBeginningAndSize(probe, begin_recv, size_recv) ||
            (begin_recv != realStart) ||
            (size_recv != roundup_actual_size))
            retval |= 3;

        return retval;
    }

    size_t roundUp(size_t s, size_t inc) {
        return (((s + inc - 1) / inc) * inc);
    }

%%methods

%%prologue
    fm = MMgc::FixedMalloc::GetFixedMalloc();

%%test findbeginnings_small0
    {
        size_t sz = FixedMalloc::kSizeClasses[0];
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_small1
    {
        size_t sz = FixedMalloc::kSizeClasses[1];
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_small2
    {
        size_t sz = FixedMalloc::kSizeClasses[2];
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_small3
    {
        size_t sz = FixedMalloc::kSizeClasses[3];
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_almost_large1
    {
        size_t sz = FixedMalloc::kLargestAlloc - 1;
        sz = sz - DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
        sz = sz + DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_almost_large2
    {
        size_t sz = FixedMalloc::kLargestAlloc;
        sz = sz - DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
        sz = sz + DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_large
    {
        size_t sz = FixedMalloc::kLargestAlloc+1;
        sz = sz - DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
        sz = sz + DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_almost_multiblock1
    {
        size_t sz = GCHeap::kBlockSize-1;
        sz = sz - DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
        sz = sz + DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_almost_multiblock2
    {
        size_t sz = GCHeap::kBlockSize;
        sz = sz - DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
        sz = sz + DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_barely_multiblock
    {
        size_t sz = GCHeap::kBlockSize+1;
        sz = sz - DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
        sz = sz + DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_multiblock_two
    {
        size_t sz = GCHeap::kBlockSize*2;
        sz = sz - DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
        sz = sz + DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

%%test findbeginnings_multiblock_ten
    {
        size_t sz = GCHeap::kBlockSize*10;
        sz = sz - DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
        sz = sz + DebugSize();
        %%verify allocateVerifyAndFree(sz) == 0
              ;
    }

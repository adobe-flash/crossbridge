/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCLargeAlloc_inlines__
#define __GCLargeAlloc_inlines__

namespace MMgc
{
    /*static*/
    REALLY_INLINE GCLargeAlloc::LargeBlock* GCLargeAlloc::GetLargeBlock(const void *addr)
    {
        return (LargeBlock*)GetBlockHeader(addr);
    }

    /*static*/
    REALLY_INLINE GCLargeAlloc::LargeBlock* GCLargeAlloc::Next(LargeBlock* b)
    {
        return (LargeBlock*)b->next;
    }

#ifndef MMGC_FASTBITS
    REALLY_INLINE gcbits_t& GCLargeAlloc::GetGCBits(const void *realptr)
    {
        return GetLargeBlock(realptr)->flags[0];
    }
#endif

    /*static*/
    REALLY_INLINE bool GCLargeAlloc::IsLargeBlock(const void *item)
    {
        // The pointer should be 4K aligned plus 16 bytes
        // Mac inserts 16 bytes for new[] so make it more general
        return (((uintptr_t)item & GCHeap::kOffsetMask) == sizeof(LargeBlock));
    }

    /*static*/
    REALLY_INLINE bool GCLargeAlloc::IsProtectedAgainstFree(const void *item)
    {
        LargeBlock *block = GetLargeBlock(item);
        return (block->flags[0] & kQueued) != 0 || (block->flags[1] & kProtected) != 0;
    }

    /*static*/
    REALLY_INLINE void GCLargeAlloc::ProtectAgainstFree(const void *item)
    {
        LargeBlock *block = GetLargeBlock(item);
        block->flags[1] |= kProtected;
    }

    /*static*/
    REALLY_INLINE void GCLargeAlloc::UnprotectAgainstFree(const void *item)
    {
        LargeBlock *block = GetLargeBlock(item);
        block->flags[1] &= ~kProtected;
    }

    /*static*/
    REALLY_INLINE void* GCLargeAlloc::FindBeginning(const void *item)
    {
        LargeBlock *block = GetLargeBlock(item);
        GCAssertMsg(item >= block->GetObject(), "Can't call FindBeginning on something pointing to GC header");
        return block->GetObject();
    }

    REALLY_INLINE int GCLargeAlloc::LargeBlock::GetNumBlocks() const
    {
        return (size + sizeof(LargeBlock)) / GCHeap::kBlockSize;
    }

    REALLY_INLINE void* GCLargeAlloc::LargeBlock::GetObject() const
    {
        return (void*) (this+1);
    }

    REALLY_INLINE GCLargeAllocIterator::GCLargeAllocIterator(MMgc::GCLargeAlloc* alloc)
        : alloc(alloc)
        , block(alloc->m_blocks)
    {
    }

    REALLY_INLINE bool GCLargeAllocIterator::GetNextMarkedObject(void*& out_ptr)
    {
        while (block != NULL) {
            GCLargeAlloc::LargeBlock* b = block;
            block = GCLargeAlloc::Next(block);
            if ((b->flags[0] & kMark) != 0 && (b->containsPointers != 0)) {
                out_ptr = GetUserPointer(b->GetObject());
                return true;
            }
        }
        return false;
    }
}

#endif /* __GCLargeAlloc_inlines__ */

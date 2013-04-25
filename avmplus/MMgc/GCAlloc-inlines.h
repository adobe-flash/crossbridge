/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCAlloc_inlines__
#define __GCAlloc_inlines__

// Inline functions for GCAlloc, GCAlloc::GCBlock, and GCAllocIterator

namespace MMgc
{
    REALLY_INLINE GCBlockHeader* GetBlockHeader(const void* item)
    {
        return (GCBlockHeader*)(uintptr_t(item) & GCHeap::kBlockMask);
    }

    /*static*/
    REALLY_INLINE GCAlloc::GCBlock *GCAlloc::GetBlock(const void *item)
    {
        return (GCBlock*)GetBlockHeader(item);
    }

    /*static*/
    REALLY_INLINE GCAlloc::GCBlock* GCAlloc::Next(GCAlloc::GCBlock* b)
    {
        return (GCBlock*)b->next;
    }

#ifndef MMGC_FASTBITS
    /*static*/
    REALLY_INLINE gcbits_t& GCAlloc::GetGCBits(const void* realptr)
    {
        GCBlock *block = GetBlock(realptr);
        return block->bits[GetBitsIndex(block, realptr)];
    }
#endif

    /*static*/
    REALLY_INLINE void *GCAlloc::FindBeginning(const void *item)
    {
        GCBlock *block = GetBlock(item);
        GCAssertMsg(item >= block->items, "Can't call FindBeginning on something pointing to GC header");
        return block->items + block->size * GetObjectIndex(block, item);
    }

    REALLY_INLINE void GCAlloc::SetBlockHasWeakRef(const void *userptr)
    {
        GetBlock(userptr)->slowFlags |= kFlagWeakRefs;
    }

    REALLY_INLINE void GCAlloc::AddToFreeList(GCBlock *b)
    {
        GCAssert(!IsOnEitherList(b) && !b->needsSweeping());
        b->prevFree = NULL;
        b->nextFree = m_firstFree;
        if (m_firstFree) {
            GCAssert(m_firstFree->prevFree == 0 && m_firstFree != b);
            m_firstFree->prevFree = b;
        }
        m_firstFree = b;
    }

    REALLY_INLINE void GCAlloc::RemoveFromFreeList(GCBlock *b)
    {
        GCAssert(m_firstFree == b || b->prevFree != NULL);
        GCAssert(b->prevFree == NULL || b->prevFree->nextFree == b);
        GCAssert(b->nextFree == NULL || b->nextFree->prevFree == b);
        if ( ((b->prevFree && (b->prevFree->nextFree!=b))) ||
            ((b->nextFree && (b->nextFree->prevFree!=b))) )
            VMPI_abort();

        if ( m_firstFree == b )
            m_firstFree = b->nextFree;
        else
            b->prevFree->nextFree = b->nextFree;

        if (b->nextFree)
            b->nextFree->prevFree = b->prevFree;
        b->nextFree = b->prevFree = NULL;
    }

    REALLY_INLINE void GCAlloc::AddToSweepList(GCBlock *b)
    {
        GCAssert(!IsOnEitherList(b) && !b->needsSweeping());
        b->prevFree = NULL;
        b->nextFree = m_needsSweeping;
        if (m_needsSweeping) {
            GCAssert(m_needsSweeping->prevFree == 0);
            m_needsSweeping->prevFree = b;
        }
        m_needsSweeping = b;
        b->setNeedsSweeping(kFlagNeedsSweeping);
    }

    REALLY_INLINE void GCAlloc::RemoveFromSweepList(GCBlock *b)
    {
        GCAssert(m_needsSweeping == b || b->prevFree != NULL);
        if ( ((b->prevFree && (b->prevFree->nextFree!=b))) ||
            ((b->nextFree && (b->nextFree->prevFree!=b))) )
            VMPI_abort();

        if ( m_needsSweeping == b )
            m_needsSweeping = b->nextFree;
        else
            b->prevFree->nextFree = b->nextFree;

        if (b->nextFree)
            b->nextFree->prevFree = b->prevFree;
        b->setNeedsSweeping(0);
        b->nextFree = b->prevFree = NULL;
    }

    REALLY_INLINE uint32_t GCAlloc::GetObjectIndex(const GCBlock *block, const void *item)
    {
        GCAlloc* alloc = (GCAlloc*)block->alloc;
        uint32_t index = (uint32_t)((((char*) item - block->items) * alloc->multiple) >> alloc->shift);
 #ifdef _DEBUG
        GCAssert(((char*) item - block->items) / block->size == index);
 #endif
        return index;
    }           

#ifdef MMGC_FASTBITS

    /*static*/
    REALLY_INLINE uint32_t GCAlloc::GetBitsIndex(const GCBlock *block, const void *item)
    {
        uint32_t index = (uintptr_t(item) & 0xFFF) >> block->bitsShift;
#ifdef _DEBUG
        GCAssert(index < uint32_t(((GCAlloc*)block->alloc)->m_numBitmapBytes));
#endif
        return index;
    }
    
#else // MMGC_FASTBITS
    
    /*static*/
    REALLY_INLINE uint32_t GCAlloc::GetBitsIndex(const GCBlock *block, const void *item)
    {
        return GetObjectIndex(block, item);
    }

#endif // MMGC_FASTBITS

    REALLY_INLINE int GCAlloc::GCBlock::GetCount() const
    {
        return ((GCAlloc*)alloc)->m_itemsPerBlock;
    }

    REALLY_INLINE int GCAlloc::GCBlock::needsSweeping()
    {
        return slowFlags & kFlagNeedsSweeping;
    }

    REALLY_INLINE void GCAlloc::GCBlock::setNeedsSweeping(int v)
    {
        GCAssert(v == 0 || v == kFlagNeedsSweeping);
        slowFlags = (uint8_t)((slowFlags & ~kFlagNeedsSweeping) | v);
    }

    REALLY_INLINE GCAllocIterator::GCAllocIterator(MMgc::GCAlloc* alloc)
        : alloc(alloc)
        , block(alloc->m_firstBlock)
        , idx(0)
        , limit(alloc->m_itemsPerBlock)
        , size(alloc->m_itemSize)
    {
    }

    REALLY_INLINE bool GCAllocIterator::GetNextMarkedObject(void*& out_ptr)
    {
        for (;;) {
            if (idx == limit) {
                idx = 0;
                block = GCAlloc::Next(block);
            }
            if (block == NULL)
                return false;
            uint32_t i = idx++;
            if ((GC::GetGCBits(block->items + i*size) & (kMark|kQueued)) == kMark) {
                out_ptr = GetUserPointer(block->items + i*size);
                return true;
            }
        }
    }
}

#endif /* __GCAlloc_inlines__ */

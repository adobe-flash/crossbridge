/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"
#include "StaticAssert.h"

namespace MMgc
{
    // For now, too much code assumes these are the same.
    MMGC_STATIC_ASSERT( PageMap::kPageSize == GCHeap::kBlockSize );

    namespace PageMap
    {
        template<typename PM>
        void SimpleExpandSetAll(PM *pm, GCHeap *heap, void *item,
                                       uint32_t numPages, PageType val)
        {
            pm->EnsureCapacity(heap, item, numPages);
            uintptr_t addr = (uintptr_t)item;
            while(numPages--)
            {
                GCAssert(pm->AddrToVal(addr) == 0);
                pm->AddrSet(addr, val);
                GCAssert(pm->AddrToVal(addr) == val);
                addr += GCHeap::kBlockSize;
            }
        }

        template<typename PM>
        void SimpleClearAddrs(PM *pm, void *item, uint32_t numpages)
        {
            uintptr_t addr = (uintptr_t) item;

            while(numpages--)
            {
                pm->AddrClear(addr);
                addr += GCHeap::kBlockSize;
            }
        }

        static const uintptr_t MAX_UINTPTR = ~uintptr_t(0);

        PageMapBase::PageMapBase()
            : memStart(MAX_UINTPTR)
            , memEnd(0)
        {}

#ifdef MMGC_USE_UNIFORM_PAGEMAP
        Uniform::Uniform()
            : PageMapBase()
            , pageMap(NULL)
        {}

        REALLY_INLINE uint8_t& Uniform::IndexToByteRef(uintptr_t index)
        {
            GCAssert(pageMap != NULL);
            return pageMap[index >> 2];
        }

        REALLY_INLINE void Uniform::IndexSet(uintptr_t index, PageType val)
        {
            IndexToByteRef(index) |= (val << IndexToByteShiftAmt(index));
        }

        REALLY_INLINE void Uniform::IndexClear(uintptr_t index)
        {
            IndexToByteRef(index) &= ~(3 << IndexToByteShiftAmt(index));
        }

        REALLY_INLINE void Uniform::AddrSet(uintptr_t addr, PageType val)
        {
            uintptr_t index = AddrToIndex(addr);
            GCAssert(IndexSaneForAddressRange(index));
            IndexSet(index, val);
        }

        REALLY_INLINE void Uniform::AddrClear(uintptr_t addr)
        {
            IndexClear(AddrToIndex(addr));
        }

        REALLY_INLINE
        void Uniform::EnsureCapacity(GCHeap *heap, void *item, uint32_t numPages)
        {
            // Shift logic below used to depend on pageMap byte
            // corresponding to 16k chunk (16k == kPageSize * 8/2);
            // was dependent on kPageSize == 4096 (and on implicit
            // 2-bit payload size).  Probably no longer the case,
            // but this code is about to be removed anyway, so just
            // assert (more as documentation) equivalence for now.
            // See https://bugzilla.mozilla.org/show_bug.cgi?id=581070
            MMGC_STATIC_ASSERT(PageMap::kPageSize == 4096);

            const static int kByteOffsetShift = PageMap::kPageShift + 2;
            const static int kByteOffsetMask = (1 << kByteOffsetShift)-1;

            // (For the benefit of a reader comparing this code to the
            //  pre-refactored version; derived from above kPageSize.)
            MMGC_STATIC_ASSERT(kByteOffsetShift == 14);
            MMGC_STATIC_ASSERT(kByteOffsetMask == 0x3fff);

            uintptr_t addr = (uintptr_t)item;
            size_t shiftAmount=0;
            uint8_t *dst = pageMap;

            // save the current live range in case we need to move/copy
            size_t numBytesToCopy = (memEnd-memStart)>>kByteOffsetShift;

            if(addr < memStart) {
                // round down to nearest 16k boundary, makes shift logic
                // work because it works in bytes, ie 4-page chunks
                addr &= ~kByteOffsetMask;
                // marking earlier pages
                if(memStart != MAX_UINTPTR) {
                    shiftAmount = (memStart - addr) >> kByteOffsetShift;
                 }
                memStart = addr;
            }

            // FIXME: https://bugzilla.mozilla.org/show_bug.cgi?id=588079
            // use pre-shift addr (keep a copy, or just move this code up)
            // FIXME: https://bugzilla.mozilla.org/show_bug.cgi?id=581070
            // double-check +1 here + below; unnecessary extra page allocated?
            if(addr + (numPages+1)*PageMap::kPageSize > memEnd) {
                // marking later pages
                memEnd = addr + (numPages+1)*PageMap::kPageSize;
                // round up to 16k
                memEnd = (memEnd+kByteOffsetMask)&~kByteOffsetMask;
            }

            uint32_t numBlocksNeeded =
                uint32_t(((memEnd-memStart)>>kByteOffsetShift)
                         / GCHeap::kBlockSize
                         + 1);
            if(numBlocksNeeded > heap->Size(pageMap)) {
                dst = (uint8_t*)heap->AllocNoOOM(numBlocksNeeded);
            }

            if(shiftAmount || dst != pageMap) {
                if (pageMap != NULL)
                    VMPI_memmove(dst + shiftAmount, pageMap, numBytesToCopy);
                if ( shiftAmount ) {
                    VMPI_memset(dst, 0, shiftAmount);
                }
                if(dst != pageMap) {
                    if (pageMap != NULL)
                        heap->FreeNoOOM(pageMap);
                    pageMap = dst;
                }
            }
        }

        void Uniform::ExpandSetAll(GCHeap *heap, void *item,
                                   uint32_t numPages, PageType val)
        {
            SimpleExpandSetAll(this, heap, item, numPages, val);
        }

        void Uniform::ClearAddrs(void *item, uint32_t numpages)
        {
            SimpleClearAddrs(this, item, numpages);
        }

#endif // MMGC_USE_UNIFORM_PAGEMAP

#if ! defined(MMGC_USE_UNIFORM_PAGEMAP) && ! defined(MMGC_64BIT)
        Tiered2::Tiered2()
            : PageMapBase()
        {
            for (size_t i = 0; i < tier1_entries; i++) {
                pageMap[i] = NULL;
            }
        }

        void Tiered2::DestroyPageMapVia(GCHeap *heap) {
            // Was allocated with heap->Alloc and heap->AllocNoOOM,
            // can't use heapFree here
            for (size_t i=0; i < tier1_entries; i++) {
                uint8_t *subMap = pageMap[i];
                if (subMap == NULL)
                    continue;
                heap->Free(subMap);
                pageMap[i] = NULL;
            }
        }

        void Tiered2::EnsureCapacity(GCHeap *heap, void *item, uint32_t numPages)
        {
            uintptr_t addr    = uintptr_t(item);
            // uintptr_t start_1 = uintptr_t(item) >> tier1_shift;
            // uintptr_t finis_1 = uintptr_t(item) >> tier1_shift;
            if(addr < memStart) {
                // existing state left in place; thus no copy logic to
                // accomodate (compare w/ Uniform::EnsureCapacity).
                memStart = addr;
            }

            uintptr_t addr_lim = addr + (numPages+1)*GCHeap::kBlockSize;
            if (addr_lim > memEnd) {
                // again, no copy logic to accomodate.
                memEnd = addr_lim;
            }

            // AddrToIndex drops low order bits, but we want to round *up* for limit calc.
            uint32_t i_lim = AddrToIndex1(addr_lim-1)+1;
            GCAssert(i_lim <= tier1_entries);
            for (uint32_t i = AddrToIndex1(addr); i < i_lim; i++) {
                GCAssert(tier2_entries*sizeof(uint8_t)
                         <= tier2_pages*GCHeap::kBlockSize);
                uint8_t *subMap = pageMap[i];
                if (subMap == NULL) {
                    subMap = (uint8_t*)heap->AllocNoOOM(tier2_pages);
                    pageMap[i] = subMap;
                }
            }
        }

        void Tiered2::ExpandSetAll(GCHeap *heap, void *item,
                                   uint32_t numPages, PageType val)
        {
            SimpleExpandSetAll(this, heap, item, numPages, val);
        }

        void Tiered2::ClearAddrs(void *item, uint32_t numpages)
        {
            SimpleClearAddrs(this, item, numpages);
        }

#endif // ! defined(MMGC_USE_UNIFORM_PAGEMAP) && ! defined(MMGC_64BIT)

#if ! defined(MMGC_USE_UNIFORM_PAGEMAP) && defined(MMGC_64BIT)
        Tiered4::Tiered4()
            : PageMapBase()
        {
            for (size_t i = 0; i < tier0_entries; i++) {
                pageMap[i] = NULL;
            }
        }

        PageType Tiered4::AddrToVal(uintptr_t addr) const
        {
            uint8_t* subsubMap = Tiered4::AddrToLeafBytes(addr);
            MMGC_STATIC_ASSERT(kNonGC == 0);
            if (subsubMap == NULL)
                return kNonGC;
            return LeafAddrToVal(subsubMap, addr);
        }

        void Tiered4::AddrClear(uintptr_t addr)
        {
            uint8_t* subsubMap = Tiered4::AddrToLeafBytes(addr);
            if (subsubMap == NULL)
                return;
            LeafAddrClear(subsubMap, addr);
        }

        void Tiered4::AddrSet(uintptr_t addr, PageType val)
        {
            uint8_t* subsubMap = Tiered4::AddrToLeafBytes(addr);
            GCAssert(subsubMap != NULL);
            LeafAddrSet(subsubMap, addr, val);
        }


        uint8_t *Tiered4::AddrToLeafBytes(uintptr_t addr) const
        {
            GCAssert(pageMap != NULL);

            uint32_t i = AddrToIndex0(addr);
            uint8_t ***subMap1 = pageMap[i];
            if (subMap1 == NULL)
                return NULL;

            uint32_t j = AddrToIndex1(addr);
            uint8_t **subMap2 = subMap1[j];
            if (subMap2 == NULL)
                return NULL;
            uint32_t k = AddrToIndex2(addr);
            uint8_t *subMap3 = subMap2[k];
            return subMap3; // (may be NULL)
        }

        // Note: code below assumes NULL is represented by all-bits 0.

        void Tiered4::DestroyPageMapVia(GCHeap *heap) {
            // Was allocated with heap->Alloc and heap->AllocNoOOM,
            // can't use heapFree here
            for (size_t i=0; i < tier0_entries; i++) {
                uint8_t ***subMap1 = pageMap[i];
                if (subMap1 == NULL)
                    continue;
                for (size_t j=0; j < tier1_entries; j++) {
                    uint8_t **subMap2 = subMap1[j];
                    if (subMap2 == NULL)
                        continue;
                    for (size_t k=0; k < tier2_entries; k++) {
                        uint8_t *subMap3 = subMap2[k];
                        if (subMap3 == NULL)
                            continue;
                        heap->Free(subMap3);
                    }
                    heap->Free(subMap2);
                }
                heap->Free(subMap1);
            }
        }

        void Tiered4::InitPageMap(GCHeap *heap, uintptr_t start, uintptr_t limit)
        {
            // AddrToIndex drops low order bits, but we want to round *up* for limit calc.
            uint32_t i_lim = AddrToIndex0(limit-1)+1;
            GCAssert(i_lim <= tier0_entries);
            uint32_t i = AddrToIndex0(start), j = AddrToIndex1(start), k = AddrToIndex2(start);

            // common case: [x,x_lim) is small with respect to number
            // of entries (for x in {i,j,k}).  Easier to modify zero-
            // initialized array than to allocate non-initialized and
            // then use 3 loops on 0--x, x--x_lim, x_lim--x_entries.

            for (; i < i_lim; i++, j=0, k=0) {
                uint8_t ***subMap1 = pageMap[i];
                uint32_t j_lim = (i+1 < i_lim) ? tier1_entries : AddrToIndex1(limit-1)+1;
                if (subMap1 == NULL) {
                    subMap1 = (uint8_t***)heap->AllocNoOOM(tier1_pages);
                    pageMap[i] = subMap1;
                }
                for (; j < j_lim; j++, k=0) {
                    uint8_t **subMap2 = subMap1[j];
                    uint32_t k_lim = (j+1 < j_lim) ? tier2_entries : AddrToIndex2(limit-1)+1;
                    if (subMap2 == NULL) {
                        subMap2 = (uint8_t**)heap->AllocNoOOM(tier2_pages);
                        subMap1[j] = subMap2;
                    }
                    for (; k < k_lim; k++) {
                        uint8_t *subMap3 = subMap2[k];
                        if (subMap3 == NULL) {
                            subMap3 = (uint8_t*)heap->AllocNoOOM(tier3_pages);
                            subMap2[k] = subMap3;
                        }
                    }
                }
            }
        }

        void Tiered4::EnsureCapacity(GCHeap *heap, void *item, uint32_t numPages)
        {
            uintptr_t addr    = uintptr_t(item);
            if(addr < memStart) {
                // existing state left in place, no need to align addr
                // in any particular fashion.  (compare w/
                // Uniform::EnsureCapacity).
                memStart = addr;
            }

            // FIXME: https://bugzilla.mozilla.org/show_bug.cgi?id=593351
            // double-check +1 here + below; unnecessary extra page allocated?
            uintptr_t addr_lim = addr + (numPages+1)*PageMap::kPageSize;
            if (addr_lim > memEnd) {
                // again, no copy logic to accomodate.
                memEnd = addr_lim;
            }

            // check each node in a tier has capacity for tier's entry count.
            MMGC_STATIC_ASSERT(Tiered4::tier2_entries*sizeof(uint8_t*)
                               <= Tiered4::tier2_pages*GCHeap::kBlockSize);
            MMGC_STATIC_ASSERT(Tiered4::tier3_entries*sizeof(uint8_t)
                               <= Tiered4::tier3_pages*GCHeap::kBlockSize);

            InitPageMap(heap, addr, addr_lim);
        }

        CacheT4::CacheT4()
            : Tiered4()
            , cached_addr_prefix(CacheT4::uncached)
            , cached_leaf_bytes(NULL)
        {}


        DelayT4::DelayT4()
            : CacheT4()
            , stillInitialDelay(true)
        {}

        void DelayT4::DestroyPageMapVia(GCHeap *heap)
        {
            if (stillInitialDelay) {
                if (cached_addr_prefix == CacheT4::uncached) {
                    // nothing allocated, do nothing
                    GCAssert(cached_leaf_bytes == NULL);
                } else {
                    // allocation of tree structure was delayed
                    // indefinitely but for a single leaf
                    GCAssert(cached_leaf_bytes != NULL);
                    heap->Free(cached_leaf_bytes);
                    cached_leaf_bytes = NULL;
                    cached_addr_prefix = 0;
                }
            } else {
                // resort to standard free.
                this->CacheT4::DestroyPageMapVia(heap);
            }
        }

        void DelayT4::SetupDelayedPagemap(GCHeap *heap,
                                          uintptr_t start,
                                          uintptr_t limit)
        {
            // track that cached leaf is installed at most once.
            bool used_cached_leaf = false;
            bool use_cached_leaf;

            uint32_t i_cache;
            uint32_t j_cache;
            uint32_t k_cache;

            if (cached_addr_prefix != CacheT4::uncached) {
                use_cached_leaf = true;
                i_cache = AddrToIndex0(cached_addr_prefix);
                j_cache = AddrToIndex1(cached_addr_prefix);
                k_cache = AddrToIndex2(cached_addr_prefix);
            } else {
                use_cached_leaf = false;
                // !use_cached_leaf implies should not be read.
                i_cache = uint32_t(-1);
                j_cache = uint32_t(-1);
                k_cache = uint32_t(-1);
            }

            MMGC_STATIC_ASSERT(tier1_entries*sizeof(uint8_t**)
                               <= tier1_pages*GCHeap::kBlockSize);
            MMGC_STATIC_ASSERT(tier2_entries*sizeof(uint8_t*)
                               <= tier2_pages*GCHeap::kBlockSize);
            MMGC_STATIC_ASSERT(tier3_entries*sizeof(uint8_t)
                               <= tier3_pages*GCHeap::kBlockSize);

            GCAssert(limit != 0); // a limit of 0 would underflow below.
            // AddrToIndex drops low order bits, but we want to round *up* for limit calc.
            uint32_t i_lim = AddrToIndex0(limit-1)+1;
            GCAssert(i_lim <= tier0_entries);
            uint32_t i = AddrToIndex0(start), j = AddrToIndex1(start), k = AddrToIndex2(start);

            // common case: [x,x_lim) is small with respect to number
            // of entries (for x in {i,j,k}).  Easier to modify zero-
            // initialized array than to allocate non-initialized and
            // then use 3 loops on 0--x, x--x_lim, x_lim--x_entries.

            for (; i < i_lim; i++, j=0, k=0) {
                uint8_t ***subMap1 = pageMap[i];
                uint32_t j_lim = (i+1 < i_lim) ? tier1_entries : AddrToIndex1(limit-1)+1;
                if (subMap1 == NULL) {
                    subMap1 = (uint8_t***)heap->AllocNoOOM(tier1_pages);
                    pageMap[i] = subMap1;
                }
                for (; j < j_lim; j++, k=0) {
                    uint8_t **subMap2 = subMap1[j];
                    uint32_t k_lim = (j+1 < j_lim) ? tier2_entries : AddrToIndex2(limit-1)+1;
                    if (subMap2 == NULL) {
                        subMap2 = (uint8_t**)heap->AllocNoOOM(tier2_pages);
                        subMap1[j] = subMap2;
                    }
                    for (; k < k_lim; k++) {
                        uint8_t *subMap3 = subMap2[k];
                        if (use_cached_leaf && i == i_cache && j == j_cache && k == k_cache) {
                            GCAssert(! used_cached_leaf);
                            GCAssert(subMap3 == NULL);

                            subMap2[k] = cached_leaf_bytes;
                            used_cached_leaf = true;

                            // future invocations should not re-attempt to
                            // insert the cached leaf.
                            cached_addr_prefix = CacheT4::uncached;

                        } else if (subMap3 == NULL) {
                            subMap3 = (uint8_t*)heap->AllocNoOOM(tier3_pages);
                            subMap2[k] = subMap3;
                        }
                    }
                }
            }

            GCAssert(use_cached_leaf == used_cached_leaf);
        }

        void DelayT4::EnsureCapacity(GCHeap *heap, void *item, uint32_t numPages)
        {
            const uintptr_t memStart_orig = memStart, memEnd_orig = memEnd;

            if (! stillInitialDelay) {
                // we've already gone beyond 1 leaf and we should
                // resort to non-delayed expansion logic.
                this->CacheT4::EnsureCapacity(heap, item, numPages);
                return;
            }

            uintptr_t addr = uintptr_t(item);
            if(addr < memStart)
                memStart = addr;

            // the +1 might be unnecessary (see XXX w/ Uniform::EnsureCapacity)
            uintptr_t addr_lim = addr + (numPages+1)*GCHeap::kBlockSize;
            if (addr_lim > memEnd)
                memEnd = addr_lim;

            bool fits_in_one_leaf;
            uintptr_t addr_prefix = AddrPrefix(addr);
            if (AddrPrefix(addr_lim) != addr_prefix) {
                // can't work, don't bother trying to use one leaf
                fits_in_one_leaf = false;
            } else if (cached_addr_prefix == CacheT4::uncached) {
                // if both pageMap and cache are unset, then this
                // is the first EnsureCapacity call; use it to set
                // up the single leaf.
                fits_in_one_leaf = true;
            } else if (cached_addr_prefix == addr_prefix) {
                // [addr,addr_lim) fit in one leaf we already have
                fits_in_one_leaf = true;
            } else {
                fits_in_one_leaf = false;
            }

            // If it fits, then use one leaf; o/w perform
            // delayed allocation of page map tree structure.
            if (fits_in_one_leaf) {
                if (cached_addr_prefix == CacheT4::uncached) {
                    cached_addr_prefix = AddrPrefix(addr);
                    cached_leaf_bytes =
                        (uint8_t*)heap->AllocNoOOM(tier3_pages);
                } else {
                    // the one cached leaf suffices.
                }
            } else {
                // Invariant: start is uninit'd value iff end is uninit'd value
                GCAssert((memStart_orig == MAX_UINTPTR) == (memEnd_orig == 0));
                // Do not compute w/ uninit'd values (limit of 0 is forbidden).
                if (memEnd_orig != 0)
                    SetupDelayedPagemap(heap, memStart_orig, memEnd_orig);
                SetupDelayedPagemap(heap, addr, addr_lim);
                stillInitialDelay = false;
            }
        }

        void Tiered4::ExpandSetAll(GCHeap *heap, void *item,
                                   uint32_t numPages, PageType val)
        {
            SimpleExpandSetAll(this, heap, item, numPages, val);
        }

        void Tiered4::ClearAddrs(void *item, uint32_t numpages)
        {
            SimpleClearAddrs(this, item, numpages);
        }

        void CacheT4::ExpandSetAll(GCHeap *heap, void *item,
                                   uint32_t numPages, PageType val)
        {
            SimpleExpandSetAll(this, heap, item, numPages, val);
        }

        void CacheT4::ClearAddrs(void *item, uint32_t numpages)
        {
            SimpleClearAddrs(this, item, numpages);
        }

        void DelayT4::ExpandSetAll(GCHeap *heap, void *item,
                                   uint32_t numPages, PageType val)
        {
            SimpleExpandSetAll(this, heap, item, numPages, val);
        }
#endif // ! defined(MMGC_USE_UNIFORM_PAGEMAP) && defined(MMGC_64BIT)
    }
}

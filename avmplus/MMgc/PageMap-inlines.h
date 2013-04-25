/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MMgc__PageMap_inlines__
#define __MMgc__PageMap_inlines__

namespace MMgc
{
    namespace PageMap
    {
        REALLY_INLINE uintptr_t PageMapBase::MemStart() const
        {
            return memStart;
        }

        REALLY_INLINE uintptr_t PageMapBase::MemEnd() const
        {
            return memEnd;
        }

        REALLY_INLINE bool PageMapBase::AddrIsMappable(uintptr_t addr) const
        {
            return addr >= memStart && addr < memEnd;
        }

#ifdef MMGC_USE_UNIFORM_PAGEMAP
        REALLY_INLINE void Uniform::DestroyPageMapVia(GCHeap *heap)
        {
            if (pageMap != NULL)
                heap->Free(pageMap);
            pageMap = NULL;
        }

        REALLY_INLINE uintptr_t Uniform::AddrToIndex(uintptr_t addr) const
        {
            return (addr - memStart) >> kPageShift;
        }

        REALLY_INLINE uint8_t Uniform::IndexToByte(uintptr_t index) const
        {
            GCAssert(pageMap != NULL);
            return pageMap[index >> 2];
        }

        REALLY_INLINE uint32_t Uniform::IndexToByteShiftAmt(uintptr_t index) const
        {
            // shift amount to determine position in the byte
            // (times 2 b/c 2 bits per page)
            return (index & 0x3) * 2;
        }

        REALLY_INLINE PageType Uniform::IndexToVal(uintptr_t index) const
        {
            // 3 ... is mask for 2 bits, shifted to the left by shiftAmount
            // finally shift back by shift amount to get the value 0, 1 or 3
            // (further explanation in Uniform class definition.)
            //return (pageMap[addr >> 2] & (3<<shiftAmount)) >> shiftAmount;
            uint8_t v = (IndexToByte(index) >> IndexToByteShiftAmt(index)) & 3;
            return PageType(v);
        }

#ifdef DEBUG
        REALLY_INLINE bool Uniform::IndexSaneForAddressRange(uintptr_t index)
        {
            // The index is amount to shift the address to compute
            // corresponding index of its payload in pageMap.

            // The index value includes 2 bits to extract a 2-bit
            // payload from a byte, so lookup eventually shifts index
            // down by 2 to find offset into the pageMap bytemap.

            // Doing index >> 2 leaves (maxaddrbits - kPageShift - 2)
            // bits usable for the lookup into the bytemap, where
            // maxaddrbits is the maximal number of bits of a *usable*
            // address (e.g. 48 on AMD64, 32 on 32-bit architectures).

            // orig. address falls within: 0..2^maxaddrbits
            // indexes (before >> 2)   in: 0..2^(maxaddrbits - kPageShift)
            // byteoffset (after >> 2) in: 0..2^(maxaddrbits - kPageShift - 2)
            //                         ==  0..2^(maxaddrbits - 2) / kPageSize
#ifdef MMGC_64BIT
            static const uintptr_t maxaddr_div_4 = uintptr_t(1) << 46;
#else
            static const uintptr_t maxaddr_div_4 = uintptr_t(1) << 30;
#endif
            (void)maxaddr_div_4;
            return (index >> 2) < (maxaddr_div_4 / kPageSize);
        }
#endif

        REALLY_INLINE PageType Uniform::AddrToVal(uintptr_t addr) const
        {
            uintptr_t index = AddrToIndex(addr);
            GCAssert(IndexSaneForAddressRange(index));
            return IndexToVal(index);
        }
#endif // MMGC_USE_UNIFORM_PAGEMAP

#if ! defined(MMGC_USE_UNIFORM_PAGEMAP) && ! defined(MMGC_64BIT)
        REALLY_INLINE uint32_t Tiered2::AddrToIndex1(uintptr_t addr) const
        {
            uint32_t rtn = uint32_t(addr >> tier1_shift);
            GCAssert(rtn < tier1_entries);
            return rtn;
        }

        REALLY_INLINE uint32_t Tiered2::AddrToIndex2(uintptr_t addr) const
        {
            uint32_t rtn = uint32_t(addr >> tier2_shift) & tier2_postshift_mask;
            GCAssert(rtn < tier2_entries);
            return rtn;
        }

        REALLY_INLINE uint32_t Tiered2::AddrToByteShiftAmt(uintptr_t addr) const
        {
            // shift amount to determine position in the byte (times 2 b/c 2 bits per page)
            return (uint32_t(addr >> kPageShift) & 0x3) * 2;
        }

        REALLY_INLINE uint8_t *Tiered2::AddrToLeafBytes(uintptr_t addr) const
        {
            GCAssert(pageMap != NULL);
            uint32_t i = AddrToIndex1(addr);
            uint8_t *subMap = pageMap[i];
            return subMap; // (may be NULL)
        }

        REALLY_INLINE PageType Tiered2::LeafAddrToVal(uint8_t* subMap, uintptr_t addr) const
        {
            uint32_t k = AddrToIndex2(addr);
            uint8_t byte = subMap[k];
            uint8_t val = (byte >> AddrToByteShiftAmt(addr)) & 0x3;
            return PageType(val);
        }

        REALLY_INLINE void Tiered2::LeafAddrClear(uint8_t* subMap, uintptr_t addr)
        {
            uint32_t k = AddrToIndex2(addr);
            uint32_t shift = AddrToByteShiftAmt(addr);
            subMap[k] &= ~(0x3 << shift);
        }

        REALLY_INLINE void Tiered2::LeafAddrSet(uint8_t* subMap, uintptr_t addr, PageType val)
        {
            uint32_t k = AddrToIndex2(addr);
            uint8_t byte_old = subMap[k];
            (void)byte_old;
            uint32_t shift = AddrToByteShiftAmt(addr);
            GCAssert(((byte_old >> shift)&0x3) == 0);
            subMap[k] |= (val << shift);
        }

        REALLY_INLINE PageType Tiered2::AddrToVal(uintptr_t addr) const
        {
            uint8_t* subMap = Tiered2::AddrToLeafBytes(addr);

            MMGC_STATIC_ASSERT(kNonGC == 0);
            if (subMap == NULL)
                return kNonGC;

            return LeafAddrToVal(subMap, addr);
        }

        REALLY_INLINE void Tiered2::AddrClear(uintptr_t addr)
        {
            uint8_t* subMap = Tiered2::AddrToLeafBytes(addr);
            if (subMap == NULL)
                return;
            LeafAddrClear(subMap, addr);
        }

        REALLY_INLINE void Tiered2::AddrSet(uintptr_t addr, PageType val)
        {
            uint8_t* subMap = Tiered2::AddrToLeafBytes(addr);
            GCAssert(subMap != NULL);
            LeafAddrSet(subMap, addr, val);
        }
#endif // ! defined(MMGC_USE_UNIFORM_PAGEMAP) && ! defined(MMGC_64BIT)


#if ! defined(MMGC_USE_UNIFORM_PAGEMAP) && defined(MMGC_64BIT)
        /* static */
        REALLY_INLINE uint32_t Tiered4::AddrToIndex0(uintptr_t addr)
        {
            uint32_t rtn = uint32_t(addr >> tier0_shift);
            GCAssert(rtn < tier0_entries);
            return rtn;
        }

        /* static */
        REALLY_INLINE uint32_t Tiered4::AddrToIndex1(uintptr_t addr)
        {
            uint32_t rtn = uint32_t(addr >> tier1_shift) & tier1_postshift_mask;
            GCAssert(rtn < tier1_entries);
            return rtn;
        }

        /* static */
        REALLY_INLINE uint32_t Tiered4::AddrToIndex2(uintptr_t addr)
        {
            uint32_t rtn = uint32_t(addr >> tier2_shift) & tier2_postshift_mask;
            GCAssert(rtn < tier2_entries);
            return rtn;
        }

        /* static */
        REALLY_INLINE uint32_t Tiered4::AddrToIndex3(uintptr_t addr)
        {
            uint32_t rtn = uint32_t(addr >> tier3_shift) & tier3_postshift_mask;
            return rtn;
        }

        /* static */
        REALLY_INLINE uint32_t Tiered4::AddrToByteShiftAmt(uintptr_t addr)
        {
            // shift amount to determine position in the byte (times 2 b/c 2 bits per page)
            return (uint32_t(addr >> kPageShift) & 0x3) * 2;
        }

        REALLY_INLINE PageType Tiered4::LeafAddrToVal(uint8_t* subsubMap, uintptr_t addr) const
        {
            uint32_t k = AddrToIndex3(addr);
            uint8_t byte = subsubMap[k];
            uint8_t val = (byte >> AddrToByteShiftAmt(addr)) & 0x3;
            return PageType(val);
        }

        REALLY_INLINE void Tiered4::LeafAddrClear(uint8_t* subsubMap, uintptr_t addr)
        {
            uint32_t k = AddrToIndex3(addr);
            uint32_t shift = AddrToByteShiftAmt(addr);
            subsubMap[k] &= ~(0x3 << shift);
        }

        REALLY_INLINE void Tiered4::LeafAddrSet(uint8_t* subsubMap, uintptr_t addr, PageType val)
        {
            uint32_t k = AddrToIndex3(addr);
            uint8_t byte_old = subsubMap[k];
            (void)byte_old;
            uint32_t shift = AddrToByteShiftAmt(addr);
            GCAssert(((byte_old >> shift)&0x3) == 0);
            subsubMap[k] |= (val << shift);
        }

        /*static*/
        REALLY_INLINE uintptr_t CacheT4::AddrPrefix(uintptr_t addr)
        {
            return (addr & ~((1 << Tiered4::tier2_shift) - 1));
        }

        REALLY_INLINE bool CacheT4::CacheHit(uintptr_t addr) const
        {
            // Note that CacheT4::uncached is selected so that it will
            // not match any AddrPrefix, simplifying orignal logic:
            // return ((cached_addr_prefix != CacheT4::uncached) &&
            //         (cached_addr_prefix == AddrPrefix(addr)));
            bool retval = (cached_addr_prefix == AddrPrefix(addr));
            GCAssert((cached_addr_prefix != CacheT4::uncached) || !retval);
            return retval;
        }

        REALLY_INLINE bool CacheT4::UpdateCache(uintptr_t addr) const
        {
            if (CacheHit(addr))
                return true;

            uint8_t* trial_leaf = Tiered4::AddrToLeafBytes(addr);
            if (trial_leaf == NULL) {
                return false;
            } else {
                cached_leaf_bytes = trial_leaf;
                cached_addr_prefix = AddrPrefix(addr);
                return true;
            }
        }

        REALLY_INLINE PageType CacheT4::AddrToVal(uintptr_t addr) const
        {
            MMGC_STATIC_ASSERT(kNonGC == 0);
            if (UpdateCache(addr))
                return Tiered4::LeafAddrToVal(cached_leaf_bytes, addr);
            else
                return kNonGC;
        }

        REALLY_INLINE void CacheT4::AddrSet(uintptr_t addr, PageType val)
        {
            bool statusFlag = UpdateCache(addr);
            (void)statusFlag;
            GCAssert(statusFlag); // settable should always be cacheable.
            Tiered4::LeafAddrSet(cached_leaf_bytes, addr, val);
            GCAssert(AddrToVal(addr) == val);
        }

        REALLY_INLINE void CacheT4::AddrClear(uintptr_t addr)
        {
            if (UpdateCache(addr))
                Tiered4::LeafAddrClear(cached_leaf_bytes, addr);
            else
                (void)0; // uncacheable implies unmapped implies already clear.
        }
#endif // ! defined(MMGC_USE_UNIFORM_PAGEMAP) && defined(MMGC_64BIT)

    }
}

#endif /* __MMgc__PageMap_inlines__ */

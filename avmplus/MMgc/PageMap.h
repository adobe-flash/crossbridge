/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MMgc__PageMap__
#define __MMgc__PageMap__

namespace MMgc
{
    namespace PageMap
    {
        // Goal: address-to-payload mapping, where payload is a
        // small series of bits.  For current PageType enum,
        // payloadbits = 2.

        // Note that 'two bits per page' is ingrained deeply in the GC
        // at this point
        //
        // (at one point occurrences of 33333333 appeared elsewhere in
        //  the code, but now that appears only associated with
        //  blocks, not pages)
        enum PageType
        {
            kNonGC = 0,
            kGCAllocPage = 1,
            kGCLargeAllocPageRest = 2,
            kGCLargeAllocPageFirst = 3
        };

        // Question: should we be using word-oriented operations
        // rather than byte-oriented ones?

        // kPageSize is granularity; ie the portion of address space,
        // in bytes, that each 2-bit payload in pagemap covers.
        // (Compare against kBlockSize, the unit of allocation for the
        // pagemap structures themselves.)
        // Short term: keep PageMap::kPageSize == GCHeap::kBlockSize
        // Long term: allow block size to be multiple of page size.
        const static uint32_t kPageSize  = 4096;

        const static uint32_t kPageShift = 12;
        MMGC_STATIC_ASSERT( (1 << kPageShift) == kPageSize );

        // PageMapBase collects a few members shared amongst all page maps
        class PageMapBase
         {
         public:
             /** @return lower bound (inclusive) of page map address range. */
             uintptr_t MemStart() const;

             /** @return upper bound (exclusive) in page map address range. */
             uintptr_t MemEnd() const;

             /** @return true iff addr is in domain of page map. */
             bool AddrIsMappable(uintptr_t addr) const;
        protected:
            uintptr_t memStart;
            uintptr_t memEnd;
            PageMapBase();
        private:
            // (prevent introduction of default copiers)
            PageMapBase(PageMapBase&);
            PageMapBase& operator=(PageMapBase&);
        };

        // NOTE: the public methods of the Uniform pagemap class are
        // the interface that client code should be coded against;
        // every pagemap implementation must support the public
        // methods provided by the Uniform implementation (and should
        // eschew adding new public methods in release code).

        // FIXME: remove Uniform (but 1st copy its specs to other PageMap impl)
        // See: https://bugzilla.mozilla.org/show_bug.cgi?id=588878
#ifdef MMGC_USE_UNIFORM_PAGEMAP
        // Uniform is the original simple (but poorly scaling) implementation
        class Uniform : private PageMapBase
        {
        public:
            Uniform();

            // adjust access (aka "re-export") utilty methods.
            using PageMapBase::MemStart;
            using PageMapBase::MemEnd;
            using PageMapBase::AddrIsMappable;

            /**
             * Destroy method.  Must invoke before destruction.
             * @param heap that was used for allocation via ExpandSetAll.
             */
            void DestroyPageMapVia(GCHeap *heap);

            /** @return value for addr in page map. */
            PageType AddrToVal(uintptr_t addr) const;

            /**
             * Ensures AddrIsMappable for all addrs in [item,item+numpages),
             * mapping each addr to val.
             * @param h heap from which to allocate new internal state.
             * @param item first address of range added; must be page-aligned.
             * @param numpages breadth of address range added to domain, in pages.
             * @param val to associate with each addr in the range.
             */
            void ExpandSetAll(GCHeap *h, void *item, uint32_t numpages, PageType val);

            /** Clears entry for all addrs in [item,item+numpages). */
            void ClearAddrs(void *item, uint32_t numpages);

        private:
            // Need to extract the byte (via indexing into byte map),
            // shift it so payload is in low bits, and mask out the
            // payload.
            //
            // Generally, idx = f(addr), where f is appropriate
            // subtract-and-shift to convert addr into byte array index
            // into the pageMap.
            //
            // So:
            //   payload = (pageMap[idx >> IdxShift] >> ByteShift) & Mask
            // where
            //
            // PayloadPerByte = 8/payloadbits
            // IdxShift   = log_2(PayloadPerByte)
            //            = 3 - log_2(payloadbits)
            // ByteShift  = (idx & (PayloadPerByte - 1))*payloadbits
            // Mask       = 2^payloadbits - 1
            //
            // For example:
            //
            // Payload
            // Bits   IdxShift  ByteShift       Mask
            // 1      3         (idx & 0x7)*1   0x1  (trivial case)
            // 2      2         (idx & 0x3)*2   0x3  (relevant case for now)
            // 4      1         (idx & 0x1)*4   0xf
            // 8      0         0               0xff (trivial case)

            // (helper implements ExpandSetAll via EnsureCapacity and AddrSet
            //  methods of PM)
            template<typename PM>
            friend void SimpleExpandSetAll(PM *pagemap, GCHeap *heap,
                                           void *item, uint32_t numpages,
                                           PageType val);

            // (helper implements ClearAddrs via AddrClear method of PM)
            template<typename PM>
            friend void SimpleClearAddrs(PM *pm,void *item, uint32_t numpages);

            /** Ensures AddrIsMappable for all addrs in [item,item+numpages).*/
            void EnsureCapacity(GCHeap *heap, void *item, uint32_t numpages);

            /** Associates val with key addr; requires AddrIsMappable(addr).*/
            void AddrSet(uintptr_t addr, PageType val);

            /** Clears entry for addr; requires AddrIsMappable(addr). */
            void AddrClear(uintptr_t addr);

            /**
             * @param addr key for future lookup; assumes AddrIsMappable.
             * @return abstract index suitable for passing to Index methods.
             * Note that EnsureCapacity invocations invalidate these indexes.
             */
            uintptr_t AddrToIndex(uintptr_t addr) const;

            /** @return payload value for idx in page map. */
            PageType  IndexToVal(uintptr_t idx) const;

            /** Modifies payload at idx to map to val. */
            void IndexSet(uintptr_t idx, PageType val);

            /** @param idx : which value to clear in page map. */
            void IndexClear(uintptr_t idx);

            /** @return ByteShift (see notes above). */
            uint32_t IndexToByteShiftAmt(uintptr_t index) const;

            /** @return byte in page map for index (see notes above). */
            uint8_t IndexToByte(uintptr_t index) const;

            /** @return byte ref in page map for index (see notes above). */
            uint8_t& IndexToByteRef(uintptr_t index);

#ifdef DEBUG
            /** @return true if index is reasonable for host address space. */
            static bool IndexSaneForAddressRange(uintptr_t index);
#endif

            uint8_t *pageMap;
        };
#endif // MMGC_USE_UNIFORM_PAGEMAP

#if ! defined(MMGC_USE_UNIFORM_PAGEMAP) && ! defined(MMGC_64BIT)
        // Tiered2 is two level tree; leaves are bitmaps w/ payload.
        // Scalable on 32-bit systems but not 64-bit ones.
        // Fairly simplistic; assumes EnsureCapacity calls imply high
        // chance of assigning non-zero payloads to addrs in range.
        class Tiered2 : protected PageMapBase
        {
            // Goal: interchangable interface with Uniform, but more
            // sparsely structured, to cover 32-bit address range.
        protected:
            static const uint32_t tier1_nbits = 6;
            static const uint32_t tier2_nbits = 12;

            // 12 : kPageShift, covers 4096 byte page
            //  2 : index a bit-pair in a byte
            //
            // sum of two tiers + 2 + 12 should be exactly 32, the
            // upper-bound on address width for 32-bit systems:

            // values of below constants derive from above bit-widths.
            // tier 2 must yield page-sized units (static-asserted below).
            static const uint32_t tier1_entries = 1 << tier1_nbits;
            static const uint32_t tier2_entries = 1 << tier2_nbits;

            static const uint32_t tier2_pages = tier2_entries*sizeof(uint8_t) / GCHeap::kBlockSize;
            MMGC_STATIC_ASSERT(tier2_pages * GCHeap::kBlockSize == tier2_entries*sizeof(uint8_t));

            static const uintptr_t tier2_shift = kPageShift+2;
            static const uint32_t tier2_postshift_mask = (1 << tier2_nbits)-1;
            static const uintptr_t tier1_shift = tier2_shift + tier2_nbits;
            // Doc for public methods: see above.
        public:
            Tiered2();

            // adjust access (aka "re-export") utilty methods.
            using PageMapBase::MemStart;
            using PageMapBase::MemEnd;
            using PageMapBase::AddrIsMappable;

            void DestroyPageMapVia(GCHeap *heap);
            PageType AddrToVal(uintptr_t addr) const;
            void ExpandSetAll(GCHeap *h, void *item, uint32_t np, PageType val);
            void ClearAddrs(void *item, uint32_t numpages);
        protected:
            // (helper implements ExpandSetAll via EnsureCapacity and AddrSet)
            template<typename PM>
            friend void SimpleExpandSetAll(PM*,GCHeap*,void*,uint32_t,PageType);
            // (helper implements ClearAddrs via AddrClear)
            template<typename PM>
            friend void SimpleClearAddrs(PM*,void*,uint32_t);

            void AddrSet(uintptr_t addr, PageType val);
            void AddrClear(uintptr_t addr);
            void EnsureCapacity(GCHeap *heap, void *item, uint32_t numpages);
            uint8_t* AddrToLeafBytes(uintptr_t addr) const;
            PageType LeafAddrToVal(uint8_t* bytes, uintptr_t addr) const;
            void LeafAddrSet(uint8_t* bytes, uintptr_t addr, PageType val);
            void LeafAddrClear(uint8_t* bytes, uintptr_t addr);

            uint32_t AddrToIndex1(uintptr_t addr) const;
            uint32_t AddrToIndex2(uintptr_t addr) const;
            uint32_t AddrToByteShiftAmt(uintptr_t addr) const;
        protected:

            // since tier1_entries is small, it seems reasonable
            // to allocate its array inline as a member of this class.
            uint8_t *pageMap[tier1_entries];
        };
#endif // ! defined(MMGC_USE_UNIFORM_PAGEMAP) && ! defined(MMGC_64BIT)

#if ! defined(MMGC_USE_UNIFORM_PAGEMAP) && defined(MMGC_64BIT)
        // Tiered4 is four level tree; leaves are bitmaps w/ payload.
        // Fairly simplistic; assumes EnsureCapacity calls imply high
        // chance of assigning non-zero payloads to addrs in range.
        class Tiered4 : protected PageMapBase
        {
            // Goal: interchangable interface with Uniform, but more
            // sparse than Tiered2, to cover 64-bit address range.
        protected:
            // tier0 inlined into member itself (0 is sound here;
            // however code not optimized for tier0_nbits = 0 case)
            static const uint32_t tier0_nbits =  3;
            // remaining tiers are allocated in page-sized units.
            static const uint32_t tier1_nbits =  9;
            static const uint32_t tier2_nbits =  9;
            static const uint32_t tier3_nbits = 13;

            // 12 : covers 4096 byte page
            //  2 : index a bit-pair in a byte
            //
            // sum of all three + 2 + 12 should be at least 48, the
            // initial (but not for-all-time) upper-bound on address width:
            //
            //   AMD64 Volume 2, page 118: "Currently, the AMD64
            //   architecture defines a mechanism for translating
            //   48-bit virtual addresses to 52-bit physical
            //   addresses. The mechanism used to translate a full
            //   64-bit virtual address is reserved and will be
            //   described in a future AMD64 architectural
            //   specification."

            // values of below constants derive from above bit-widths.
            // tiers >= 1 must yield page-sized units (static-asserted below).
            static const uint32_t tier0_entries = 1 << tier0_nbits;
            static const uint32_t tier1_entries = 1 << tier1_nbits;
            static const uint32_t tier2_entries = 1 << tier2_nbits;
            static const uint32_t tier3_entries = 1 << tier3_nbits;

            static const uint32_t tier1_pages = tier1_entries*sizeof(uint8_t**) / GCHeap::kBlockSize;
            static const uint32_t tier2_pages = tier2_entries*sizeof(uint8_t*) / GCHeap::kBlockSize;
            static const uint32_t tier3_pages = tier3_entries*sizeof(uint8_t) / GCHeap::kBlockSize;

            MMGC_STATIC_ASSERT(tier1_pages * GCHeap::kBlockSize == tier1_entries*sizeof(uint8_t**));
            MMGC_STATIC_ASSERT(tier2_pages * GCHeap::kBlockSize == tier2_entries*sizeof(uint8_t*));
            MMGC_STATIC_ASSERT(tier3_pages * GCHeap::kBlockSize == tier3_entries*sizeof(uint8_t));

            static const uintptr_t tier3_shift = kPageShift+2;
            static const uint32_t tier3_postshift_mask = (1 << tier3_nbits)-1;
            static const uintptr_t tier2_shift = tier3_shift + tier3_nbits;
            static const uint32_t tier2_postshift_mask = (1 << tier2_nbits)-1;
            static const uintptr_t tier1_shift = tier2_shift + tier2_nbits;
            static const uint32_t tier1_postshift_mask = (1 << tier1_nbits)-1;
            static const uintptr_t tier0_shift = tier1_shift + tier1_nbits;
            // Doc for public methods: see above.
        public:
            Tiered4();

            // adjust access (aka "re-export") utilty methods.
            using PageMapBase::MemStart;
            using PageMapBase::MemEnd;
            using PageMapBase::AddrIsMappable;

            void DestroyPageMapVia(GCHeap *heap);
            PageType AddrToVal(uintptr_t addr) const;
            void ExpandSetAll(GCHeap *h, void *item, uint32_t np, PageType val);
            void ClearAddrs(void *item, uint32_t numpages);
        protected:
            // (helper implements ExpandSetAll via EnsureCapacity and AddrSet)
            template<typename PM>
            friend void SimpleExpandSetAll(PM*,GCHeap*,void*,uint32_t,PageType);
            // (helper implements ClearAddrs via AddrClear)
            template<typename PM>
            friend void SimpleClearAddrs(PM*,void*,uint32_t);

            void AddrSet(uintptr_t addr, PageType val);
            void AddrClear(uintptr_t addr);
            void EnsureCapacity(GCHeap *heap, void *item, uint32_t numpages);
            uint8_t* AddrToLeafBytes(uintptr_t addr) const;
            PageType LeafAddrToVal(uint8_t* bytes, uintptr_t addr) const;
            void LeafAddrSet(uint8_t* bytes, uintptr_t addr, PageType val);
            void LeafAddrClear(uint8_t* bytes, uintptr_t addr);

            static uint32_t AddrToIndex0(uintptr_t addr);
            static uint32_t AddrToIndex1(uintptr_t addr);
            static uint32_t AddrToIndex2(uintptr_t addr);
            static uint32_t AddrToIndex3(uintptr_t addr);
            static uint32_t AddrToByteShiftAmt(uintptr_t addr);

            /** initalizes pageMap for half-open range [start,limit). */
            void InitPageMap(GCHeap *heap, uintptr_t start, uintptr_t limit);
        protected:
            uint8_t ***pageMap[tier0_entries];
        };

        // CacheT4 is like Tiered4 but caches most-recently used leaf.
        class CacheT4 : protected Tiered4
        {
        public:
            CacheT4();

            // adjust access (aka "re-export") cache-ignorant methods.
            using Tiered4::MemStart;
            using Tiered4::MemEnd;
            using Tiered4::AddrIsMappable;
            using Tiered4::DestroyPageMapVia;

            PageType AddrToVal(uintptr_t addr) const;
            void ExpandSetAll(GCHeap *h, void *item, uint32_t np, PageType val);
            void ClearAddrs(void *item, uint32_t numpages);
        protected:
            // (helper implements ExpandSetAll via EnsureCapacity and AddrSet)
            template<typename PM>
            friend void SimpleExpandSetAll(PM*,GCHeap*,void*,uint32_t,PageType);
            // (helper implements ClearAddrs via AddrClear)
            template<typename PM>
            friend void SimpleClearAddrs(PM*,void*,uint32_t);
            void AddrSet(uintptr_t addr, PageType val);
            void AddrClear(uintptr_t addr);
            using Tiered4::EnsureCapacity;

            /**
             * Extracts prefix for (tiers 1+2) of addr.  Used for
             * comparsion with (and assignment to) cached_addr_prefix.
             */
            static uintptr_t AddrPrefix(uintptr_t addr);

            /**
             * @return true only if cache matches addr; false otherwise.
             */
            bool CacheHit(uintptr_t addr) const;

            /**
             * Attempts lookup of addr in pagemap.
             * @return status flag where true implies cache now
             *  matches addr and false implies pagemap[addr]==0
             * (Note that status of cache itself is unspecified with
             * false return.)
             */
            bool UpdateCache(uintptr_t addr) const;

            // uncached sentinel value; see below
            // (valid prefixes have lower tier3_nbits + 14 bits = 0).
            static const uintptr_t uncached = uintptr_t(-1);
            // If not uncached, then is first tier1_nbits and tier2_nbits ...
            mutable uintptr_t cached_addr_prefix;
            // ... of addresses of this bitmap.
            mutable uint8_t * cached_leaf_bytes;
        };

        // DelayT4 is like CacheT4 but uses cache as main store until
        // allocating rest of tree is unavoidable.
        class DelayT4 : protected CacheT4
        {
        public:
            DelayT4();

            // adjust access (aka "re-export") non-caching/delaying methods
            using CacheT4::MemStart;
            using CacheT4::MemEnd;
            using CacheT4::AddrIsMappable;
            using CacheT4::AddrToVal;
            using CacheT4::ClearAddrs;

            void DestroyPageMapVia(GCHeap *heap);
            void ExpandSetAll(GCHeap *h, void *item, uint32_t np, PageType val);

        protected:
            // (helper implements ExpandSetAll via EnsureCapacity and AddrSet)
            template<typename PM>
            friend void SimpleExpandSetAll(PM*,GCHeap*,void*,uint32_t,PageType);

            void EnsureCapacity(GCHeap *heap, void *item, uint32_t numpages);
            using CacheT4::AddrSet;
            using CacheT4::AddrClear;
        private:
            void SetupDelayedPagemap(GCHeap *heap,
                                     uintptr_t addr, uintptr_t addr_lim);
            bool stillInitialDelay; // true iff only cache exists.
        };
#endif // ! defined(MMGC_USE_UNIFORM_PAGEMAP) && defined(MMGC_64BIT)
    }
}

#endif /* __MMgc__PageMap__ */

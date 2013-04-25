/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCAlloc__
#define __GCAlloc__


namespace MMgc
{
    class GCAllocBase {

    friend class GC;

    public:
        virtual ~GCAllocBase();

    private:
        virtual void Free(const void* item) = 0;
    };

    // Some common functionality for GCAlloc and GCLargeAlloc follows.  (Could be
    // in a separate header file.)

    // Mark bit item.
    typedef uint8_t gcbits_t;

    /**
     * Common block header for GCAlloc and GCLargeAlloc.
     */
    struct GCBlockHeader
    {
        uint8_t         bibopTag;   // *MUST* be the first byte.  0 means "not a bibop block."  For others, see core/atom.h.
        uint8_t         bitsShift;  // Right shift for lower 12 bits of a pointer into the block to obtain the mark bit item for that pointer
                                    // bitsShift is only used if MMGC_FASTBITS is defined but its always present to simplify header layout.
        uint8_t         containsPointers;   // nonzero if the block contains pointer-containing objects
        uint8_t         rcobject;           // nonzero if the block contains RCObject instances
        uint32_t        size;   // Size of objects stored in this block
        GC*             gc;     // The GC that owns this block
        GCAllocBase*    alloc;  // the allocator that owns this block
        GCBlockHeader*  next;   // The next block in the list of blocks for the allocator
        gcbits_t*       bits;   // Variable length table of mark bit entries
    };

    GCBlockHeader* GetBlockHeader(const void* item);

    /**
     * GC bits.  These are common to small and large objects.
     * Large objects have additional bits, stored elsewhere.
     * Small objects combine kMark and kQueued into kFreelist.
     */
    enum
    {
        kMark=1,                // object has been marked
        kQueued=2,              // object is on the mark or barrier queues
        kFinalizable=4,         // object's destructor must be called when the object is destroyed
        kHasWeakRef=8,          // there's an entry for the object in the weakRefs table
        kVirtualGCTrace = 16    // object derived from GCTraceableBase and has gcTrace override(s), see GCObject.h
        // free: 32
        // free: 64
        // free: 128
    };

    /**
     *
     * This is a fast, fixed-size memory allocator for garbage-collected
     * objects.
     *
     * We observe the following invariant:
     *
     * If the size managed by the allocator is divisible by 2^k and k >= 3 then
     * the alignment of an object returned by Alloc is on a 2^k byte boundary.
     *
     * Note that any space for Debug headers is added outside GCAlloc, and that
     * GC::Alloc has a weaker invariant in Debug builds, guaranteeing at most
     * 16 bytes of alignment for k >= 4.
     *
     * Memory is allocated from the system on 4096-byte aligned boundaries,
     * which corresponds to the size of an OS page in Windows XP.  Allocation
     * of pages is performed via the GCPageAlloc class.
     *
     * In each 4096-byte block, there is a block header with marked bits,
     * finalize bits, the pointer to the next free item and "recycling"
     * free item linked list.
     *
     * The bits of the "marked" bitmap are controlled by the SetMark method.
     *
     * The bits of the "finalize" bitmap are set when an item is
     * allocated.  The value for the finalize bit is passed in as a
     * parameter to the allocation call.
     *
     * When the Sweep method is invoked, all objects that are not marked
     * with the specified mark flag are disposed of.  If the corresponding
     * finalize bit is set, the GCObject destructor is invoked on that
     * item.
     *
     * When an allocation is requested and there are no more free
     * entries, GCAlloc will request that a garbage collection take
     * place.  It will allocate new blocks if more than 20% of its
     * blocks are used after the collection, targeting a 5:1
     * heap size / minimim heap size ratio.
     *
     */
    class GCAlloc : public GCAllocBase
    {
        friend class GC;
        friend class GCAllocIterator;
        friend class ZCT;

    public:
        // The destructor needs to be public because it is called explicitly by
        // the implementation of one of the mmfx_delete macros.
        ~GCAlloc();

    private:
        struct GCBlock;

        enum { kFreelist=kMark|kQueued };

        GCAlloc(GC* gc, int itemSize, bool containsPointers, bool isRC, bool isFinalized, int sizeClassIndex, uint8_t bibopTag);

#if defined DEBUG || defined MMGC_MEMORY_PROFILER
        void* Alloc(size_t size, int flags);
#else
        void* Alloc(int flags);
#endif
        virtual void Free(const void* item);

        void Finalize();
        void FinalizationPass();
        void LazySweepPass();
        void ClearMarks();
#ifdef _DEBUG
        void CheckMarks();
        void CheckFreelist();
        static bool IsWhite(const void *item);
#endif // _DEBUG

        static void *FindBeginning(const void *item);
        static bool IsUnmarkedPointer(const void *val);
        static void SetBlockHasWeakRef(const void *userptr);

        // Return the actual size of items managed by this allocator (includes debugging overheads)
        REALLY_INLINE uint32_t GetItemSize() { return m_itemSize; }
        
        // Return the number of live objects owned by this allocator in *numAlloc and
        // the number of objects (live and free) owned by this allocator in *maxAlloc.
        void GetAllocStats(int& numAlloc, int& maxAlloc) const;

        REALLY_INLINE int GetNumAlloc() const
        {
            int numAlloc, maxAlloc;
            GetAllocStats(numAlloc, maxAlloc);
            return numAlloc;
        }

        REALLY_INLINE bool ContainsPointers() const { return containsPointers; }
        REALLY_INLINE bool ContainsRCObjects() const { return containsRCObjects; }

        void GetBitsPages(void **pages);

        //This method is for more fine grained allocation details
        //It reports the total number of bytes requested (i.e. ask size) and
        //the number of bytes actually allocated.
        void GetUsageInfo(size_t& totalAskSize, size_t& totalAllocated);

#ifdef MMGC_MEMORY_PROFILER
        size_t GetTotalAskSize() { return m_totalAskSize; }
#endif
        size_t GetTotalAllocatedBytes() { return m_totalAllocatedBytes; }
    private:
        const static int kBlockSize = 4096;         // This must be the same as GCHeap::kBlockSize, we check it in GCAlloc::GCAlloc

        const static short kFlagNeedsSweeping = 1;  // set if the block had finalized objects and needs to be swept
        const static short kFlagWeakRefs = 2;       // set if the block may have weak refs and we should check during free

        // Objects on the free list all have a next pointer in the first word and
        // the object index within its block as the second word.  Only the low 16 bits
        // of the second word are significant: the reference counter messes around
        // with the high 16 bits during pinning; it may pin dead objects.  Those that
        // use the index must be careful to mask off the high bits.

        struct GCBlock : public GCBlockHeader
        {
            GCBlock* prev;          // the previous block on the list of all blocks ('next' is in the block header)
            void*  firstFree;       // first item on free list
            GCBlock *prevFree;      // the previous block on the lists of blocks with free or sweepable objects
            GCBlock *nextFree;      // the next block on the lists of blocks with free or sweepable objects
            short numFree;          // the number of free objects in this block
            uint8_t slowFlags;      // flags for special circumstances: kFlagNeedsSweeping, etc
            bool finalizeState:1;   // whether we've been visited during the Finalize stage
            char   *items;          // pointer to the array of objects in the block

            int GetCount() const;
            void FreeSweptItem(const void *item, int index);
            int needsSweeping();
            void setNeedsSweeping(int v);
        };

        static GCBlock *GetBlock(const void *item);

#ifdef MMGC_MEMORY_INFO
        static void VerifyFreeBlockIntegrity(void* item, uint32_t size, uint32_t limit=~0U);
#endif

        // The list of chunk blocks
        GCBlock* m_firstBlock;
        GCBlock* m_lastBlock;

        // The lowest priority block that has free items
        GCBlock* m_firstFree;

        // List of blocks that need sweeping
        GCBlock* m_needsSweeping;

        // Quick list of free objects.  See comment in GCAlloc.cpp for general information.

        void *m_qList;              // Linked list of some free objects for this allocator
        int   m_qBudget;            // Number of items we can yet free before obtaining a larger budget
        int   m_qBudgetObtained;    // Quick list budget actually obtained from the GC for this allocator
        const uint32_t    m_itemSize;
        const int    m_itemsPerBlock;
        size_t m_totalAllocatedBytes;
#ifdef MMGC_FASTBITS
        // Right shift for lower 12 bits of a pointer into the block to obtain
        // the gcbits_t item for that pointer.  Is copied into the block header.

        // The idea here is that the byte table may have some unused entries and that
        // that allows a unique byte index for an object to be computed with a simple
        // mask and shift off the object pointer (the mask is constant, the shift is
        // variable).  That benefits the write barrier, the marker (especially precise
        // marking), and other hot code.  Code that walks the byte map (eg the sweeper)
        // should do so by stepping through objects one by one and computing the byte
        // index for each (this is cheap) rather than examining bytes in the byte map
        // in sequence.
        //
        // The number of bytes required for objects of size n is the same as for
        // objects of size m where m is the next lower power-of-two object size
        // below n.  The shift for n is then the same as the shift for m: log2(m).
        //
        // The key is that a unique byte index will be assigned to each object position
        // in the block even for non-power-of-two object sizes.  I don't have a mathematical
        // proof for this, but it's easy to test it exhaustively (and I've done so).
        //
        // The amount of waste in the byte map is at most 50%, but average waste for
        // object sizes up to 256 is 26%.  Waste is a little higher than that, because
        // the bitmap is sized to cover the block, including the block header - that
        // too removes instructions from the hot path later.
        const uint32_t m_bitsShift;
#endif
        const int m_numBitmapBytes;
        const int m_sizeClassIndex;

#ifdef MMGC_MEMORY_PROFILER
        size_t m_totalAskSize;
#endif

        const bool m_bitsInPage;
        uint8_t  m_bibopTag;

        // fast divide numbers for GetObjectIndex
        const uint16_t multiple;
        const uint16_t shift;

        const bool containsPointers;
        const bool containsRCObjects;
        const bool containsFinalizedObjects;
        bool m_finalized;

#ifdef _DEBUG
        bool IsOnEitherList(GCBlock *b);
        void VerifyNotFree(GCBlock* b, const void *item);
#endif

        GCBlock* CreateChunk(int flags);
        void UnlinkChunk(GCBlock *b);
        void FreeChunk(GCBlock* b);

        // not a hot method
        void AddToFreeList(GCBlock *b);

        // not a hot method
        void RemoveFromFreeList(GCBlock *b);

        // not a hot method
        void AddToSweepList(GCBlock *b);

        // not a hot method
        void RemoveFromSweepList(GCBlock *b);

#if defined DEBUG || defined MMGC_MEMORY_PROFILER
        void* AllocSlow(size_t askSize, int flags);
        void* AllocFromQuickList(size_t askSize, int flags);
#else
        void* AllocSlow(int flags);
        void* AllocFromQuickList(int flags);
#endif
        void FillQuickList(GCBlock* b);
        void CoalesceQuickList();
        void QuickListBudgetExhausted();
        void FreeSlow(GCBlock* b, int index, const void* item);
        void ClearNonRCObject(void* item, size_t size);

        bool Sweep(GCBlock *b);
        void SweepGuts(GCBlock *b);

        void ClearMarks(GCAlloc::GCBlock* block);
        void SweepNeedsSweeping();

#ifdef _DEBUG
        static bool IsPointerIntoGCObject(const void *item);
        static int ConservativeGetMark(const void *item, bool bogusPointerReturnValue);
#endif

        // NOTE that GetObjectIndex and GetBitsIndex are not the same if MMGC_FASTBITS
        // is defined.  That is an efficiency concern; the representation of the bits
        // table allows the bits index to be computed more quickly, and that shortens
        // the hot control paths.  Both functions are very hot.

        // Compute the offset within the block of the given object.
        static uint32_t GetObjectIndex(const GCBlock *block, const void *item);

        // Compute the offset within the bits table of the given object.
        static uint32_t GetBitsIndex(const GCBlock *block, const void *item);
        
#ifndef MMGC_FASTBITS
        static gcbits_t& GetGCBits(const void* realptr);
#endif

        static void ClearBits(GCBlock *block, int index, int bits);

        // not a hot method
        static void ClearQueued(const void *userptr);

        static uint16_t ComputeMultiply(uint16_t d);
        static uint16_t ComputeShift(uint16_t d);

    private:
        GC* const m_gc;

    private:
        static GCBlock* Next(GCBlock* b);
    };

    /**
     * A utility class used by the marker to handle mark stack overflow: it abstracts
     * iterating across marked, non-free objects in one allocator instance.
     *
     * No blocks must be added or removed during the iteration.  If an object's
     * bits are changed, those changes will visible to the iterator if the object has
     * not yet been reached by the iteration.
     */
    class GCAllocIterator
    {
    public:
        GCAllocIterator(MMgc::GCAlloc* alloc);

        bool GetNextMarkedObject(void*& out_ptr);

    private:
        GCAlloc* const alloc;
        GCAlloc::GCBlock* block;
        uint32_t idx;
        uint32_t limit;
        uint32_t size;
    };
}

#endif /* __GCAlloc__ */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCLargeAlloc__
#define __GCLargeAlloc__

namespace MMgc
{
    /**
     * This is a garbage collecting allocator for large memory blocks.
     */
    class GCLargeAlloc : public GCAllocBase
    {
        friend class GC;
        friend class GCLargeAllocIterator;
    private:

        // Additional per-object flags for large objects.  These are stored in flags[1].
        // Note that the marker (GC::MarkItem_*) knows about flags[1], but otherwise the 
        // knowledge is confined to GCLargeAlloc code.

        enum {
            kProtected        = 0x01    // Object is protected from Free, see comments around GC::MarkItem
        };

    public:
        GCLargeAlloc(GC* gc);
        ~GCLargeAlloc();

#if defined DEBUG || defined MMGC_MEMORY_PROFILER
        void* Alloc(size_t originalSize, size_t requestSize, int flags);
#else
        void* Alloc(size_t requestSize, int flags);
#endif
        virtual void Free(const void *ptr);

        void Finalize();
        void ClearMarks();

        static bool IsLargeBlock(const void *item);

#ifdef _DEBUG
        static bool IsWhite(const void *item);
#endif
        static void ProtectAgainstFree(const void *item);

        static void UnprotectAgainstFree(const void *item);

        static bool IsProtectedAgainstFree(const void *item);

        static void* FindBeginning(const void *item);

        //This method returns the number bytes allocated by FixedMalloc
        size_t GetBytesInUse();

        //This method is for more fine grained allocation details
        //It reports the total number of bytes requested (i.e. ask size) and
        //the number of bytes actually allocated.  The latter is the same
        //number as reported by GetBytesInUse()
        void GetUsageInfo(size_t& totalAskSize, size_t& totalAllocated);

        size_t GetTotalAllocatedBytes() { return m_totalAllocatedBytes; }
    private:
        // This can subclass GCBlockHeader because the byte map is not variable length:
        // LargeBlock contains exactly the space we need for the mark bits for
        // the large object.

        struct LargeBlock : public GCBlockHeader
        {
            // We use flags[0] for the standard GC bits and flags[1] for additional
            // large object bits.
            //
            // Static checks in GC.cpp test that sizeof(gcbits_t) == 1 and that LargeBlock
            // alignment is 8 bytes.
            gcbits_t flags[4];
#ifndef MMGC_64BIT
            uint32_t padding;    // Pad to 8-byte aligned.
#endif
            int GetNumBlocks() const;

            // returns a pointer to the object (realptr in GetUserPointer/GetRealPointer duality)
            void* GetObject() const;
        };

        static LargeBlock* GetLargeBlock(const void *addr);

        // not a hot method
        static void ClearQueued(const void *userptr);

#ifndef MMGC_FASTBITS
        static gcbits_t& GetGCBits(const void* realptr);
#endif

        // The list of chunk blocks
        LargeBlock* m_blocks;
#ifdef MMGC_MEMORY_PROFILER
        size_t m_totalAskSize;
#endif

        bool m_startedFinalize;
        size_t m_totalAllocatedBytes;

#ifdef _DEBUG
        static bool ConservativeGetMark(const void *item, bool bogusPointerReturnValue);
#endif

    protected:
        GC *m_gc;

    public:
        static LargeBlock* Next(LargeBlock* b);
    };

    /**
     * A utility class used by the marker to handle mark stack overflow: it abstracts
     * iterating across marked, non-free objects in one allocator instance.
     *
     * No blocks must be added or removed during the iteration.  If an object's
     * bits are changed, those changes will visible to the iterator if the object has
     * not yet been reached by the iteration.
     */
    class GCLargeAllocIterator
    {
    public:
        GCLargeAllocIterator(MMgc::GCLargeAlloc* alloc);

        bool GetNextMarkedObject(void*& out_ptr);

    private:
        GCLargeAlloc* const alloc;
        GCLargeAlloc::LargeBlock* block;
    };
}

#endif /* __GCLargeAlloc__ */

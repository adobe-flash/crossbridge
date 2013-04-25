/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __Malloc__
#define __Malloc__

namespace avmplus {
    namespace ST_mmgc_fixedmalloc_findbeginning {
        class ST_mmgc_fixedmalloc_findbeginning;
    }
}

namespace MMgc
{
    /**
     * A general purpose memory allocator.
     *
     * FixedMalloc is a singleton, obtainable by calling FixedMalloc::GetFixedMalloc.
     * The "owner" of the FixedMalloc is GCHeap, the block manager.
     *
     * FixedMalloc uses size classes; each size class is handled by a FixedAllocSafe
     * instance.  Large objects are handled specially.  All objects are headerless.
     */
    class FixedMalloc
    {
        friend class GCHeap;
        friend class GCRoot; // expose FindBeginningAndSize to GCRoot
        friend class avmplus::ST_mmgc_fixedmalloc_findbeginning::ST_mmgc_fixedmalloc_findbeginning;

    public:
        /**
         * Obtain a FixedMalloc instance.
         *
         * @return the FixedMalloc singleton.
         */
        static FixedMalloc *GetFixedMalloc();

        /**
         * Obtain a FixedMalloc instance.
         *
         * @return the FixedMalloc singleton.
         *
         * @note Backward compatible name for GetFixedMalloc, not used by Tamarin.
         */
        static FixedMalloc *GetInstance();

        /**
         * Allocate one object from this allocator.
         *
         * @param size   The size of the object.
         * @param flags  A bit vector of allocation options.
         *
         * @return  A pointer to the object.  The pointer may be NULL only if kCanFail is
         *          part of flags.  The memory is zeroed only if kZero is part of flags.
         */
        void* Alloc(size_t size, FixedMallocOpts flags=kNone);

        /**
         * Allocate one object from this allocator, may return NULL.
         *
         * @param size   The size of the object.
         *
         * @return  A pointer to the object.  The pointer may be NULL.
         *          The memory is /not/ necessarily zeroed; for zeroed memory, use Alloc().
         *
         * @note Exactly like Alloc with flags=kCanFail.
         */
        void *PleaseAlloc(size_t size);

        /**
         * Allocate one object from this allocator.
         *
         * @param size   The size of the object.
         * @param flags  A bit vector of allocation options.
         *
         * @return  A pointer to the object.  The pointer may be NULL only if kCanFail is
         *          part of flags.  The memory is zeroed only if kZero is part of flags.
         *
         * @note Exactly like Alloc, but guaranteed not to be inlined - used by ::new etc.
         */
        void* FASTCALL OutOfLineAlloc(size_t size, FixedMallocOpts flags=kNone);

        /**
         * Allocate space for an array of objects from this allocator.
         *
         * @param count  The number of objects
         * @param size   The size of each part object
         * @param flags  A bit vector of allocation options
         *
         * @return  A pointer to the aggregate object.  The pointer may be NULL only if
         *          kCanFail is part of flags.  The memory is zeroed only if kZero is part
         *          of flags.
         *
         * @note  Unlike 'calloc' in the C library, this does /not/ zero the memory
         *        unless kZero is passed in flags.  The name 'Calloc' comes from the
         *        shape of the API.
         */
        void *Calloc(size_t count, size_t size, FixedMallocOpts flags=kNone);

        /**
         * Free an object allocated through FixedMalloc.
         *
         * @param item  The object to free.
         */
        void Free(void *item);

        /**
         * Free an object allocated through FixedMalloc.
         *
         * @param item  The object to free.
         *
         * @note  Exactly like Free, but guaranteed not to be inlined - used by ::delete etc.
         */
        void FASTCALL OutOfLineFree(void* p);

        /**
         * Obtain the size of an object allocated through FixedMalloc.
         *
         * @param  item  An object reference.
         *
         * @return the allocated size of 'item'
         */
        size_t Size(const void *item);

        /**
         * Obtain FixedMalloc's heap usage.
         *
         * @return The total number of /blocks/ managed by FixedMalloc, where the
         *         block size is given by GCHeap::kBlockSize.
         */
        size_t GetTotalSize();

        /**
         * Obtain current (not running total) allocation information for FixedMalloc.
         *
         * @param totalAskSize    (out) The total number of bytes requested
         * @param totalAllocated  (out) The number of bytes actually allocated
         */
        void GetUsageInfo(size_t& totalAskSize, size_t& totalAllocated);

        /**
         * Obtain current allocation information for FixedMalloc.
         *
         * @return the number bytes currently allocated by FixedMalloc.
         *
         * @note  The returned value is the totalAllocated value returned from GetUsageInfo.
         */
        size_t GetBytesInUse();

#ifdef MMGC_MEMORY_PROFILER
        /**
         * Print semi-structured human-readable data about FixedMalloc memory usage
         * on the VMPI_log channel.
         */
        void DumpMemoryInfo();
#endif

    private:
#ifdef DEBUG
        // Data type used for tracking live large objects, used by EnsureFixedMallocMemory.
        struct LargeObject;
#endif

        // Return true if item is a large-object item allocated through FixedMalloc.
        static bool IsLargeAlloc(const void *item);
        
        // Initialize FixedMalloc.  Must be called from GCHeap during GCHeap setup.
        void InitInstance(GCHeap *heap);

        // Destroy FixedMalloc and free all resources.
        void DestroyInstance();

        // Return the total number of blocks allocated by FixedMalloc for large-object allocations.
        size_t GetNumLargeBlocks();

        // Record that 'blocksAllocated' blocks are about to be or have been allocated 
        // in support of large-object allocation.
        void UpdateLargeAllocStats(void* item, size_t blocksAllocated);
        
        // Record that 'blocksFreed' blocks are about to be or have been freed 
        // in support of large-object freeing.
        void UpdateLargeFreeStats(void* item, size_t blocksFreed);

#ifdef DEBUG
        // Check that item was allocated by an allocator owned by this FixedMalloc,
        // otherwise trigger an assertion failure.
        void EnsureFixedMallocMemory(const void* item);

#ifndef AVMPLUS_SAMPLER
        // Track large object 'item', which is newly allocated.
        void AddToLargeObjectTracker(const void* item);

        // Untrack large object 'item', which is about to be freed.
        void RemoveFromLargeObjectTracker(const void* item);
#endif // !AVMPLUS_SAMPLER
#endif // DEBUG

        // Find the beginning of an object into which 'addr' points.
        //
        // The addr /must/ point into a FixedMalloc-managed block, but
        // it could point into freed storage, or an address that does
        // not belong to any actual object.
        //
        // Return an object pointer if the pointer is into some object
        // in the block (could be a free object), NULL otherwise.
        //
        // @see FixedMalloc::FindBeginningAndSize
        // @see FixedAlloc::FindBeginning
        const void* FindBeginning(const void *addr);

        // Finds beginning + size of object into which 'addr' points.
        //
        // The addr /must/ point into a FixedMalloc-managed block, but
        // it could point into freed storage, or an address that does
        // not belong to any actual object.
        //
        // If 'addr' points into a (potentially freed) object, then
        // sets begin_recv and size_recv respectively to the beginning
        // address and size of the object, and returns true.
        //
        // If 'addr' does not point into an object in a FixedMalloc-
        // managed block, then leaves the recv parameters unchanged,
        // and returns false.
        //
        // @see FixedMalloc::FindBeginning
        // @see FixedAlloc::FindBeginning
        bool FindBeginningAndSize(const void* addr,
                                  const void* &begin_recv, size_t &size_recv);

        // Return a thread-safe allocator for objects of the given size.
        FixedAllocSafe* FindAllocatorForSize(size_t size);

        // Return an object of at least the requested size, allocated with the given
        // flags.  The object's real size will be an integral number of blocks.
        void *LargeAlloc(size_t size, FixedMallocOpts flags=kNone);

        // Free the item returned from LargeAlloc.
        void LargeFree(void *item);

        // Return the allocated size (in bytes) of 'item', which must have been returned
        // from LargeAlloc.
        size_t LargeSize(const void *item);
        
    private:
        static FixedMalloc *instance;   // The singleton FixedMalloc

#ifdef MMGC_64BIT
        const static int kLargestAlloc = 2016;  // The largest small-object allocation
#else
        const static int kLargestAlloc = 2032;  // The largest small-object allocation
#endif
        const static int kNumSizeClasses = 41;  // The number of small-object size classes
        
        // A table whose nth entry is the maximum size accomodated by the
        // allocator in the nth entry of the m_allocs table.
        const static int16_t kSizeClasses[kNumSizeClasses];
        
        // The number of entries in the table mapping a request not greater than
        // kLargestAlloc to the appropriate index in m_allocs.
        const static unsigned kMaxSizeClassIndex = (kLargestAlloc>>3)+1;
        
        // A table mapping a request not greater than kLargestAlloc to the appropriate
        // index in m_allocs.  The mapping is complicated and explained in comments
        // in FixedMalloc.cpp, also see the implementation of FindAllocatorForSize.
        const static uint8_t kSizeClassIndex[kMaxSizeClassIndex];

    private:
        GCHeap *m_heap;                             // The heap from which we allocate, set in InitInstance
        FixedAllocSafe m_allocs[kNumSizeClasses];   // The array of size-segregated allocators, set in InitInstance

        class FindBeginningRootsCache {
            friend class GCRoot;
            friend class FixedMalloc;

            // Records that 'addr' is the start of a FixedMalloc-managed
            // object of size 'size', anticipating an imminent invocation
            // of FindBeginningAndSize on an address within the object.
            //
            // (Caller must ensure that 'addr' is currently the beginning
            // of a FixedMalloc-managed object of size 'size'.)
            //
            // @see FixedMalloc::FindBeginningAndSize.
            void Stash(const void *addr, size_t size);

            // If 'addr' cached, then returns true and sets
            // 'begin_recv' and 'size_recv' to beginning and size of
            // the object containing 'addr' accordingly.  Otherwise
            // returns false, leaving 'begin_recv' and 'size_recv'
            // unchanged.
            //
            // Synchronized on m_lock.
            bool Lookup(const void* addr,
                        const void* &begin_recv,
                        size_t &size_recv);

            // Removes 'addr' from the cache, so that subsequent
            // FindBeginningAndSize invocations on its addresses
            // go through the uncached slow path.
            //
            // @see FixedMalloc::FindBeginningAndSize.
            void Clear(void *addr);

            // Initialize cache; called from FixedMalloc during GCHeap setup.
            void Init();

            // Destroy cache and free all resources.
            void Destroy();

            // m_lock protects m_objSize and m_objBegin.
            // (m_objBegin may be read unprotected.)
            vmpi_spin_lock_t m_lock;
            const void *m_objBegin;  // Most recently stashed addr
            size_t m_objSize;        //   and size for FindBeginning
        };

        FindBeginningRootsCache m_rootFindCache;

        vmpi_spin_lock_t m_largeAllocInfoLock;  // Protects numLargeBlocks and totalAskSizeLargeAllocs

        size_t numLargeBlocks;              // Number of large-object blocks owned by this FixedMalloc
#ifdef MMGC_MEMORY_PROFILER
        size_t totalAskSizeLargeAllocs;     // The current number of bytes requested for large objects
#endif
#if defined DEBUG
        vmpi_spin_lock_t m_largeObjectLock; // Protects largeObjects
        LargeObject      *largeObjects;     // Data structure of live large objects, initially NULL
#endif
    };
}
#endif /* __Malloc__ */

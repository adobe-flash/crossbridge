/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCHeap__
#define __GCHeap__

namespace avmplus { namespace ST_mmgc_gcoption { class ST_mmgc_gcoption; } };

namespace MMgc
{
    class GCHeapConfig
    {
    public:
        GCHeapConfig();

        // If true then optionString represents a GC option parsed by
        // ParseAndApplyOption that will also require a successive
        // parameter token (e.g. [-memlimit d] or [-load L,B, ...]).
        //
        // Note that clients are *not* required to check this before
        // invoking ParseAndApplyOption with a null successorString;
        // they only must check this if they plan passing a non-null
        // successorString.  (This is all largely a hack to interface
        // with avmshell's command line parsing in a backwards compatible
        // manner; note when the parameter value is itself encoded in the
        // option string that this method will return false.)
        bool IsGCOptionWithParam(const char *optionString);

        // ParseAndApplyOption incorporates options into the config;
        // it returns true for GC options, valid or invalid, and false
        // for non GC options, and it will *only* change the config when
        // given a valid GC option.
        //
        // If optionString does not represent a GC option, then leaves
        // this config unchanged and returns false (and leaves wrongFmt
        // in an unspecified state; don't read it in this case).
        //
        // If optionString represents a GC option but the option as a
        // whole is malformatted or illegal, then sets wrongFmt to
        // true, then leaves this config *unchanged*, and returns
        // *true*.  (This case includes situations where the
        // optionString requires a successive parameter token that was
        // not passed as a successorString.)
        //
        // If option encodes a legal GC option, then sets wrongFmt to
        // false, incorporates the option into this config, and
        // returns true.
        //
        // GC options that take parameters can be handed by either
        // (but not both) of the following:
        // - encoding the parameter value in the optionString
        //   (separated from the option key by either a space charater
        //   or an equals-sign character).
        // - passing the parameter value as the successor string.
        bool ParseAndApplyOption(const char *optionString, bool &wrongFmt,
                                 const char *successorString = 0);

        static const size_t kNumLoadFactors = 7;
        static const size_t kDefaultHeapLimit = (size_t)-1;

        size_t initialSize;
        /**
         * if the heap gets this big we stop expanding
         */
        size_t heapLimit;

        /**
         * if the heap gets this big we fire memory status
         * notifications and engage collection activities
         */
        size_t heapSoftLimit;

        /**
         * In DEBUG builds, if dispersiveAdversarial is non-zero then
         * region reservation attempts to exercise low and high ends
         * of address space. Its magnitude roughly corresponds to how
         * much filler we attempt to insert between regions.
         */
        size_t dispersiveAdversarial;

        /**
         * If the application wants the allocator to exit when memory
         * runs out and reclamation efforts fail set this to a
         * non-zero value.   Defaults to zero.
         */
        uint32_t OOMExitCode;

        /**
         * If useVirtualMemory is true then we use virtual memory (reserve/commit/decommit/unreserve)
         * for GCHeap memory.  Otherwise we go through aligned allocation/deallocation platform
         * interfaces.
         */
        bool useVirtualMemory;

        /**
         * If trimVirtualMemory is true then we try to reduce the use of virtual address
         * space by deallocating regions that have no used blocks in them.
         */
        bool trimVirtualMemory;

        /**
         * If mergeContiguousRegions is true then we merge adjoining regions into a single
         * region.  This is currently (now == 2010-03-19) unused.
         */
        bool mergeContiguousRegions;

        /**
         * If sloppyCommit is true then VMPI_commitMemory() and VMPI_decommitMemory()
         * can operate on ranges of memory containing both committed and decommitted blocks.
         */
        bool sloppyCommit;

        /**
         * In DEBUG builds, if checkFixedMemory is true then a test is made to check whether
         * the fixed object about to be freed is owned by FixedMalloc.  That test is somewhat
         * expensive, so it can be toggled by means of this flag.
         *
         * This flag will probably disappear in the future when expected changes to GCHeap
         * makes the test much cheaper.
         *
         * The flag defaults to 'true' and can be changed to 'false' after startup, but
         * then sticks at false.  (Any large objects that have been allocated with it
         * set to 'false' will not be properly registered, and when trying to free
         * them with the value subsequently set to 'true' an assert may be triggered.)
         */
        bool checkFixedMemory();

        void clearCheckFixedMemory();
        
        bool verbose;
        bool returnMemory;
        bool gcstats;
        bool autoGCStats;
        int32_t gcbehavior;     // Print gross history and policy decisions (MMGC_POLICY_PROFILING): 0=off, 1=at end, 2=after every gc and at end
        bool eagerSweeping;     // Enable full-heap sweeping at the end of Sweep()
#ifdef MMGC_HEAP_GRAPH
        bool dumpFalsePositives;
#endif
        double gcLoad[kNumLoadFactors];       // GC load factors: policy aims for a heap size that is gcLoad*H where H is the live size following GC
        double gcLoadCutoff[kNumLoadFactors]; // Heap sizes (MB) following GC below which the corresponding load factor applies, last entry is +infinity
        double gcLoadCeiling;   // Max multiple of gcLoad policy should use after adjusting L for various factors (0=unlimited)
        double gcEfficiency;    // Max fraction of time to spend in the collector while the incremental collector is active
        
    private:
        bool _checkFixedMemory;
    };

    /**
     * The GCManager centralizes management of all the memory allocators in the
     * system, and provides iteration facilities.
     *
     * The GCHeap singleton holds the only instance of this manager.
     */
    class GCManager
    {
    public:
        GCManager() {}

        /**
         * Can't have a destructor as it'll be called too late, call destroy to
         * free any resources.
         */
        void destroy();

        /**
         * Register the GC with the manager.  GC must not already be registered.
         */
        bool tryAddGC(GC* gc);

        /**
         * Unregister the GC with the manager.  The GC must be registered.
         */
        void removeGC(GC* gc);

        /**
         * Tell every other GC that 'gc' is starting a collection (ie there may be memory pressure there).
         */
        void signalStartCollection(GC* gc);

        /**
         * Tell every other GC that 'gc' is finished with its collection.
         */
        void signalEndCollection(GC* gc);

        /**
         * @return the registered GC's.
         */
        BasicList<GC*>& gcs();

    private:
        BasicList<GC*> collectors;          // array of collectors
    };

    /**
     * GCHeap is a heap manager for the Flash Player's garbage collector.
     *
     * Memory is allocated from the operating system in large chunks
     * aligned on page boundaries (4096-byte aligned).  All allocation
     * requests are specified in numbers of 4096-byte pages.  The first
     * 4 bytes of each page is reserved for GCHeap's use.
     *
     * Allocations may fail if there is no free block large enough
     * to satisfy the request.  When this occurs, the GC class will
     * choose to either garbage collect or expand the heap, and then
     * call GCHeap again to retry the allocation.
     *
     * When blocks are freed, they are coalesced with their neighbors
     * when possible to reduce fragmentation.  A "boundary tag" scheme
     * similar to Doug Lea's malloc is used to make coalescing fast.
     * To keep things simple, coalescing is always performed immediately
     * when a block is freed, and not deferred.
     *
     * Allocations are performed using multiple free lists.  Blocks that
     * are <= kUniqueThreshold pages in size get their own free list.
     * Blocks above the threshold get mapped to more granular free lists,
     * and all blocks above kHugeThreshold get mapped to the last free list.
     * This approach is borrowed from Hans Boehm's C/C++ Garbage Collector.
     *
     * Free lists are doubly-linked lists sorted in size order.  The
     * sorting is done to ensure best-fit allocation.  Each free list
     * is itself a list node, and the lists are circular, in a technique
     * also borrowed from Doug Lea's malloc.  This makes for very simple
     * logic to insert/remove nodes.
     *
     * Only Windows is currently supported, although this should be easy
     * to get working on other platforms.  Memory is allocated in large
     * (64K minimum) chunks using the VirtualAlloc API.  Every time the
     * heap is expanded, a new heap region is created corresponding to
     * the address space returned by VirtualAlloc.  Heap regions are
     * allocated contiguously if possible to reduce fragmentation.
     */
    class GCHeap
    {
        friend class GC;
        friend class FixedAlloc;
        friend class FixedMalloc;
        friend class GCPolicyManager;
        friend class avmplus::ST_mmgc_gcoption::ST_mmgc_gcoption;
    public:
        // -- Constants

        /** Size of a block */
        const static uint32_t kBlockSize = 4096;
        const static uint32_t kBlockShift = 12;
        const static uintptr_t kBlockMask = ~(uintptr_t(kBlockSize) - 1);   // Clear lower 12 bits
        const static uintptr_t kOffsetMask = (uintptr_t(kBlockSize) - 1);   // Clear upper 20 or 52 bits

        /**
         * Max allowable size for any allocation = 2^32 - 1  bytes
         * This value is based on the max value on 32-bit systems
         * and acts as a cap on the size of an allocation request
         * Overflow detection routines CheckForAllocSizeOverflow()
         * use this value to check for overflows
         */
        const static size_t kMaxObjectSize = 0xFFFFFFFF;

        /** Default size of address space reserved per region in blocks */
#ifdef MMGC_64BIT
        const static uint32_t kDefaultReserve = 4096;
#else
        const static uint32_t kDefaultReserve = 512;
#endif

        /** Sizes up to this many blocks each have their own free list. */
        const static uint32_t kUniqueThreshold = 16;

        /**
         * Sizes of at least this many heap blocks are mapped to a
         * single free list.
         */
        const static uint32_t kHugeThreshold = 128;

        /**
         * Allocations greater than this are passed on to the OS
         */
        const static uint32_t kOSAllocThreshold = 256;

        /** In between sizes map this many distinct sizes to a single bin. */
        const static uint32_t kFreeListCompression = 8;

        /** Calculated number of free lists */
        const static uint32_t kNumFreeLists = (kHugeThreshold-kUniqueThreshold)/kFreeListCompression+kUniqueThreshold;

        /** Minimum heap increment, in blocks */
        const static uint32_t kMinHeapIncrement = 32;

        /** if this much of the heap is free decommit some memory */
        const static uint32_t kDecommitThresholdPercentage = 25;

        /** if this much of the heap is free un-reserve it */
        const static uint32_t kReleaseThresholdPercentage = 50;

        // Poison values used by MMgc
        const static uintptr_t GCEndOfObjectPoison = 0xdeadbeef;// First word past legitimate payload in GC object
        const static uintptr_t GCFreedPoison = 0xcacacaca;      // Object fill when GC object has been freed
        const static uintptr_t GCSweptPoison = 0xbabababa;      // Object fill when GC object has been swept
        const static uintptr_t FXFreshPoison = 0xfafafafa;      // Object fill in fresh FixedAlloc object (before any zeroing)
        const static uintptr_t FXFreedPoison = 0xedededed;      // Object fill when FixedAlloc object has been freed
        const static uintptr_t MMFreedPoison = 0xfbfbfbfb;      // Object fill when heap block has been freed by GCHeap

        // Tag values used for debugging in GCGlobalNew
        const static uint32_t MMScalarTag         = 0xafafafafU;// Tag for a non-array object
        const static uint32_t MMNormalArrayTag    = 0xbfbf0001U;// Tag for an array object with constructed/destructed elements
        const static uint32_t MMPrimitiveArrayTag = 0xbfbf0002U;// Tag for an array object with primitive elements, must be one greater than the normal tag

#ifdef DEBUG
        const static char LeafApiEnforcementPoison = 0xa;
#endif
        
        /**
         * Init must be called to set up the GCHeap singleton
         */
        static void Init(const GCHeapConfig& props);

        /* legacy API */
        static void Init(GCMallocFuncPtr malloc = NULL, GCFreeFuncPtr free = NULL, int initialSize=128);

        /**
         * Destroy the GCHeap singleton, returns the number of bytes still allocated in FixedMalloc (ie leaked)
         */
        static size_t Destroy();

        /**
         * Get the GCHeap singleton; can be NULL (e.g. on OOM).
         */
        static GCHeap *GetGCHeap();

        static void EnterLockInit();
        static void EnterLockDestroy();

        /**
         * Acquires global Enter mutex.
         */
        static bool EnterLock();

        /**
         * Releases global Enter mutex.
         */
        static bool EnterRelease();

        /**
         * Signal a too-large allocation request.  This /will/ cause an immediate shutdown of
         * the entire system.  (The alternative is to return a NULL pointer, which has the
         * same effect but with considerably less grace.)  Clients that allocate arbitrarily
         * large objects based on application or end-user data may wish to be concerned about
         * checking whether the object might be too large.
         */
#if defined (__GNUC__)
        static void SignalObjectTooLarge() __attribute__((noreturn));
#else
        static void SignalObjectTooLarge();
#endif

        /**
         * Signal an inconsistent heap state (an unrecoverable program error).  This /will/
         * cause an immediate shutdown of the entire system.  (The alternative is to return
         * a NULL pointer, which has the same effect but with considerably less grace.)
         */
#if defined (__GNUC__)
        static void SignalInconsistentHeapState(const char* reason) __attribute__((noreturn));
#else
        static void SignalInconsistentHeapState(const char* reason);
#endif

        /**
         * Signal that the caller is about to longjmp to or past an MMGC_ENTER, and that
         * actions should be taken to leave the heap in a consistent state.
         *
         * MMgc code does not use this, it's intended for use by external agents.  It can
         * be called without ill effect even if there is no active GC or GCHeap.
         *
         * longjmp'ing to or past an MMGC_ENTER is not supported in all contexts: host code
         * should never do it on call-backs on allocation, deallocation, and finalization hooks.
         */
        static void SignalImminentAbort();

        /**
         * Signal that client code has performed an allocation from memory known not to be
         * controlled by MMgc, and wants this memory accounted for.  (A typical case is when
         * system memory is allocated for rendering buffers.)
         *
         * Let 'total memory volume' be the sum of the memory volume controlled by MMgc,
         * plus the volume added by SignalExternalAllocation, minus the volume subtracted
         * by SignalExternalDeallocation.  MMgc performs its out-of-memory avoidance
         * actions (actions for crossing the soft and hard limits) based on total memory
         * volume.
         */
        static void SignalExternalAllocation(size_t nbytes);

        /**
         * Signal that client code has performed a deallocation of memory known not to be
         * controlled by MMgc, and wants this memory accounted for.
         *
         * @see SignalExternalAllocation
         */
        static void SignalExternalDeallocation(size_t nbytes);

        /**
         * Signal that client code wants the MMgc to release memory back to the system.
         * This will trigger synchronous and asynchronous memory cleanup.
         *
         * @param bytesToFree very rough number of bytes to be freed, will try to free
         *                    memory up to that amount, but may be less or more.
         *                    When not defined defaults to kMaxObjectSize, which will
         *                    release as much as possible.
         */
        static void SignalExternalFreeMemory(size_t bytesToFree = kMaxObjectSize);


        FixedMalloc* GetFixedMalloc();

        /**
        * flags to be passed as second argument to alloc
        */
        enum AllocFlags
        {
            kExpand=1,
            kZero=2,
            kProfile=4,
            kCanFail=8,
            kNoOOMHandling=16   // INTERNAL USE ONLY
        };

        // Default flags for Alloc, AllocNoOOM
        static const uint32_t flags_Alloc = kExpand | kZero | kProfile;

        /**
         * Allocates a block from the heap.
         * @param size the number of pages (kBlockSize bytes apiece)
         *             to allocate.
         * @param flags  The allocation flags
         * @param alignment  The alignment expressed in the number of pages.
         *             This must not be zero and it should be greater than 1 only when
         *             absolutely necessary.  (The VMPI layer may require greater alignment
         *             for code memory on some platforms.
         *
         * @return pointer to beginning of block, or NULL if kCanFail was in flags
         * and the allocation failed.
         */
        void *Alloc(size_t size, uint32_t flags=flags_Alloc, size_t alignment=1);

        /**
         * Allocates a block from the heap, but is guaranteed never to run OOM handling.
         * You can call this without kCanFail but a failed allocation will result in
         * an immediate abort.
         */
        void *AllocNoOOM(size_t size, uint32_t flags=flags_Alloc);

        /**
         * Signal that code memory is about to be allocated (accounting).  May invoke
         * OOM handling in order to make memory available if we're pushing up against
         * preset limits.
         * @param size the number of blocks
         * @param gcheap_memory   true if the memory will be allocated from GCHeap and
         * will be accounted for there, and we're just tracking that it is code memory;
         * false if an external native API will be used.
         */
        void SignalCodeMemoryAllocation(size_t size, bool gcheap_memory);

        /**
         * Frees a block.
         * @param item the block to free.  This must be the same
         *             pointer that was previously returned by
         *             a call to Alloc.
         */
        void Free(void *item);

        /**
         * Frees a block, but is guaranteed never to run OOM handling (which could
         * happen if internal data structures are shuffled in response to
         * a large part of the heap becoming free and being decommitted, say).
         */
        void FreeNoOOM(void* item);

        /**
         * Frees a block; does not record the deallocation even if profiler enabled.
         */
        void FreeNoProfile(void *item);

        /**
         * Signal that code memory was deallocated (accounting).
         * @param size           the number of blocks
         * @param gcheap_memory  true if the memory was allocated from GCHeap and has
         * been accounted for and we're just tracking that it is code memory.
         */
        void SignalCodeMemoryDeallocated(size_t size, bool gcheap_memory);

        /**
         * Added for NJ's portability needs cause it doesn't always MMgc
         */
        void Free(void *item, size_t /*ignore*/);

        // Return the size (in blocks) of a valid item
        size_t Size(const void *item);

        // Return the size (in blocks) of an item, or (size_t)-1 if the item is not valid.
        size_t SafeSize(const void *item);

        /**
         * Returns the used heap size, that is, the total
         * space actually used by allocated objects.
         * @return the minimum heap size in pages (kBlockSize bytes apiece)
         */
        size_t GetUsedHeapSize() const;

        /**
         * Returns the "free heap size", that is, the difference in the
         * total heap size and the used heap size
         * @return the minimum heap size in pages (kBlockSize bytes apiece)
         */
        size_t GetFreeHeapSize() const;

        /**
         * @return the amount of code memory currently allocated.  GCHeap does not
         * have a notion of "free" and "used" code memory.
         */
        size_t GetTotalCodeSize() const;

        /**
         * @return the amount of code memory currently allocated by GCHeap.  GCHeap does not
         * have a notion of "free" and "used" code memory.
         *
         * @see GetTotalCodeSize, SignalCodeMemoryAllocation and SignalCodeMemoryDeallocated
         */
        size_t GetTotalGCHeapAllocatedCodeSize() const;
#ifdef MMGC_POLICY_PROFILING
        /**
         * Returns the peak value for the total amount of space managed by the heap
         * and the amount of private memory at the point where the maximum heap
         * number was recorded.
         */
        void GetMaxTotalHeapSize(size_t& heapBlocks, size_t& privateBlocks);
#endif

        /**
         * Returns the total heap size, that is, the total amount
         * of space managed by the heap, including all used and
         * free space.
         * @return the total heap size in pages (kBlockSize bytes apiece)
         */
        size_t GetTotalHeapSize() const;

        /**
         * gives memory back to the OS when there hasn't been any memory activity in a while
         * and we have lots of free memory
         */
        void Decommit();

        void PreventDestruct();
        void AllowDestruct();




        static size_t SizeToBlocks(size_t bytes);

#ifdef MMGC_HOOKS
        /* Hooks are normally disabled in RELEASE builds, as there is a slight cost added
           to some hot paths.  */
        /* controls whether AllocHook and FreeHook are called */
        void EnableHooks();
        bool HooksEnabled() const;
        void AllocHook(const void *item, size_t askSize, size_t gotSize, bool managed);
        // called when object is determined to be garbage but we can't write to it yet
        void FinalizeHook(const void *item, size_t size);
        // called when object is really dead and can be poisoned
        void FreeHook(const void *item, size_t size, int poison);
        // called when object is initialized-as-if-freed and can be poisoned
        void PseudoFreeHook(const void *item, size_t size, int poison);
#endif

#ifdef MMGC_MEMORY_PROFILER
        MemoryProfiler *GetProfiler();
        void DumpFatties();
#endif

        // Every new GC must register itself with the GCHeap.
        void AddGC(GC *gc);

        // When the GC is destroyed it must remove itself from the GCHeap.
        void RemoveGC(GC *gc);

        void AddOOMCallback(OOMCallback *p);

        void RemoveOOMCallback(OOMCallback *p);

        // Signal a failure to allocate 'size' bytes from the system heap (VMPI_alloc
        // or other internal allocation).  The value 'attempt' denotes the number of
        // previous attempts made to satisfy this particular memory request; the
        // implementation is at liberty to have a cutoff for the number of attempts
        // and must signal an abort if the number of attempts exceeds the cutoff.
        // (Normally the cutoff would be one previous attempt.)
        void SystemOOMEvent(size_t size, int attempt);

#if defined (__GNUC__)
        void Abort() __attribute__((noreturn));
#else
        void Abort();
#endif

        MemoryStatus GetStatus();

        /**
         * CheckForAllocSizeOverflow checks whether an allocation request
         * would request an object larger than what MMgc can accomodate.
         *
         * Overflow detection logic:
         * Currently, in MMgc all object allocations are capped to 2^32-1 (=kMaxObjectSize)
         * which is the largest object size on 32-bit systems.  To detect overflow
         * the standard way is to add the values and look for wraparound by checking if
         * the result is less than either of the operands.  However, on 64-bit systems
         * sizeof(size_t) == 8 bytes and the wraparound check would not work.  So this
         * method also checks if the result exceeds 2^32-1 to conform our allocation size cap.
         * Requirement:
         * All allocation routines and methods that add extra payload (such as headers) to
         * allocation requests should call this method for overflow detection.
         * @param size requested size pertaining to object being allocated
         * @param extra amount of extra bytes accompanying an allocation
         * @return:  The sum of size and extra.
         * @note This method may not return.  It is designed to terminate
         * the program if an overflow is detected.
         */
        static size_t CheckForAllocSizeOverflow(size_t size, size_t extra);

        /**
         * CheckForCallocSizeOverflow
         * This method is designed to check whether an allocation request
         * of N objects for a given size might result in numeric overflow.
         * Overflow detection logic:
         * In this method we detect overflow occurring from result of (N * M) where
         * N = number of objects and M = size of one object.
         * Currently, in MMgc all object allocations are capped to 2^32-1 (=kMaxObjectSize)
         * which is the largest object size on 32-bit systems.
         * To detect overflow, we first check if either of N or M exceeds kMaxObjectSize.
         * This check is a guard against overflow on 64-bit systems where sizeof(size_t) is
         * greater than 4 bytes.  If this check succeeds then we perform a 64-bit based
         * product i.e. (N * M) and check if the result exceeds kMaxObjectSize.
         * Requirement:
         * All allocation routines and methods that are servicing allocation requests
         * based on a product of object size and a number of objects should call this method prior to allocation
         * @param count number of objects being allocated
         * @param elsize requested size pertaining to a single object being allocated
         * @return: None.  This method is designed to call Abort() which in turn terminates
         * the program if an overflow is detected.
         */
        static size_t CheckForCallocSizeOverflow(size_t count, size_t elsize);

        /**
         * CheckForNewSizeOverflow
         * This method is designed to check whether an allocation request
         * of N objects for a given size might result in numeric overflow
         * in a call to operator new[].
         * Overflow detection logic:
         * In this method we detect overflow occurring from result of (N * M) where
         * N = number of objects and M = size of one object.
         * Currently, in MMgc all object allocations are capped to 2^32-1 (=kMaxObjectSize)
         * which is the largest object size on 32-bit systems.
         * To detect overflow, we first check if either of N or M exceeds kMaxObjectSize.
         * This check is a guard against overflow on 64-bit systems where sizeof(size_t) is
         * greater than 4 bytes.  If this check succeeds then we perform a 64-bit based
         * product i.e. (N * M) and check if the result exceeds kMaxObjectSize.
         * Requirement:
         * Code that is allocating arrays using operator new in the form "new type[count]"
         * should call this method to validate that the requested size will not cause overflow
         * @param count number of objects being allocated
         * @param elsize requested size pertaining to a single object being allocated
         * @param canFail specifies if the method should return 0 (true) or call Abort (false)
         * @return: This method will return the count parameter that was passed in if
         * there was no overflow. If an overflow is detected, the method will return 0 if
         * the canFail parameter is true, or will call Abort() otherwise
         */
        static size_t CheckForNewSizeOverflow(size_t count, size_t elsize, bool canFail);

        /** The native VM page size (in bytes) for the current architecture */
        const size_t kNativePageSize;

        GCHeapConfig &Config();

        void log_percentage(const char *, size_t bytes, size_t relativeTo);

        void DumpMemoryInfo();

#ifdef MMGC_MEMORY_PROFILER
#ifdef MMGC_USE_SYSTEM_MALLOC
        void TrackSystemAlloc(void *addr, size_t askSize);
        void TrackSystemFree(void *addr);
#endif //MMGC_USE_SYSTEM_MALLOC
#endif

        void *GetStackEntryAddress();
        EnterFrame *GetEnterFrame();

        GC* SetActiveGC(GC* gc);

        bool IsStackEntered();

        // remove this and make them always enabled once its possible
        void SetEntryChecks(bool to);

        // Heap regions
        // (ought to be private but some VMPI implementations
        // currently need to peek at it)
        class Region
        {
        public:
            Region(GCHeap *heap, char *baseAddr, char *rTop, char *cTop, size_t blockId);
            Region *prev;
            char *baseAddr;
            char *reserveTop;
            char *commitTop;
            size_t blockId;
        };
        Region *lastRegion;

        static bool ShouldNotEnter();

        bool IsAddressInHeap(void *);

#ifdef DEBUG
        /** illegal to perform an allocation during OOM status notifications
            use this check in places where we are doing an alloc that might
            call GCHeap::Alloc
         */
        void CheckForOOMAbortAllocation();
#endif

        /** Return true if enough free memory exists such that if Decommit was called
            we would no longer be above the soft limit */
        bool QueryCanReturnToNormal();

    private:

        GCHeap(const GCHeapConfig &config);
        void DestroyInstance();

        /** Resets the GCHeap statics. This is called from both GCHeap::GCHeap() and GCHeap::DestroyInstance()
            It sould be called at the start and end of GCHeap's lifetime. It avoids any mismatch in the static
            variable initial values
         */
        static void ResetStatics();

#ifdef MMGC_MEMORY_PROFILER
        static void InitProfiler();
        static bool IsProfilerInitialized();
#endif

        /**
         * Expands the heap by size pages.
         *
         * Expands the heap by "size" blocks, such that a single contiguous
         * allocation of "size" blocks can be performed.  This method is
         * also called to create the initial heap.
         *
         * On Windows, this uses the VirtualAlloc API to obtain memory.
         * VirtualAlloc can _reserve_ memory, _commit_ memory or both at
         * the same time.  Reserved memory is just virtual address space.
         * It consumes the address space of the process but isn't really
         * allocated yet; there are no pages committed to it yet.
         * Memory allocation really occurs when reserved pages are
         * committed.  Our strategy in GCHeap is to reserve a fairly large
         * chunk of address space, and then commit pages from it as needed.
         * By doing this, we're more likely to get contiguous regions in
         * memory for our heap.
         *
         * By default, we reserve 16MB (4096 pages) per heap region.
         * The amount to reserve by default is controlled by kDefaultReserve.
         * That shouldn't be a big deal, as the process address space is 2GB.
         * As we're usually a plug-in, however, we don't want to make it too
         * big because it's not all our memory.
         *
         * The goal of reserving so much address space is so that subsequent
         * expansions of the heap are able to obtain contiguous memory blocks.
         * If we can keep the heap contiguous, that reduces fragmentation
         * and the possibility of many small "Balkanized" heap regions.
         *
         * Algorithm: When an allocation is requested,
         * 1. If there is enough reserved but uncommitted memory in the
         *    last-created region to satisfy the request, commit that memory
         *    and exit with success, also check decommitted list
         * 2. Try to reserve a new region contiguous with the last-created
         *    region.  Go for a 16MB reservation or the requested size,
         *    whichever is bigger.
         * 3. If we tried for 16MB reserved space and it didn't work, try
         *    to reserve again, but for the requested size.
         * 4. If we were able to retrieve a contiguous region in Step 2 or 3,
         *    commit any leftover memory from the last-created region,
         *    commit the remainer from the newly created region, and exit
         *    with success.
         * 5. OK, the contiguous region didn't work out, so allocate a
         *    non-contiguous region.  Go for 16MB or the requested size
         *    again, whichever is bigger.
         * 6. If we tried for 16MB reserved space and it didn't work, try
         *    to reserve again, but for the requested size.
         * 7. Commit the requested size out of the newly created region
         *    and exit with success.
         *
         * If we are able to reserve memory but can't commit it, then, well
         * there isn't enough memory.  We free the reserved memory and
         * exit with failure.
         *
         * @param size the number of pages to expand the heap by
         */
        bool ExpandHeap(size_t size);
        bool ExpandHeapInternal(size_t size);

        // Block struct used for free lists and memory traversal
        class HeapBlock
        {
        public:
            char *baseAddr;   // base address of block's memory
            size_t size;         // size of this block
            size_t sizePrevious; // size of previous block
            HeapBlock *prev;      // prev entry on free list
            HeapBlock *next;      // next entry on free list
            bool committed;   // is block fully committed?
            bool dirty;       // needs zero'ing, only valid if committed
#if defined(MMGC_MEMORY_PROFILER) && defined(MMGC_MEMORY_INFO)
            StackTrace *allocTrace;
            StackTrace *freeTrace;
#endif
            bool inUse() const;

            bool isSentinel() const;

            char *endAddr() const;

            void Clear();

            void Init(char *baseAddr, size_t size, bool dirty);

            void FreelistInit();
        };

        // Core methods

        // Add a block to the free list, coalescing committed blocks with adjacent
        // committed blocks.  The coalesced block is marked dirty if makeDirty is true
        // or if any of the blocks that were coalesced were dirty.
        void AddToFreeList(HeapBlock *block, bool makeDirty=false);

        // Add a block to the free list, prior to pointToInsert.
        void AddToFreeList(HeapBlock *block, HeapBlock* pointToInsert);

        void *AllocHelper(size_t size, bool expand, bool& zero, size_t alignment);
        HeapBlock *AllocBlock(size_t size, bool& zero, size_t alignment);
        HeapBlock* AllocCommittedBlock(HeapBlock* block, size_t size, bool& zero, size_t alignment);
        HeapBlock* CreateCommittedBlock(HeapBlock* block, size_t size, size_t alignment);
        void PruneDecommittedBlock(HeapBlock* block, size_t available, size_t request);
        void FreeBlock(HeapBlock *block);
        void FreeAll();

        void FreeInternal(const void *item, bool profile, bool oomHandling);

        HeapBlock *Split(HeapBlock *block, size_t size);

        /**
         * Reserves region(s) of memory in system's virtual address space.
         *
         * Treat as synonymous with VMPI_reserveMemoryRegion(NULL,---);
         * when debugging, this exercises areas of specification that are
         * hard to reach otherwise.
         *
         * @see VMPI_reserveMemoryRegion
         */
        char* ReserveSomeRegion(size_t sizeInBytes);

        // abandon a block of memory that may maps completely to the committed portion of region
        void RemoveBlock(HeapBlock *block, bool release=true);

        // Large* handle allocations larger than kOSAllocThreshold.
        void *LargeAlloc(size_t size, size_t alignment);
        void LargeFree(const void *item);
        size_t LargeAllocSize(const void *item);

#ifdef MMGC_MAC
        // Abandon a block of memory that may be in the middle of a
        // region.  On mac decommit is a two step process, release and
        // reserve, another thread could steal the memory between the
        // two operations so we have to be prepared to ditch a block
        // we try to decommit.  This is a horrible hack that can go
        // away if OS X fixes its mmap impl to be like windows, linux
        // and solaris (atomic decommit with VirtualFree/mmap)
        void RemoveLostBlock(HeapBlock *block);
#endif

        void Commit(HeapBlock *block);

        HeapBlock *InteriorAddrToBlock(const void *item) const;
        HeapBlock *BaseAddrToBlock(const void *item) const;
        Region *AddrToRegion(const void *item) const;
        void RemoveRegion(Region *r, bool release=true);

        // debug only freelist consistency checks
        void CheckFreelist();

        bool BlocksAreContiguous(void *item1, void *item2);

        // textual heap representation, very nice!
        void DumpHeapRep();

        //log a character for "count" times
        void LogChar(char c, size_t count);

        // Remove a block from a free list (inlined for speed)
        void RemoveFromList(HeapBlock *block);

        // Map a number of blocks to the appropriate large block free list index
        uint32_t GetFreeListIndex(size_t size);

        bool HardLimitExceeded(size_t additionalAllocationAmt = 0);
        bool SoftLimitExceeded(size_t additionalAllocationAmt = 0);
        void StatusChangeNotify(MemoryStatus to);
        void SendFreeMemorySignal(size_t minimumBlocksToFree = kMaxObjectSize);
        void CheckForMemoryLimitsExceeded();
        void CheckForStatusReturnToNormal();
        void CheckForHardLimitExceeded();
        void CheckForSoftLimitExceeded(size_t request);
        bool FreeMemoryExceedsDecommitThreshold();

        void ValidateHeapBlocks();

        void ReleaseMemory(char *address, size_t size);

        void Enter(EnterFrame *frame);
        void Leave();

        bool statusNotificationBeingSent();

        bool statusNotNormalOrAbort();

        size_t numHeapBlocksToNumBlocks(size_t numBlocks);

        void CheckForNewMaxTotalHeapSize();

        /**
         * Regions are allocated from the blocks GCHeap manages
         * similar to the HeapBlocks.  Regions can come and go so we
         * maintain a freelist although in practice they come and go
         * rarely we want don't want any longevity bugs
         */
        Region *NewRegion(char *baseAddr, char *rTop, char *cTop, size_t blockId);
        void FreeRegion(Region *r);

        // Used to allocate a Region outside ExpandHeap.
        bool EnsureFreeRegion(bool allowExpansion);
        bool HaveFreeRegion() const;

        bool newPagesDirty();

        // data section
        static GCHeap *instance;
        static size_t leakedBytes;

        static vmpi_spin_lock_t instanceEnterLock;
        static bool instanceEnterLockInitialized;

        FixedMalloc fixedMalloc;
        Region *freeRegion;
        Region *nextRegion;
        HeapBlock *blocks;
        size_t blocksLen;
        size_t numDecommitted;
        size_t numRegionBlocks;
        HeapBlock freelists[kNumFreeLists];
        size_t numAlloc;
        size_t gcheapCodeMemory;
        size_t externalCodeMemory;
        size_t externalPressure;
        vmpi_spin_lock_t m_spinlock;
        GCHeapConfig config;
        GCManager gcManager;
        BasicList<OOMCallback*> callbacks;

        GCThreadLocal<EnterFrame*> enterFrame;
        friend class EnterFrame;
        MemoryStatus status;
        uint32_t enterCount;
        uint32_t preventDestruct;
        bool m_oomHandling;                 // temporarily false when allocating or deallocating with kNoOOMHandling
        bool m_notificationBeingSent;
        vmpi_spin_lock_t gclog_spinlock;    // a lock used by GC::gclog for exclusive access to GCHeap::DumpMemoryInfo

#ifdef MMGC_MEMORY_PROFILER
        static MemoryProfiler *profiler;
        bool hasSpy; //flag indicating whether profiler spy is active or not.  If active, AllocHook will call VMPI_spyCallback
#endif

        size_t maxTotalHeapSize;    // in bytes
#ifdef MMGC_POLICY_PROFILING
        size_t maxPrivateMemory;    // in bytes
#endif
        // number of blocks in LargeAlloc allocations
        size_t largeAllocs;

#ifdef MMGC_HOOKS
        bool hooksEnabled;
#endif

        bool entryChecksEnabled;
        bool abortStatusNotificationSent;
    };


}

#endif /* __GCHeap__ */

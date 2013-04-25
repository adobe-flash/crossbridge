/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"
#include <float.h>

#if defined(AVMPLUS_SAMPLER) || (defined(VMCFG_TELEMETRY_SAMPLER) && defined(DEBUGGER))
namespace avmplus
{
    void recordAllocationSample(const void* item, size_t size);
    void recordDeallocationSample(const void* item, size_t size);
}
#endif

#if defined MMGC_POLICY_PROFILING && !defined AVMSHELL_BUILD
extern void RedirectLogOutput(void (*)(const char*));
static FILE* fp = NULL;

void logToFile(const char* s)
{
    fprintf(fp, "%s", s);
    fflush(fp);
}

static void startGCLogToFile()
{
    fp = fopen("gcbehavior.txt", "w");
    if (fp != NULL)
        RedirectLogOutput(logToFile);
}

static void endGCLogToFile()
{
    RedirectLogOutput(NULL);
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}
#endif // MMGC_POLICY_PROFILING && !AVMSHELL_BUILD

namespace MMgc
{
    GCHeap *GCHeap::instance = NULL;
    bool GCHeap::instanceEnterLockInitialized = false;
    vmpi_spin_lock_t GCHeap::instanceEnterLock;

    // GCHeap instance has the C++ runtime call dtor which causes problems
    AVMPLUS_ALIGN8(uint8_t) heapSpace[sizeof(GCHeap)];

    const size_t kLargeItemBlockId = ~0U;

    size_t GCHeap::leakedBytes;

#ifdef MMGC_MEMORY_PROFILER
    MemoryProfiler* GCHeap::profiler = (MemoryProfiler*)-1;
#endif

    GCHeapConfig::GCHeapConfig() :
        initialSize(512),
        heapLimit(kDefaultHeapLimit),
        heapSoftLimit(0),
        dispersiveAdversarial(0), // 0 means dispersive allocation is off.
        OOMExitCode(0),
        useVirtualMemory(AVMPI_useVirtualMemory()),
        trimVirtualMemory(true),
        mergeContiguousRegions(AVMPI_canMergeContiguousRegions()),
        sloppyCommit(AVMPI_canCommitAlreadyCommittedMemory()),
        verbose(false),
        returnMemory(true),
        gcstats(false), // tracking
        autoGCStats(false), // auto printing
#ifdef AVMSHELL_BUILD
        gcbehavior(0),  // controlled by command line switch
#else
        gcbehavior(2),   // unconditional, if MMGC_POLICY_PROFILING is on
#endif
        eagerSweeping(false),
#ifdef MMGC_HEAP_GRAPH
        dumpFalsePositives(false),
#endif
        gcLoadCeiling(1.15), // Bug 619885: need > 1.0 to get belt loosening effect
        gcEfficiency(0.25),
        _checkFixedMemory(true) // See comment in GCHeap.h for why the default must be 'true'
    {
        // Bugzilla 544695 - large heaps need to be controlled more tightly than
        // small heaps due to the reliance of the Player on the GC for removing some
        // objects from the AS2 scriptThreadList and because people don't want to
        // use as much memory as a single policy across all heap sizes would require.
        // As reference counting takes care of a lot of storage management, there's
        // little harm in running the incremental GC more aggressively in large
        // heaps - most of the time is spent elsewhere.
        //
        // Bug 619885: Note that mark/cons ratio is 1/(L-1); L=1.125 implies
        // mark/cons is 8x.  Programs that allocate rapidly would suffer significantly
        // with smaller limit for L.  (See also loadCeiling for other belt-loosening.)

        GCAssert(GCHeapConfig::kNumLoadFactors >= 7);

        gcLoad[0] = 2.5;   gcLoadCutoff[0] = 10;     // Breathing room for warmup
        gcLoad[1] = 2.0;   gcLoadCutoff[1] = 25;     // Classical 2x factor
        gcLoad[2] = 1.75;  gcLoadCutoff[2] = 50;     // Tighten
        gcLoad[3] = 1.5;   gcLoadCutoff[3] = 75;     //   the
        gcLoad[4] = 1.25;  gcLoadCutoff[4] = 150;    //     screws
        gcLoad[5] = 1.2;   gcLoadCutoff[5] = 300;    // Large heaps are
        gcLoad[6] = 1.125; gcLoadCutoff[6] = DBL_MAX;//   controlled (very) tightly

#ifdef MMGC_64BIT
        trimVirtualMemory = false; // no need
#endif
        const char *envValue = VMPI_getenv("MMGC_HEAP_LIMIT");
        if(envValue)
            heapLimit = VMPI_strtol(envValue, 0, 10);
        envValue = VMPI_getenv("MMGC_HEAP_SOFT_LIMIT");
        if(envValue)
            heapSoftLimit = VMPI_strtol(envValue, 0, 10);
    }

    bool GCHeapConfig::IsGCOptionWithParam(const char *arg)
    {
        if (!VMPI_strcmp(arg, "-memlimit")
            || !VMPI_strcmp(arg, "-load")
            || !VMPI_strcmp(arg, "-loadCeiling")
            || !VMPI_strcmp(arg, "-gcwork")
            || !VMPI_strcmp(arg, "-gcstack"))
            return true;
        else
            return false;
    }

    static bool HasPrefix(const char* aString, const char *aStr)
    {
        size_t n = VMPI_strlen(aStr);
        return (VMPI_strncmp(aString, aStr, n) == 0);
    }

    static const char * useDefaultOrSkipForward(const char *arg,
                                                const char *key,
                                                const char *dflt)
    {
        size_t n = VMPI_strlen(key);
        if (VMPI_strcmp(arg, key) == 0) {
            return dflt;
        } else if (HasPrefix(arg, key) &&
                   VMPI_strcmp(arg, key) > 0) {
            AvmAssert(dflt == NULL);
            const char *param = arg + n;
            while (*param != 0 && (*param == ' ' || *param == '='))
                param++;
            return param;
        } else {
            return NULL;
        }
    }

    bool GCHeapConfig::ParseAndApplyOption(const char *arg, bool &wrong,
                                           const char *successorString/*=0*/)
    {
        wrong = false; // assume input is valid until we see otherwise.

        if (!VMPI_strcmp(arg, "-memstats")) {
            gcstats = true;
            autoGCStats = true;
            return true;
        }
        else if (!VMPI_strcmp(arg, "-memstats-verbose")) {
            gcstats = true;
            autoGCStats = true;
            verbose = true;
            return true;
        }
        else if (HasPrefix(arg, "-memlimit")) {
            const char *param =
                useDefaultOrSkipForward(arg, "-memlimit", successorString);
            if (param == NULL) {
                wrong = true;
                return true;
            }
            heapLimit = VMPI_strtol(param, 0, 10);
            return true;
        }
#ifdef MMGC_POLICY_PROFILING
        else if (!VMPI_strcmp(arg, "-gcbehavior")) {
            gcbehavior = 2;
            return true;
        }
        else if (!VMPI_strcmp(arg, "-gcsummary")) {
            gcbehavior = 1;
            return true;
        }
#endif
        else if (!VMPI_strcmp(arg, "-eagersweep")) {
            eagerSweeping = true;
            return true;
        }
        else if (HasPrefix(arg, "-load") && !HasPrefix(arg, "-loadCeiling")) {
            const char *param =
                useDefaultOrSkipForward(arg, "-load", successorString);
            if (param == NULL) {
                wrong = true;
                return true;
            }

            double load;
            double limit;
            int nchar;
            const char* val = param;
            size_t k = 0;
            // limit=0 is legal, it means unlimited
            for (;;) {
                if (k < kNumLoadFactors) {
                    if (VMPI_sscanf(val, "%lf,%lf%n", &load, &limit, &nchar) == 2 &&
                        load > 1.0 &&
                        limit >= 0.0)
                    {
                        k++;
                        val += nchar;
                        if (*val == 0) {
                            break;
                        }
                        if (*val == ',') {
                            val++;
                            continue;
                        }
                    }
                    else if (VMPI_sscanf(val, "%lf%n", &load, &nchar) == 1 &&
                             val[nchar] == 0 &&
                             load > 1.0)
                    {
                        break;
                    }
                }
                wrong = true;
                return true;
            }
            // Above loop validated the param string; below loop
            // applies it.  The control flow of below loop *must*
            // match that of the above loop *exactly*; otherwise the
            // validation will be out of sync with the configuration.
            val = param;
            k = 0;
            for (;;) {
                if (k < kNumLoadFactors) {
                    if (VMPI_sscanf(val, "%lf,%lf%n", &load, &limit, &nchar) == 2 &&
                        load > 1.0 &&
                        limit >= 0.0)
                    {
                        gcLoad[k] = load;
                        gcLoadCutoff[k] = limit;
                        k++;
                        val += nchar;
                        if (*val == 0) {
                            gcLoadCutoff[k-1] = DBL_MAX;
                            break;
                        }
                        if (*val == ',') {
                            val++;
                            continue;
                        }
                    }
                    else if (VMPI_sscanf(val, "%lf%n", &load, &nchar) == 1 &&
                             val[nchar] == 0 &&
                             load > 1.0)
                    {
                        gcLoad[k] = load;
                        gcLoadCutoff[k] = DBL_MAX;
                        break;
                    }
                }
                // (see above note; if we get here, we're out of sync)
                GCAssert(false);
            }
            return true;
        }
        else if (HasPrefix(arg, "-loadCeiling")) {
            const char *param =
                useDefaultOrSkipForward(arg, "-loadCeiling", successorString);
            if (param == NULL) {
                wrong = true;
                return true;
            }

            double ceiling;
            int nchar;
            const char* val = param;
            if (VMPI_sscanf(val, "%lf%n", &ceiling, &nchar) == 1 &&
                size_t(nchar) == VMPI_strlen(val) &&
                ceiling >= 1.0)
            {
                gcLoadCeiling = ceiling;
                return true;
            }
            else
            {
                wrong = true;
                return true;
            }
        }
        else if (HasPrefix(arg, "-gcwork")) {
            const char* param =
                useDefaultOrSkipForward(arg, "-gcwork", successorString);
            if (param == NULL) {
                wrong = true;
                return true;
            }

            double work;
            int nchar;
            const char* val = param;
            if (VMPI_sscanf(val, "%lf%n", &work, &nchar) == 1 && size_t(nchar) == VMPI_strlen(val) && work > 0.0 && work <= 1.0) {
                gcEfficiency = work;
                return true;
            }
            else {
                wrong = true;
                return true;
            }
        }

        // arg unmatched; option not handled here.
        return false;
    }

    /* static */
    void GCHeap::ResetStatics()
    {
        instance = NULL;
#ifdef MMGC_MEMORY_PROFILER
        if(profiler && IsProfilerInitialized())
            delete profiler;
        profiler = (MemoryProfiler*)-1;
#endif
    }

    void GCHeap::Init(const GCHeapConfig& config)
    {
        GCAssert(instance == NULL);
        void *p = (void*)heapSpace;
        instance = new (p) GCHeap(config);
    }

    size_t GCHeap::Destroy()
    {
        EnterLock();
        GCAssert(instance != NULL);
        instance->DestroyInstance();
        EnterRelease();
        return leakedBytes;
    }

    GCHeap::GCHeap(const GCHeapConfig& c)
        : kNativePageSize(VMPI_getVMPageSize()),
          lastRegion(NULL),
          freeRegion(NULL),
          nextRegion(NULL),
          blocks(NULL),
          blocksLen(0),
          numDecommitted(0),
          numRegionBlocks(0),
          numAlloc(0),
          gcheapCodeMemory(0),
          externalCodeMemory(0),
          externalPressure(0),
          config(c),
          status(kMemNormal),
          enterCount(0),
          preventDestruct(0),
          m_oomHandling(true),
          m_notificationBeingSent(false),
    #ifdef MMGC_MEMORY_PROFILER
          hasSpy(false),
    #endif
          maxTotalHeapSize(0),
    #ifdef MMGC_POLICY_PROFILING
          maxPrivateMemory(0),
    #endif
          largeAllocs(0),
    #ifdef MMGC_HOOKS
          hooksEnabled(false),
    #endif
          entryChecksEnabled(true),
          abortStatusNotificationSent(false)
    {
        VMPI_lockInit(&m_spinlock);
        VMPI_lockInit(&gclog_spinlock);

        // ResetStatics should be called at the start here before using/initializing any statics
        ResetStatics();

        // Initialize free lists
        HeapBlock *block = freelists;
        for (uint32_t i=0; i<kNumFreeLists; i++) {
            block->FreelistInit();
            block++;
        }

        // Create the initial heap
        {
            MMGC_LOCK(m_spinlock);
            if (!ExpandHeap((int)config.initialSize))
            {
                Abort();
            }
        }

        fixedMalloc.InitInstance(this);

        instance = this;

#ifdef MMGC_MEMORY_PROFILER
        //create profiler if turned on and if it is not already created
        if(!IsProfilerInitialized())
        {
            InitProfiler();
        }

        if(profiler)
        {
            hooksEnabled = true; // set only after creating profiler
            hasSpy = AVMPI_spySetup();
        }
#endif

#ifdef MMGC_MEMORY_INFO
        hooksEnabled = true; // always track allocs in DEBUG builds
#endif

#if defined MMGC_POLICY_PROFILING && !defined AVMSHELL_BUILD
        startGCLogToFile();
#endif
    }

    void GCHeap::DestroyInstance()
    {
#ifdef MMGC_MEMORY_PROFILER
        if (profiler) 
            profiler->DumpAllocationProfile();
#endif
#if defined MMGC_POLICY_PROFILING && !defined AVMSHELL_BUILD
        endGCLogToFile();
#endif

        gcManager.destroy();
        callbacks.Destroy();

        leakedBytes = GetFixedMalloc()->GetBytesInUse();
        fixedMalloc.DestroyInstance();
        GCAssertMsg(leakedBytes == 0 || GetStatus() == kMemAbort, "Leaks!");

        size_t internalNum = BaseAddrToBlock(blocks)->size + numRegionBlocks;

        // numAlloc should just be the size of the HeapBlock's space
        if(numAlloc != internalNum && status != kMemAbort)
        {
            for (unsigned int i=0; i<blocksLen; i++)
            {
                HeapBlock *block = &blocks[i];
                if(block->inUse() && block->baseAddr && block->baseAddr != (char*)blocks)
                {
#ifndef DEBUG
                    if (config.verbose)
#endif
                    {
                        GCLog("Block 0x%x not freed\n", block->baseAddr);
                    }
#if defined(MMGC_MEMORY_PROFILER) && defined(MMGC_MEMORY_INFO)
                    if(block->allocTrace)
                        PrintStackTrace(block->allocTrace);
#endif
                }
            }
            GCAssert(false);
        }

#ifdef MMGC_MEMORY_PROFILER
        hooksEnabled = false;

        if(hasSpy)
            AVMPI_spyTeardown();
#endif

        FreeAll();
        ResetStatics();

        // Acquire all the locks before destroying them to make reasonably
        // sure we're the last consumers.  This is probably not exactly
        // right, see https://bugzilla.mozilla.org/show_bug.cgi?id=548347
        // and linked bugs for a discussion.  Note we can't acquire these
        // much higher up because we get into situations where GCHeap code
        // will want to lock these locks, but they are already locked.
        
        VMPI_lockAcquire(&m_spinlock);
        VMPI_lockRelease(&m_spinlock);
        VMPI_lockDestroy(&m_spinlock);
        
        VMPI_lockAcquire(&gclog_spinlock);
        VMPI_lockRelease(&gclog_spinlock);
        VMPI_lockDestroy(&gclog_spinlock);

        if(enterFrame)
            enterFrame->Destroy();  // Destroy the pointed-to value
        enterFrame.destroy();       // Destroy the thread-local itself
    }

    void* GCHeap::Alloc(size_t size, uint32_t flags, size_t alignment)
    {
        GCAssert(size > 0);
        GCAssert(alignment > 0);
#ifdef DEBUG
        {
            // Alignment must be a power of 2
            size_t a = alignment;
            while ((a & 1) == 0)
                a >>= 1;
            GCAssert(a == 1);
        }
#endif

        void *baseAddr = 0;
        bool zero = (flags & kZero) != 0;
        bool expand = (flags & kExpand) != 0;
        {
            MMGC_LOCK(m_spinlock);

            bool saved_oomHandling = m_oomHandling;
            m_oomHandling = saved_oomHandling && (flags & kNoOOMHandling) == 0;

            baseAddr = AllocHelper(size, expand, zero, alignment);

            //  If expand didn't work, or expand flag not set, then try to free
            //  memory then realloc from the heap
            if (!baseAddr)
            {
                SendFreeMemorySignal(size);
                baseAddr = AllocHelper(size, expand, zero, alignment);
            }

            //  If we're still unable to allocate, we're done
            if (!baseAddr)
            {
                if (flags & kCanFail)
                {
                    m_oomHandling = saved_oomHandling;
                    return NULL;
                } else {
                    Abort();
                }
            }

            numAlloc += size;

#ifdef MMGC_MEMORY_PROFILER
            if((flags & kProfile) && HooksEnabled() && profiler) {
                profiler->RecordAllocation(baseAddr, size * kBlockSize, size * kBlockSize, /*managed=*/false);
            }
#endif

            //  Only check for memory limits if we're allowing OOM notifications
            if (m_oomHandling)
            {
                CheckForMemoryLimitsExceeded();
            }

            m_oomHandling = saved_oomHandling;
        }

        GCAssert(Size(baseAddr) == size);

        // Zero out the memory, if requested to do so
        if (zero) {
            // These pages may have been seen by valgrind before and
            // they become unaddressable when we last called
            // FREELIKE_BLOCK or MEMPOOL_DESTROY, use MAKE_MEM_DEFINED
            // to silence write to freed memory errors.
            VALGRIND_MAKE_MEM_DEFINED(baseAddr, size * kBlockSize);
            VMPI_memset(baseAddr, 0, size * kBlockSize);
            // and then make the memory undefined again, we do this because
            // we do this because either the VALGRIND_MALLOCLIKE_BLOCK call
            // below will define it, or the suballocator will, ie this is
            // here to keep the sub allocators honest.
            VALGRIND_MAKE_MEM_UNDEFINED(baseAddr, size * kBlockSize);
        }

        // Fail the allocation if we're a "canFail" allocation that has pushed beyond one of our limits.
        if((flags & kCanFail) != 0 && (status == kMemSoftLimit || SoftLimitExceeded() || HardLimitExceeded() ))
        {
            FreeInternal(baseAddr, (flags & kProfile) != 0, m_oomHandling);
            return NULL;
        }

        // We utilize the "profile" flag to tell the difference
        // between client requests and sub-allocator requests.  Direct
        // client requests are reported to valgrind here, sub
        // allocators need to tell valgrind about memory themselves.
        if ((flags & kProfile) != 0) {
            VALGRIND_MALLOCLIKE_BLOCK(baseAddr, size * kBlockSize, 0, (flags&kZero) != 0);
        }

        GCAssert(((uintptr_t)baseAddr >> kBlockShift) % alignment == 0);
        return baseAddr;
    }

    void *GCHeap::AllocHelper(size_t size, bool expand, bool& zero, size_t alignment)
    {
        // first try to find it in our existing free memory
        HeapBlock *block = AllocBlock(size, zero, alignment);

        //  Try to expand if the flag is set
        if(!block && expand) {

            //  Look ahead at memory limits to see if we should trigger a free memory signal
            if ( (HardLimitExceeded(size) || SoftLimitExceeded(size)))
            {
                SendFreeMemorySignal(size);
            }

            // Don't allow our memory consumption to grow beyond hard limit
            if(HardLimitExceeded(size))
                return NULL;

            if(size >= kOSAllocThreshold && config.useVirtualMemory) {
                return LargeAlloc(size, alignment);
            } else {
                ExpandHeap(size);
                block = AllocBlock(size, zero, alignment);
            }
        }

#if defined(MMGC_MEMORY_PROFILER) && defined(MMGC_MEMORY_INFO)
        if(profiler && block)
            block->allocTrace = profiler->GetStackTrace();
#endif

        return block != NULL ? block->baseAddr : NULL;
    }

    void GCHeap::SignalCodeMemoryAllocation(size_t size, bool gcheap_memory)
    {
        if (gcheap_memory)
            gcheapCodeMemory += size;
        else
            externalCodeMemory += size;
        CheckForMemoryLimitsExceeded();
    }

    void GCHeap::CheckForMemoryLimitsExceeded()
    {

        //  If we're already in the process of sending out memory notifications, don't bother verifying now.
        if (status == MMgc::kMemAbort || statusNotificationBeingSent())
            return;

        size_t overage = 0;
        if (SoftLimitExceeded())
        {
            overage = GetTotalHeapSize() + externalPressure/kBlockSize - config.heapSoftLimit;
        }
        else if (HardLimitExceeded())
        {
            overage = (GetTotalHeapSize() + externalPressure/kBlockSize) - config.heapLimit + (config.heapLimit / 10);
        }

        if (overage)
        {
            SendFreeMemorySignal(overage);

            CheckForHardLimitExceeded();

            CheckForSoftLimitExceeded(overage);
        }
    }

    void GCHeap::FreeInternal(const void *item, bool profile, bool oomHandling)
    {
        (void)profile;

        // recursive free calls are allowed from StatusChangeNotify
        MMGC_LOCK(m_spinlock);

        bool saved_oomHandling = m_oomHandling;
        m_oomHandling = saved_oomHandling && oomHandling;

        HeapBlock *block = BaseAddrToBlock(item);

        size_t size;
        if(block == NULL) {
            size = LargeAllocSize(item);
        } else {
            size = block->size;
        }

        // Update metrics
        GCAssert(numAlloc >= (unsigned int)size);
        numAlloc -= size;

#if defined(MMGC_MEMORY_PROFILER) && defined(MMGC_MEMORY_INFO)
        if(profiler && block)
            block->freeTrace = profiler->GetStackTrace();
#endif

#ifdef MMGC_MEMORY_PROFILER
        if(profile && HooksEnabled() && profiler) {
            profiler->RecordDeallocation(item, size * kBlockSize);
        }
#endif

        if(block)
            FreeBlock(block);
        else
            LargeFree(item);

        if (profile)
            VALGRIND_FREELIKE_BLOCK(item, 0);

        m_oomHandling = saved_oomHandling;
    }

    void GCHeap::Decommit()
    {
        // keep at least initialSize free
        if(!config.returnMemory)
            return;

        // don't decommit if OOM handling is disabled; there's a guard in the OOM code so this
        // should never happen, but belt and suspenders...
        if (!m_oomHandling)
            return;

        size_t heapSize = GetTotalHeapSize();
        size_t freeSize = GetFreeHeapSize();

        size_t decommitSize = 0;
        // commit if > kDecommitThresholdPercentage is free
        if(FreeMemoryExceedsDecommitThreshold())
        {
            decommitSize = int((freeSize * 100 - heapSize * kDecommitThresholdPercentage) / 100);
        }
        //  If we're over the heapLimit, attempt to decommit enough to get just under the limit
        else if ( (heapSize > config.heapLimit) && ((heapSize - freeSize) < config.heapLimit))
        {
            decommitSize = heapSize - config.heapLimit + 1;

        }
        //  If we're over the SoftLimit, attempt to decommit enough to get just under the softLimit
        else if ((config.heapSoftLimit!= 0) &&  (heapSize > config.heapSoftLimit) && ((heapSize - freeSize) < config.heapSoftLimit))
        {
            decommitSize = heapSize - config.heapSoftLimit + 1;
        }
        else {
            return;
        }

        if ((decommitSize < (size_t)kMinHeapIncrement) && (freeSize > (size_t)kMinHeapIncrement))
        {

            decommitSize = kMinHeapIncrement;
        }

        //  Don't decommit more than our initial config size.
        if (heapSize - decommitSize < config.initialSize)
        {
            decommitSize = heapSize - config.initialSize;
        }


        MMGC_LOCK(m_spinlock);

    restart:

        // search from the end of the free list so we decommit big blocks
        HeapBlock *freelist = freelists+kNumFreeLists-1;

        HeapBlock *endOfBigFreelists = &freelists[GetFreeListIndex(1)];

        for (; freelist >= endOfBigFreelists && decommitSize > 0; freelist--)
        {
#ifdef MMGC_MAC
            // We may call RemoveLostBlock below which splits regions
            // and may need to create a new one, don't let it expand
            // though, expanding while Decommit'ing would be silly.
            if(!EnsureFreeRegion(/*expand=*/false))
                return;
#endif

            HeapBlock *block = freelist;
            while ((block = block->prev) != freelist && decommitSize > 0)
            {
                // decommitting already decommitted blocks doesn't help
                // temporary replacement for commented out conditional below
                GCAssert(block->size != 0);
                if(!block->committed /*|| block->size == 0*/)
                    continue;

                if(config.useVirtualMemory)
                {
                    RemoveFromList(block);
                    if((size_t)block->size > decommitSize)
                    {
                        HeapBlock *newBlock = Split(block, (int)decommitSize);
                        AddToFreeList(newBlock);
                    }

                    Region *region = AddrToRegion(block->baseAddr);
                    if(config.trimVirtualMemory &&
                       freeSize * 100 > heapSize * kReleaseThresholdPercentage &&
                       // if block is as big or bigger than a region, free the whole region
                       block->baseAddr <= region->baseAddr &&
                       region->reserveTop <= block->endAddr() )
                    {

                        if(block->baseAddr < region->baseAddr)
                        {
                            HeapBlock *newBlock = Split(block, int((region->baseAddr - block->baseAddr) / kBlockSize));
                            AddToFreeList(block);
                            block = newBlock;
                        }
                        if(block->endAddr() > region->reserveTop)
                        {
                            HeapBlock *newBlock = Split(block, int((region->reserveTop - block->baseAddr) / kBlockSize));
                            AddToFreeList(newBlock);
                        }

                        decommitSize -= block->size;
                        RemoveBlock(block);
                        goto restart;
                    }
                    else if(AVMPI_decommitMemory(block->baseAddr, block->size * kBlockSize))
                    {
                        block->committed = false;
                        block->dirty = false;
                        decommitSize -= block->size;
                        if(config.verbose) {
                            GCLog("decommitted %d page block from %p\n", block->size, block->baseAddr);
                        }
                    }
                    else
                    {
#ifdef MMGC_MAC
                        // this can happen on mac where we release and re-reserve the memory and another thread may steal it from us
                        RemoveLostBlock(block);
                        goto restart;
#else
                        // if the VM API's fail us bail
                        VMPI_abort();
#endif
                    }

                    numDecommitted += block->size;

                    // merge with previous/next if not in use and not committed
                    HeapBlock *prev = block - block->sizePrevious;
                    if(block->sizePrevious != 0 && !prev->committed && !prev->inUse()) {
                        RemoveFromList(prev);

                        prev->size += block->size;

                        block->size = 0;
                        block->sizePrevious = 0;
                        block->baseAddr = 0;

                        block = prev;
                    }

                    HeapBlock *next = block + block->size;
                    if(next->size != 0 && !next->committed && !next->inUse()) {
                        RemoveFromList(next);

                        block->size += next->size;

                        next->size = 0;
                        next->sizePrevious = 0;
                        next->baseAddr = 0;
                    }

                    next = block + block->size;
                    next->sizePrevious = block->size;

                    // add this block to the back of the bus to make sure we consume committed memory
                    // first
                    HeapBlock *backOfTheBus = &freelists[kNumFreeLists-1];
                    HeapBlock *pointToInsert = backOfTheBus;
                    while ((pointToInsert = pointToInsert->next) !=  backOfTheBus) {
                        if (pointToInsert->size >= block->size && !pointToInsert->committed) {
                            break;
                        }
                    }
                    AddToFreeList(block, pointToInsert);

                    // so we keep going through freelist properly
                    block = freelist;

                } else { // not using virtual memory

                    // if we aren't using mmap we can only do something if the block maps to a region
                    // that is completely empty
                    Region *region = AddrToRegion(block->baseAddr);
                    if(block->baseAddr == region->baseAddr && // beginnings match
                       region->commitTop == block->baseAddr + block->size*kBlockSize) {

                        RemoveFromList(block);

                        RemoveBlock(block);

                        goto restart;
                    }
                }
            }
        }

        if(config.verbose)
            DumpHeapRep();
        CheckForStatusReturnToNormal();
    }

    // m_spinlock is held
    void GCHeap::CheckForHardLimitExceeded()
    {
        if (!HardLimitExceeded())
            return;

        Abort();
    }

    // m_spinlock is held
    void GCHeap::CheckForSoftLimitExceeded(size_t request)
    {
        if(config.heapSoftLimit == 0 || status != kMemNormal || !SoftLimitExceeded())
            return;

        size_t externalBlocks = externalPressure / kBlockSize;
        GCDebugMsg(false, "*** Alloc exceeded softlimit: ask for %u, usedheapsize =%u, totalHeap =%u, of which external =%u\n",
                   unsigned(request),
                   unsigned(GetUsedHeapSize() + externalBlocks),
                   unsigned(GetTotalHeapSize() + externalBlocks),
                   unsigned(externalBlocks));

        if(statusNotificationBeingSent())
            return;

        StatusChangeNotify(kMemSoftLimit);
    }

    // m_spinlock is held
    void GCHeap::CheckForStatusReturnToNormal()
    {
        if(!statusNotificationBeingSent() && statusNotNormalOrAbort())
        {
            size_t externalBlocks = externalPressure / kBlockSize;
            size_t total = GetTotalHeapSize() + externalBlocks;

            // return to normal if we drop below heapSoftLimit
            if(config.heapSoftLimit != 0 && status == kMemSoftLimit)
            {
                if (!SoftLimitExceeded())
                {
                    size_t used = GetUsedHeapSize() + externalBlocks;
                    GCDebugMsg(false, "### Alloc dropped below softlimit: usedheapsize =%u, totalHeap =%u, of which external =%u\n",
                               unsigned(used),
                               unsigned(total),
                               unsigned(externalBlocks) );
                    StatusChangeNotify(kMemNormal);
                }
            }
            // or if we shrink to below %10 of the max
            else if ((maxTotalHeapSize / kBlockSize + externalBlocks) * 9 > total * 10)
                StatusChangeNotify(kMemNormal);
        }
    }

#ifdef MMGC_MAC

    void GCHeap::RemoveLostBlock(HeapBlock *block)
    {
        if(config.verbose) {
            GCLog("Removing block %p %d\n", block->baseAddr, block->size);
            DumpHeapRep();
        }

        {
            Region *region = AddrToRegion(block->baseAddr);
            if(region->baseAddr == block->baseAddr && region->reserveTop == block->endAddr()) {
                RemoveBlock(block, /*release*/false);
                return;
            }
        }

        while(AddrToRegion(block->baseAddr) != AddrToRegion(block->endAddr()-1)) {
            // split block into parts mapping to regions
            Region *r = AddrToRegion(block->baseAddr);
            size_t numBlocks = (r->commitTop - block->baseAddr) / kBlockSize;
            char *next = Split(block, numBlocks)->baseAddr;
            // remove it
            RemoveLostBlock(block);
            block = BaseAddrToBlock(next);
        }

        Region *region = AddrToRegion(block->baseAddr);
        // save these off since we'll potentially shift region
        char *regionBaseAddr = region->baseAddr;
        size_t regionBlockId = region->blockId;

        // if we don't line up with beginning or end we need a new region
        if(block->baseAddr != region->baseAddr && region->commitTop != block->endAddr()) {

            GCAssertMsg(HaveFreeRegion(), "Decommit was supposed to ensure this!");

            NewRegion(block->endAddr(), region->reserveTop,
                      region->commitTop > block->endAddr() ? region->commitTop : block->endAddr(),
                      region->blockId + (block->endAddr() - region->baseAddr) / kBlockSize);

            if(region->baseAddr != block->baseAddr) {
                // end this region at the start of block going away
                region->reserveTop = block->baseAddr;
                if(region->commitTop > block->baseAddr)
                    region->commitTop = block->baseAddr;
            }

        } else if(region->baseAddr == block->baseAddr) {
            region->blockId += block->size;
            region->baseAddr = block->endAddr();
        } else if(region->commitTop == block->endAddr()) {
            // end this region at the start of block going away
            region->reserveTop = block->baseAddr;
            if(region->commitTop > block->baseAddr)
                region->commitTop = block->baseAddr;
        } else {
            GCAssertMsg(false, "This shouldn't be possible");
        }


        // create temporary region for this block
        Region temp(this, block->baseAddr, block->endAddr(), block->endAddr(), regionBlockId +  (block->baseAddr - regionBaseAddr) / kBlockSize);

        RemoveBlock(block, /*release*/false);

        // pop temp from freelist, put there by RemoveBlock
        freeRegion = *(Region**)freeRegion;



#ifdef DEBUG
        // doing this here is an extra validation step
        if(config.verbose)
        {
            DumpHeapRep();
        }
#endif
    }

#endif

    void GCHeap::RemoveBlock(HeapBlock *block, bool release)
    {
        Region *region = AddrToRegion(block->baseAddr);

        GCAssert(region->baseAddr == block->baseAddr);
        GCAssert(region->reserveTop == block->endAddr());

        size_t newBlocksLen = blocksLen - block->size;

        HeapBlock *nextBlock = block + block->size;

        bool need_sentinel = false;
        bool remove_sentinel = false;

        if( block->sizePrevious && nextBlock->size ) {
            // This block is contiguous with the blocks before and after it
            // so we need to add a sentinel
            need_sentinel = true;
        }
        else if ( !block->sizePrevious && !nextBlock->size ) {
            // the block is not contigous with the block before or after it - we need to remove a sentinel
            // since there would already be one on each side.
            remove_sentinel = true;
        }

        // update nextblock's sizePrevious
        nextBlock->sizePrevious = need_sentinel ? 0 : block->sizePrevious;

        // Add space for the sentinel - the remaining blocks won't be contiguous
        if(need_sentinel)
            ++newBlocksLen;
        else if(remove_sentinel)
            --newBlocksLen;

        // just re-use blocks; small wastage possible
        HeapBlock *newBlocks = blocks;

        // the memmove will overwrite this so save it
        size_t blockSize = block->size;

        size_t offset = int(block-blocks);
        int32_t sen_offset = 0;
        HeapBlock *src = block + block->size;

        if( need_sentinel ) {
            offset = int(block-blocks)+1;
            sen_offset = 1;
            HeapBlock* sentinel = newBlocks + (block-blocks);
            sentinel->baseAddr = NULL;
            sentinel->size = 0;
            sentinel->sizePrevious = block->sizePrevious;
            sentinel->prev = NULL;
            sentinel->next = NULL;
#if defined(MMGC_MEMORY_PROFILER) && defined(MMGC_MEMORY_INFO)
            sentinel->allocTrace = 0;
#endif
        }
        else if( remove_sentinel ) {
            // skip trailing sentinel
            src++;
            sen_offset = -1;
        }

        // copy blocks after
        int lastChunkSize = int((blocks + blocksLen) - src);
        GCAssert(lastChunkSize + offset == newBlocksLen);
        memmove(newBlocks + offset, src, lastChunkSize * sizeof(HeapBlock));

        // Fix up the prev/next pointers of each freelist.  This is a little more complicated
        // than the similiar code in ExpandHeap because blocks after the one we are free'ing
        // are sliding down by block->size
        HeapBlock *fl = freelists;
        for (uint32_t i=0; i<kNumFreeLists; i++) {
            HeapBlock *temp = fl;
            do {
                if (temp->prev != fl) {
                    if(temp->prev > block) {
                        temp->prev = newBlocks + (temp->prev-blocks-blockSize) + sen_offset;
                    }
                }
                if (temp->next != fl) {
                    if(temp->next > block) {
                        temp->next = newBlocks + (temp->next-blocks-blockSize) + sen_offset;
                    }
                }
            } while ((temp = temp->next) != fl);
            fl++;
        }

        // Need to decrement blockId for regions in blocks after block
        Region *r = lastRegion;
        while(r) {
            if(r->blockId > region->blockId && r->blockId != kLargeItemBlockId) {
                r->blockId -= (blockSize-sen_offset);
            }
            r = r->prev;
        }

        blocksLen = newBlocksLen;
        RemoveRegion(region, release);

        // make sure we did everything correctly
        CheckFreelist();
        ValidateHeapBlocks();
    }

    void GCHeap::ValidateHeapBlocks()
    {
#ifdef _DEBUG
        // iterate through HeapBlocks making sure:
        // non-contiguous regions have a sentinel
        HeapBlock *block = blocks;
        while(block - blocks < (intptr_t)blocksLen) {
            Region *r = AddrToRegion(block->baseAddr);
            if(r && r->baseAddr == block->baseAddr)
                GCAssert(r->blockId == (size_t)(block-blocks));

            HeapBlock *next = NULL;
            if(block->size) {
                next = block + block->size;
                GCAssert(next - blocks < (intptr_t)blocksLen);
                GCAssert(next->sizePrevious == block->size);
            }
            HeapBlock *prev = NULL;
            if(block->sizePrevious) {
                prev = block - block->sizePrevious;
                GCAssert(prev - blocks >= 0);
                GCAssert(prev->size == block->sizePrevious);
            } else if(block != blocks) {
                // I have no prev and I'm not the first, check sentinel
                HeapBlock *sentinel = block-1;
                GCAssert(sentinel->baseAddr == NULL);
                GCAssert(sentinel->size == 0);
                GCAssert(sentinel->sizePrevious != 0);
            }
            if(block->baseAddr) {
                if(prev)
                    GCAssert(block->baseAddr == prev->baseAddr + (kBlockSize * prev->size));
                block = next;
                // we should always end on a sentinel
                GCAssert(next - blocks < (int)blocksLen);
            } else {
                // block is a sentinel
                GCAssert(block->size == 0);
                // FIXME: the following asserts are firing and we need to understand why, could be bugs
                // make sure last block ends at commitTop
                Region *prevRegion = AddrToRegion(prev->baseAddr + (prev->size*kBlockSize) - 1);
                GCAssert(prev->baseAddr + (prev->size*kBlockSize) == prevRegion->commitTop);
                block++;
                // either we've reached the end or the next isn't a sentinel
                GCAssert(block - blocks == (intptr_t)blocksLen || block->size != 0);
            }
        }
        GCAssert(block - blocks == (intptr_t)blocksLen);
#endif
    }

    GCHeap::Region *GCHeap::AddrToRegion(const void *item) const
    {
        // Linear search of regions list to find this address.
        // The regions list should usually be pretty short.
        for (Region *region = lastRegion;
             region != NULL;
             region = region->prev)
        {
            if (item >= region->baseAddr && item < region->reserveTop) {
                return region;
            }
        }
        return NULL;
    }

    GCHeap::HeapBlock* GCHeap::BaseAddrToBlock(const void *item) const
    {
        Region *region = AddrToRegion(item);
        if(region) {
            if(region->blockId == kLargeItemBlockId)
                return NULL;
            size_t index = ((char*)item - region->baseAddr) / kBlockSize;
            HeapBlock *b = blocks + region->blockId + index;
            GCAssert(item == b->baseAddr);
            return b;
        }
        return NULL;
    }

    GCHeap::HeapBlock* GCHeap::InteriorAddrToBlock(const void *item) const
    {
        Region *region = AddrToRegion(item);
        if(region) {
            if(region->blockId == kLargeItemBlockId)
                return NULL;
            size_t index = ((char*)item - region->baseAddr) / kBlockSize;
            HeapBlock *b = blocks + region->blockId + index;
            // if b->size is 0 then either this is an unallocated
            // address or the actual block size > kBlockSize and we
            // need to backtrack to actual block
            while (b->size == 0) {
                // should find block before falling off region start;
                // e.g. (index == 0) implies (b->size > 0).
                GCAssert(index > 0);
                index--;
                b--;
            }
            GCAssert(item >= b->baseAddr && item < b->baseAddr + b->size * GCHeap::kBlockSize);
            return b;
        }
        return NULL;
    }

    size_t GCHeap::SafeSize(const void *item)
    {
        MMGC_LOCK(m_spinlock);
        GCAssert((uintptr_t(item) & kOffsetMask) == 0);
        HeapBlock *block = InteriorAddrToBlock(item);
        if (block)
            return block->size;
        Region *r = AddrToRegion(item);
        if(r && r->blockId == kLargeItemBlockId)
            return LargeAllocSize(item);
        return (size_t)-1;
    }

    // Return the number of blocks of slop at the beginning of an object
    // starting at baseAddr for the given alignment.  Alignment is a
    // number of blocks and must be a power of 2.  baseAddr must
    // point to the beginning of a block.

    static inline size_t alignmentSlop(char* baseAddr, size_t alignment)
    {
        return (alignment - (size_t)(((uintptr_t)baseAddr >> GCHeap::kBlockShift) & (alignment - 1))) & (alignment - 1);
    }

#ifdef DEBUG
    // Reserves region of size == sizeInBytes, while attempting to
    // insert filler >= fill_sz bytes between pairs of consecutively
    // reserved regions.  (Goal: exercise address space extremities
    // w/o actually committing memory within the filler area itself.)
    static char* reserveSomeRegionDispersively(size_t fill_sz, size_t sizeInBytes) {
        static bool retLowAddr = false; // each call toggles low/high.

        void *mem0 = AVMPI_reserveMemoryRegion(NULL, fill_sz);
        void *mem1 = AVMPI_reserveMemoryRegion(NULL, fill_sz);

        if ((retLowAddr && mem0 > mem1) || ( !retLowAddr && mem0 < mem1)) {
            void *swap_tmp = mem0;
            mem0 = mem1;
            mem1 = swap_tmp;
        }

        AVMPI_releaseMemoryRegion(mem0, fill_sz);
        char *addr = (char*)AVMPI_reserveMemoryRegion(mem0, sizeInBytes);
        AVMPI_releaseMemoryRegion(mem1, fill_sz);
        if (addr == NULL) {
            addr = (char*)AVMPI_reserveMemoryRegion(NULL, sizeInBytes);
        }
        retLowAddr = ! retLowAddr;

        return addr;
    }
#endif

    REALLY_INLINE char *GCHeap::ReserveSomeRegion(size_t sizeInBytes)
    {
#ifdef DEBUG
        if (!config.dispersiveAdversarial)
            return (char*)AVMPI_reserveMemoryRegion(NULL, sizeInBytes);
        else
            return reserveSomeRegionDispersively(config.dispersiveAdversarial,
                                                 sizeInBytes);
#else
        return (char*)AVMPI_reserveMemoryRegion(NULL, sizeInBytes);
#endif
    }

    void *GCHeap::LargeAlloc(size_t size, size_t alignment)
    {
        GCAssert(config.useVirtualMemory);
        
        size_t sizeInBytes = size * kBlockSize;

        if(!EnsureFreeRegion(true))
            return NULL;

        char* addr = ReserveSomeRegion(sizeInBytes);

        if(!addr)
            return NULL;

        size_t unalignedSize = sizeInBytes;

        if(alignmentSlop(addr, alignment) != 0) {
            AVMPI_releaseMemoryRegion(addr, sizeInBytes);
            unalignedSize = sizeInBytes + (alignment-1) * kBlockSize;
            addr = ReserveSomeRegion(unalignedSize);
            if(!addr)
                return NULL;
        }

        char *alignedAddr = addr + alignmentSlop(addr, alignment) * kBlockSize;
        if(!AVMPI_commitMemory(alignedAddr, sizeInBytes)) {
            AVMPI_releaseMemoryRegion(addr, sizeInBytes);
            return NULL;
        }

        // Note that we don't actually track the beginning of the committed region
        // LargeFree doesn't need it.
        NewRegion(addr,
                  addr + unalignedSize, // reserveTop
                  alignedAddr + sizeInBytes, // commitTop
                  kLargeItemBlockId);
        largeAllocs += size;
        CheckForNewMaxTotalHeapSize();

        return alignedAddr;
    }

    void GCHeap::LargeFree(const void *item)
    {
        GCAssert(config.useVirtualMemory);

        size_t size = LargeAllocSize(item);
        largeAllocs -= size;
        Region *r = AddrToRegion(item);
        // Must use r->baseAddr which may be less than item due to alignment,
        // and we must calculate full size
        AVMPI_releaseMemoryRegion(r->baseAddr, r->reserveTop - r->baseAddr);
        RemoveRegion(r, false);
    }

    GCHeap::HeapBlock* GCHeap::AllocBlock(size_t size, bool& zero, size_t alignment)
    {
        uint32_t startList = GetFreeListIndex(size);
        HeapBlock *freelist = &freelists[startList];

        HeapBlock *decommittedSuitableBlock = NULL;

        // Search for a big enough block in the free lists

        for (uint32_t i = startList; i < kNumFreeLists; i++)
        {
            HeapBlock *block = freelist;
            while ((block = block->next) != freelist)
            {
                // Prefer a single committed block that is at least large enough.

                if (block->size >= size + alignmentSlop(block->baseAddr, alignment) && block->committed) {
                    RemoveFromList(block);
                    return AllocCommittedBlock(block, size, zero, alignment);
                }

                // Look for a sequence of decommitted and committed blocks that together would
                // be large enough, in case a single committed block can't be found.

                if(config.useVirtualMemory && decommittedSuitableBlock == NULL && !block->committed)
                {
                    // Note, 'block' should be invariant throughout this scope, it's the block
                    // whose successors and predecessors we're inspecting

                    GCAssert(!block->inUse());

                    size_t totalSize = block->size;
                    HeapBlock *firstFree = block;
                    size_t firstSlop = alignmentSlop(firstFree->baseAddr, alignment);

                    // Coalesce with predecessors
                    while(totalSize < size + firstSlop && firstFree->sizePrevious != 0)
                    {
                        HeapBlock *prevBlock = firstFree - firstFree->sizePrevious;
                        if(prevBlock->inUse() || prevBlock->size == 0)
                            break;
                        totalSize += prevBlock->size;
                        firstFree = prevBlock;
                        firstSlop = alignmentSlop(firstFree->baseAddr, alignment);
                    }

                    // Coalesce with successors
                    HeapBlock *nextBlock = block + block->size;
                    while (totalSize < size + firstSlop && !(nextBlock->inUse() || nextBlock->size == 0)) {
                        totalSize += nextBlock->size;
                        nextBlock = nextBlock + nextBlock->size;
                    }

                    // Keep it if it's large enough
                    if(totalSize >= size + firstSlop)
                        decommittedSuitableBlock = firstFree;
                }
            }
            freelist++;
        }

        // We only get here if we couldn't find a single committed large enough block.

        if (decommittedSuitableBlock != NULL)
            return AllocCommittedBlock(CreateCommittedBlock(decommittedSuitableBlock, size, alignment),
                                       size,
                                       zero,
                                       alignment);

        return NULL;
    }

    GCHeap::HeapBlock* GCHeap::AllocCommittedBlock(HeapBlock* block, size_t size, bool& zero, size_t alignment)
    {
        GCAssert(block->committed);
        GCAssert(block->size >= size);
        GCAssert(block->inUse());

        size_t slop = alignmentSlop(block->baseAddr, alignment);

        if (slop > 0)
        {
            HeapBlock *oldBlock = block;
            block = Split(block, slop);
            AddToFreeList(oldBlock);
            GCAssert(alignmentSlop(block->baseAddr, alignment) == 0);
            GCAssert(block->size >= size);
        }

        if(block->size > size)
        {
            HeapBlock *newBlock = Split(block, size);
            AddToFreeList(newBlock);
        }

        CheckFreelist();

        zero = block->dirty && zero;

#ifdef _DEBUG
        if (!block->dirty)
        {
            union {
                const char* base_c;
                const uint32_t* base_u;
            };
            base_c = block->baseAddr;
            GCAssert(*base_u == 0);
        }
#endif
        return block;
    }

    // Turn a sequence of committed and uncommitted blocks into a single committed
    // block that's at least large enough to satisfy the request.

    GCHeap::HeapBlock* GCHeap::CreateCommittedBlock(HeapBlock* block, size_t size, size_t alignment)
    {
        RemoveFromList(block);

        // We'll need to allocate extra space to account for the space that will
        // later be removed from the start of the block.

        size += alignmentSlop(block->baseAddr, alignment);

        // If the first block is too small then coalesce it with the following blocks
        // to create a block that's large enough.  Some parts of the total block may
        // already be committed.  If the platform allows it we commit the entire
        // range with one call even if parts were committed before, on the assumption
        // that that is faster than several commit() calls, one for each decommitted
        // block.  (We have no current data to support that; now == 201-03-19.)

        if(block->size < size)
        {
            size_t amountRecommitted = block->committed ? 0 : block->size;
            bool dirty = block->dirty;

            // The first block needs to be committed when sloppyCommit is disabled.
            if(!config.sloppyCommit && !block->committed)
                Commit(block);

            while(block->size < size)
            {
                // Coalesce the next block into this one.

                HeapBlock *nextBlock = block + block->size;
                RemoveFromList(nextBlock);

                if (nextBlock->committed)
                    dirty = dirty || nextBlock->dirty;
                else
                {
                    if (block->size + nextBlock->size >= size)  // Last block?
                        PruneDecommittedBlock(nextBlock, block->size + nextBlock->size, size);

                    amountRecommitted += nextBlock->size;

                    if (!config.sloppyCommit)
                        Commit(nextBlock);
                }

                block->size += nextBlock->size;

                nextBlock->size = 0;
                nextBlock->baseAddr = 0;
                nextBlock->sizePrevious = 0;
            }

            (block + block->size)->sizePrevious = block->size;

            GCAssert(amountRecommitted > 0);

            if (config.sloppyCommit)
                Commit(block);
            block->dirty = dirty;
        }
        else
        {
            PruneDecommittedBlock(block, block->size, size);
            Commit(block);
        }

        GCAssert(block->size >= size && block->committed);

        CheckFreelist();

        return block;
    }

    // If the tail of a coalesced block is decommitted and committing it creates
    // a block that's too large for the request then we may wish to split the tail
    // before committing it in order to avoid committing memory we won't need.
    //
    // 'available' is the amount of memory available including the memory in 'block',
    // and 'request' is the amount of memory required.

    void GCHeap::PruneDecommittedBlock(HeapBlock* block, size_t available, size_t request)
    {
        GCAssert(available >= request);
        GCAssert(!block->committed);

        size_t toCommit = request > kMinHeapIncrement ? request : kMinHeapIncrement;
        size_t leftOver = available - request;

        if (available > toCommit && leftOver > 0)
        {
            HeapBlock *newBlock = Split(block, block->size - leftOver);
            AddToFreeList(newBlock);
        }
    }

    GCHeap::HeapBlock *GCHeap::Split(HeapBlock *block, size_t size)
    {
        GCAssert(block->size > size);
        HeapBlock *newBlock = block + size;
        newBlock->Init(block->baseAddr + kBlockSize * size, block->size - size, block->dirty);
        newBlock->sizePrevious = size;
        newBlock->committed = block->committed;
        block->size = size;

        // Update sizePrevious in block after that
        HeapBlock *nextBlock = newBlock + newBlock->size;
        nextBlock->sizePrevious = newBlock->size;

        return newBlock;
    }

    void GCHeap::Commit(HeapBlock *block)
    {
        GCAssert(config.sloppyCommit || !block->committed);

        if(!AVMPI_commitMemory(block->baseAddr, block->size * kBlockSize))
        {
            GCAssert(false);
        }
        if(config.verbose) {
            GCLog("recommitted %d pages\n", block->size);
            DumpHeapRep();
        }
        numDecommitted -= block->size;
        block->committed = true;
        block->dirty = AVMPI_areNewPagesDirty();
    }

#ifdef _DEBUG
    // Non-debug version in GCHeap.h
    void GCHeap::CheckFreelist()
    {
        HeapBlock *freelist = freelists;
        for (uint32_t i = 0; i < kNumFreeLists; i++)
        {
            HeapBlock *block = freelist;
            while((block = block->next) != freelist)
            {
                GCAssert(block != block->next);
                GCAssert(block != block->next->next || block->next == freelist);

                // Coalescing is eager so no block on the list should have adjacent blocks
                // that are also on the free list and in the same committed state

                if(block->sizePrevious)
                {
                    HeapBlock *prev = block - block->sizePrevious;
                    GCAssert(block->sizePrevious == prev->size);
                    GCAssert(prev->inUse() || prev->size == 0 || prev->committed != block->committed);
                }
                {
                    HeapBlock *next = block + block->size;
                    GCAssert(next->inUse() || next->size == 0 || next->committed != block->committed);
                }
            }
            freelist++;
        }
#if 0
// Debugging code to find problems with block/region layout
// This code is slow, but can be useful for tracking down issues
// It verifies that the memory for each block corresponds to one or more regions
// and that each region points to a valid starting block
        Region* r = lastRegion;

        int block_idx = 0;
        bool errors =false;
        for(block_idx = 0; block_idx < blocksLen; ++block_idx){
            HeapBlock* b = blocks + block_idx;

            if( !b->size )
                continue;

            int contig_size = 0;
            r = lastRegion;

            while( r ){
                if(b->baseAddr >= r->baseAddr && b->baseAddr < r->reserveTop ) {
                    // starts in this region
                    char* end = b->baseAddr + b->size*kBlockSize;
                    if(end > (r->reserveTop + contig_size) ){
                        GCLog("error, block %d %p %d did not find a matching region\n", block_idx, b->baseAddr, b->size);
                        GCLog("Started in region %p - %p, contig size: %d\n", r->baseAddr, r->reserveTop, contig_size);
                        errors = true;
                        break;
                    }
                }
                else if( r->prev && r->prev->reserveTop==r->baseAddr){
                    contig_size +=r->reserveTop - r->baseAddr;
                }
                else{
                    contig_size = 0;
                }

                r = r->prev;
            }
        }

        while(r)
            {
                if(!blocks[r->blockId].size){
                    for( int i = r->blockId-1; i >= 0 ; --i )
                        if( blocks[i].size){
                            //Look for spanning blocks
                            if( ((blocks[i].baseAddr + blocks[i].size*kBlockSize) <= r->baseAddr) ) {
                                GCLog("Invalid block id for region %p-%p %d\n", r->baseAddr, r->reserveTop, i);
                                errors =true;
                                break;
                            }
                            else
                                break;
                        }
                }
                r = r->prev;
           }
        if( errors ){
            r = lastRegion;
            while(r) {
                GCLog("%p - %p\n", r->baseAddr, r->reserveTop);
                r = r->prev;
            }
            for(int b = 0; b < blocksLen; ++b ){
                if(!blocks[b].size)
                    continue;
                GCLog("%d %p %d\n", b, blocks[b].baseAddr, blocks[b].size);
            }
            asm("int3");
        }
#endif
    }
#endif // DEBUG

    bool GCHeap::BlocksAreContiguous(void *item1, void *item2)
    {
        Region *r1 = AddrToRegion(item1);
        Region *r2 = AddrToRegion(item2);
        return r1 == r2 || r1->reserveTop == r2->baseAddr;
    }

    void GCHeap::AddToFreeList(HeapBlock *block, HeapBlock* pointToInsert)
    {
        CheckFreelist();

        block->next = pointToInsert;
        block->prev = pointToInsert->prev;
        block->prev->next = block;
        pointToInsert->prev = block;

        CheckFreelist();
    }

    void GCHeap::AddToFreeList(HeapBlock* block, bool makeDirty)
    {
        GCAssert(block->size != 0);

        // Try to coalesce a committed block with its committed non-sentinel predecessor
        if(block->committed && block->sizePrevious)
        {
            HeapBlock *prevBlock = block - block->sizePrevious;
            GCAssert(prevBlock->size > 0 || !prevBlock->committed);

            if (!prevBlock->inUse() && prevBlock->committed)
            {
                // Remove predecessor block from free list
                RemoveFromList(prevBlock);

                // Increase size of predecessor block
                prevBlock->size += block->size;

                block->size = 0;
                block->sizePrevious = 0;
                block->baseAddr = 0;

                block = prevBlock;
                makeDirty = makeDirty || block->dirty;
            }
        }

        // Try to coalesce a committed block with its committed non-sentinel successor
        if (block->committed)
        {
            HeapBlock *nextBlock = block + block->size;
            // This is not correct - sentinels are not necessarily committed.  We
            // may or may not want to fix that.
            //GCAssert(nextBlock->size > 0 || !nextBlock->committed);

            if (!nextBlock->inUse() && nextBlock->committed) {
                // Remove successor block from free list
                RemoveFromList(nextBlock);

                // Increase size of current block
                block->size += nextBlock->size;
                nextBlock->size = 0;
                nextBlock->baseAddr = 0;
                nextBlock->sizePrevious = 0;
                makeDirty = makeDirty || nextBlock->dirty;
            }
        }

        // Update sizePrevious in the next block
        HeapBlock *nextBlock = block + block->size;
        nextBlock->sizePrevious = block->size;

        block->dirty = block->dirty || makeDirty;

        // Add the coalesced block to the right free list, in the right
        // position.  Free lists are ordered by increasing block size.
        {
            int index = GetFreeListIndex(block->size);
            HeapBlock *freelist = &freelists[index];
            HeapBlock *pointToInsert = freelist;

            // If the block size is below kUniqueThreshold then its free list
            // will have blocks of only one size and no search is needed.

            if (block->size >= kUniqueThreshold) {
                while ((pointToInsert = pointToInsert->next) != freelist) {
                    if (pointToInsert->size >= block->size) {
                        break;
                    }
                }
            }

            AddToFreeList(block, pointToInsert);
        }
    }

    void GCHeap::FreeBlock(HeapBlock *block)
    {
        GCAssert(block->inUse());

#ifdef _DEBUG
        // trash it. fb == free block
        if (!RUNNING_ON_VALGRIND)
            VMPI_memset(block->baseAddr, uint8_t(MMFreedPoison), block->size * kBlockSize);
#endif

        AddToFreeList(block, true);
    }

    void GCHeap::CheckForNewMaxTotalHeapSize()
    {
        // GetTotalHeapSize is probably fairly cheap; even so this strikes me
        // as a bit of a hack.
        size_t heapSizeNow = GetTotalHeapSize() * kBlockSize;
        if (heapSizeNow > maxTotalHeapSize) {
            maxTotalHeapSize = heapSizeNow;
#ifdef MMGC_POLICY_PROFILING
            // The guard on instance being non-NULL is a hack, to be fixed later (now=2009-07-20).
            // Some VMPI layers (WinMo is at least one of them) try to grab the GCHeap instance to get
            // at the map of private pages.  But the GCHeap instance is not available during the initial
            // call to ExpandHeap.  So sidestep that problem here.
            //
            // Note that if CheckForNewMaxTotalHeapSize is only called once then maxPrivateMemory
            // will be out of sync with maxTotalHeapSize, see also bugzilla 608684.
            if (instance != NULL)
                maxPrivateMemory = AVMPI_getPrivateResidentPageCount() * VMPI_getVMPageSize();
#endif
        }
    }

    bool GCHeap::ExpandHeap( size_t askSize)
    {
        bool bRetVal = ExpandHeapInternal(askSize);
        CheckForNewMaxTotalHeapSize();
        return bRetVal;
    }

    bool GCHeap::HardLimitExceeded(size_t additionalAllocationAmt)
    {
        return GetTotalHeapSize() + externalPressure/kBlockSize + additionalAllocationAmt > config.heapLimit;
    }

    bool GCHeap::SoftLimitExceeded(size_t additionalAllocationAmt)
    {
        if (config.heapSoftLimit == 0) return false;
        return GetTotalHeapSize() + externalPressure/kBlockSize + additionalAllocationAmt > config.heapSoftLimit;
    }

#define roundUp(_s, _inc) (((_s + _inc - 1) / _inc) * _inc)

    bool GCHeap::ExpandHeapInternal(size_t askSize)
    {
        size_t size = askSize;

#ifdef _DEBUG
        // Turn this switch on to test bridging of contiguous
        // regions.
        bool test_bridging = false;
        size_t defaultReserve = test_bridging ? (size+kMinHeapIncrement) : kDefaultReserve;
#else
        const size_t defaultReserve = kDefaultReserve;
#endif

        char *baseAddr = NULL;
        char *newRegionAddr = NULL;
        size_t newRegionSize = 0;
        bool contiguous = false;
        size_t commitAvail = 0;

        // Round up to the nearest kMinHeapIncrement
        size = roundUp(size, kMinHeapIncrement);

        // when we allocate a new region the space needed for the HeapBlocks, if it won't fit
        // in existing space it must fit in new space so we may need to increase the new space

        HeapBlock *newBlocks = blocks;

        if(blocksLen != 0 || // first time through just take what we need out of initialSize instead of adjusting
           config.initialSize == 0) // unless initializeSize is zero of course
        {
            int extraBlocks = 1; // for potential new sentinel
            if(nextRegion == NULL) {
                extraBlocks++; // may need a new page for regions
            }
            size_t curHeapBlocksSize = blocks ? BaseAddrToBlock(blocks)->size : 0;
            size_t newHeapBlocksSize = numHeapBlocksToNumBlocks(blocksLen + size + extraBlocks);

            // size is based on newSize and vice versa, loop to settle (typically one loop, sometimes two)
            while(newHeapBlocksSize > curHeapBlocksSize)
            {
                // use askSize so HeapBlock's can fit in rounding slop
                size = roundUp(askSize + newHeapBlocksSize + extraBlocks, kMinHeapIncrement);

                // tells us use new memory for blocks below
                newBlocks = NULL;

                // since newSize is based on size we have to repeat in case it changes
                curHeapBlocksSize = newHeapBlocksSize;
                newHeapBlocksSize = numHeapBlocksToNumBlocks(blocksLen + size + extraBlocks);
            }
        }

        // At this point we have adjusted/calculated the final size that would need to be committed.
        // We need to check that against the hardlimit to see if we are going to go above it.
        if (HardLimitExceeded(size))
            return false;

        if(config.useVirtualMemory)
        {
            Region *region = lastRegion;
            if (region != NULL)
            {
                commitAvail = (int)((region->reserveTop - region->commitTop) / kBlockSize);

                // Can this request be satisfied purely by committing more memory that
                // is already reserved?
                if (size <= commitAvail) {
                    if (AVMPI_commitMemory(region->commitTop, size * kBlockSize))
                    {
                        // Succeeded!
                        baseAddr = region->commitTop;

                        // check for continuity, we can only be contiguous with the end since
                        // we don't have a "block insert" facility
                        HeapBlock *last = &blocks[blocksLen-1] - blocks[blocksLen-1].sizePrevious;
                        contiguous = last->baseAddr + last->size * kBlockSize == baseAddr;

                        // Update the commit top.
                        region->commitTop += size*kBlockSize;

                        // Go set up the block list.
                        goto gotMemory;
                    }
                    else
                    {
                        // If we can't commit memory we've already reserved,
                        // no other trick is going to work.  Fail.
                        return false;
                    }
                }

                // Try to reserve a region contiguous to the last region.

                // - Try for the "default reservation size" if it's larger than
                //   the requested block.
                if (defaultReserve > size) {
                    newRegionAddr = (char*) AVMPI_reserveMemoryRegion(region->reserveTop,
                                                  defaultReserve * kBlockSize);
                    newRegionSize = defaultReserve;
                }

                // - If the default reservation size didn't work or isn't big
                //   enough, go for the exact amount requested, minus the
                //   committable space in the current region.
                if (newRegionAddr == NULL) {
                    newRegionAddr = (char*) AVMPI_reserveMemoryRegion(region->reserveTop,
                                                  (size - commitAvail)*kBlockSize);
                    newRegionSize = size - commitAvail;

                    // check for contiguity
                    if(newRegionAddr && newRegionAddr != region->reserveTop) {
                        // we can't use this space since we need commitAvail from prev region to meet
                        // the size requested, toss it
                        ReleaseMemory(newRegionAddr,  newRegionSize*kBlockSize);
                        newRegionAddr = NULL;
                        newRegionSize = 0;
                    }
                }

                if (newRegionAddr == region->reserveTop)  // we'll use the region below as a separate region if its not contiguous
                {
                    // We were able to reserve some space.

                    // Commit available space from the existing region.
                    if (commitAvail != 0) {
                        if (!AVMPI_commitMemory(region->commitTop, commitAvail * kBlockSize))
                        {
                            // We couldn't commit even this space.  We're doomed.
                            // Un-reserve the space we just reserved and fail.
                            ReleaseMemory(newRegionAddr, newRegionSize);
                            return false;
                        }
                    }

                    // Commit needed space from the new region.
                    if (!AVMPI_commitMemory(newRegionAddr, (size - commitAvail) * kBlockSize))
                    {
                        // We couldn't commit this space.  We can't meet the
                        // request.  Un-commit any memory we just committed,
                        // un-reserve any memory we just reserved, and fail.
                        if (commitAvail != 0) {
                            AVMPI_decommitMemory(region->commitTop,
                                           commitAvail * kBlockSize);
                        }
                        ReleaseMemory(newRegionAddr,
                                      (size-commitAvail)*kBlockSize);
                        return false;
                    }

                    // We successfully reserved a new contiguous region
                    // and committed the memory we need.  Finish up.
                    baseAddr = region->commitTop;
                    region->commitTop = lastRegion->reserveTop;

                    // check for continuity, we can only be contiguous with the end since
                    // we don't have a "block insert" facility
                    HeapBlock *last = &blocks[blocksLen-1] - blocks[blocksLen-1].sizePrevious;
                    contiguous = last->baseAddr + last->size * kBlockSize == baseAddr;

                    goto gotMemory;
                }
            }

            // We were unable to allocate a contiguous region, or there
            // was no existing region to be contiguous to because this
            // is the first-ever expansion.  Allocate a non-contiguous region.

            // Don't use any of the available space in the current region.
            commitAvail = 0;

            // - Go for the default reservation size unless the requested
            //   size is bigger.
            if (newRegionAddr == NULL && size < defaultReserve) {
                newRegionAddr = ReserveSomeRegion(defaultReserve*kBlockSize);
                newRegionSize = defaultReserve;
            }

            // - If that failed or the requested size is bigger than default,
            //   go for the requested size exactly.
            if (newRegionAddr == NULL) {
                newRegionAddr = ReserveSomeRegion(size*kBlockSize);
                newRegionSize = size;
            }

            // - If that didn't work, give up.
            if (newRegionAddr == NULL) {
                return false;
            }

            // - Try to commit the memory.
            if (AVMPI_commitMemory(newRegionAddr,
                             size*kBlockSize) == 0)
            {
                // Failed.  Un-reserve the memory and fail.
                ReleaseMemory(newRegionAddr, newRegionSize*kBlockSize);
                return false;
            }

            // If we got here, we've successfully allocated a
            // non-contiguous region.
            baseAddr = newRegionAddr;
            contiguous = false;

        }
        else
        {
            // Allocate the requested amount of space as a new region.
            newRegionAddr = (char*)AVMPI_allocateAlignedMemory(size * kBlockSize);
            baseAddr = newRegionAddr;
            newRegionSize = size;

            // If that didn't work, give up.
            if (newRegionAddr == NULL) {
                return false;
            }
        }

    gotMemory:

        // If we were able to allocate a contiguous block, remove
        // the old top sentinel.
        if (contiguous) {
            blocksLen--;
        }

        // Expand the block list.
        size_t newBlocksLen = blocksLen + size;

        // Add space for the "top" sentinel
        newBlocksLen++;

        if (!newBlocks) {
            newBlocks = (HeapBlock*)(void *)baseAddr;
        }

        // Copy all the existing blocks.
        if (blocks && blocks != newBlocks) {
            VMPI_memcpy(newBlocks, blocks, blocksLen * sizeof(HeapBlock));

            // Fix up the prev/next pointers of each freelist.
            HeapBlock *freelist = freelists;
            for (uint32_t i=0; i<kNumFreeLists; i++) {
                HeapBlock *temp = freelist;
                do {
                    if (temp->prev != freelist) {
                        temp->prev = newBlocks + (temp->prev-blocks);
                    }
                    if (temp->next != freelist) {
                        temp->next = newBlocks + (temp->next-blocks);
                    }
                } while ((temp = temp->next) != freelist);
                freelist++;
            }
            CheckFreelist();
        }

        // Create a single free block for the new space,
        // and add it to the free list.
        HeapBlock *block = newBlocks+blocksLen;
        block->Init(baseAddr, size, newPagesDirty());

        // link up contiguous blocks
        if(blocksLen && contiguous)
        {
            // search backwards for first real block
            HeapBlock *b = &blocks[blocksLen-1];
            while(b->size == 0)
            {
                b--;
                GCAssert(b >= blocks);
            }
            block->sizePrevious = b->size;
            GCAssert((block - block->sizePrevious)->size == b->size);
        }

        // if baseAddr was used for HeapBlocks split
        if((char*)newBlocks == baseAddr)
        {
            size_t numBlocksNeededForHeapBlocks = numHeapBlocksToNumBlocks(newBlocksLen);
            HeapBlock *next = Split(block, numBlocksNeededForHeapBlocks);
            // this space counts as used space
            numAlloc += numBlocksNeededForHeapBlocks;
            block = next;
        }

        // get space for region allocations
        if(nextRegion == NULL) {
            nextRegion = (Region*)(void *)block->baseAddr;
            HeapBlock *next = Split(block, 1);
            // this space counts as used space
            numAlloc++;
            numRegionBlocks++;
            block = next;
        }

        // Save off and add after initializing all blocks.
        HeapBlock *newBlock = block;

        // Initialize the rest of the new blocks to empty.
        size_t freeBlockSize = block->size;

        for (uint32_t i=1; i < freeBlockSize; i++) {
            block++;
            block->Clear();
        }

        // Fill in the sentinel for the top of the heap.
        block++;
        block->Clear();
        block->sizePrevious = freeBlockSize;

        AddToFreeList(newBlock);

        // save for free'ing
        void *oldBlocks = blocks;

        blocks = newBlocks;
        blocksLen = newBlocksLen;

        // free old blocks space using new blocks (FreeBlock poisons blocks so can't use old blocks)
        if (oldBlocks && oldBlocks != newBlocks) {
            HeapBlock *oldBlocksHB = BaseAddrToBlock(oldBlocks);
            numAlloc -= oldBlocksHB->size;
            FreeBlock(oldBlocksHB);
        }

        // If we created a new region, save the base address so we can free later.
        if (newRegionAddr) {
            /*  The mergeContiguousRegions bit is broken, since we
                loop over all regions we may be contiguous with an
                existing older HeapBlock and we don't support inserting a
                new address range arbritrarily into the HeapBlock
                array (contiguous regions must be contiguous heap
                blocks vis-a-vie the region block id)
            if(contiguous &&
                config.mergeContiguousRegions) {
                lastRegion->reserveTop += newRegionSize*kBlockSize;
                lastRegion->commitTop +=
                (size-commitAvail)*kBlockSize;
                } else
            */ {
                Region *newRegion = NewRegion(newRegionAddr,  // baseAddr
                                              newRegionAddr+newRegionSize*kBlockSize, // reserve top
                                              newRegionAddr+(size-commitAvail)*kBlockSize, // commit top
                                              newBlocksLen-(size-commitAvail)-1); // block id

                if(config.verbose)
                    GCLog("reserved new region, %p - %p %s\n",
                          newRegion->baseAddr,
                          newRegion->reserveTop,
                          contiguous ? "contiguous" : "non-contiguous");
            }
        }

        CheckFreelist();

        if(config.verbose) {
            GCLog("heap expanded by %d pages\n", size);
            DumpHeapRep();
        }
        ValidateHeapBlocks();

        // Success!
        return true;
    }

    void GCHeap::RemoveRegion(Region *region, bool release)
    {
        Region **next = &lastRegion;
        while(*next != region)
            next = &((*next)->prev);
        *next = region->prev;
        if(release) {
            ReleaseMemory(region->baseAddr,
                          region->reserveTop-region->baseAddr);
        }
        if(config.verbose) {
            GCLog("unreserved region 0x%p - 0x%p (commitTop: %p)\n", region->baseAddr, region->reserveTop, region->commitTop);
            DumpHeapRep();
        }
        FreeRegion(region);
    }

    void GCHeap::FreeAll()
    {
        // Release all of the heap regions
        while (lastRegion != NULL) {
            Region *region = lastRegion;
            lastRegion = lastRegion->prev;
            if(region->blockId == kLargeItemBlockId) {
                // leaks can happen during abort
                GCAssertMsg(status == kMemAbort, "Allocation of large object not freed");
                AVMPI_releaseMemoryRegion(region->baseAddr, region->reserveTop - region->baseAddr);
            } else {
                ReleaseMemory(region->baseAddr,
                              region->reserveTop-region->baseAddr);
            }
        }
    }

#ifdef MMGC_HOOKS
    void GCHeap::AllocHook(const void *item, size_t askSize, size_t gotSize, bool managed)
    {
        (void)item;
        (void)askSize;
        (void)gotSize;
        (void)managed;
#ifdef MMGC_MEMORY_PROFILER
        if(hasSpy) {
            AVMPI_spyCallback();
        }
        if(profiler)
            profiler->RecordAllocation(item, askSize, gotSize, managed);
#else
        (void)managed;
#endif

#ifdef MMGC_MEMORY_INFO
        DebugDecorate(item, gotSize);
#endif
#if defined(AVMPLUS_SAMPLER) || (defined(VMCFG_TELEMETRY_SAMPLER) && defined(DEBUGGER))
        avmplus::recordAllocationSample(item, gotSize);
#endif
    }

    void GCHeap::FinalizeHook(const void *item, size_t size)
    {
        (void)item,(void)size;
#ifdef MMGC_MEMORY_PROFILER
        if(profiler)
            profiler->RecordDeallocation(item, size);
#endif

#if defined(AVMPLUS_SAMPLER) || (defined(VMCFG_TELEMETRY_SAMPLER) && defined(DEBUGGER))
        avmplus::recordDeallocationSample(item, size);
#endif
    }

    void GCHeap::FreeHook(const void *item, size_t size, int poison)
    {
        (void)poison,(void)item,(void)size;
#ifdef MMGC_MEMORY_INFO
        DebugFree(item, poison, size, true);
#endif
    }

    void GCHeap::PseudoFreeHook(const void *item, size_t size, int poison)
    {
        (void)poison,(void)item,(void)size;
#ifdef MMGC_MEMORY_INFO
        DebugFree(item, poison, size, false);
#endif
    }
#endif // MMGC_HOOKS

    EnterFrame::EnterFrame() :
        m_heap(NULL),
        m_gc(NULL),
        m_abortUnwindList(NULL),
        m_previous(NULL),
        m_suspended(false)
    {
        GCHeap *heap = GCHeap::GetGCHeap();
        EnterFrame *ef = m_previous = heap->GetEnterFrame();

        if(ef && ef->Suspended()) {
            // propagate the active gc from the suspended frame
            m_gc = ef->GetActiveGC();
        }

        if(ef == NULL || ef->Suspended()) {
            m_heap = heap;
            heap->Enter(this);
        }
    }

    // this is the first thing we run after the Abort longjmp
    EnterFrame::~EnterFrame()
    {
        if(m_heap) {
            GCHeap *heap = m_heap;
            // this prevents us from doing multiple jumps in case leave results in more allocations
            m_heap = NULL;
            heap->Leave();
        }
    }

    void EnterFrame::UnwindAllObjects()
    {
        while(m_abortUnwindList)
        {
            AbortUnwindObject *previous = m_abortUnwindList;
            m_abortUnwindList->Unwind();
            //  The unwind call may remove the handler or may leave it on this list.  If it leaves it, then make sure to advance the list,
            //  otherwise, the list will automatically advance if it's removed.
            if (m_abortUnwindList == previous)
            {
                m_abortUnwindList = m_abortUnwindList->next;
            }
        }
    }

    void EnterFrame::AddAbortUnwindObject(AbortUnwindObject *obj)
    {
        GCAssertMsg(!EnterFrame::IsAbortUnwindObjectInList(obj), "obj can't be added to list twice!");
        //  Push it on the front
        obj->next = m_abortUnwindList;
        if (m_abortUnwindList)
        {
            m_abortUnwindList->previous = obj;
        }
        m_abortUnwindList = obj;
    }

    void EnterFrame::RemoveAbortUnwindObject(AbortUnwindObject *obj)
    {
        GCAssertMsg(obj == m_abortUnwindList || obj->previous != NULL, "Object not in list");

        if (obj == m_abortUnwindList)
        {
            m_abortUnwindList = obj->next;
        }

        if (obj->previous != NULL)
        {
            (obj->previous)->next = obj->next;
        }
        if (obj->next != NULL)
        {
            (obj->next)->previous = obj->previous;
        }

        obj->next = NULL;
        obj->previous = NULL;
    }

#ifdef DEBUG

    AbortUnwindObject::~AbortUnwindObject()
    {
        GCAssertMsg(!EnterFrame::IsAbortUnwindObjectInList(this), "RemoveAbortUnwindObject not called, dangling pointer in list.");
    }

    /*static*/
    bool EnterFrame::IsAbortUnwindObjectInList(AbortUnwindObject *obj)
    {
        GCHeap *heap = GCHeap::GetGCHeap();
        EnterFrame *frame;
        if(heap && (frame = heap->GetEnterFrame()) != NULL)
        {
            AbortUnwindObject *list = frame->m_abortUnwindList;
            while(list) {
                if(list == obj)
                    return true;
                list = list->next;
            }
        }
        return false;
    }
#endif

    SuspendEnterFrame::SuspendEnterFrame() : m_ef(NULL)
    {
        GCHeap *heap = GCHeap::GetGCHeap();
        if(heap) {
            EnterFrame *ef = heap->GetEnterFrame();
            if(ef) {
                ef->Suspend();
                m_ef = ef;
            }
        }
    }

    SuspendEnterFrame::~SuspendEnterFrame()
    {
        if(m_ef)
            m_ef->Resume();
        GCHeap *heap = GCHeap::GetGCHeap();
        GCAssertMsg(heap->GetEnterFrame() == m_ef, "EnterFrame's not unwound properly");
        if(heap->GetStatus() == kMemAbort)
            heap->Abort();
    }

    void GCHeap::SystemOOMEvent(size_t size, int attempt)
    {
        MMGC_LOCK(m_spinlock);
        if (attempt == 0 && !statusNotificationBeingSent())
            SendFreeMemorySignal(size/kBlockSize + 1);
        else
            Abort();
    }

    /*static*/
    void GCHeap::SignalObjectTooLarge()
    {
        GCLog("Implementation limit exceeded: attempting to allocate too-large object\n");
        GetGCHeap()->Abort();
    }

    /*static*/
    void GCHeap::SignalInconsistentHeapState(const char* reason)
    {
        GCAssert(!"Inconsistent heap state; aborting");
        GCLog("Inconsistent heap state: %s\n", reason);
        GetGCHeap()->Abort();
    }

    /*static*/
    void GCHeap::SignalExternalAllocation(size_t nbytes)
    {
        GCHeap* heap = GetGCHeap();

        MMGC_LOCK(heap->m_spinlock);

        heap->externalPressure += nbytes;

        heap->CheckForMemoryLimitsExceeded();

    }

    /*static*/
    void GCHeap::SignalExternalDeallocation(size_t nbytes)
    {
        GCHeap* heap = GetGCHeap();

        MMGC_LOCK(heap->m_spinlock);

        heap->externalPressure -= nbytes;
        heap->CheckForStatusReturnToNormal();
    }

    /*static*/
    void GCHeap::SignalExternalFreeMemory(size_t minimumBytesToFree /*= kMaxObjectSize */)
    {
        GCHeap* heap = GetGCHeap();
        GCAssertMsg(heap != NULL, "GCHeap not valid!");

        MMGC_LOCK(heap->m_spinlock);

        // When calling SendFreeMemorySignal with kMaxObjectSize it will try to release
        // as much memory as possible. Otherwise it interprets the parameter as number
        // of blocks to be freed. This function uses bytes instead. The value is converted
        // to blocks here, except when kMaxObjectSize has been passed in so that it will
        // still trigger freeing maximum amount of memory. The division may lose some
        // precision, but SendFreeMemorySignal adds one more block to the requested amount
        // so that is ok.
        heap->SendFreeMemorySignal((minimumBytesToFree != kMaxObjectSize) ? minimumBytesToFree / GCHeap::kBlockSize : minimumBytesToFree);
    }

    // This can *always* be called.  It will clean up the state on the current thread
    // if appropriate, otherwise do nothing.  It *must* be called by host code if the
    // host code jumps past an MMGC_ENTER instance.  (The Flash player does that, in
    // some circumstances.)

    /*static*/
    void GCHeap::SignalImminentAbort()
    {
        if (instance == NULL)
            return;
        EnterFrame* ef = GetGCHeap()->GetEnterFrame();
        if (ef == NULL)
            return;

        // We don't know if we're holding the lock but we can release it anyhow,
        // on the assumption that this operation will not cause problems if the
        // lock is not held or is held by another thread.
        //
        // Release lock so we don't deadlock if exit or longjmp end up coming
        // back to GCHeap (all callers must have this lock).

        VMPI_lockRelease(&instance->m_spinlock);

        // If the current thread is holding a lock for a GC that's not currently active on the thread
        // then break the lock: the current thread is collecting in that GC, but the Abort has cancelled
        // the collection.
        ef->UnwindAllObjects();

        // Clear the enterFrame because we're jumping past MMGC_ENTER.
        GetGCHeap()->enterFrame = NULL;
    }

    void GCHeap::Abort()
    {
        status = kMemAbort;
        EnterFrame *ef = enterFrame;

        //  If we hit abort, we need to turn m_oomHandling back on so that listeners are guaranteed to get this signal
        //  We also need to set m_notoficationThread to NULL in case we hit abort while we were processing another memory status change
        m_oomHandling = true;
        m_notificationBeingSent = false;

        GCLog("error: out of memory\n");

        // release lock so we don't deadlock if exit or longjmp end up coming
        // back to GCHeap (all callers must have this lock)
        VMPI_lockRelease(&m_spinlock);

        // Lock must not be held when we call VMPI_exit, deadlocks ensue on Linux
        if(config.OOMExitCode != 0)
        {
            VMPI_exit(config.OOMExitCode);
        }

        if (ef != NULL)
        {
            // Guard against repeated jumps: ef->m_heap doubles as a flag.  We go Abort->longjmp->~EnterFrame->Leave
            // and Leave calls StatusChangeNotify and the host code might do another allocation during shutdown
            // in which case we want to go to VMPI_abort instead.  At that point m_heap will be NULL and the right
            // thing happens.
            if (ef->m_heap != NULL)
            {
                ef->UnwindAllObjects();
                VMPI_longjmpNoUnwind(ef->jmpbuf, 1);
            }
        }
        GCAssertMsg(false, "MMGC_ENTER missing or we allocated more memory trying to shutdown");
        VMPI_abort();
    }

    void GCHeap::Enter(EnterFrame *frame)
    {
        enterCount++;
        enterFrame = frame;
    }

    void GCHeap::Leave()
    {
        {
            MMGC_LOCK(m_spinlock);

            if(status == kMemAbort && !abortStatusNotificationSent) {
                abortStatusNotificationSent = true;
                StatusChangeNotify(kMemAbort);
            }
        }

        EnterLock();

        // do this after StatusChangeNotify it affects ShouldNotEnter

        // need to check if enterFrame is valid, it might have been nulled out by SignalImminentAbort
        EnterFrame* enter = enterFrame;
        if (enter)
            enterFrame = enter->Previous();

        enterCount--;

        // last one out of the pool pulls the plug
        if(status == kMemAbort && enterCount == 0 && abortStatusNotificationSent && preventDestruct == 0) {
            DestroyInstance();
        }
        EnterRelease();
    }
    void GCHeap::log_percentage(const char *name, size_t bytes, size_t bytes_compare)
    {
        bytes_compare = size_t((bytes*100.0)/bytes_compare);
        if(bytes > 1<<20) {
            GCLog("%s %u (%.1fM) %u%%\n", name, (unsigned int)(bytes / GCHeap::kBlockSize), bytes * 1.0 / (1024*1024), (unsigned int)(bytes_compare));
        } else {
            GCLog("%s %u (%uK) %u%%\n", name, (unsigned int)(bytes / GCHeap::kBlockSize), (unsigned int)(bytes / 1024), (unsigned int)(bytes_compare));
        }
    }

    void GCHeap::DumpMemoryInfo()
    {
        MMGC_LOCK(m_spinlock);
        size_t priv = AVMPI_getPrivateResidentPageCount() * VMPI_getVMPageSize();
        size_t mmgc = GetTotalHeapSize() * GCHeap::kBlockSize;
        size_t unmanaged = GetFixedMalloc()->GetTotalSize() * GCHeap::kBlockSize;
        size_t fixed_alloced;
        size_t fixed_asksize;
        GetFixedMalloc()->GetUsageInfo(fixed_asksize, fixed_alloced);

        size_t gc_total=0;
        size_t gc_allocated_total =0;
        size_t gc_ask_total = 0;
        size_t gc_count = 0;
        BasicListIterator<GC*> iter(gcManager.gcs());
        GC* gc;
        while((gc = iter.next()) != NULL)
        {
#ifdef MMGC_MEMORY_PROFILER
            GCLog("[mem] GC 0x%p:%s\n", (void*)gc, GetAllocationName(gc));
#else
            GCLog("[mem] GC 0x%p\n", (void*)gc);
#endif
            gc->DumpMemoryInfo();

            size_t ask;
            size_t allocated;
            gc->GetUsageInfo(ask, allocated);
            gc_ask_total += ask;
            gc_allocated_total += allocated;
            gc_count += 1;

            gc_total += gc->GetNumBlocks() * kBlockSize;
        }

#ifdef MMGC_MEMORY_PROFILER
        fixedMalloc.DumpMemoryInfo();
#endif

        // Gross stats are not meaningful if the profiler is running, see bugzilla 490014.
        // Disabling their printing is just an expedient fix to avoid misleading data being
        // printed.  There are other, more complicated, fixes we should adopt.

        GCLog("[mem] ------- gross stats -----\n");
#ifdef MMGC_MEMORY_PROFILER
        if (GCHeap::GetGCHeap()->GetProfiler() == NULL)
#endif
        {
            log_percentage("[mem] private", priv, priv);
            log_percentage("[mem]\t mmgc", mmgc, priv);
            log_percentage("[mem]\t\t unmanaged", unmanaged, priv);
            log_percentage("[mem]\t\t managed", gc_total, priv);
            log_percentage("[mem]\t\t free",  (size_t)GetFreeHeapSize() * GCHeap::kBlockSize, priv);
            log_percentage("[mem]\t other",  priv - mmgc, priv);
            log_percentage("[mem] \tunmanaged overhead ", unmanaged-fixed_alloced, unmanaged);
            log_percentage("[mem] \tmanaged overhead ", gc_total - gc_allocated_total, gc_total);
#ifdef MMGC_MEMORY_PROFILER
            if(HooksEnabled())
            {
                log_percentage("[mem] \tunmanaged internal wastage", fixed_alloced - fixed_asksize, fixed_alloced);
                log_percentage("[mem] \tmanaged internal wastage", gc_allocated_total - gc_ask_total, gc_allocated_total);
            }
#endif
            GCLog("[mem] number of collectors %u\n", unsigned(gc_count));
        }
#ifdef MMGC_MEMORY_PROFILER
        else
            GCLog("[mem] No gross stats available when profiler is enabled.\n");
#endif
        GCLog("[mem] -------- gross stats end -----\n");

#ifdef MMGC_MEMORY_PROFILER
        if(hasSpy)
            DumpFatties();
#endif

        if (config.verbose)
            DumpHeapRep();
    }

    void GCHeap::LogChar(char c, size_t count)
    {
        char tmp[100];
        char* buf = count < 100 ? tmp : (char*)VMPI_alloc(count+1);
        if (buf == NULL)
            return;
        VMPI_memset(buf, c, count);
        buf[count] = '\0';

        GCLog(buf);
        if (buf != tmp)
            VMPI_free(buf);
    }

    void GCHeap::DumpHeapRep()
    {
        Region **regions = NULL;
        Region *r = lastRegion;
        int numRegions = 0;

        GCLog("Heap representation format: \n");
        GCLog("region base address - commitTop/reserveTop\n");
        GCLog("[0 == free, 1 == committed, - = uncommitted]*\n");

        // count and sort regions
        while(r) {
            numRegions++;
            r = r->prev;
        }
        regions = (Region**) VMPI_alloc(sizeof(Region*)*numRegions);
        if (regions == NULL)
            return;
        r = lastRegion;
        for(int i=0; i < numRegions; i++, r = r->prev) {
            int insert = i;
            for(int j=0; j < i; j++) {
                if(r->baseAddr < regions[j]->baseAddr) {
                    memmove(&regions[j+1], &regions[j], sizeof(Region*) * (i - j));
                    insert = j;
                    break;
                }
            }
            regions[insert] = r;
        }

        HeapBlock *spanningBlock = NULL;
        for(int i=0; i < numRegions; i++)
        {
            r = regions[i];
            GCLog("0x%p -  0x%p/0x%p\n", r->baseAddr, r->commitTop, r->reserveTop);
            char c;
            char *addr = r->baseAddr;

            if(spanningBlock) {
                GCAssert(spanningBlock->baseAddr + (spanningBlock->size * kBlockSize) > r->baseAddr);
                GCAssert(spanningBlock->baseAddr < r->baseAddr);
                char *end = spanningBlock->baseAddr + (spanningBlock->size * kBlockSize);
                if(end > r->reserveTop)
                    end = r->reserveTop;

                LogChar(spanningBlock->inUse() ? '1' : '0', (end - addr)/kBlockSize);
                addr = end;

                if(addr == spanningBlock->baseAddr + (spanningBlock->size * kBlockSize))
                    spanningBlock = NULL;
            }
            HeapBlock *hb;
            while(addr != r->commitTop && (hb = BaseAddrToBlock(addr)) != NULL) {
                GCAssert(hb->size != 0);

                if(hb->inUse())
                    c = '1';
                else if(hb->committed)
                    c = '0';
                else
                    c = '-';
                size_t i, n;
                for(i=0, n=hb->size; i < n; i++, addr += GCHeap::kBlockSize) {
                    if(addr == r->reserveTop) {
                        // end of region!
                        spanningBlock = hb;
                        break;
                    }
                }

                LogChar(c, i);
            }

            LogChar('-', (r->reserveTop - addr) / kBlockSize);

            GCLog("\n");
        }
        VMPI_free(regions);
    }

#ifdef MMGC_MEMORY_PROFILER

    /* static */
    void GCHeap::InitProfiler()
    {
        GCAssert(IsProfilerInitialized() == false);

        profiler = AVMPI_isMemoryProfilingEnabled() ? new MemoryProfiler() : NULL;
    }

#endif //MMGC_MEMORY_PROFILER

#ifdef MMGC_MEMORY_PROFILER
#ifdef MMGC_USE_SYSTEM_MALLOC

    void GCHeap::TrackSystemAlloc(void *addr, size_t askSize)
    {
        MMGC_LOCK(m_spinlock);
        if(!IsProfilerInitialized())
            InitProfiler();
        if(profiler)
            profiler->RecordAllocation(addr, askSize, VMPI_size(addr), /*managed=*/false);
    }

    void GCHeap::TrackSystemFree(void *addr)
    {
        MMGC_LOCK(m_spinlock);
        if(addr && profiler)
            profiler->RecordDeallocation(addr, VMPI_size(addr));
    }

#endif //MMGC_USE_SYSTEM_MALLOC
#endif // MMGC_MEMORY_PROFILER

    void GCHeap::ReleaseMemory(char *address, size_t size)
    {
        if(config.useVirtualMemory) {
            bool success = AVMPI_releaseMemoryRegion(address, size);
            GCAssert(success);
            (void)success;
        } else {
            AVMPI_releaseAlignedMemory(address);
        }
    }

    void GCManager::destroy()
    {
        collectors.Destroy();
    }

    void GCManager::signalStartCollection(GC* gc)
    {
        BasicListIterator<GC*> iter(collectors);
        GC* otherGC;
        while((otherGC = iter.next()) != NULL)
            otherGC->policy.signalStartCollection(gc);
    }

    void GCManager::signalEndCollection(GC* gc)
    {
        BasicListIterator<GC*> iter(collectors);
        GC* otherGC;
        while((otherGC = iter.next()) != NULL)
            otherGC->policy.signalStartCollection(gc);
    }

    /* this method is the heart of the OOM system.
       its here that we call out to the mutator which may call
       back in to free memory or try to get more.

       Note! The caller needs to hold on to the m_spinlock before calling this!
    */

    void GCHeap::SendFreeMemorySignal(size_t minimumBlocksToFree)
    {
        //  If we're already in the process of sending out memory notifications, don't bother verifying now.
        //  Also, we only want to send the "free memory" signal while our memory is in a normal state.  Once
        //  we've entered softLimit or abort state, we want to allow the softlimit or abort processing to return
        //  the heap to normal before continuing.

        if (statusNotificationBeingSent() || status != kMemNormal || !m_oomHandling)
            return;

        m_notificationBeingSent = true;

        size_t startingTotal = GetTotalHeapSize() + externalPressure / kBlockSize;

        BasicListIterator<OOMCallback*> iter(callbacks);
        OOMCallback *cb = NULL;
        bool bContinue = true;
        do {
            cb = iter.next();
            if(cb)
            {
                VMPI_lockRelease(&m_spinlock);
                cb->memoryStatusChange(kFreeMemoryIfPossible, kFreeMemoryIfPossible);
                Decommit();
                VMPI_lockAcquire(&m_spinlock);

                size_t currentTotal = GetTotalHeapSize() + externalPressure / kBlockSize;

                //  If we've freed MORE than the minimum amount, we can stop freeing
                if ((startingTotal - currentTotal) > minimumBlocksToFree)
                {
                    bContinue = false;
                }
            }
        } while(cb != NULL && bContinue);

        iter.MarkCursorInList();

        m_notificationBeingSent = false;
    }

    void GCHeap::StatusChangeNotify(MemoryStatus to)
    {
        //  If we're already in the process of sending this notification, don't resend
        if ((statusNotificationBeingSent() && to == status) || !m_oomHandling)
            return;

        m_notificationBeingSent = true;

        MemoryStatus oldStatus = status;
        status = to;

        BasicListIterator<OOMCallback*> iter(callbacks);
        OOMCallback *cb = NULL;
        do {
            {
                cb = iter.next();
            }
            if(cb)
            {
                VMPI_lockRelease(&m_spinlock);
                cb->memoryStatusChange(oldStatus, to);
                VMPI_lockAcquire(&m_spinlock);
            }
        } while(cb != NULL);

        m_notificationBeingSent = false;

        CheckForStatusReturnToNormal();
    }

    /*static*/
    bool GCHeap::ShouldNotEnter()
    {
        // don't enter if the heap is already gone or we're aborting but not on the aborting call stack in a nested enter call
        GCHeap *heap = GetGCHeap();
        if(heap == NULL ||
           (heap->GetStatus() == kMemAbort &&
            (heap->GetEnterFrame() == NULL || heap->GetEnterFrame()->Suspended())))
            return true;
        return false;
    }

    bool GCHeap::IsAddressInHeap(void *addr)
    {
        void *block = (void*)(uintptr_t(addr) & kBlockMask);
        return SafeSize(block) != (size_t)-1;
    }

    // Every new GC must register itself with the GCHeap.
    void GCHeap::AddGC(GC *gc)
    {
        bool bAdded = false;
        {
            MMGC_LOCK(m_spinlock);
            bAdded = gcManager.tryAddGC(gc);
        }
        if (!bAdded)
        {
            Abort();
        }
    }

    // When the GC is destroyed it must remove itself from the GCHeap.
    void GCHeap::RemoveGC(GC *gc)
    {
        MMGC_LOCK(m_spinlock);
        gcManager.removeGC(gc);
        EnterFrame* ef = GetEnterFrame();
        if (ef && ef->GetActiveGC() == gc)
            ef->SetActiveGC(NULL);
    }

    void GCHeap::AddOOMCallback(OOMCallback *p)
    {
        bool bAdded = false;
        {
            MMGC_LOCK(m_spinlock);
            bAdded = callbacks.Add(p);
        }
        if (!bAdded)
        {
            Abort();
        }
    }

    void GCHeap::RemoveOOMCallback(OOMCallback *p)
    {
        MMGC_LOCK(m_spinlock);
        callbacks.Remove(p);
    }

    bool GCHeap::EnsureFreeRegion(bool allowExpansion)
    {
        if(!HaveFreeRegion()) {
            bool zero = false;
            HeapBlock *block = AllocBlock(1, zero, 1);
            if(block) {
                nextRegion = (Region*)(void *)block->baseAddr;
            } else if(allowExpansion) {
                ExpandHeap(1);
                // We must have hit the hard limit or OS limit
                if(nextRegion == NULL)
                    return false;
            }
        }
        return true;
    }

    GCHeap::Region *GCHeap::NewRegion(char *baseAddr, char *rTop, char *cTop, size_t blockId)
    {
        Region *r = freeRegion;
        if(r) {
            freeRegion = *(Region**)freeRegion;
        } else {
            r = nextRegion++;
            if(roundUp((uintptr_t)nextRegion, kBlockSize) - (uintptr_t)nextRegion < sizeof(Region))
                nextRegion = NULL; // fresh page allocated in ExpandHeap
        }
        new (r) Region(this, baseAddr, rTop, cTop, blockId);
        return r;
    }

    void GCHeap::FreeRegion(Region *r)
    {
        if(r == lastRegion)
            lastRegion = r->prev;
        *(Region**)r = freeRegion;
        freeRegion = r;

    }

    /*static*/
    void GCHeap::EnterLockInit()
    {
        if (!instanceEnterLockInitialized)
        {
            instanceEnterLockInitialized = true;
            VMPI_lockInit(&instanceEnterLock);
        }
    }

    /*static*/
    void GCHeap::EnterLockDestroy()
    {
        GCAssert(instanceEnterLockInitialized);
        VMPI_lockDestroy(&instanceEnterLock);
        instanceEnterLockInitialized = false;
    }

    GCHeap::Region::Region(GCHeap *heap, char *baseAddr, char *rTop, char *cTop, size_t blockId)
        : prev(heap->lastRegion),
          baseAddr(baseAddr),
          reserveTop(rTop),
          commitTop(cTop),
          blockId(blockId)
    {
        heap->lastRegion = this;
    }

#ifdef DEBUG
    void GCHeap::CheckForOOMAbortAllocation()
    {
        if(m_notificationBeingSent && status == kMemAbort)
            GCAssertMsg(false, "Its not legal to perform allocations during OOM kMemAbort callback");
    }
#endif

    bool GCHeap::QueryCanReturnToNormal()
    {
        // must be below soft limit _AND_ above decommit threshold
        return GetUsedHeapSize() + externalPressure/kBlockSize < config.heapSoftLimit &&
            FreeMemoryExceedsDecommitThreshold();
    }

    bool GCHeap::FreeMemoryExceedsDecommitThreshold()
    {
        return GetFreeHeapSize() * 100 > GetTotalHeapSize() * kDecommitThresholdPercentage;
    }
}

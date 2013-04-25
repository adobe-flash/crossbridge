/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCMemoryProfiler__
#define __GCMemoryProfiler__

namespace MMgc
{
    void PrintAllocStackTrace(const void *item);
    void PrintDeleteStackTrace(const void *item);
    const char* GetAllocationName(const void *obj);

#ifdef MMGC_MEMORY_PROFILER

    const int kMaxStackTrace = 16; // RtlCaptureStackBackTrace stops working when this is 32
    
    bool knownMMgcAddress(uintptr_t ip);
    bool knownDestructorAddress(uintptr_t ip);
    bool lookupFunctionName(uintptr_t ip);
    
    class StackTrace : public GCAllocObject
    {
    public:
        StackTrace(uintptr_t *trace)
            : size(0)
            , totalSize(0)
            , sweepSize(0)
            , firstNonMMgcFrame(-1)
            , package(NULL)
            , category(NULL)
            , name(NULL)
            , count(0)
            , totalCount(0)
            , sweepCount(0)
            , master(NULL)
            , skip(0)
            , managed(false)
            , wasTracedConservatively(false)
        {
            VMPI_memcpy(ips, trace, kMaxStackTrace * sizeof(uintptr_t));
        }
        uintptr_t ips[kMaxStackTrace];
        size_t size;
        size_t totalSize;
        size_t sweepSize;
        int firstNonMMgcFrame;
        const char *package;
        const char *category;
        const char *name;
        uint32_t count;
        uint32_t totalCount;
        uint32_t sweepCount;
        StackTrace *master;
        uint8_t skip;
        bool managed;
        bool wasTracedConservatively;

        int getFirstNonMMgcFrame()
        {
            if(firstNonMMgcFrame==-1) {
                int i;
                for ( i=0; i < kMaxStackTrace && knownMMgcAddress(ips[i]); i++ )
                    ;
                firstNonMMgcFrame = i;
            }
            return firstNonMMgcFrame;
        }
    };
    
    void SetMemTag(const char *memtag);
    void SetMemType(const void *memtype);
    void PrintStackTrace(StackTrace *trace, int limit=INT_MAX);

#define MMGC_MEM_TAG(_x) if(MMgc::GCHeap::GetGCHeap()->HooksEnabled()) MMgc::SetMemTag(_x)
#define MMGC_MEM_TYPE(_x) if(MMgc::GCHeap::GetGCHeap()->HooksEnabled()) MMgc::SetMemType(_x)
    
    class GCStackTraceHashtableKeyHandler
    {
    public:
        inline static uint32_t hash(const void* k)
        {
            const int* array = (const int*)k;
            int hash = 0;
            for(int i=0;i<kMaxStackTrace; i++)
                hash += array[i];
            return uint32_t(hash);
        }

        inline static bool equal(const void* k1, const void* k2)
        {
            if (k1 == NULL || k2 == NULL)
                return false;
            return VMPI_memcmp(k1, k2, kMaxStackTrace * sizeof(void*)) == 0;
        }
    };

    typedef GCHashtableBase<const void*, GCStackTraceHashtableKeyHandler,GCHashtableAllocHandler_VMPI> GCStackTraceHashtable_VMPI;

    template <class StackTraceHandler>
    class ObjectPopulationProfiler : public GCAllocObject
    {
    public:
        enum DumpMode {
            BY_VOLUME,
            BY_COUNT
        };

        // If 'roots' is true then root info is collected and printed, otherwise accountForRoot()
        // should not be called.  Ditto for 'stacks' and accountForStack().
        ObjectPopulationProfiler(GC* gc, const char* profileName, bool roots=true, bool stacks=true);
        virtual ~ObjectPopulationProfiler();

        void accountForObject(const void* obj);
        void accountForRoot(size_t size);
        void accountForStack(size_t size);

        void dumpTopBacktraces(int howmany, DumpMode mode=BY_VOLUME);

        // Maps StackTrace* to PopulationNode*
        struct PopulationNode;
        typedef GCHashtableBase<PopulationNode*,StackTraceHandler,GCHashtableAllocHandler_VMPI> NodeTable;
    protected:
        // Obtain the stack trace we're interested in for this object.  It will
        // be called by accountForObject.
        //
        // For the ObjectPopulationProfiler it's the allocation stack, but for
        // derived profilers it may be something else.
        virtual StackTrace* obtainStackTrace(const void* obj);

        // Print a single informational line describing the activity we're profiling.
        //
        // For the ObjectPopulationProfiler it's the number of bytes and objects scanned.
        virtual void dumpObjectInfo(unsigned long long numbytes, unsigned numobjects);
        
        // Query whether this address at the bottom of the trace should be skipped during
        // trace printing (it may follow other skipped addresses).  This is useful for
        // filtering out info about MMgc, for example.
        //
        // Skip MMgc addresses
        virtual bool skipThisAddress(uintptr_t ip);

    protected:
        GC* gc;
        GCHeap* heap;
        const char* profileName;
        
    private:
        // Roots and stacks
        struct Summary
        {
            Summary() : numobjects(0), numbytes(0) {}

            size_t   numobjects;
            uint64_t numbytes;
        };

        Summary roots;
        Summary rootHistogram[32];
        Summary stacks;
        Summary stackHistogram[32];
        
        // Objects
        
        NodeTable nodes;
        const bool root_info;
        const bool stack_info;
    };

    class AllocationSiteHandler
    {
    public:
        inline static uint32_t hash(const void* k)
        {
            StackTrace *trace = (StackTrace*)k;
            return (uint32_t)trace->ips[trace->getFirstNonMMgcFrame()];
        }

        inline static bool equal(const void* k1, const void* k2)
        {
            StackTrace *t1 = (StackTrace*)k1;
            StackTrace *t2 = (StackTrace*)k2;
            return t1 == t2 || t1->getFirstNonMMgcFrame() == t2->getFirstNonMMgcFrame();
        }
        
        inline static int GetSignificance(StackTrace *trace)
        {
            return trace->getFirstNonMMgcFrame()+1;
        }
    };

    typedef ObjectPopulationProfiler<AllocationSiteHandler> AllocationSiteProfiler;
    
#ifdef MMGC_WEAKREF_PROFILER
    class WeakRefAllocationSiteProfiler : public ObjectPopulationProfiler<AllocationSiteHandler>
    {
    public:
        WeakRefAllocationSiteProfiler(GC* gc, const char* profileName);
        
        void reportPopulation(uint32_t pop);

        void reportGCStats(uint32_t scanned, uint32_t removed);

        uint32_t peakPopulation;
        uint64_t scannedAtGC;
        uint64_t removedAtGC;
        uint32_t collections;

    protected:
        void dumpObjectInfo(unsigned long long numbytes, unsigned numobjects);
    };
#endif

    class DeletionNodeHandler
    {
    public:
        inline static uint32_t hash(const void* k)
        {
            return GCHashtableKeyHandler::hash(k);
        }

        inline static bool equal(const void* k1, const void* k2)
        {
            StackTrace *a = (StackTrace*)k1;
            StackTrace *b = (StackTrace*)k2;
            int i;
            for ( i=0; i < kMaxStackTrace && a->ips[i] == b->ips[i] ; i++ )
                ;
            
            if (i == kMaxStackTrace) {
                return true;
            }
            
            // We stopped too soon, but maybe it's OK to stop here?  Our criterion is
            // that the ones we stopped at are not in MMgc and are not destructors.
            
            if (knownMMgcAddress(a->ips[i]) || knownDestructorAddress(a->ips[i]) ||
                knownMMgcAddress(b->ips[i]) || knownDestructorAddress(b->ips[i]))
                return false;
            
            // Name resolution disabled?
            
            //if (last_ip == 0)
              //  return false;
            
            return true;
        }


        inline static int GetSignificance(StackTrace*)
        {
            return kMaxStackTrace;
        }
    };

    class DeletionProfiler : public ObjectPopulationProfiler<DeletionNodeHandler>
    {
    public:
        DeletionProfiler(GC* gc, const char* profileName);

    protected:
        // For the DeletionProfiler we want the stack at the point of
        // deletion, and we get that by overriding obtainStackTrace and
        // relying on accountForObject being called when the object
        // is deleted.
        virtual StackTrace* obtainStackTrace(const void* obj);
        
        // For the DeletionProfiler it's the number of bytes and objects deleted
        // from a particular locus.
        virtual void dumpObjectInfo(unsigned long long numbytes, unsigned numobjects);
        
        // Skip MMgc addresses
        virtual bool skipThisAddress(uintptr_t ip);
    private:
    };

    class MemoryProfiler : public GCAllocObject
    {
    public:
        MemoryProfiler();
        ~MemoryProfiler();
        void RecordAllocation(const void *item, size_t askSize, size_t gotSize, bool managed);
        void RecordDeallocation(const void *item, size_t size);
        void DumpAllocationProfile();
        void DumpFatties();
        void DumpSimple();
        const char *GetAllocationName(const void *obj);
        StackTrace *GetAllocationTrace(const void *obj);
        StackTrace *GetDeletionTrace(const void *obj);
        StackTrace *GetStackTrace();
        size_t GetAskSize(const void* item);

        enum SortMode {
            BY_VOLUME,
            BY_COUNT
        };

        // Dump top allocation stacks, sorted.
        // - limit is the number of stacks to dump; 0 means all
        // - sort is the field to sort by: totalSize or totalCount
        void DumpSimpleByTotal(uint32_t limit=40, SortMode sort=BY_VOLUME);
        
    private:
        StackTrace *GetStackTraceLocked();
        StackTrace *GetAllocationTraceLocked(const void *obj);

        const char *Intern(const char *name, size_t len);
        const char *GetPackage(StackTrace *trace);
        const char *GetAllocationNameFromTrace(StackTrace *trace);
        const char *GetAllocationCategory(StackTrace *trace);

        // Note: it's important to use the VMPI variant of GCHashtable for all of these.

        // intern table of StackTrace*
        GCStackTraceHashtable_VMPI stackTraceMap;

        // intern table of strings used by Profiler
        GCStringHashtable_VMPI stringsTable;

        // hash table for address -> name lookup
        GCHashtable_VMPI nameTable;

        //table to store allocation specific information
        GCHashtable_VMPI allocInfoTable;

        vmpi_spin_lock_t lock;
    };

#else // MMGC_MEMORY_PROFILER

#define MMGC_MEM_TAG(_x)
#define MMGC_MEM_TYPE(_x)

#endif // !MMGC_MEMORY_PROFILER

#ifndef MMGC_MEMORY_INFO

#define GetRealPointer(_x) _x
#define GetUserPointer(_x) _x
#define DebugSize() 0
#define USER_POINTER_WORDS 0
    
#else

    // USER_POINTER_WORDS is the number of 32-bit words to add to the realptr to get
    // the userptr.
    //
    // Traditionally USER_POINTER_WORDS is 2.  For proper alignment of float4 boxes
    // we require USER_POINTER_WORDS to be 4, however.  See comments in GC::GC for
    // more about that, bugzilla 697672 for a discussion, and comments on GCAlloc::Alloc
    // for an interesting invariant that depends on USER_POINTER_WORDS.
#ifdef VMCFG_FLOAT
    #define USER_POINTER_WORDS 4
#else
    #define USER_POINTER_WORDS 2
#endif

    /**
    * How much extra size does DebugDecorate need?
    */
    size_t DebugSize();

    /**
    * decorate memory with debug information, return pointer to memory to return to caller
    */
    void DebugDecorate(const void *item, size_t size);

    /**
    * Given a pointer to user memory do debug checks and return pointer to real memory
    */
    void *DebugFree(const void *item, int poison, size_t size, bool actualFree);

    /**
    * Given a user pointer back up to real beginning
    */
    inline void *GetRealPointer(const void *item) { return (void*)((uintptr_t) item -  USER_POINTER_WORDS * sizeof(int32_t)); }

    /**
    * Given a real pointer return pointer to user memory
    */
    inline void *GetUserPointer(const void *item) { return (void*)((uintptr_t) item +  USER_POINTER_WORDS * sizeof(int32_t)); }

    /**
    * Print error messsage and stack traces for allocation/free of memory
    * that has been written over after being deleted
    */
    void ReportDeletedMemoryWrite(const void* item);

#endif //MMGC_MEMORY_INFO

#if USER_POINTER_WORDS != 0 && USER_POINTER_WORDS != 2 && USER_POINTER_WORDS != 4
    #error "Are you sure about this value of USER_POINTER_WORDS?"
#endif

} // namespace MMgc

#endif //!__GCMemoryProfiler__


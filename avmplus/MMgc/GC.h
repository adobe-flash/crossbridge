/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GC__
#define __GC__


#ifdef VMCFG_TELEMETRY
namespace telemetry
{
    class ITelemetry;
}
#endif

#define MMGC_GCENTER(_gc)  MMgc::GCAutoEnter __mmgc_auto_enter(_gc);

// MMGC_GC_ROOT_THREAD is obsolete; the mechanism is not safe.
//
// Instead you must use a pattern like this:
//
//    gc->CreateRootFromCurrentStack(fn, arg)
//
// where 'fn' takes a single void* and will be passed 'arg'.  While 'fn' is active the
// stack and registers active at the time CreateRootFromCurrentStack was called will
// be a GC root.

// Enable our own alloca() replacement that always allocates in the heap, this is good on
// systems with limited memory or limited stack
#define AVMPLUS_PARAM_ALLOCA_CUTOFF     4000    // Don't make real alloca() blow the stack; this limit is heuristic
#define AVMPLUS_PARAM_ALLOCA_DEFSIZE    1000    // Default number of bytes in a stack segment for heap-based alloca()
#ifdef AVMPLUS_HEAP_ALLOCA
#  define avmStackAlloc(core, autoptr, nbytes)    core->gc->allocaPush(nbytes, autoptr)
#  define avmStackAllocGC(gc, autoptr, nbytes)   gc->allocaPush(nbytes, autoptr)
#else
#  define avmStackAlloc(core, autoptr, nbytes)  (nbytes > AVMPLUS_PARAM_ALLOCA_CUTOFF ? core->gc->allocaPush(nbytes, autoptr) : alloca(nbytes))
#  define avmStackAllocGC(gc, autoptr, nbytes)  (nbytes > AVMPLUS_PARAM_ALLOCA_CUTOFF ? gc->allocaPush(nbytes, autoptr) : alloca(nbytes))
#endif
#define avmStackAllocArray(core, autoptr, nelem, elemsize) avmStackAlloc(core, autoptr, MMgc::GCHeap::CheckForCallocSizeOverflow(nelem, elemsize))
#define avmStackAllocArrayGC(gc, autoptr, nelem, elemsize) avmStackAllocGC(gc, autoptr, MMgc::GCHeap::CheckForCallocSizeOverflow(nelem, elemsize))

// For backwards compatibility with the Flash Player until we can rename its uses of the VMPI_ macros
#define VMPI_alloca    avmStackAlloc
#define VMPI_alloca_gc avmStackAllocGC

// Exact garbage collection (opt-in) macros.  These are not merely macros,
// they are also processed by a script that generates tracing code.
//
// doc/mmgc/exactgc-cookbook.html is a gentle user's guide, while the
// comment block in utils/exactgc.as is not gentle at all.

// Class is C++ implementation of AS3 class: Use this to declare the
// class.
#define GC_AS3_EXACT(cls, basecls)  cls : public basecls
#define GC_AS3_EXACT_IFDEF(cls, basecls, cond)  cls : public basecls
#define GC_AS3_EXACT_IFNDEF(cls, basecls, cond)  cls : public basecls
#define GC_AS3_EXACT_IF(cls, basecls, cond)  cls : public basecls
#define GC_AS3_EXACT_WITH_HOOK(cls, basecls)  cls : public basecls
#define GC_AS3_EXACT_WITH_HOOK_IFDEF(cls, basecls, cond)  cls : public basecls
#define GC_AS3_EXACT_WITH_HOOK_IFNDEF(cls, basecls, cond)  cls : public basecls
#define GC_AS3_EXACT_WITH_HOOK_IF(cls, basecls, cond)  cls : public basecls

// Class is internal to the runtime or host software, not exposed to
// AS3: Use this to declare the class.
#define GC_CPP_EXACT(cls, basecls)  cls : public basecls
#define GC_CPP_EXACT_IFDEF(cls, basecls, cond)  cls : public basecls
#define GC_CPP_EXACT_IFNDEF(cls, basecls, cond)  cls : public basecls
#define GC_CPP_EXACT_IF(cls, basecls, cond)  cls : public basecls
#define GC_CPP_EXACT_WITH_HOOK(cls, basecls)  cls : public basecls
#define GC_CPP_EXACT_WITH_HOOK_IFDEF(cls, basecls, cond)  cls : public basecls
#define GC_CPP_EXACT_WITH_HOOK_IFNDEF(cls, basecls, cond)  cls : public basecls
#define GC_CPP_EXACT_WITH_HOOK_IF(cls, basecls, cond)  cls : public basecls

// Before the first data member in a class
#define GC_DATA_BEGIN(cls)  GC_DECLARE_EXACT_METHODS

// After the last data member in a class
#define GC_DATA_END(cls)

// Convenient shorthand, lots of glue classes have no manually
// defined data on the C++ side.
#define GC_NO_DATA(cls) \
    GC_DATA_BEGIN(cls) \
    GC_DATA_END(cls)

// Exact atom field
#define GC_ATOM(field) field
#define GC_ATOM_IFDEF(field,cond) field
#define GC_ATOM_IFNDEF(field,cond) field
#define GC_ATOM_IF(field,cond) field

// Exact pointer field
#define GC_POINTER(field) field
#define GC_POINTER_IFDEF(field,cond) field
#define GC_POINTER_IFNDEF(field,cond) field
#define GC_POINTER_IF(field,cond) field

// Conservative pointer field
#define GC_CONSERVATIVE(field) field
#define GC_CONSERVATIVE_IFDEF(field,cond) field
#define GC_CONSERVATIVE_IFNDEF(field,cond) field
#define GC_CONSERVATIVE_IF(field,cond) field

// Substructure fields: the field must provide a method 'void gcTrace(MMgc::GC* gc)'
// that is typically inline.  All List<> fields are substructure fields, and the
// List<> templates provide the tracing functions.
#define GC_STRUCTURE(field) field
#define GC_STRUCTURE_IFDEF(field,cond) field
#define GC_STRUCTURE_IFNDEF(field,cond) field
#define GC_STRUCTURE_IF(field,cond) field

// Pointer, atom, and structure array fields: The field_and_size are used
// to declare the array, the form must be fieldname[constantExpr] and there
// can be no commas in it (it can't be quoted).  The 'count' is an efficient
// expression that yields the allocated size of the array; if it contains
// commas or parentheses it /must/ be enclosed in double quotes.

#define GC_POINTERS(field_and_size,count) field_and_size
#define GC_POINTERS_SMALL(field_and_size,count) field_and_size // note, this is a hint

#define GC_ATOMS(field_and_size,count) field_and_size
#define GC_ATOMS_SMALL(field_and_size,count) field_and_size // note, this is a hint

#define GC_STRUCTURES(field_and_size,count) field_and_size
#define GC_STRUCTURES_SMALL(field_and_size,count) field_and_size

// Used by the GC annotation macros, can also be used manually in a
// pinch but normally you can ignore this.  We don't adorn gcTrace
// with virtual and instead rely on the superclass to say its virtual.
// This allows us to exactly trace structures using the existing
// machinery without adding a vtable to them.  Structure tracing is
// also why the cursor argument is optional, so this declaration can
// handle GC_STRUCTURE decorations.

#define GC_DECLARE_EXACT_METHODS \
    private:\
        static const uint32_t gcTracePointerOffsets[];\
    public: \
        /*sometimes virtual*/ MMgc::GCTracerCheckResult gcTraceOffsetIsTraced(uint32_t) const; \
        /*sometimes virtual*/ bool gcTrace(MMgc::GC* gc, size_t cursor=0);


namespace avmplus
{
    class AvmCore;
    class Traits;
#ifdef VMCFG_SELFTEST
    namespace ST_mmgc_basics { class ST_mmgc_basics; }
#endif
}

namespace MMgc
{
    class GCAutoEnter;

    class   GCExactDummyClass;
    typedef GCExactDummyClass* GCExactFlag;
    const   GCExactFlag kExact = 0;
    
    class GCNoFinalizeDummyClass;
    typedef GCNoFinalizeDummyClass* GCNoFinalizeFlag;
    const GCNoFinalizeFlag kNoFinalize = 0;

    /**
     * Memory types for dependent memory
     * @see SignalDependentAllocation, SignalDependentAllocation
     */
    enum DependentMemoryType { typeByteArray, typeUnknown, typeCount };
    
    /**
     * GCRoot is root in the reachability graph, it contains a pointer a size
     * and will be searched for things.
     */
    class GCRoot
    {
        friend class GC;
#ifdef VMCFG_SELFTEST
        friend class avmplus::ST_mmgc_basics::ST_mmgc_basics;
#endif
        GCRoot();
        void init(GC*gc, const void *object, size_t size, bool isStackMemory, bool isExactlyTraced);

        // Flags that are stored in the 'size' field
        static const int kIsStackMemory = 1;
        static const int kIsExactlyTraced = 2;
        static const int kFlags = 3;

    public:
        /** subclassing constructor - conservatively traced */
        GCRoot(GC *gc);
        
        /** subclassing constructor - exactly traced */
        GCRoot(GC *gc, GCExactFlag flag);

        /**
         * The return value and both arguments are technically unnecessary, but by using this
         * signature the GCRoot tracers will be trivially compatible with the tracer generator
         * system; roots can therefore be annotated in the usual manner and tracers for roots
         * can be generated.
         */
        virtual bool gcTrace(GC* gc, size_t cursor=0);

        /**
         * Special-purpose constructor.  Provided in order to allow non-FixedMalloc memory
         * to be root memory (used by AIR/APE).
         *
         * If you're about to use this constructor please consider carefully whether you
         * should be using the StackMemory subclass instead.
         */
        GCRoot(GC* gc, const void* object, size_t size);

        virtual ~GCRoot();

        // override new and delete so we can know the objects extents (via FixedMalloc::Size())
        void *operator new(size_t size);

        void operator delete (void *object);

        const void *Get() const { return object; }
        const void *End() const { return (char*)object + Size(); }
        size_t Size() const { return size & ~kFlags; }
        bool IsStackMemory() const { return (size & kIsStackMemory) ? true : false; }
        bool IsExactlyTraced() const { return (size & kIsExactlyTraced) ? true : false; }

        GC *GetGC() const;
        /** only used for late registration of message channels */
        void Register(GC* _gc);
        /** if your object goes away after the GC is deleted this can be useful */
        void Destroy();

        //  GCRoot's GCMember definition, no WriteBarrier but handle ref count updates.
        template<class T>
        class GCMember : public GCRef<T>
        {
        private:
            //  'set' method handles logic whenever the GC pointer value changes
            void set(T* tNew);
            
            // The AvmCore code uses an invalid pointer (AVMPLUS_STRING_DELETED) for the intern
            // table, whose value is '1'.
            bool valid();

        public:
            explicit GCMember();
            explicit REALLY_INLINE GCMember(const GCMember& other);

            //  This constructor takes any other GCRef<T2>
            template<class T2>
            explicit GCMember(const GCRef<T2>& other);
            
            explicit GCMember(T* valuePtr);
            
            //  Make sure to decrement the refcount on RCObjects when this GCMember is destroyed
            ~GCMember();

            //  Override assignement to make sure our 'set' operation is invoked
            template<class T2>
            void operator=(const GCRef<T2>& other);

            void operator=(T *tNew);

            //  Handle the default assignement operator
            GCMember& operator=(const GCMember& other);
        };
        
    private:
        GC * gc;
        GCRoot *next;
        GCRoot *prev;
        const void *object;             // High w-2 bits: the pointer.  Bit 1: available.  Bit 0: available.
        size_t size;                    // High w-2 bits: the size in bytes.  Bit 1: isExactlyTraced flag.   Bit 0: isStackMemory flag.
        uintptr_t markStackSentinel;    // If not 0 this is the index on the mark stack that holds the RootProtector for this root.

        void GetConservativeWorkItem(const void*& _object, uint32_t& _size, bool& _isStackMemory) const;

        uintptr_t GetMarkStackSentinelPointer();

        // SetMarkStackSentinelPointer stores the item index and clears
        // the existing one if it exists.  Since all the roots are
        // pushed in StartIncrementalMark and FinishIncrementalMark
        // the root can be on the stack twice.
        void SetMarkStackSentinelPointer(uintptr_t);

        // ClearMarkStackSentinelPointer just sets the item index to
        // zero, use SetMarkStackSentinelPointer(0) to also clear
        // the work item off the mark stack.
        void ClearMarkStackSentinelPointer();
        
    protected:
        // THESE METHODS ARE ONLY FOR MMGC INTERNAL USE!

        GCRoot(GC*gc, const void *object, size_t size, bool isStackMemory, bool isExactlyTraced);

        void PrivilegedSet(const void *object, uint32_t size);
    };

    /**
     * StackMemory is root memory that is only scanned at the end of GC, not at
     * the beginning.  It should be used for heap memory that functions as a stack:
     * alloca memory, or the AS1 stack.
     */
    class StackMemory : public GCRoot
    {
    public:
        /**
         * Initialize the stack memory.
         */
        StackMemory(GC *gc, const void *object, size_t size, bool isExactlyTraced=false);

        /**
         * Return the value of the stack top in the range [0,size].  The GC will scan the memory
         * in the range [object,object+top).  The default implementation just returns "size".
         */
        virtual size_t Top();

        /** Reinitialize with this memory instead of the old memory.  To clear use Set(NULL,0). */
        void Set(const void *object, size_t size);
    };

    /**
     * GCCallback is an interface that allows the application to get
     * callbacks at interesting GC points.
     */
    class GCCallback
    {
        friend class GC;
        friend class ZCT;
    public:
        GCCallback(GC *gc);
        virtual ~GCCallback();

        GC *GetGC() const { return gc; }
        /** if your object goes away after the GC is deleted this can be useful */
        void Destroy();

        /**
         * This method is invoked after all marking and before any
         * sweeping, useful for bookkeeping based on whether things
         * got marked
         */
        virtual void presweep();

        /**
         * This method is invoked after all sweeping
         */
        virtual void postsweep();

        // called before a ZCT reap begins
        virtual void prereap();

        // called after a ZCT reap completes
        virtual void postreap();

        /**
         * This method is called before an RC object is reaped
         */
        virtual void prereap(void* /*rcobj*/);

    private:
        GC *gc;
        GCCallback *nextCB;
        GCCallback *prevCB;
    };

    #ifdef MMGC_64BIT
    #define HIDDENPTRMASK (uintptr_t(0x1L)<<63)
    #else
    #define HIDDENPTRMASK (uintptr_t(0x1L)<<31)
    #endif

    template <class T>
    class GCHiddenPointer
    {
    public:
        GCHiddenPointer(T obj=NULL) { set(obj); }
        operator T() const { return (T) (val^HIDDENPTRMASK);     }
        T operator=(T tNew)
        {
            set(tNew);
            return (T)this;
        }
        T operator->() const { return (T) (val^HIDDENPTRMASK); }

    private:
        // private to prevent its use and someone adding it, GCC creates
        // WriteBarrier's on the stack with it
        GCHiddenPointer(const GCHiddenPointer<T>&) { GCAssert(false); }

        void set(T obj)
        {
            uintptr_t p = (uintptr_t)obj;
            val = p ^ HIDDENPTRMASK;
        }
        uintptr_t val;
    };

    /**
     * GCObjectLock is abstract and instances of it are returned from GC::LockObject.
     */
    class GCObjectLock;

    /**
     * This is a general-purpose garbage collector used by the Flash Player.
     * Application code must implement the GCRoot interface to mark all
     * reachable data.  Unreachable data is automatically destroyed.
     * Objects may optionally be finalized by subclassing the GCObject
     * interface.
     *
     * This garbage collector is intended to be modular, such that
     * it can be used in other products or replaced with a different
     * collector in the future.
     *
     * Memory allocation and garbage collection strategy:
     * Mark and sweep garbage collector using fixed size classes
     * implemented by the GCAlloc class.  Memory blocks are obtained
     * from the OS via the GCHeap heap manager class.
     *
     * When an allocation fails because a suitable memory block is
     * not available, the garbage collector decides either to garbage
     * collect to free up space, or to expand the heap.  The heuristic
     * used to make the decision to collect or expand is taken from the
     * Boehm-Demers-Weiser (BDW) garbage collector and memory allocator.
     * The BDW algorithm is (pseudo-code):
     *
     *    if (allocs since collect >= heap size / FSD)
     *      collect
     *    else
     *      expand(request size + heap size / FSD)
     *
     * The FSD is the "Free Space Divisor."  For the first cut, I'm trying
     * 4.  TODO: Try various FSD values against real Player workloads
     * to find the optimum value.
     *
     */
    class GC : public OOMCallback
    {
        friend class GCRoot;
        friend class GCHeap;
        friend class GCCallback;
        friend class GCAlloc;
        friend class GCLargeAlloc;
        friend class GCMarkStack;
        friend class GCWeakRef;
        friend class RCObject;
        friend class ZCT;
        friend class AutoRCRootSegment;
        friend class GCPolicyManager;

        // WriteBarrier classes use private write barriers.
        template<class T> friend class WriteBarrier;
        template<class T> friend class WriteBarrierRC;
        template<class T> friend class GCMemberBase;
        friend class AtomWBCore;

        // The new GCMember delegates to methods in these classes that
        // use the private write barriers.
        friend class GCObject;
        friend class LeafObject;
        friend class GCTraceableBase;
#ifdef VMCFG_SELFTEST
        friend class avmplus::ST_mmgc_basics::ST_mmgc_basics;
#endif
        friend class avmplus::Traits;    // We may be able to throttle back on this by making TracePointer visible, but OK for now
    public:

        /**
         * If you need context vars use this!
         * FIXME: document context variables.
         */
        enum
        {
            GCV_COREPLAYER,
            GCV_AVMCORE,
            GCV_COUNT
        };

        void *GetGCContextVariable(int var) const;

        void SetGCContextVariable(int var, void *val);

        /**
         * @return the AvmCore instance associated with this GC.
         */
        avmplus::AvmCore *core() const;

        /**
        * Convenience to access the GC instance associated with the current
        * execution context.
        */
       static GC* current();

       /**
        * Convenience to access the GC instance associated with the current
        * execution context.
        */
       static GC* GetActiveGC();

       /**
         * greedy is a debugging flag.  When set, every allocation will cause
         * a garbage collection.  This makes code run abysmally slow, but can
         * be useful for detecting mark bugs or computing true peak live size.
         * (The flag is effective in both debug and release builds.)
         */
        const bool greedy;

        /**
         * nogc is a debugging flag.  When set, garbage collection never happens.
         * (The flag is effective in both debug and release builds.)
         */
        const bool nogc;

        /**
         * incremental controls the behavior of the garbage collection.  If true,
         * collection is incremental (and pause times are more or less controlled);
         * if false, collection is stop-the-world.
         */
        const bool incremental;

        /**
         * drcEnabled controls whether DRC is employed.  This is true
         * by default and disabling it is only recommended for
         * experimentation.  When false we merely short circuit the
         * adding of zero count items to the zero count table.  RC
         * updates still occur in the write barriers.  Applications
         * dependent on finalization immediacy provided by DRC could
         * break.
         */
        const bool drcEnabled;

        /**
         * findUnmarkedPointers is a debugging flag.  If true, the GC will scan the
         * heap at the end of garbage collection, asserting that every word that
         * looks like it conservatively points to an object points to an object that
         * was marked by the collector.  The flag is available in release builds but
         * effective in debug builds only.)
         */
        bool findUnmarkedPointers;

#ifdef DEBUG
        /**
         * validateDefRef is a debugging flag.  It turns on code that does a
         * trace before reaping zero count object and asserting on any objects
         * that get marked.  Significant space cost.
         */
        bool validateDefRef;

        /**
         * keepDRCHistory is a debugging flag.  It turns on code that keeps the call
         * stacks of all IncrementRef and DecrementRef operations on each RC object,
         * and prints this history when the reference count is already zero
         * on entry to DecrementRef.  Significant space cost.  (The flag is available
         * in release builds but effective in debug builds only.)
         */
        bool keepDRCHistory;
#endif

        bool dontAddToZCTDuringCollection;
        bool incrementalValidation;

#ifdef _DEBUG
        bool incrementalValidationPedantic;
#endif

        /* NULL config means select defaults for configuration parameters. */
        GC(GCHeap *heap, GCConfig& config);

        virtual ~GC();

        /**
         * ForceSweepAtShutdown allows clients to force all finalization to occur first
         * and then delete the GC later.  ~GC will still call this just in case.
         * See bugzilla 636376.
         */
        void ForceSweepAtShutdown();

        /**
         * Causes an immediate stop-the-world garbage collection (or finishes any
         * incremental collection underway). See comments above the Collect(double) method.
         */
        void Collect(bool scanNativeStack=true, bool okToShrinkHeapTarget=true);

        /**
         * Causes an immediate stop-the-world garbage collection (or finishes any
         * incremental collection underway) if the fraction of the allocation budget
         * used is greater than or equal to allocationBudgetFractionUsed.
         *
         * The parameter allocationBudgetFractionUsed will be pinned to the range [0.25,1].
         *
         * The heap target size will *not* be reduced after this collection even if it
         * would normally have been reduced based on live storage.  The reason is that
         * this call is used to help schedule GC pauses but should otherwise not impact
         * GC policy more than absolutely necessary.
         */
        void Collect(double allocationBudgetFractionUsed);
        
        /**
         * Do a full collection at the next MMGC_GCENTER macro site
         */
        void QueueCollection();

        /**
         * flags to be passed as second argument to alloc
         */
        enum AllocFlags
        {
            kNoFlags=0,
            kZero=1,
            kContainsPointers=2,
            kFinalize=4,            // This must match kFinalizable in GCAlloc.h
            kRCObject=8,
            kInternalExact=16,      // INTERNAL USE.  This must match kVirtualGCTrace in GCAlloc.h
            kCanFail=32
        };

        /**
         * Main interface for allocating memory.  Default flags are
         * no finalization, not containing pointers, not zero'd, and not ref-counted.
         *
         * This function returns NULL only if kCanFail is passed in 'flags'.
         * Note that kContainsPointers implies kZero, pointer containing memory is
         * always zeroed (see bug 594533).
         *
         * We have the following invariants:
         *
         * A pointer returned from Alloc is aligned at least on an 8-byte boundary.
         *
         * There are at present no useful guarantees about better alignment like that,
         * notably there are no guarantees about 16-byte alignment to benefit float4.
         */
        void *Alloc(size_t size, int flags=0);

        /**
         * Specialized implementations of Alloc().  Flags are omitted, each function is annotated
         * with the flags they assume.   Additionally, 'size' is usually known statically in the
         * calling context (typically a 'new' operator).  Finally, these are always inlined.
         * The result is that most computation boils away and we're left with just a call to the
         * underlying primitive operator.
         */
        void *AllocPtrZero(size_t size);                 // Flags: GC::kContainsPointers|GC::kZero
        void *AllocPtrZeroExact(size_t size);            // Flags: GC::kContainsPointers|GC::kZero|GC::kInternalExact
        void *AllocPtrZeroFinalized(size_t size);        // Flags: GC::kContainsPointers|GC::kZero|GC::kFinalize
        void *AllocPtrZeroFinalizedExact(size_t size);   // Flags: GC::kContainsPointers|GC::kZero|GC::kFinalize|GC::kInternalExact
        void *AllocRCObject(size_t size);                // Flags: GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize
        void *AllocRCObjectExact(size_t size);           // Flags: GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize|GC::kInternalExact

        /**
         * Like Alloc but optimized for the case of allocating one 8-byte non-pointer-containing
         * non-finalizable non-rc non-zeroed object (a box for an IEEE double).
         */
        void* AllocDouble();

        /* Bibop-type allocators */
        void* AllocBibop(GCAlloc* bibopAlloc);
        template<uint8_t bibopType> void* AllocBibopType();

#define  AllocFloat     AllocBibopType<avmplus::AtomConstants::kBibopFloatType>
#define  AllocFloat4    AllocBibopType<avmplus::AtomConstants::kBibopFloat4Type>

#if defined _DEBUG || defined AVMPLUS_SAMPLER || defined MMGC_MEMORY_PROFILER
        /**
         * The actual allocator in situations where there's a lot of singing and
         * dancing around book-keeping.
         */
        void* AllocSlow(GCAlloc* bibopAlloc);
#endif

        /**
         * Like Alloc but allocating a little extra memory; factored out as a
         * separate function in order to allow for a fast object overflow check.
         */
        void *AllocExtra(size_t size, size_t extra, int flags=0);

        /**
         * Specialized implementations of Alloc().  See above for explanations.
         */
        void *AllocExtraPtrZero(size_t size, size_t extra);             // Flags: GC::kContainsPointers|GC::kZero
        void *AllocExtraPtrZeroExact(size_t size, size_t extra);        // Flags: GC::kContainsPointers|GC::kZero|GC::kInternalExact
        void *AllocExtraPtrZeroFinalized(size_t size, size_t extra);    // Flags: GC::kContainsPointers|GC::kZero|GC::kFinalize
        void *AllocExtraPtrZeroFinalizedExact(size_t size, size_t extra); // Flags: GC::kContainsPointers|GC::kZero|GC::kFinalize|GC::kInternalExact
        void *AllocExtraRCObject(size_t size, size_t extra);            // Flags: GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize
        void *AllocExtraRCObjectExact(size_t size, size_t extra);       // Flags: GC::kContainsPointers|GC::kZero|GC::kRCObject|GC::kFinalize|GC::kInternalExact

        /**
         * Out-of-line version of AllocExtra, used by the specialized versions
         */
        void *OutOfLineAllocExtra(size_t size, size_t extra, int flags);

        /**
         * Just like Alloc but can return NULL
         */
        void *PleaseAlloc(size_t size, int flags=0);

        /**
         * Signal that client code has performed an allocation from memory known not to be
         * controlled by this GC, and wants this memory accounted for because its lifetime
         * is tied to objects that this GC controls.  (A typical case is when fixed or system
         * memory is tied to a host object, eg BitmapData in Flash Player.)
         *
         * The GC may use this information to drive garbage collection, and may perform
         * GC activity during this call.
         *
         * @param type is the type of the dependent memory
         */
        void SignalDependentAllocation(size_t nbytes, DependentMemoryType type=typeUnknown);

        /**
         * Signal that client code has performed a deallocation of memory known not to be
         * controlled by this GC, and wants this memory accounted for.
         *
         * The GC may use this information to drive garbage collection, and may perform
         * GC activity during this call.
         *
         * It is crucial that SignalDependentDeallocation is called only to account for
         * objects previously accounted for by SignalDependentAllocation, and that it is
         * called for all objects accounted for by SignalDependentAllocation as those
         * objects are being destroyed.  The GC verifies neither of those conditions.
         *
         * @param type is the type of the dependent memory
         *
         * @see SignalDependentAllocation
         */
        void SignalDependentDeallocation(size_t nbytes, DependentMemoryType type=typeUnknown);

        /**
         * Move a subarray of pointers to managed objects from within one managed object
         * to within another managed object, with efficient handling of write barriers.  The
         * source and destination objects can be the same object.
         *
         * @param dstObject is a pointer to the base of the destination object (as returned from GC::Alloc, say)
         * @param dstArray is the pointer to the base of the destination slot array within dstObject
         * @param dstOffset is the offset within dstArray, in number of pointers, of the first destination slot
         * @param srcArray is the pointer to the base of the source slot array within some unspecified source object
         * @param srcOffset is the offset within srcArray, in number of pointers, of the first source slot
         * @param numPointers is the number of pointers to move.
         */
        void movePointers(void* dstObject, void **dstArray, uint32_t dstOffset, const void **srcArray, uint32_t srcOffset, size_t numPointers);

        /**
         * Move a subarray of pointers to managed objects from one place within
         * one managed object to another place within the same object, with efficient
         * handling of write barriers.
         *
         * NOTE that the units of offsets are bytes, not pointers, allowing for a pointer array
         * that is offset from the start of an object.
         *
         * FIXME: Bugzilla 615193 This method is misnamed, "Block" has a specific meaning within
         * MMgc and "managed object" ain't it.
         *
         * @param array is some base address inside the managed object
         * @param dstOffsetInBytes is the offset in bytes from array at which to start writing pointers
         * @param srcOffsetInBytes is the offset in bytes from array at which to start reading poitners
         * @param numPointers is the number of pointers to move
         * @param zeroEmptied is true if the space that is emptied (the top area if moving down, the bottom area
         *        if moving up) should be zeroed.  Only pass false for this parameter if you know that that
         *        will not create reference counting or false retention bugs.
         */
        void movePointersWithinBlock(void** array, uint32_t dstOffsetInBytes, uint32_t srcOffsetInBytes, size_t numPointers, bool zeroEmptied = true);

        /**
         * Given a a GC memory block, reverse the order of numPointers worth of pointers (or pointer-sized data), starting at offsetInBytes from mem.
         *
         * FIXME: Bugzilla 615193: This method is misnamed, "Block" has a specific meaning within
         * MMgc and "managed object" ain't it.
         */
        void reversePointersWithinBlock(void* mem, size_t offsetInBytes, size_t numPointers);

    private:
#ifdef DEBUG
        void AllocPrologue(size_t size);
        void* AllocEpilogue(void* item, int flags);
#endif
        /**
         * Signal that we've allocated some memory and that collection can be triggered
         * if necessary.
         */
        void SignalAllocWork(size_t size);

        /**
         * Signal that we've freed some memory.  This should not have any side effects,
         * ie, it should not trigger collection.
         */
        void SignalFreeWork(size_t size);

        const static size_t kLargestAlloc = 1968;

        // See comments for HandleLargeMarkItem.
        const static size_t kMarkItemSplitThreshold = kLargestAlloc;

        class RCRootSegment : public StackMemory
        {
        public:
            RCRootSegment(GC* gc, void* mem, size_t size);
            void*           mem;
            size_t          size;
            RCRootSegment*  prev;
            RCRootSegment*  next;
        };

        RCRootSegment* rcRootSegments;

        void AddRCRootSegment(RCRootSegment *segment);

        void RemoveRCRootSegment(RCRootSegment *segment);

    public:

        class AutoRCRootSegment : public RCRootSegment
        {
        public:
            AutoRCRootSegment(GC* gc, void* mem, size_t size);
            ~AutoRCRootSegment();
        };


        /**
         * Allocate memory that will be scanned for pointers to GC memory
         * and from which pointers to RC objects will pin those objects.
         */
        void* AllocRCRoot(size_t size);

        /**
         * Free memory allocated with AllocRCRoot.
         */
        void FreeRCRoot(void* mem);

        /**
         * Save the processor context on the stack, make a GC root from the stack, and call 'fn'
         * with 'arg' as the only argument.  The GC root is active during the invocation of 'fn'.
         *
         * There are two main uses for this:
         *
         * 1) the main thread is halting to let another thread run doing
         *    possible GC activity (debugger use case)
         *
         * 2) the stack is moving to another area of memory (stack switching)
         *    but the old stack must still get scanned
         */
        void CreateRootFromCurrentStack(void (*fn)(void* arg), void* arg);

        /**
         * overflow checking way to call Alloc for a # of n size'd items,
         * all instance of Alloc(num*sizeof(thing)) should be replaced with:
         * Calloc(num, sizeof(thing))
         */
        void *Calloc(size_t num, size_t elsize, int flags=0);

        /**
         * Free a GC allocated object.  The pointer may be NULL.
         * The object must not be finalizable - the finalizer will not be called.
         */
        void Free(const void *ptr);

        /**
         * Free a GC allocated object.  The pointer must not be NULL.
         * The object must not be finalizable - the finalizer will not be called.
         */
        void FreeNotNull(const void *ptr);

        /**
         * Zero the body of the object but leave any vtable and header bits unchanged.
         * Use with caution - intended for certain debugging applications.
         */
        void Zero(const void *ptr);

        /**
         * @return the size of a managed object given a user or real pointer to its
         * beginning.  The returned value may be bigger than what was asked for.
         */
        static size_t Size(const void *ptr);

        /**
         * @return the GC object associated with a managed object.
         * this item must be a small item (<1968) or the first page of a large allocation,
         * it won't work on secondary pages of a large allocation.
         */
        static GC* GetGC(const void *item);

        /**
         * @return the mark (live) bit for the given object: zero if not set,
         * nonzero if set.
         */
        static int GetMark(const void *userptr);

        /**
         * Mark the given object as live.
         */
        static void SetMark(const void *userptr);

        /**
         * @return the queued bit for the given object: zero if not set, nonzero if set.
         */
        static int GetQueued(const void *userptr);

        // Not a hot method
        void ClearQueued(const void *userptr);

    private:
        // not a hot method
        static void ClearFinalized(const void *userptr);

    public:
        // not a hot method
        static int IsFinalized(const void *userptr);

        // not a hot method
        static int HasWeakRef(const void *userptr);

        /**
         * Mark the given object as having (flag==true) or not having (flag==false)
         * a weak reference.
         */
        static void SetHasWeakRef(const void *userptr, bool flag);

        /**
         * Query the exactly traced flag.
         */
        static int IsExactlyTraced(const void* userptr);
        
        /**
         * Used by sub-allocators to obtain memory.
         */
        void* AllocBlock(int size, PageMap::PageType pageType, bool zero=true, bool canFail=false);

        void FreeBlock(void *ptr, uint32_t size);

        // Host API: this should go away but Player code is currently using it.  VM code should not use it.
        GCHeap *GetGCHeap() const;

        void ReapZCT(bool scanNativeStack=true);

        bool Reaping();

#ifdef _DEBUG
        // Test whether the RCObject cleaned itself properly by zeroing everything.
        void RCObjectZeroCheck(RCObject *);

        // Test whether 'item' points into a non-free object in the gc'd heap.
        bool IsPointerIntoGCObject(const void* item);
#endif

        /**
         * Do as much marking as possible in the allotted time slice
         */
        void IncrementalMark();

        /**
         * Perform some garbage collection work: the allocation budget has been exhausted.
         */
        void CollectionWork();

        /**
         * Are we currently marking?
         */
        bool IncrementalMarking();

        /**
         * Is the write barrier active?  If true, then pointer stores *must* be filtered
         * through the write barrier (see the "write barrier" section directly below for
         * the relevant APIs).
         *
         * NOTE: In the past this has been equivalent to IncrementalMarking(), or even to
         * reading the "marking" member, but in general BarrierActive() is different from
         * those two methods (generational collectors' barriers are always active, for example).
         */
        bool BarrierActive();
        
        // The following APIs are really private and are shared only with specific code
        // that's not in the GC class.  Alas C++ does not provide a good solution for
        // expressing that, so they're public.
        //
        // *** DO NOT USE THESE APIs EXCEPT AS ANNOTATED ***

    public:
        // *** PRIVATE TO MMGC ***
        //
        // To be called from delete operators in GCObject / GCFinalizedObject / etc.  Does not
        // check that the object is not finalizable.  Separate from FreeFromGCNotNull to allow
        // profiling / diagnosis of explicit deletion.

        void FreeFromDelete(const void *ptr);

    private:
        // To be called from the GC/ZCT.  Does not check that the object is not finalizable.
        void FreeFromGCNotNull(const void *ptr);
        
        void AbortFree(const void* item);

        //////////////////////////////////////////////////////////////////////////
        //
        // Write barrier.  Those that are REALLY_INLINE are defined in WriteBarrier.h.

        /**
         * Perform the actual store of value into *address. Abstraction so we can put
         * the heap graph code in one place, all write barriers need to call this.
         */
        static void WriteField(const void *address, const void *value);

        /**
         * WriteField with assertions that value and pointer at address are NULL
         * or not RCObjects.
         */
        static void WriteFieldNonRC(const void *address, const void *value);

        /**
         * Do a WriteField and also update reference counts.
         */
        static void WriteFieldRC(const void *address, const void *value);

        /**
         * Helper for write barrier's given only an address, container is
         * calculated using FindBeginning.
         */
        void InlineWriteBarrierGuardedTrap(const void *address);

        /**
         * These functions are the implementations of the out-of-line
         * functions below, see below for docs.
         */
        static void InlineWriteBarrier(const void *address, const void *value);
        static void InlineWriteBarrierRC(const void *address, const void *value);
        static void InlineWriteBarrierRC_ctor(const void *address, const void *value);
        static void InlineWriteBarrierRC_dtor(const void *address);

        /**
         * Static out of line versions of write barriers, primarily
         * for use from smart pointer classes WriteBarrier<T> and
         * WriteBarrierRC<T>.
         *
         * These have the added overhead of having to calculate the
         * container address and GC*.  An assert will fire if the
         * 'value' passed to WriteBarrier is an RCObject or if there
         * is a pointer at 'address' that is an RCObject.  Similarly
         * the RC forms will assert on non-RCObjects.
         */
        static void FASTCALL WriteBarrier(const void *address, const void *value);
        static void FASTCALL WriteBarrierRC(const void *address, const void *value);
        // Optimized version that ignores the value at address, it must be NULL.
        static void FASTCALL WriteBarrierRC_ctor(const void *address, const void *value);

    public:

        /**
         * This is the implementation of the WBRC_NULL so its
         * public. There's no write barrier performed, just NULL the
         * field and decrement the ref count.
         */
        static void FASTCALL WriteBarrierRC_dtor(const void *address);
        
    private:

        /**
         * The marker is running and the container object has just been transitioned from
         * 'marked' to 'queued'.
         */
        void WriteBarrierHit(const void* container);

    private:
        /**
         * Implementation of privateWriteBarrier; too large to be inlined everywhere.
         */
        /*REALLY_INLINE*/ void privateInlineWriteBarrier(const void *container, const void *address, const void *value);

        /**
         * Like the preceding but computes 'container' only if 'marking' is true (important optimization).
         */
        /*REALLY_INLINE*/ void privateInlineWriteBarrier(const void *address, const void *value);

        /**
         * Implementation of privateWriteBarrierRC; too large to be inlined everywhere.
         */
        /*REALLY_INLINE*/ void privateInlineWriteBarrierRC(const void *container, const void *address, const void *value);

        /**
         * Like the preceding but computes 'container' only if 'marking' is true (important optimization).
         */
        /*REALLY_INLINE*/ void privateInlineWriteBarrierRC(const void *address, const void *value);

        /**
         * Out-of-line implementation of ConservativeWriteBarrierNoSubstitute, called only
         * when 'marking' is true.
         */
        void privateConservativeWriteBarrierNoSubstitute(const void *address);

    public:
        /**
         * General, conditional write barrier trap.  Tests that incremental marking is in fact ongoing
         * and that the container is black (marked, not queued); if so, it makes the container gray
         * by scheduling it for rescanning at some appropriate time.
         *
         * Container must be a non-NULL untagged pointer to the beginning of an object on a page
         * owned by this GC.
         */
        void WriteBarrierTrap(const void *container);

        /**
         * Inline implementation of WriteBarrierTrap; probably too large to be inlined everywhere
         * so use sparingly.  AvmCore::atomWriteBarrier benefits substantially from inlining it.
         *
         * PRECONDITION: 'marking' must be true when this is called.
         */
        /*REALLY_INLINE*/ void InlineWriteBarrierTrap(const void *container);

        /**
         * Standard write barrier write for non-RC values.  If marking is ongoing, and the 'container'
         * is black (ie marked and not queued) and the 'value' is white (ie unmarked and not queued)
         * then make sure to queue value for marking.
         *
         * Finally stores value into *address.
         *
         * 'container' may be NULL, in which case this is just a store and 'address' can be arbitrary.
         *
         * If 'container' is not NULL then it is constrained as for WriteBarrierTrap, and 'address'
         * must point into the object referenced by 'container'.
         *
         * Value can be anything except a reference to an RCObject.
         *
         * This is called by the WB macro in WriteBarrier.h - not an API to be used otherwise.
         */
        void privateWriteBarrier(const void *container, const void *address, const void *value);

        /**
         * Standard write barrier write for RC values.  If marking is ongoing, and the 'container'
         * is black (ie marked and not queued) and the 'value' is white (ie unmarked and not queued)
         * then make sure to queue value for marking.
         *
         * Finally stores value into *address, adjusting the reference counts of both the old value
         * and the new value.
         *
         * 'container' is constrained as for WriteBarrierTrap.
         *
         * 'address' must point into the object referenced by 'container'.  The value there, if not
         * NULL, must be a (possibly tagged) pointer to an RC object.
         *
         * 'value', if not NULL, must point to an RC object owned by this GC.
         *
         * This is called by the WBRC macro in WriteBarrier.h - not an API to be used otherwise.
         */
        void privateWriteBarrierRC(const void *container, const void *address, const void *value);

        /**
         * Host API: if 'address' points to a GC page (it can point into an object, not just
         * to the start of one), and 'value' points to a GC page as well, record that 'value'
         * has been stored into the object containing 'address'.
         *
         * The function does /not/ perform the store (this is what NoSubstitute means).
         *
         * The function works for both RCObjects and GCObjects.
         */
        void ConservativeWriteBarrierNoSubstitute(const void *address, const void *value);

        /**
         * Host API: 'container' must be a non-NULL untagged pointer to the beginning of an
         * object on a page owned by this GC.  Record that 'value' has been stored into
         * 'container'.
         *
         * The function does /not/ perform the store (this is what NoSubstitute means).
         *
         * The function works for both RCObjects and GCObjects.
         */
        void WriteBarrierNoSubstitute(const void *container, const void *value);

    public:
        GCPolicyManager policy;

    public:

        /**
         * @return true if the gc item contains pointers.  Item must be valid GC pointer.
         */
        static bool ContainsPointers(const void *item);

        /**
         * Will reliably return NULL if gcItem does not point into
         * managed memory (and _DEBUG code will also assert if that
         * happens - it should not unless allowGarbage is true).  Note
         * that this function is for pointers to bytes inside GC items,
         * pointers to GC block headers (ie before the first object on
         * the page) will return NULL.
         */
        void *FindBeginningGuarded(const void *gcItem, bool allowGarbage=false);

        // Legacy public API.  DO NOT USE from within AVM code.
        void *FindBeginning(const void *gcItem);

        // Used on the critical path of the write barrier path.  gcItem must point into
        // managed memory, and NULL is never returned.  Don't use this for GCAssert and
        // similar purposes, but use it when speed is key.
        void *FindBeginningFast(const void *gcItem);

        /**
         * @return true if the useritem is an RCObject.  Item must be valid GC pointer.
         */
        static bool IsRCObject(const void *userptr);

        /**
         * True during Sweep phase.  Application code can use this to
         * determine if it's being called (directly or indirectly) from a
         * finalizer.
         */
        bool Collecting();

        /**
         * True during the Sweep phase, while the GC is calling presweep callbacks.
         */
        bool Presweeping();

        /**
         * @return true if the item points to a page containing managed objects.
         * Any pointer can be passed in here.
         */
        bool IsPointerToGCPage(const void *item);

        /**
         * @return true is realptr points to the beginning of a GC object.
         */
        bool IsPointerToGCObject(const void *realptr);

        /**
         * GC initialization time, in ticks.  Used for logging.
         * FIXME: why is this public?
         */
        const uint64_t t0;

        // FIXME: these three static methods could be lifted out of the GC class to the global level,
        // they're simple utilities.
        static double duration(uint64_t start);

        // a tick is the unit of VMPI_getPerformanceFrequency()
        static uint64_t ticksToMicros(uint64_t ticks);

        static uint64_t ticksToMillis(uint64_t ticks);

        /**
         * Total number of bytes of pointer-containing memory scanned by this
         * GC.  Used to measure marking rate, which is
         * <code>bytesMarked()/ticksToMillis(markTicks())</code>.
         */
        uint64_t bytesMarked();

        /**
         * Total time spent doing incremental marking, in ticks.  See
         * bytesMarked.
         */
        uint64_t markTicks();

        // calls to mark item
        uint32_t lastStartMarkIncrementCount;
        uint32_t markIncrements();

        /**
         * Number of calls to Sweep().
         */
        uint32_t sweeps;

        /**
         * Time of the latest FinishIncrementalMark() call, in ticks.
         */
        uint64_t sweepStart;

        // called at some apropos moment from the mututor, ideally at a point
        // where all possible GC references will be below the current stack pointer
        // (ie in memory we can safely zero).  This will return right away if
        // called more than once between collections so it can safely be called
        // a lot without impacting performance
        void CleanStack(bool force=false);

        bool Destroying();

        /**
         * @return a reference to the mark bits for the given realptr.
         * The realptr must point to the beginning of an object owned
         * by this GC.
         */
        static gcbits_t& GetGCBits(const void* realptr);

        /**
         * @return a GCWeakRef object whose get method will return userptr
         * until its collected at which point NULL will be returned.
         * userptr must be a valid live GC object.
         */
        static GCWeakRef *GetWeakRef(const void *userptr);

        // a WeakRef that always refers to null. useful if you need one.
        GCWeakRef* emptyWeakRef;

        // The allowRehash argument should not be 'false' willy-nilly.
        void ClearWeakRef(const void *obj, bool allowRehash=true);

        // @return The highest stack address, ie the stack 'base' in common parlance.
        // @fixme This is misnamed, 'GetStackBase' would be better.  We can't
        // rename the function now (== 8 January 2010) because it's used externally
        // by performance code and we don't want the churn, but it should be renamed
        // and post-Flash10.1 we will do so.
        uintptr_t GetStackTop() const;

        uintptr_t GetStackEnter() const;

        bool GetMarkStackOverflow() const { return m_markStackOverflow; }

        // If the object is not marked and is not on the mark queue, then mark it and
        // return true.  Otherwise return false.
        //
        // This method is intended to be used by gcTrace implementations that trace
        // single-owner objects directly (eg, the atom table of an InlineHashtable),
        // where the owned object does not have a vtable.  Use sparingly.
        static bool TraceObjectGuard(const void* userptr);

        // Let the contents at *loc be 'obj'.
        // 'obj' is a possibly-null pointer to an object that may or may not be marked or queued and large or small
        // and may or may not contain pointers.  The object will be marked (if not pointer-containing) or pushed
        // onto the mark queue (if pointer-containing) if not already marked or queued.
        template <class T>
        void TraceLocation(T* const * loc);

        template <class T>
        void TraceLocation(MMgc::WriteBarrier<T> const * loc);
        
        template <class T>
        void TraceLocation(MMgc::WriteBarrierRC<T> const * loc);

        template <class T>
        void TraceLocation(MMgc::GCMemberBase<T> const * loc);
        
        void TraceAtom(AtomWBCore * loc);
        
        // ditto, but the pointer at *loc may have tags or other information in the low three bits that we will discard.
        void TraceLocation(uintptr_t* loc);

        // ditto, but the pointer at *loc will have tags in the low three bits that we will use to direct interpretation
        // of the value.  (This should be called TraceLocation but as long as Atom is an uintptr_t C++ gets in the way.)
        void TraceAtom(avmplus::Atom* loc);

        // Let the contents at *loc be 'val'.
        // 'val' is some value that may be a pointer.  It may or may not be NULL.  It may or may
        // not carry a tag.  It may or may not point to GC'd storage.  If it points to GC'd storage
        // then it may or may not point to an object (as opposed to into one, or into free space).
        // If it is a pointer to an object then that object may or may not already be marked or queued
        // and large or small.  If the object is neither marked nor queued it is pushed onto the mark
        // queue (if it contains pointers) or marked (if it does not contain pointers).
        void TraceConservativeLocation(uintptr_t* loc);

        // Trace an array of GCMembers.
        template <class T>
        void TraceLocations(MMgc::GCMemberBase<T>* p, size_t numobjects);

        // Trace an array of pointers as if by TraceLocation(void**).  Does not bounds check.
        template <class T>
        void TraceLocations(T** p, size_t numobjects);

        // Trace an array of possibly-tagged pointer values as if by TraceLocation(uintptr_t*).  Does not bounds check.
        void TraceLocations(uintptr_t* p, size_t numobjects);

        // Trace an array of tagged values as if by TraceLocation(Atom*).  Does not bounds check.
        // (This should be called TraceLocation but as long as Atom is an uintptr_t C++ gets in the way.)
        void TraceAtoms(avmplus::Atom* atoms, size_t numatoms);
        
    private:
        // 'val' is some value that may be a pointer.  It may or may not be NULL.  It may or may
        // not carry a tag.  It may or may not point to GC'd storage.  If it points to GC'd storage
        // then it may or may not point to an object (as opposed to into one, or into free space).
        // If it is a pointer to an object then that object may or may not already be marked or queued
        // and large or small.  If the object is neither marked nor queued it is pushed onto the mark
        // queue (if it contains pointers) or marked (if it does not contain pointers).  If
        // 'handleInteriorPtrs' is true then the pointer is recognized as a pointer even if it points
        // into an object.
        // 'loc' is the location within the containing object from which 'val' was read.
        void TraceConservativePointer(uintptr_t val, bool handleInteriorPtrs HEAP_GRAPH_ARG(uintptr_t* loc));
        
        // 'obj' is a possibly-null pointer to an object that may or may not be marked or queued and large or small
        // and may or may not contain pointers.  'loc' is the location from which 'obj' was read.  The object will
        // be marked (if not pointer-containing) or pushed onto the mark queue (if pointer-containing) if not already
        // marked or queued.
        void TracePointer(void* obj /* user pointer */ HEAP_GRAPH_ARG(const uintptr_t *loc));

        // Trace an array of tagged atom values.  Does not bounds check - assumes that
        // there are at least 'numatoms' atoms to trace.
        void TraceAtomValue(avmplus::Atom atom HEAP_GRAPH_ARG(avmplus::Atom *loc));
        
    public:
#ifdef DEBUG
        // Check that invariants for an inactive GC hold
        void ShouldBeInactive();
#endif

        bool onThread();

        // Take any pointer and determine if its an rc object,
        // specifically that its on a GC page marked for RC objects
        // and points to the beginning of that object.  Basically
        // shorthand for IsPointerToGCObject && IsRCObject.  Any input
        // is safe, this never crashes or asserts.
        bool IsRCObjectSafe(const void *anyptr);

#ifdef VMCFG_TELEMETRY
        // Given a memory type, returns the total dependent memory
        // allocated for that memory type
        //
        // @see SignalDependentAllocation, SignalDependentDeallocation
        size_t getDependentMemory(DependentMemoryType memType);

        telemetry::ITelemetry* getTelemetry();
        void setTelemetry(telemetry::ITelemetry* telemetry);
    private:
        size_t m_dependentMemory[typeCount];
        telemetry::ITelemetry* m_telemetry;
        
        void UpdateDependentAllocation(uint32_t bytes, DependentMemoryType memType);
        void UpdateDependentDeallocation(uint32_t bytes, DependentMemoryType memType);
#endif

    private:

        // heapAlloc is like heap->Alloc except that it also calls policy.signalBlockAllocation
        // if the allocation succeeded.
        void *heapAlloc(size_t size, int flags=GCHeap::flags_Alloc);

        // heapFree is like heap->Free except that it also calls policy.signalBlockDeallocation.
        void heapFree(void *ptr, size_t siz=0, bool profile=true);

        friend class GCAutoEnter;
        friend class GCAutoEnterPause;
        bool ThreadEnter(GCAutoEnter *enter, bool doCollectionWork, bool tryEnter);
        void ThreadLeave(bool doCollectionWork, GC *prevGC);
        void ThreadEdgeWork();

        GCAutoEnter *GetAutoEnter();

        vmpi_spin_lock_t m_gcLock;

        // store a handle to the thread that create the GC to ensure thread safety
        vmpi_thread_t m_gcThread;

        // Will be called from GCHeap::AbortCleanup before a longjmp that jumps past the
        // GC (effectively aborting an operation, be it allocation or GC).  This function
        // must be aware of the internal state of the GC and must take
        // care to make that state consistent.
        void SignalImminentAbort();

        void gclog(const char *format, ...);
        void log_mem(const char *name, size_t s, size_t comp );

        const static int kNumSizeClasses = 40;

        uint32_t *AllocBits(int numBytes, int sizeClass);

        void FreeBits(uint32_t *bits, int sizeClass);

        uint32_t *m_bitsFreelists[kNumSizeClasses];
        uint32_t *m_bitsNext;

        GCHashtable weakRefs;

        // If a weak reference in the weakref table points to an unmarked object then
        // clear the weak reference and remove it from the weakref table.  If a weak
        // reference in the table points to a marked object and is itself unmarked then
        // mark it.
        void MarkOrClearWeakRefs();

        // BEGIN FLAGS
        // The flags are hot, group them and hope they end up in the same cache line

        // True when the GC is being destroyed
        bool destroying;

        /**
         * True if incremental marking is on and some objects have been marked.
         * This means write barriers are enabled.
         *
         * The GC thread may read and write this flag.  Application threads in
         * requests have read-only access.
         *
         * It is possible for marking==true and collecting==false but not vice versa.
         */
        bool marking;

        /**
         * True during the sweep phase of collection.  Several things have to
         * behave a little differently during this phase.  For example,
         * GC::Free() does nothing during sweep phase; otherwise finalizers
         * could be called twice.
         *
         * Also, Collect() uses this to protect itself from recursive calls
         * (from badly behaved finalizers).
         *
         * It is possible for marking==true and collecting==false but not vice versa.
         */
        bool collecting;

        /**
         * True if a pre-Reap validation trace is underway to find
         * reachable zero count objects.
         */
        bool performingDRCValidationTrace;

        /**
         * True during the sweep phase of collection, while we're calling presweep
         * callbacks.  We use this to implement the read barrier in GCWeakRef::get,
         * when a presweep resurrects an unmarked object.
         */
        bool presweeping;

        /**
         * Nonzero if IncrementalMark is currently active.  This is more specific than 'marking';
         * Collect() uses this to protect itself from recursive calls during OOM handling.
         * (Those calls should not happen because the only allocation to happen during marking
         * is the mark stack and it should be careful not to invoke OOM handling, but debugging
         * code and other ad-hoc code may interfere.  This state variable is cheap insurance.)
         *
         * The flag is a counter because some functions call both Mark and MarkItem_*, and then
         * Mark also calls MarkItem_*, so the flag may needs to be set and reset on several levels.
         * The counter takes care of this naturally.
         */
        uint32_t markerActive;

        // END FLAGS


        // we track the top and bottom of the stack for cleaning purposes.
        // the top tells us how far up the stack as been dirtied.
        // the bottom is also tracked so we can ensure we're on the same
        // stack that the GC responsible for cleaning.  necessary if multiple
        // threads use the GC.  only thread that creates the GC will have its stack
        // tracked and cleaned.
        bool stackCleaned;
        const void *rememberedStackTop;

        // Bugzilla 754281: Tracks the stack entry point according to
        // the most recently constructed GCAutoEnter.  Used for
        // 1. Heuristic stack-cleaning (see GC::CleanStack()), and
        // 2. Precise bounds for stack-scanning when determining GC
        //    root set (only if MMGC_HAS_TRUSTWORTHY_GET_STACK_ENTER
        //    is defined; see GC::GetStackTop()).
        GCAutoEnter* stackEnter;
        uint32_t enterCount;

#ifdef VMCFG_SELECTABLE_EXACT_TRACING
        const gcbits_t runtimeSelectableExactnessFlag; // 0 or kVirtualGCTrace
#endif

        GCMarkStack m_incrementalWork;
        void StartIncrementalMark();
        void FinishIncrementalMark(bool scanNativeStack, bool okToShrinkHeapTarget=true);

        GCMarkStack m_barrierWork;
        void CheckBarrierWork();
        void FlushBarrierWork();

        bool m_markStackOverflow;
        void HandleMarkStackOverflow();
        
        void SignalMarkStackOverflow_GCObject(const void *p);
        void SignalMarkStackOverflow_NonGCObject();
        
        // Set at initialization to the deepest recursion level MarkItem_* is allowed to
        // reach.  Managed entirely within MarkItem.
        uint32_t mark_item_recursion_control;

#ifdef _DEBUG
        // Works on any address
        bool IsWhite(const void *item);
#endif

        // Used heavily by the write barrier.
        bool IsMarkedThenMakeQueued(const void* userptr);

        // Used heavily by GC::Free.
        bool IsQueued(const void* userptr);

        const static int16_t kSizeClasses[kNumSizeClasses];
        const static uint8_t kSizeClassIndex[246];

        // These two members help optimize GC::Alloc: by keeping a pointer in the GC instance
        // for the kSizeClassIndex table we avoid code generation SNAFUs when compiling with -fPIC,
        // which is the default on Mac at least.  (GCC generates a call-to-next-instruction-and-pop
        // to obtain the PC address, from which it can compute the table address.  Keeping the
        // member here effectively hoists that computation out of the allocator.)  And by keeping
        // a lookup table of allocators indexed by the flag bits of interest we avoid a decision
        // tree inside GC::Alloc.

        const uint8_t* const sizeClassIndex;
        GCAlloc** allocsTable[(kRCObject|kFinalize|kContainsPointers)+1];

        void *m_contextVars[GCV_COUNT];

        /**
         * Tracks pages in use; 2 bits per page (see PageType enum).
         */
#ifdef MMGC_USE_UNIFORM_PAGEMAP
        PageMap::Uniform pageMap;
#else
#ifdef MMGC_64BIT
        PageMap::DelayT4 pageMap;
#else
        PageMap::Tiered2 pageMap;
#endif // MMGC_64BIT
#endif // MMGC_USE_UNIFORM_PAGEMAP

        // This is very hot
        PageMap::PageType GetPageMapValue(uintptr_t addr) const;

        // This is warm - used in IsPointerToGCPage and in FindBeginningGuarded
        PageMap::PageType GetPageMapValueGuarded(uintptr_t addr);

        /**
         * Set the pageMap bits for numpages starting from given address.
         * Those bits must be zero beforehand.
         */
        void MarkGCPages(void *item, uint32_t numpages, PageMap::PageType val);

        /**
         * Zero out the pageMap bits for numpages starting from given address.
         */
        void UnmarkGCPages(void *item, uint32_t numpages);

        /**
         * Mark a region of memory, this will search all memory pointed to recursively
         * and mark any GC Objects it finds
         */
        void ConservativeMarkRegion(const void *base, size_t bytes);

        GCAlloc *containsPointersNonfinalizedAllocs[kNumSizeClasses]; // Non-RC non-finalized objects containing pointers
        GCAlloc *containsPointersFinalizedAllocs[kNumSizeClasses];    // Non-RC finalized objects containing pointers
        GCAlloc *containsPointersRCAllocs[kNumSizeClasses];           // RC finalized objects containing pointers
        GCAlloc *noPointersNonfinalizedAllocs[kNumSizeClasses];       // Non-RC non-finalized objects not containing pointers
        GCAlloc *noPointersFinalizedAllocs[kNumSizeClasses];          // Non-RC finalized objects not containing pointers
        GCAlloc *bibopAllocFloat;
        GCAlloc *bibopAllocFloat4;
        GCLargeAlloc *largeAlloc;
        GCHeap *heap;

#ifdef _DEBUG
    public:
#else
    private:
#endif
        void ClearMarks();

#ifdef DEBUG
        /**
         * Do a full GC mark phase for DRC validation purposes.
         */
        void DRCValidationTrace(bool scanNativeStack=true);
#endif

    private:
        void Finalize();
        void Sweep();
        void MarkNonstackRoots(bool deep=false);
        void MarkStackRoots();
        void MarkRoots(bool deep, bool stackroots);
        void Mark();
        void Mark(uint32_t count);
        void MarkQueueAndStack(bool scanNativeStack=true);
        void MarkTopItem_NonGCObject();
        void MarkItem_GCObject(const void* object);
        void MarkItem_ExactObjectTail(const void* object, size_t cursor);
        void MarkItem_ConservativeOrNonGCObject(const void* object, uint32_t size, GCMarkStack::TypeTag type, const void* baseptr, bool interiorPtrs);
        void SplitExactGCObject(const void* object);
        void SplitItem_ConservativeOrNonGCObject(const void* object, uint32_t& size, GCMarkStack::TypeTag type, const void* baseptr);
        void EstablishSweepInvariants();
        void ClearMarkStack();
        void AbortInProgressMarking();

        static void DoCleanStack(void* stackPointer, void* arg);
        static void DoMarkFromStack(void* stackPointer, void* arg);

    public:
        // Sweep all small-block pages that need sweeping
        void SweepNeedsSweeping();

    private:
        bool finalizedValue;

        void AddToSmallEmptyBlockList(GCAlloc::GCBlock *b);

        /**
         * List of pages to be swept, built up in Finalize.
         */
        GCAlloc::GCBlock *smallEmptyPageList;

        void AddToLargeEmptyBlockList(GCLargeAlloc::LargeBlock *lb);

        /**
         * List of pages to be swept, built up in Finalize.
         */
        GCLargeAlloc::LargeBlock *largeEmptyPageList;

        /**
         * Free list budget management for small-block allocators
         */
        void ObtainQuickListBudget(size_t nbytes);
        void RelinquishQuickListBudget(size_t nbytes);

        size_t remainingQuickListBudget;
        uint32_t victimIterator;

        vmpi_spin_lock_t m_rootListLock;

        GCRoot *m_roots;
        void AddRoot(GCRoot *root);
        void RemoveRoot(GCRoot *root);

        /**
         * Points to the head of a linked list of callback objects.
         */
        GCCallback *m_callbacks;
        void AddCallback(GCCallback *cb);
        void RemoveCallback(GCCallback *cb);

        // Deferred ref counting implementation
        ZCT zct;
#ifdef MMGC_REFCOUNT_PROFILING
        void AddToZCT(RCObject *obj, bool initial=false);
#else
        void AddToZCT(RCObject *obj);
#endif

#ifdef MMGC_REFCOUNT_PROFILING
        REALLY_INLINE void RemoveFromZCT(RCObject *obj, bool final=false);
#else
        REALLY_INLINE void RemoveFromZCT(RCObject *obj);
#endif

    private:
        void DoPreSweepCallbacks();
        void DoPostSweepCallbacks();
        void DoPreReapCallbacks();
        void DoPreReapCallbacks(void* rcObject);
        void DoPostReapCallbacks();

    public:
        // PreventImmediateReaping is used by Flash Player for older content: it means to flag
        // the object so that it won't be reaped until it goes through a 1 -> 0 reference count
        // transition.  Also see the 'dontAddToZCTDuringCollection' configuration variable.
        void PreventImmediateReaping(RCObject* obj);

        static const void *Pointer(const void *p);

    public:
        /**
         * LockObject protects the object against being garbage collected, despite no references to
         * the object being found in a root, the stack, or any other heap object.  The object must
         * not be NULL.  Locks on an object accumulate: if you lock an object n times you must
         * unlock it n times - call UnlockObject on all the returned locks - to make it collectable
         * again.  Each lock returned is distinct and becomes an invalid object reference the moment
         * it is unlocked.
         *
         * GCObjectLock is an opaque type that should be treated like
         * an unmanaged pointer, no write barriers are needed and they
         * can be stored anywhere.
         */
        GCObjectLock* LockObject(const void* obj);

        /**
         * Return the object locked by the lock.
         *
         * NOTE! It is an ERROR to call this if the lock has been unlocked!  YOUR PROGRAM WILL CRASH.
         */
        const void* GetLockedObject(GCObjectLock* lock);

        /**
         * UnlockObject unlocks a lock that was returned by LockObject.
         *
         * NOTE! It is an ERROR to call this if the lock has been unlocked!  YOUR PROGRAM WILL CRASH.
         */
        void UnlockObject(GCObjectLock* lock);
        
    private:
        GCObjectLock* lockedObjects;

    public:
        void DumpMemoryInfo();
#ifdef MMGC_MEMORY_PROFILER
        void DumpPauseInfo();
#endif
#ifdef MMGC_CONSERVATIVE_PROFILER
        ObjectPopulationProfiler<AllocationSiteHandler>* demos;
#endif
#ifdef MMGC_WEAKREF_PROFILER
        WeakRefAllocationSiteProfiler* weaklings;
#endif

    private:
#ifdef MMGC_DELETION_PROFILER
        DeletionProfiler* deletos;
        void ProfileExplicitDeletion(const void* item);
#endif

    public:
        // Used by the sampler.  Attempt to push the object p, but ignore mark stack
        // overflow if it fails.
        void Push_GCObject_MayFail(const void *p);

    private:
        // Push various kinds of items onto the mark stack.
        void Push_GCObject(const void *p);
        void Push_LargeExactObjectTail(const void* p, size_t cursor);
        void Push_StackMemory(const void* p, uint32_t size, const void* baseptr);
        void Push_LargeObjectChunk(const void *p, uint32_t size, const void* baseptr);
        void Push_LargeRootChunk(const void *p, uint32_t size, const void* baseptr);
        bool Push_RootProtector(const void* p);
        void Push_LargeObjectProtector(const void* p);
        
#ifdef _DEBUG
        // Assert invariants on GCObjects about to go on the mark stack.
        void WorkItemInvariants_GCObject(const void *p);
        
        /**
         * Check the consistency of the free lists for all the allocators.
         */
        void CheckFreelists();

        /**
         * Conservatively find pointers in the entire heap referencing unmarked objects,
         * triggering assertions when one is found.  This is a sanity check on the GC
         * marking algorithm.
         */
        void FindUnmarkedPointers();

        /**
         * Conservatively find pointers in an address range referencing unmarked objects,
         * triggering assertions when one is found.
         */
        void UnmarkedScan(const void *mem, size_t size);

        // methods for incremental verification

        /**
         * Scan a region of memory for white pointers. Used by
         * FindMissingWriteBarriers.
         */
        void WhitePointerScan(const void *mem, size_t size);

        /**
         * Scan all GC memory (skipping roots). If a GC object is black make sure
         * it has no pointers to white objects.
         */
        void FindMissingWriteBarriers();

#endif

public:

        void DumpAlloc(GCAlloc *a, size_t& internal_waste, size_t& overhead);

        //This method returns the number bytes allocated for GC objects
        size_t GetBytesInUse();

        //This method returns the number of bytes allocated for GC objects
        //Also see GetBytesInUse()
        size_t GetBytesInUseFast();

        //This method is for more fine grained allocation details
        //It reports the total number of bytes requested (i.e. ask size) and
        //the number of bytes actually allocated.  The latter is the same
        //number as reported by GetBytesInUse()
        void GetUsageInfo(size_t& totalAskSize, size_t& totalAllocated);

        size_t GetNumBlocks();

        virtual void memoryStatusChange(MemoryStatus oldStatus, MemoryStatus newStatus);

        /* A portable replacement for alloca().
         *
         * Memory is allocated from the heap and not from the stack.  It is freed in
         * one of two ways: If the function returns normally then an auto_ptr like
         * mechanism frees the memory.  If the function leaves by throwing an exception
         * (or if one of its callees throws an exception) then the exception
         * handling mechanism in Exception.{h,cpp} frees the memory by releasing
         * everything that is still allocated that was allocated since the exception
         * handler was erected.
         *
         * The auto_ptr mechanism, based on the class MMgc::GC::AllocaAutoPtr, cannot be
         * circumvented, as allocaPush() takes a reference to such an object as an argument.
         *
         * Typical usage:
         *
         *    MMgc::GC::AllocaAutoPtr _ptr;                      // by convention prefixed by "_"
         *    int* ptr = (int*)core->allocaPush(_ptr, nbytes);  // by convention same name, no "_"
         *
         * In practice the avmStackAlloc() macro should be used so that
         * real alloca() can be used on platforms where that makes sense.
         *
         * Benchmarks suggest that the performance differences from using this mechanism
         * instead of real alloca() are slight to nonexistent, and that the heap allocation
         * sometimes provides a performance improvement.
         */
    public:
        struct AllocaStackSegment
        {
            void* start;                // first address; also, the RCRoot pointer
            void* limit;                // address past data
            void* top;                  // address past live if this segment is not the top
            AllocaStackSegment* prev;   // segments further from the top
        };

        void allocaInit();
        void allocaShutdown();
        void allocaUnwind();            // restore to initial state
        void allocaPopToSlow(void* top);
        void* allocaPushSlow(size_t nbytes);
        void pushAllocaSegment(size_t nbytes);
        void popAllocaSegment();

        AllocaStackSegment* top_segment;// segment at the stack top
        void* stacktop;                 // current first free word in top_segment
#ifdef _DEBUG
        size_t stackdepth;              // useful to have for debugging
#endif

    public:
        /* See documentation above */

        class AllocaAutoPtr
        {
            friend class GC;
        public:
            AllocaAutoPtr();
            ~AllocaAutoPtr();
        private:
            GC* gc;
            void* unwindPtr;
        };

        void* allocaTop();

        void allocaPopTo(void* top);

        void* allocaPush(size_t nbytes, AllocaAutoPtr& x);

#ifdef DEBUGGER
    public:
        // Attach an opaque sampler to this GC, overriding any previous sampler
        void* GetAttachedSampler();

        // Retrieve the opaque sampler from this GC, NULL if none has been attached
        void SetAttachedSampler(void *sampler);

    private:
        // An attached sampler, or NULL.  We need to attach the sampler to the GC
        // because it moves with the GC/AvmCore pair, it is not attached to a
        // particular thread.
        void* m_sampler;
#endif

#ifdef MMGC_HEAP_GRAPH

    public:

        void addToBlacklist(const void *gcptr);
        void removeFromBlacklist(const void *gcptr);
        // Public utility to dump against marker graph.
        void DumpBackPointerChain(const void *gcptr);

    private:

        GCHashtable blacklist;

        void printBlacklist();
        void pruneBlacklist();

        const void *findGCGraphBeginning(const void *addr, bool &wasDeletedGCRoot);
        void dumpBackPointerChain(const void *obj, HeapGraph &g);
        void dumpBackPointerChainHelper(const void *p, HeapGraph& g);
        HeapGraph mutatorGraph;
        HeapGraph markerGraph;
#endif

#ifdef DEBUG

        // Debugging code that validates reference count during reaping.
        void DefRefValidate(RCObject* obj);
        bool BackPointerChainStillValid(const void *obj);

    public:

        // Called by generated tracer checking code.
        static MMgc::GCTracerCheckResult CheckOffsetIsInList(uint32_t offset, const uint32_t offsets[],size_t len);

        // Called by write barrier classes to verify that its being traced.
        static void TracePointerCheck(const void *derivedPointer);
#endif // DEBUG
    };

    // helper class to wipe out vtable pointer of members for DRC
    class Cleaner
    {
    public:
        Cleaner();
        ~Cleaner();
        // don't let myself move between objects
        Cleaner& operator=(const Cleaner& /*rhs*/);
        void set(const void * _v, size_t _size);

        int *v;
        size_t size;
    };

    /**
     * Stack object that takes care of many things including defining stack
     * boundaries, doing low stack queued collections/ZCT reaps and stack cleaning
     */
    class GCAutoEnter : public AbortUnwindObject
    {
    public:
        enum EnterType {
            kNormal=0,
            kTryEnter=1
        };
        GCAutoEnter(GC *gc, EnterType type=kNormal);
        ~GCAutoEnter();
        bool Entered();
        void Destroy(bool doCollectionWork);
        virtual void Unwind();
    private:
        GC* m_gc;
        GC* m_prevgc;
    };

    /**
     * Undoes the affects of GCAutoEnter on entry and reapplies them upon exit
     */
    class GCAutoEnterPause
    {
    public:
        GCAutoEnterPause(GC *gc);
        ~GCAutoEnterPause();
    private:
        GC* gc;
        GCAutoEnter *enterSave;
    };
}

#endif /* __GC__ */

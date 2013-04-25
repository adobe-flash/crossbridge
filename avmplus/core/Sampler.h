/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Sampler__
#define __avmplus_Sampler__

namespace avmplus
{
#ifdef DEBUGGER

    //
    // Note that "SamplerObjectType" is atomlike, but is NOT an Atom:
    //
    // -- the lower three bits are SamplerObjectTypeKind
    // -- if kind is kSOT_Object, upper bits are a VTable*
    // -- if kind is kSOT_String or kSOT_Namespace, upper bits are a Toplevel*
    // -- if kind is kSOT_Empty, upper bits are zero
    //
    // note that for efficiency, the Toplevel* in the String/Namespace case is filled in lazily downstream
    // (via sotSetToplevel), to avoid having String/Namespace ctors look up the proper Toplevel
    // when not sampling.
    //
    // To reinforce the fact that this Isn't An Atom, it's now its own opaque type, with
    // lovely little accessors. Please use them now, rather than casting to intptr and
    // dancing on the bits directly.
    //

    enum SamplerObjectTypeKind
    {
        kSOT_Object = 0,
        kSOT_String = 1,
        kSOT_Namespace = 2,
        kSOT_Empty = 3
    };

    REALLY_INLINE SamplerObjectType sotNamespace()
    {
        // start with Toplevel == NULL, filled in implicitly later
        return SamplerObjectType(kSOT_Namespace);
    }

    REALLY_INLINE SamplerObjectType sotString()
    {
        // start with Toplevel == NULL, filled in implicitly later
        return SamplerObjectType(kSOT_String);
    }

    REALLY_INLINE SamplerObjectType sotEmpty()
    {
        return SamplerObjectType(kSOT_Empty);
    }

    REALLY_INLINE SamplerObjectType sotObject(VTable* vt)
    {
        MMGC_STATIC_ASSERT(kSOT_Object == 0);
        return SamplerObjectType(uintptr_t(vt) | uintptr_t(kSOT_Object));
    }

    REALLY_INLINE SamplerObjectTypeKind sotGetKind(SamplerObjectType sot)
    {
        return SamplerObjectTypeKind(uintptr_t(sot) & 7);
    }

    REALLY_INLINE VTable* sotGetVTable(SamplerObjectType sot)
    {
        MMGC_STATIC_ASSERT(kSOT_Object == 0);
        AvmAssert(sotGetKind(sot) == kSOT_Object);
        return (VTable*)(sot);
    }

    REALLY_INLINE Toplevel* sotGetToplevel(SamplerObjectType sot)
    {
        AvmAssert(sotGetKind(sot) == kSOT_String || sotGetKind(sot) == kSOT_Namespace);
        return (Toplevel*)(uintptr_t(sot) & ~7);
    }

    REALLY_INLINE SamplerObjectType sotSetToplevel(SamplerObjectType sot, Toplevel* toplevel)
    {
        SamplerObjectTypeKind const sk = sotGetKind(sot);
        if (sk == kSOT_String || sk == kSOT_Namespace)
        {
            sot = SamplerObjectType(uintptr_t(toplevel) | uintptr_t(sk));
        }
        return sot;
    }

    // return value will be a VTable* or a Toplevel* -- intended only for use in adding GC work items
    REALLY_INLINE void* sotGetGCPointer(SamplerObjectType sot)
    {
        return (void*)(uintptr_t(sot) & ~7);
    }

    // This structure is used to read/write data to the sample stream.
    // The fields are written out to the sample stream as they are defined here.
    struct Sample
    {
        uint64_t micros;
        uint32_t sampleType;
        union {
            // not filled in for sampleType==DELETED_OBJECT_SAMPLE
            struct {
                // Number of StackTraceElements in the trace
                uint32_t depth;
                // Beginning of an array of StackTraceElement.  Basically, an MethodInfo*, Stringp, Stringp, uint32_t for each entry.
                void *trace;
            } stack;
            // deleted object size record, instead of stack
            uint64_t size;
        };
        // filled for DELETED_OBJECT_SAMPLE + NEW_OBJECT_SAMPLE
        uint64_t id;

        // Following three fields are only filled in for sampleType==NEW_OBJECT_SAMPLE or NEW_AUX_SAMPLE
        // They are not present in the sample stream for other sample types
        SamplerObjectType sot;
        void *ptr;
        uint64_t alloc_size; // size for new mem sample
    };

    // The abstract interface for a memory sampler. Use the AttachSampler() method to attach an
    // IMemorySampler based class to MMgc for getting called back when allocations/deallocations occur. 
    class IMemorySampler
    {
    public:
        // Called when a new auxiliary memory allocation occurs.
        virtual void recordAllocation(const void *item, size_t size) = 0;

        // Called when a memory deallocation occurs.
        virtual void recordDeallocation(const void *item, size_t size) = 0;

        // Called when a new ScriptObject based object has been allocated.
        virtual void recordNewObjectAllocation(AvmPlusScriptableObject *obj, avmplus::SamplerObjectType sot) = 0;

        virtual ~IMemorySampler() {}
    };

    // Set the current memory sampler. A memory sampler is tied to a particular GC/core pair.    
    void AttachSampler(IMemorySampler* sampler);
    
    // Get the current memory sampler.
    IMemorySampler* GetSampler();

    class Sampler : public MMgc::GCRoot, public IMemorySampler
    {
    public:
        Sampler(AvmCore*);
        virtual ~Sampler();

        enum SampleType
        {
            RAW_SAMPLE=0x55555555,
            NEW_OBJECT_SAMPLE=0xaaaaaaaa,
            DELETED_OBJECT_SAMPLE=0xdddddddd,
            NEW_AUX_SAMPLE=0xeeeeeeee
        };

        enum { SAMPLE_FREQUENCY_MILLIS = 1 };

        // should use opaque Cursor type instead of uint8_t*
        uint8_t *getSamples(uint32_t &num);
        void readSample(uint8_t *&p, Sample &s);

        void init(bool sampling, bool autoStart);
        void sampleCheck() { if(takeSample) sample(); }

        void recordAllocation(const void *item, size_t size);
        void recordDeallocation(const void *item, size_t size);
        void recordNewObjectAllocation(AvmPlusScriptableObject *obj, avmplus::SamplerObjectType sot);
        uint64_t recordAllocationInfo(AvmPlusScriptableObject *obj, SamplerObjectType sot);
        uint64_t recordAllocationSample(const void* item, uint64_t size, bool callback_ok = true, bool forceWrite = false);
        void recordDeallocationSample(const void* item, uint64_t size);

        virtual void startSampling();
        virtual void stopSampling();
        virtual void clearSamples();
        virtual void pauseSampling();

        void sampleInternalAllocs(bool b);

        void setCallback(ScriptObject* callback);


        // called by VM after initBuiltin's
        void initSampling();

        void createFakeFunction(const char *name);
        Stringp getFakeFunctionName(const char* name);

        void presweep();
        void postsweep();

        inline uint32_t sampleCount() const { return numSamples; }
        inline bool activelySampling() { return samplingNow; }
        inline bool sampling() const { return _sampling; }
        inline uint64_t getSampleBufferId() const { return sampleBufferId; }

    private:

        void sample();

        void rewind(uint8_t*&b, uint32_t amount)
        {
            b -= amount;
        }

        int sampleSpaceCheck(bool callback_ok = true);

        // sampleTimeMicros is the timestamp you want to have written to the sample,
        // or 0 to use the current time.
        void writeRawSample(SampleType sampleType, uint64_t sampleTimeMicros = 0);

        // Returns the current time, as it should be written into samples.
        static uint64_t nowMicros();

    // ------------------------ DATA SECTION BEGIN
    public:
        VTable*             sampleIteratorVTable;
        VTable*             slotIteratorVTable;
    protected:
        AvmCore*            core;
    private:
        RCList<String>      fakeMethodNames;
        uint64_t            allocId;
        uint64_t            sampleBufferId;
        uint8_t*            samples;
        uint8_t*            currentSample;
        uint8_t*            lastAllocSample;
        uint64_t            lastSampleCheckMicros; // the last time we considered writing a timer sample
        GCMember<ScriptObject> callback;
        uintptr_t           timerHandle;
        MMgc::GCHashtableBase<uint64_t,MMgc::GCHashtableKeyHandler,MMgc::GCHashtableAllocHandler_VMPI>      uids;       // important to use the VMPI variant for non-MMGC-based memory allocation.
        MMgc::GCHashtable_VMPI      ptrSamples; // important to use the VMPI variant for non-MMGC-based memory allocation.
        volatile int32_t    takeSample;
        uint32_t            numSamples;
        uint32_t            samples_size;
        bool                samplingNow;
        bool                samplingAllAllocs;
        bool                runningCallback;
        bool                autoStartSampling;  // if true we call startSampling as early as possible during startup
        bool                _sampling;          // are we sampling at all
    // ------------------------ DATA SECTION END
    };

    #define SAMPLE_FRAME(_strp, _core)  avmplus::CallStackNode __fcsn((avmplus::AvmCore*)_core, _strp)
    #define SAMPLE_CHECK()              __fcsn.sampleCheck();

#else

    #define SAMPLE_FRAME(_x, _s)
    #define SAMPLE_CHECK()

#endif // DEBUGGER
}

#ifdef VMCFG_TELEMETRY_SAMPLER
namespace telemetry
{
    /* An interface that lets the VM call into the host when it's time to take a sample.
       See the sampler description in AvmCore.h for more details. */
    struct ISampler
    {
        virtual void takeSample() = 0;
    };
	
    /* An opaque handle that represents a function on a call stack. A given function should have the same handle for
	 the entire duration of a run. */
    typedef avmplus::MethodInfo* FunctionHandle;
}
#endif // VMCFG_TELEMETRY_SAMPLER

#endif // __avmplus_Sampler__


/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCObject__
#define __GCObject__

#ifdef __GNUC__
    #define GNUC_ONLY(x) x
#else
    #define GNUC_ONLY(x)
#endif

// These are declared but not implemented so we can catch uses of them
// as link errors.   If we remove these we run the risk of new (GC*) being
// compiled as a call to placement new.
void *operator new(size_t size, MMgc::GC *gc);
void *operator new(size_t size, MMgc::GC *gc, int flags);

namespace MMgc
{
    /**
     * Base class for GC pointer containing structures that aren't
     * directly heap allocated but are used to compose other GCObject
     * or GCRoots.  In practice this introduces the GCMember local
     * definition and hides new/delete.
     */    
    class GCInlineObject
    {
    public:
#ifdef DEBUG
        /**
         * The function gcTraceOffsetIsTraced is overriden in classes
         * with generated tracers (its declared by GC_DATA_BEGIN).  In
         * classes with manual tracers this implementation here is
         * used and it returns kPassDueToManualTrace which tells the
         * safety check to stop the checking process.  We can't
         * perform the safety check unless the entire class is
         * annotated with the exactgc system. Note that the form of
         * the generated function is:

           GCTracerCheckResult result;
           if((result = super->gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound) 
               return result;
           if((someStruct.gcTraceOffsetIsTraced(off, offsetof(MyClass, someStruct))) != MMgc::kOffsetNotFound)
               return result;
           return GC::CheckOffsetIsInList(off,myOffsets);

           * Its important for "super" above to never to be
           * GCInlineObject or GCTraceableBase because we would always
           * return kPassDueToManualTracer and skip the safety
           * checking.  The trace gen script accounts for this by not
           * generating the super call when the base class doesn't
           * have a tracer (ie GCInlineObject or GCTraceableBase and
           * its immediate subclasses).
         */
        GCTracerCheckResult gcTraceOffsetIsTraced(uint32_t) const 
        {
            return kPassDueToManualTracer;
        }
#endif

        void gcTrace(GC*, size_t cursor=0)
        {
            (void)cursor;
            GCAssertMsg(false, "This class needs a gcTrace, did you forget to annotate it or write a custom gcTrace?");
        }

        //  GCObject's local definition of GCMember
        template<class T>
        class GCMember : public GCMemberBase<T>
        {
        public:

            template<class T2>
            void operator=(const GCRef<T2>& other);

            void operator=(T* tNew);

            //  Since we're locking up the copy constructor, we need to explicitly define the default constructor
            explicit GCMember();
        
            template<class T2>
            explicit GCMember(const GCRef<T2>& other);

            explicit GCMember(T* valuePtr);
            
            explicit GCMember(const GCMember<T>& other);
        };
    private:
        // Private and unimplemented to prevent heap allocation.
        static void *operator new(size_t size) GNUC_ONLY(throw());
        static void operator delete(void *gcObject);
    };

    /**
     * Base class for GC managed objects that aren't finalized and are
     * conservatively traced.
     */
    class GCObject
    {
    public:

        // Methods used by GCMember<T>
        REALLY_INLINE static void WriteBarrier(void **address, void *value)
        {
            GC::WriteBarrier(address, value);
        }

        REALLY_INLINE static void WriteBarrier_dtor(void **address)
        {
            *address = NULL;
        }
        
        // These are noops overridden by RCObject to do the right thing.
        REALLY_INLINE void IncrementRef() {}
        REALLY_INLINE void DecrementRef() {}
                        
        //  GCObject's local definition of GCMember
        template<class T>
        class GCMember : public GCMemberBase<T>
        {
        public:
                        
            template<class T2>
            void operator =(const GCRef<T2>& other);

            void operator=(T* tNew);

            //  Since we're locking up the copy constructor, we need to explicitly define the default constructor
            explicit GCMember();
        
            template<class T2>
            explicit GCMember(const GCRef<T2>& other);

            explicit GCMember(T* valuePtr);
            
            explicit GCMember(const GCMember<T>& other);
        };
        
        // 'throw()' annotation to avoid GCC warning: 'operator new' must not return NULL unless it is declared 'throw()' (or -fcheck-new is in effect)
        static void *operator new(size_t size, GC *gc, size_t extra) GNUC_ONLY(throw());

        static void *operator new(size_t size, GC *gc) GNUC_ONLY(throw());

        static void operator delete(void *gcObject);

        GCWeakRef *GetWeakRef() const;

        //  Create a GCRef from "this" with parameter for auto template specialization.
        template <class T>
        REALLY_INLINE GCRef<T> GetThisRef(const T *ths) const
        {
            GCAssert(ths == (const T*)this);
            GCRef<T> ref((T*)ths);
            return ref;
        }
    };

    /**
     * Base class for objects that have no GC pointers in them.  A
     * LeafObject has all the same APIs as GCObject except it does not
     * define GCMember as it makes no sense for a LeafObject to have a
     * GCMember.
     */
    class LeafObject
    {
    public:

        // Methods used by GCMember<T>
        REALLY_INLINE static void WriteBarrier(void **address, void *value)
        {
            GC::WriteBarrier(address, value);
        }

        REALLY_INLINE static void WriteBarrier_dtor(void **address)
        {
            *address = NULL;
        }
        
        // These are noops overridden by RCObject to do the right thing.
        REALLY_INLINE void IncrementRef() {}
        REALLY_INLINE void DecrementRef() {}

        static void *operator new(size_t size, GC *gc) GNUC_ONLY(throw())
        {
            return gc->Alloc(size, GC::kZero);
        }
        
        // On purpose! Forward thinking 
        //        static void operator delete(void *gcObject);

        GCWeakRef *GetWeakRef() const
        {
            return GC::GetWeakRef(this);
        }

        //  Create a GCRef from "this" with parameter for auto template specialization.
        template <class T>
        REALLY_INLINE GCRef<T> GetThisRef(const T *ths) const
        {
            GCAssert(ths == (const T*)this);
            GCRef<T> ref((T*)ths);
            return ref;
        }
    };

    /**
     * LeafVector provides a zero overhead fixed length array of
     * primitives that don't need to be scanned by the GC for
     * pointers.  The New factory method safely calculates the amount
     * of space needed, ie it checks for overflow.  Zeroing of memory
     * is not done by default, GC::kZero can be passed in the optional
     * 'flags' template parameter to have the memory zeroed.
     * 
     * LeafVectors are not subclassable and access to the array of
     * elements must happen via the AsArray method.
     */
    template<class T, GC::AllocFlags flags=GC::kNoFlags>
    class LeafVector : public LeafObject
    {
    public:

        /**
         * Return a pointer to the first element
         */
        T* AsArray() 
        { 
#ifdef DEBUG
            // check that the apiEnforcement field is untouched
            for(char *dummy = (char*)&apiEnforcement, *end=(char*)(&apiEnforcement+1); dummy < end; dummy++)
            {
                GCAssertMsg(*dummy == GCHeap::LeafApiEnforcementPoison, "LeafVector underwrite, make sure AsArray is being used");
            }
#endif
            return elements;
        }

        /**
         * Allocate a LeafVector that can store 'count' elements of T.
         * A count of zero will allocate space for 1 element.
         */
        static LeafVector<T,flags>* New(GC *gc, size_t count)
        {
            return new (gc, count) LeafVector<T,flags>();
        }

        // On purpose! Forward thinking 
        //        static void operator delete(void *gcObject);
        
    private:

        // 'throw()' annotation to avoid GCC warning: 'operator new' must not return NULL unless it is declared 'throw()' (or -fcheck-new is in effect)
        static void *operator new(size_t size, GC *gc, size_t count) GNUC_ONLY(throw())
        {
            // Account for element already built into size, if you ask for 0 you get 1.
            if(count > 0)
                count--;
            size_t arraySize = GCHeap::CheckForCallocSizeOverflow(sizeof(T), count);
            size_t bytes = GCHeap::CheckForAllocSizeOverflow(size, arraySize);
            return gc->Alloc(bytes, flags & GC::kZero);
        }

        // Private ctor to prevent subclassing.
        LeafVector() 
        {
#ifdef DEBUG
            VMPI_memset(&apiEnforcement, GCHeap::LeafApiEnforcementPoison, sizeof(T));
#endif
        }

#ifdef DEBUG
        // prevent raw casting from LeafVector to T*
        T apiEnforcement;
#endif
        T elements[1];
    };

    // GCTraceableBase is internal to MMgc.

    class GCTraceableBase
    {
        friend class GC;
#ifdef DEBUG
    protected:
        // See comments for GCInlineObject gcTraceOffsetIsTraced.
        virtual GCTracerCheckResult gcTraceOffsetIsTraced(uint32_t) const 
        { 
            return kPassDueToManualTracer; 
        }
#endif

    public:
        virtual ~GCTraceableBase() { /* must be empty */ }

        // The gcTrace method will be called to exactly mark the object.  It must trace
        // all locations in the object that may contain pointers to managed objects.
        //
        // The gcTrace method may be called multiple times on a single object,
        // in order to allow a large object to be marked piecemeal.  On the
        // first call 'cursor' will be 0, on the next it will be 1, and so on.
        // Any value must be accepted for 'cursor' but unless it is 0 then it
        // will be one greater than it was on the last invocation for the same
        // object.  On each invocation the method should attempt to do marking
        // work corresponding roughly to kLargestAlloc/sizeof(void*) words.
        //
        // The method must return 'true' if there is more marking work to be done,
        // otherwise 'false'.  If 'true' is returned then the object will be re-visited
        // with cursor incremented.
        //
        // Note that marking may start over without first reaching the end
        // of the object, as a result of updates interleaved with marking or
        // mark stack overflow.  A restarted marking is indicated by a cursor == 0.
        //
        // The base version of this method aborts the process - you must
        // override if there's a chance that it can be called, and you
        // must never delegate to the base method.

        virtual bool gcTrace(GC* gc, size_t cursor);
        
        // The class must have an empty operator 'delete' to prevent the C++
        // compiler from inserting a call to the system's delete operator into
        // the base class destructor.
        static void operator delete(void *) { /* must be empty */ }

        // Methods used by GCMember<T>
        REALLY_INLINE static void WriteBarrier(void **address, void *value)
        {
            GC::WriteBarrier(address, value);
        }

        REALLY_INLINE static void WriteBarrier_dtor(void **address)
        {
            *address = NULL;
        }
        
        // These are noops overridden by RCObject to do the right thing.
        REALLY_INLINE void IncrementRef() {}
        REALLY_INLINE void DecrementRef() {}

        template<class T>
        class GCMember : public GCMemberBase<T>
        {
        public:
            template<class T2>
            void operator=(const GCRef<T2>& other);

            void operator=(T* tNew);

            //  Since we're locking up the copy constructor, we need to explicitly define the default constructor
            explicit GCMember();

            template <class T2>
            explicit GCMember(const GCRef<T2>& other);
            
            explicit GCMember(T* valuePtr);
            
            explicit GCMember(const GCMember<T>& other);
        };
    };

    /**
     * Base class for GC-managed objects that are not finalizable but have
     * a virtual gcTrace method.  This must be used as the most base class
     * instead of GCObject, it can't be mixed in later.
     *
     * Note that subtypes that want to be traced exactly must pass the kExact flag
     * as the second argument to the 'new' operator.
     *
     * The following two invariants must hold for exactly traced objects and their
     * tracer (see also Bugzilla 
     *
     *   - The tracer must always be able to interpret a pointer field that is
     *     all-bits-zero, because the GC may see the object and call the tracer
     *     before the constructor that initializes the field is run.
     *
     *   - No exactly traced field must ever contain a bogus value.  That's generally
     *     not hard to ensure but there are three cases to watch out for:
     *
     *     - in a "tagged union" setting, a separate tag field directs the
     *       interpretation of the pointer field.  The tag and the payload must
     *       always be set together, without any chance of the GC getting to
     *       run after one is set and before the other is set.
     *
     *     - in a "counted array" setting, a separate count field directs the
     *       interpretation of an array field (frequently a trailing array).
     *       This is like the tagged union case, but if the count is stored in
     *       a separate object it is important that the separate object pointer
     *       is not NULL, as the GC would not check that (in a GC_POINTERS setting,
     *       for example).
     *
     *     - it must be true also when the object is about to be freed, as with
     *       a call to GC::Free, because GC::Free cannot guarantee that the object
     *       will in fact be freed.
     *
     * Note that the C++ constructor settings syntax can obscure what's going
     * on, as it makes assignments and calls less visible.
     *
     *
     * The operators and methods here are exactly like those of GCObject.
     */
    class GCTraceableObject : public GCTraceableBase
    {
    public:
        // This class can only have an empty destructor.
        
        static void *operator new(size_t size, GC *gc, GCExactFlag flag, size_t extra) GNUC_ONLY(throw());
        static void *operator new(size_t size, GC *gc, size_t extra) GNUC_ONLY(throw());
        static void *operator new(size_t size, GC *gc, GCExactFlag flag) GNUC_ONLY(throw());
        static void *operator new(size_t size, GC *gc) GNUC_ONLY(throw());
        static void operator delete(void *gcObject);
        
        GCWeakRef *GetWeakRef() const;
    };

    /**
     * Baseclass for GC managed objects that are finalized.
     *
     * Note that subtypes that want to be traced exactly must pass the kExact flag
     * as the second argument to the 'new' operator.
     *
     * Invariants that must hold for exactly traced objects are documented above,
     * for GCTraceableObject.
     *
     */
    class GCFinalizedObject : public GCTraceableBase
    {
    public:
        // This class can only have an empty destructor.

        //  Create a GCRef from "this" with parameter for auto template specialization.
        template <class T>
        REALLY_INLINE GCRef<T> GetThisRef(const T *ths) const
        {
            GCAssert(ths == (const T*) this);
            GCRef<T> ref((T*)ths);
            return ref;
        }

        GCWeakRef *GetWeakRef() const;

        static void* operator new(size_t size, GC *gc, GCExactFlag flag, size_t extra);
        static void* operator new(size_t size, GC *gc, size_t extra);
        static void* operator new(size_t size, GC *gc, GCExactFlag flag);
        static void* operator new(size_t size, GC *gc);
        static void* operator new(size_t size, GC *gc, GCExactFlag, GCNoFinalizeFlag, size_t extra);
        static void* operator new(size_t size, GC *gc, GCNoFinalizeFlag, size_t extra);
        static void* operator new(size_t size, GC *gc, GCExactFlag, GCNoFinalizeFlag);
        static void* operator new(size_t size, GC *gc, GCNoFinalizeFlag);
        static void operator delete (void *gcObject);
    };

    REALLY_INLINE void *GCObject::operator new(size_t size, GC *gc, size_t extra) GNUC_ONLY(throw())
    {
        return gc->AllocExtraPtrZero(size, extra);
    }

    REALLY_INLINE void *GCObject::operator new(size_t size, GC *gc) GNUC_ONLY(throw())
    {
        return gc->AllocPtrZero(size);
    }

    REALLY_INLINE void GCObject::operator delete(void *gcObject)
    {
        GC::GetGC(gcObject)->FreeFromDelete(gcObject);
    }

    REALLY_INLINE GCWeakRef* GCObject::GetWeakRef() const
    {
        return GC::GetWeakRef(this);
    }

    REALLY_INLINE void *GCTraceableObject::operator new(size_t size, GC *gc, size_t extra) GNUC_ONLY(throw())
    {
        return gc->AllocExtraPtrZero(size, extra);
    }
    
    REALLY_INLINE void *GCTraceableObject::operator new(size_t size, GC *gc) GNUC_ONLY(throw())
    {
        return gc->AllocPtrZero(size);
    }
    
    REALLY_INLINE void *GCTraceableObject::operator new(size_t size, GC *gc, GCExactFlag, size_t extra) GNUC_ONLY(throw())
    {
        return gc->AllocExtraPtrZeroExact(size, extra);
    }
    
    REALLY_INLINE void *GCTraceableObject::operator new(size_t size, GC *gc, GCExactFlag) GNUC_ONLY(throw())
    {
        return gc->AllocPtrZeroExact(size);
    }
    
    REALLY_INLINE void GCTraceableObject::operator delete(void *gcObject)
    {
        GC::GetGC(gcObject)->FreeFromDelete(gcObject);
    }
    
    REALLY_INLINE GCWeakRef* GCTraceableObject::GetWeakRef() const
    {
        return GC::GetWeakRef(this);
    }
    
    REALLY_INLINE GCWeakRef* GCFinalizedObject::GetWeakRef() const
    {
        return GC::GetWeakRef(this);
    }

    REALLY_INLINE void* GCFinalizedObject::operator new(size_t size, GC *gc, size_t extra)
    {
        return gc->AllocExtraPtrZeroFinalized(size, extra);
    }

    REALLY_INLINE void* GCFinalizedObject::operator new(size_t size, GC *gc)
    {
        return gc->AllocPtrZeroFinalized(size);
    }

    REALLY_INLINE void* GCFinalizedObject::operator new(size_t size, GC *gc, GCExactFlag, size_t extra)
    {
        return gc->AllocExtraPtrZeroFinalizedExact(size, extra);
    }
    
    REALLY_INLINE void* GCFinalizedObject::operator new(size_t size, GC *gc, GCExactFlag)
    {
        return gc->AllocPtrZeroFinalizedExact(size);
    }
    
    REALLY_INLINE void* GCFinalizedObject::operator new(size_t size, GC *gc, GCNoFinalizeFlag, size_t extra)
    {
        return gc->AllocExtraPtrZero(size, extra);
    }
    
    REALLY_INLINE void* GCFinalizedObject::operator new(size_t size, GC *gc, GCNoFinalizeFlag)
    {
        return gc->AllocPtrZero(size);
    }
    
    REALLY_INLINE void* GCFinalizedObject::operator new(size_t size, GC *gc, GCExactFlag, GCNoFinalizeFlag, size_t extra)
    {
        return gc->AllocExtraPtrZeroExact(size, extra);
    }
    
    REALLY_INLINE void* GCFinalizedObject::operator new(size_t size, GC *gc, GCExactFlag, GCNoFinalizeFlag)
    {
        return gc->AllocPtrZeroExact(size);
    }
    
    REALLY_INLINE void GCFinalizedObject::operator delete (void *gcObject)
    {
        GC::GetGC(gcObject)->FreeFromDelete(gcObject);
    }

    /**
     * Base class for reference counted objects.
     *
     * This object always has a finalizer (the C++ destructor).  The C++ destructor /must/
     * leave the object with all-fields-zero.
     *
     * Reference counting is deferred: when an object's reference count drops to zero it
     * is inserted into the zero-count table (the ZCT), see ZCT.h etc.  If the reference
     * count grows above zero the object is removed from the ZCT again.  Every new object
     * is added to the ZCT initially - reference counts start at zero.
     *
     * Occasionally the ZCT is reaped: objects in the ZCT that are not referenced from stack
     * memory or special ZCT roots, and that are not explicitly pinned by client code, are
     * deleted by calling their finalizers and reclaiming their memory.   A finalizer may
     * make the reference counts of more referenced objects drop to zero, whereupon they
     * too are entered into the ZCT (and may be deleted by the ongoing reap, or a later
     * reap).
     *
     * (Under complicated scenarios it is possible for an object allocated during reaping
     * to be erroneously deleted, see https://bugzilla.mozilla.org/show_bug.cgi?id=506644,
     * so client code may want to take note of that.)
     *
     *
     * Invariants on 'composite'.
     *
     * The field 'compsite' holds reference counts, the ZCT index, and flag bits.  We depend
     * on object layout being such that this field is the second word of the object.  This
     * field is used for a heuristic defense-in-depth mechanism:
     *
     *  - In release builds its value is zero iff the object's destructor has been run and
     *    the object has not been reallocated (ie the object may or may not yet have been
     *    placed on a free list).
     *  - In debug builds its value is zero iff the object's destructor has been run and the
     *    object has not yet been put on the free list or been reallocated.  Once it's been
     *    put on the free list the composite word is a cookie: GCHeap::GCFreedPoison or
     *    GCHeap::GCSweptPoison.
     *
     * The RC operations check whether the object is free - and if it is, then the operations
     * have no effect.  This is useful because of the current finalization semantics:
     * finalization and destruction are intertwined, so that the C++ destructor performs both
     * jobs.  Thus an object can't be finalized without being destroyed.  Finalizers run
     * in arbitrary order; thus a finalizer for an object that's part of an object network
     * will not know whether it is safe to operate on other objects in that network.  The
     * extra guards on RC operations makes this safer.  It only works because all objects that
     * are finalizable are finalized together, before any of them are swept or reallocated.
     * It is also an expense that everyone pays for.  The only fix is a change to how
     * finalization is handled.
     */

    class RCObject : public GCFinalizedObject
    {
        friend class GC;
        friend class ZCT;
    public:
        REALLY_INLINE static void *operator new(size_t size, GC *gc, size_t extra)
        {
            return gc->AllocExtraRCObject(size, extra);
        }

        REALLY_INLINE static void *operator new(size_t size, GC *gc)
        {
            return gc->AllocRCObject(size);
        }
 
        REALLY_INLINE static void *operator new(size_t size, GC *gc, GCExactFlag, size_t extra)
        {
            return gc->AllocExtraRCObjectExact(size, extra);
        }
        
        REALLY_INLINE static void *operator new(size_t size, GC *gc, GCExactFlag)
        {
            return gc->AllocRCObjectExact(size);
        }
    public:

        // Used by GCMember<T>
        REALLY_INLINE static void WriteBarrier(void **address, void *value)
        {
            GC::WriteBarrierRC(address, value);
        }

        REALLY_INLINE static void WriteBarrier_dtor(void **address)
        {
            GC::WriteBarrierRC_dtor(address);
        }

        REALLY_INLINE RCObject()
        {
            composite = 1;
            GC::GetGC(this)->AddToZCT(this REFCOUNT_PROFILING_ARG(true));
        }

        REALLY_INLINE ~RCObject()
        {
            // The ZCT reaper removes the object from the ZCT before deleting it,
            // but when non-GC code explicitly deletes the object we may find that
            // it is already in the ZCT.  So remove it if necessary.
            if (InZCT())
                GC::GetGC(this)->RemoveFromZCT(this REFCOUNT_PROFILING_ARG(true));
            // We'd like to assert this but can't, for the moment.  The case is handled
            // in the allocator instead, see call paths ending at AbortFree.
            //GCAssert((GC::GetGC(this)->GetGCBits(this) & (kMark|kQueued)) != kQueued);
            composite = 0;
        }

        /**
         * @return true if the object is currently pinned (explicitly or by
         *         the ZCT's stack pinner).
         */
        REALLY_INLINE bool IsPinned()
        {
            return (composite & STACK_PIN) != 0;
        }

#ifdef _DEBUG
        REALLY_INLINE bool IsGCPoisoned() {
            return composite == uint32_t(GCHeap::GCFreedPoison) || composite == uint32_t(GCHeap::GCSweptPoison);
        }
#endif
        
        /**
         * Explicitly pin the object, protecting it from ZCT reaping.  The pin
         * flag /will/ be cleared if the object is subsequently added to the
         * ZCT and reaping is not ongoing.  It is not advised to call Pin()
         * except from prereap() callback handlers.
         */
        void Pin()
        {
#ifdef _DEBUG
            // This is a deleted object so ignore it.
            if(IsGCPoisoned())
                return;
#endif
            // This is a deleted/free object so ignore it.
            if (getCompositeSafe() == 0)
                return;

            composite |= STACK_PIN;
        }

        /**
         * Explicitly unpin the object, allowing it to be reaped by the ZCT.
         * It is not advised to unpin objects that weren't pinned explicitly
         * by a call to Pin(), and calls to Unpin() should come from postreap()
         * callback handlers.
         */
        void Unpin()
        {
#ifdef _DEBUG
            // This is a deleted object so ignore it.
            if(IsGCPoisoned())
                return;
#endif
            // This is a deleted/free object so ignore it.
            if (composite == 0)
                return;

            composite &= ~STACK_PIN;
        }

        /**
         * @return the object's current reference count.  The value is not
         * valid unless Sticky() returns 0.
         */
        REALLY_INLINE uint32_t RefCount() const
        {
            return (composite & RCBITS) - 1;
        }

        /**
         * @return non-zero if the object is sticky (RC operations have no effect because
         *         the RC field is invalid, either as a consequence of an RC overflow or
         *         because the sticky bit was set explicitly).
         */
        REALLY_INLINE uint32_t Sticky() const
        {
            return composite & STICKYFLAG;
        }

        /**
         * Make RC operations on the object be no-ops.
         */
        REALLY_INLINE void Stick()
        {
#ifdef _DEBUG
            // This is a deleted object so ignore it.
            if(IsGCPoisoned())
                return;
#endif
            // This is a deleted/free object so ignore it.
            if (composite == 0)
                return;

            if (InZCT())
                GC::GetGC(this)->RemoveFromZCT(this REFCOUNT_PROFILING_ARG(true));
            composite |= STICKYFLAG;
        }

        /**
         * Increment the object's reference count.
         *
         * OPTIMIZEME: this is too expensive and too large.  It should be
         * possible to do better if we play around with the positive/negative
         * boundary (eg, operations that don't cross that boundary are
         * cheap, and those that do aren't), but the special-casing of
         * composite==0 and invariants throughtout the system relying on
         * composite==0 make it hard.  We need profiling data on the breakdown
         * of the frequency of various paths through this function.
         */
        REALLY_INLINE void IncrementRef()
        {
#ifdef _DEBUG
            // This is a deleted object so ignore it.
            if(IsGCPoisoned())
                return;
#endif
            // This is a deleted/free object so ignore it.
            if (composite == 0)
                return;

            REFCOUNT_PROFILING_ONLY( GC::GetGC(this)->policy.signalIncrementRef(); )
            if(Sticky())
                return;
#ifdef _DEBUG
            GC* gc = GC::GetGC(this);
            GCAssert(gc->IsRCObjectSafe(this));
#endif

            composite++;
            if((composite&RCBITS) == RCBITS) {
                composite |= STICKYFLAG;
            } else if(InZCT()) {
                GCAssert(RefCount() == 1);
                GC::GetGC(this)->RemoveFromZCT(this);
            }

#ifdef MMGC_RC_HISTORY
            if(GC::GetGC(this)->keepDRCHistory && GCHeap::GetGCHeap()->GetProfiler())
                history.Add(GCHeap::GetGCHeap()->GetProfiler()->GetStackTrace());
#endif
        }

        /**
         * Decrement the object's reference count.
         *
         * OPTIMIZEME: this is too expensive and too large.  It should be
         * possible to do better if we play around with the positive/negative
         * boundary (eg, operations that don't cross that boundary are
         * cheap, and those that do aren't), but the special-casing of
         * composite==0 and invariants throughtout the system relying on
         * composite==0 make it hard.  We need profiling data on the breakdown
         * of the frequency of various paths through this function.
         */
        REALLY_INLINE void DecrementRef()
        {
#ifdef _DEBUG
            // This is a deleted object so ignore it.
            if(IsGCPoisoned())
                return;
#endif
            // This is a deleted/free object so ignore it.
            if (composite == 0)
                return;

            REFCOUNT_PROFILING_ONLY( GC::GetGC(this)->policy.signalDecrementRef(); )
            if(Sticky())
                return;

#ifdef _DEBUG
            GC* gc = GC::GetGC(this);
            GCAssert(gc->IsRCObjectSafe(this));

            // ???
            if(gc->Destroying())
                return;

            if(RefCount() == 0) {
#ifdef MMGC_RC_HISTORY
                DumpHistory();
#endif
                GCAssert(false);
            }
#endif

            if (RefCount() == 0)
            {
                // This is a defensive measure.  If DecrementRef is
                // ever called on a zero ref-count object, composite--
                // will cause an underflow, flipping all kinds of bits
                // in bad ways and resulting in a crash later.  Often,
                // such a DecrementRef bug would be caught by the
                // _DEBUG asserts above, but sometimes we have
                // release-only crashers like this.  Better to fail
                // gracefully at the point of failure, rather than
                // push the failure to some later point.
                return;
            }

            composite--;

#ifdef MMGC_RC_HISTORY
            if(GC::GetGC(this)->keepDRCHistory && GCHeap::GetGCHeap()->GetProfiler())
                history.Add(GCHeap::GetGCHeap()->GetProfiler()->GetStackTrace());
#endif

            if(RefCount() == 0)
                GC::GetGC(this)->AddToZCT(this);
        }

#ifdef MMGC_RC_HISTORY
        void DumpHistory();
#endif

    private:

        // @return non-zero if the object is in the ZCT
        uint32_t InZCT() const
        {
            return composite & ZCTFLAG;
        }

        // Clear the ZCT flag and the ZCT index
        void ClearZCTFlag()
        {
            composite &= ~(ZCTFLAG|ZCT_INDEX);
        }

        // @return the ZCT index.  This is only valid if InZCT returns non-zero.
        uint32_t getZCTIndex() const
        {
            return (composite & ZCT_INDEX) >> 8;
        }

        // Set the ZCT index and the ZCT flag and clear the pinned flag.
        void setZCTIndexAndUnpin(uint32_t index)
        {
            GCAssert(index <= (ZCT_INDEX>>8));
            composite = (composite&~(ZCT_INDEX|STACK_PIN)) | ((index<<8)|ZCTFLAG);
        }

        // Set the ZCT index and the ZCT flag.  If reaping==0 then clear the pinned flag,
        // otherwise preserve the pinned flag.
        void setZCTIndexAndMaybeUnpin(uint32_t index, uint32_t reaping)
        {
            GCAssert(reaping == 0 || reaping == 1);
            GCAssert(index <= (ZCT_INDEX>>8));
            composite = (composite&~(ZCT_INDEX|((~reaping&1)<<STACK_PIN_SHIFT))) | ((index<<8)|ZCTFLAG);
        }

        // Before we read composite tell valgrind its okay if
        // composite isn't defined.  Deleted RCObject pointers can
        // live on the stack so this read is always okay since we
        // check the page header and found this to be a committed
        // RCObject page.
        REALLY_INLINE uint32_t getCompositeSafe()
        {
            uint32_t *c = &composite;
            VALGRIND_MAKE_MEM_DEFINED(c, sizeof(c));
            return *c;
        }

        // Fields in 'composite'
        static const uint32_t ZCTFLAG            = 0x80000000;          // The object is in the ZCT
        static const uint32_t STICKYFLAG         = 0x40000000;          // The object is sticky (RC overflow)
        static const uint32_t STACK_PIN          = 0x20000000;          // The object has been pinned
        static const uint32_t STACK_PIN_SHIFT    = 29;
        static const uint32_t RCBITS             = 0x000000FF;          // 8 bits for the reference count
        static const uint32_t ZCT_INDEX          = 0x0FFFFF00;          // 20 bits for the ZCT index
        static const uint32_t ZCT_CAPACITY       = (ZCT_INDEX>>8) + 1;

        uint32_t composite;
#ifdef MMGC_RC_HISTORY
        // addref/decref stack traces
        BasicList<StackTrace*> history;
#endif // MMGC_MEMORY_INFO
    };

    template<class T, bool checkMissingWB=true>
    class RCPtr
    {
    public:
        RCPtr()
        {
            if(checkMissingWB)
                GCAssertMsg(GC::GetActiveGC() == NULL
                            || !GC::GetActiveGC()->IsPointerToGCPage(this),
                            "Need to use GCMember<> on GC memory");
            t = NULL;
        }
        RCPtr(T _t) : t(_t)
        {
            if(checkMissingWB)
                GCAssertMsg(GC::GetActiveGC() == NULL
                            || !GC::GetActiveGC()->IsPointerToGCPage(this),
                            "Need to use GCMember<> on GC memory");
            if(valid())
                t->IncrementRef();
        }
        ~RCPtr()
        {
            if(valid())
                t->DecrementRef();

            // 02may06 grandma : I want to enable
            //  class DataIOBase { GCMember<PlayerToplevel> const m_toplevel; }
            //
            // DataIOBase is a virtual base class, so we don't know if the
            // subclass is GCObject or not. We need it to be const, or
            // a GCObject would require a GCMember<>, and if it's const, we
            // cannot zero it out during ~DataIOBase. The simplest solution
            // seemed to be zeroing out the member here.

            t = NULL;
        }

        T operator=(T tNew)
        {
            if(valid())
                t->DecrementRef();
            t = tNew;
            if(valid())
                t->IncrementRef();
            // this cast is safe b/c other wise compilation would fail
            return (T) t;
        }

        RCPtr<T>& operator=(const RCPtr<T>& other)
        {
            if(valid())
                t->DecrementRef();
            t = other.t;
            if(valid())
                t->IncrementRef();
            return *this;
        }

        operator T() const
        {
            return (T) t;
        }

        bool operator!=(T other) { return other != t; }

        T operator->() const
        {
            return (T) t;
        }

        void Clear() { t = NULL; }

    private:
        // Hidden and meant not to be used at all.
        RCPtr(const RCPtr<T>& other);

        inline bool valid() { return (uintptr_t)t > 1; }
        T t;
    };

///////////////////////////////////////////////////////////////////////////
//
// NOTE!!  DRC and DRC_NOWB are deprecated!  Use GCMember<> when possible.

#ifndef AVMSHELL_BUILD
    // put spaces around the template arg to avoid possible digraph warnings
    #define DRC(_type) MMgc::RCPtr< _type, true >

    // This is a temporary bandaid for places where DRC is asserting due to
    // it being used on GC memory, use DRCWB instead.
    #define DRC_NOWB(_type) MMgc::RCPtr< _type, false >
#endif // AVMSHELL_BUILD

#undef GNUC_ONLY

}


#endif /* __GCObject__ */

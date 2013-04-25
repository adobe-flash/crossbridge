/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// GCWriteBarrier
//

#ifndef _WRITE_BARRIER_H_
#define _WRITE_BARRIER_H_

// inline write barrier
#define WB(gc, container, addr, value) gc->privateWriteBarrier(container, addr, (const void *) (value))

// fast manual RC write barrier
#define WBRC(gc, container, addr, value) gc->privateWriteBarrierRC(container, addr, (const void *) (value))

// fast-path versions for writing NULL (useful in dtors)

inline void write_null(void* p) { *(uintptr_t*)(p) = 0; }
#define WB_NULL(addr) write_null((void*)addr)

#define WBRC_NULL(addr) MMgc::GC::WriteBarrierRC_dtor(addr)

///////////////////////////////////////////////////////////////////////////
//
// NOTE!!  DWB and DRCWB are deprecated!  Use GCMember<> when possible.

#ifndef AVMSHELL_BUILD
    // declare write barrier
    // put spaces around the template arg to avoid possible digraph warnings
    #define DWB(type) MMgc::WriteBarrier< type >

    // declare an optimized RCObject write barrier
    // put spaces around the template arg to avoid possible digraph warnings
    #define DRCWB(type) MMgc::WriteBarrierRC< type >
#endif // AVMSHELL_BUILD

// This is used in places where the mutator has convinced itself the
// barrier isn't necessary.  FIXME: The idea is to check this in DEBUG
// builds.
#define WB_SKIP(gc, container, addr, value) *(addr) = value

#ifdef MMGC_POLICY_PROFILING
    #define POLICY_PROFILING_ONLY(x) x
#else
    #define POLICY_PROFILING_ONLY(x)
#endif

namespace MMgc
{
    /**
     * WB is a smart pointer write barrier meant to be used on any field of a GC object that
     * may point to another GC object.  A write barrier may only be avoided if if the field is
     * const and no allocations occur between the construction of the object holding the field
     * and the assignment.
     */
    template<class T> class WriteBarrier
    {
#ifdef MMGC_HEAP_GRAPH
        friend class GC;    // for location()
#endif
        
    private:
        T set(const T tNew);

    public:
        explicit WriteBarrier();
        explicit  WriteBarrier(T _t);

        REALLY_INLINE ~WriteBarrier()
        {
            t = 0;
        }

        REALLY_INLINE T operator=(const WriteBarrier<T>& wb)
        {
            return set(wb.t);
        }

        REALLY_INLINE T operator=(T tNew)
        {
            return set(tNew);
        }

        // BEHOLD ... The weird power of C++ operator overloading
        REALLY_INLINE operator T() const { return t; }

        // let us peek at it without a cast
        REALLY_INLINE T value() const { return t; }

        REALLY_INLINE bool operator!=(T other) const { return other != t; }

        REALLY_INLINE T operator->() const
        {
            return t;
        }

    private:
        // Private constructor to prevent its use and someone adding it, GCC creates
        // WriteBarriers on the stack with it
        WriteBarrier(const WriteBarrier<T>& toCopy);    // unimplemented

#ifdef MMGC_HEAP_GRAPH
        const T* location() const { return &t; }
#endif
        
        T t;
    };

    /**
     * WriteBarrierRC is a write barrier for naked (not pointer swizzled) RC objects.
     * the only thing going in and out of the slot is NULL or a valid RCObject
     */
    template<class T> class WriteBarrierRC
    {
#ifdef MMGC_HEAP_GRAPH
        friend class GC;    // for location()
#endif

    private:
        T set(const T tNew);

    public:
        explicit WriteBarrierRC();
        explicit WriteBarrierRC(const T _t);

        ~WriteBarrierRC();

        void set(MMgc::GC* gc, void* container, T newValue);

        REALLY_INLINE T operator=(const WriteBarrierRC<T>& wb)
        {
            return set(wb.t);
        }

        REALLY_INLINE T operator=(T tNew)
        {
            return set(tNew);
        }

        REALLY_INLINE T value() const { return t; }

        REALLY_INLINE operator T() const { return t; }

        REALLY_INLINE bool operator!=(T other) const { return other != t; }

        REALLY_INLINE T operator->() const
        {
            return t;
        }

        // Clear() clears the smart pointer without decrementing the reference count of any object stored in
        // the smart pointer.  It is essentially useful in situations where the caller has already deleted
        // the object and needs to ensure that the destruction of the smart pointer does not access the
        // deleted storage.

        REALLY_INLINE void Clear() { t = 0; }

    private:
        // Private constructor to prevent its use and someone adding it, GCC creates
        // WriteBarrierRCs on the stack with it
        WriteBarrierRC(const WriteBarrierRC<T>& toCopy);

#ifdef MMGC_HEAP_GRAPH
        const T* location() const { return &t; }
#endif
        
        T t;
    };

    // This is intended to be subclassed by avmplus::AtomWB /only/.  It provides the
    // minimum of functionality required by exact tracing of AtomWB smart pointers.

    class AtomWBCore
    {
        friend class GC;
        
    public:
        /** @return the Atom value held by the pointer. */
        avmplus::Atom value() const { return m_atom; }
        
    protected:
        explicit AtomWBCore(avmplus::Atom a);
        explicit AtomWBCore();
        
        avmplus::Atom m_atom;
        
    private:
        explicit AtomWBCore(const AtomWBCore& toCopy);  // unimplemented
        void operator=(const AtomWBCore& wb);           // unimplemented
        
        /**
         * @return the location of the Atom slot.  Used by GC::TraceAtom(AtomWBCore*)
         * to implement MMGC_HEAP_GRAPH.
         */
        avmplus::Atom* location() { return &m_atom; }
    };
}

#endif // _WRITE_BARRIER_H_

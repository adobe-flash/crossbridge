/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// GCWriteBarrier
//

#ifndef _WRITE_BARRIER_INLINES_H_
#define _WRITE_BARRIER_INLINES_H_

namespace MMgc
{
    /*static private*/
    REALLY_INLINE void GC::WriteField(const void *address, const void *value)
    {
#ifdef DEBUG
        GCAssert(((uintptr_t)address & 3) == 0);
        GC *gc = GetActiveGC();
        GCAssertMsg(gc != NULL, "Illegal for a GC write to occur when the GC isn't entered");
        GCAssert(gc->IsPointerIntoGCObject(address));
        GCAssert(Pointer(value)==NULL || gc->IsPointerToGCObject(GetRealPointer(Pointer(value))));
        GCAssert(ContainsPointers(gc->FindBeginning(address)));
#endif
        MMGC_WB_EDGE(address, value);
        *(uintptr_t*)address = (uintptr_t) value;
    }

    /*static private*/
    REALLY_INLINE void GC::WriteFieldNonRC(const void *address, const void *value)
    {
#ifdef DEBUG
        GC *gc = GetActiveGC();
        GCAssertMsg(gc != NULL, "Illegal for a GC write to occur when the GC isn't entered");
        if(Pointer(value) != NULL)
            GCAssert(!gc->IsRCObjectSafe(Pointer(value)));
        const void *valAtAddr = Pointer(*(void**)address);
        if(valAtAddr != NULL)
            GCAssert(!gc->IsRCObjectSafe(valAtAddr));
#endif
        WriteField(address, value);
    }

    /*static private*/
    REALLY_INLINE void GC::WriteFieldRC(const void *address, const void *value)
    {
#ifdef DEBUG
        GC *gc = GetActiveGC();
        if(Pointer(value) != NULL)
            GCAssert(gc->IsRCObjectSafe(Pointer(value)));
        const void *valAtAddr = Pointer(*(void**)address);
        if(valAtAddr != NULL)
            GCAssert(gc->IsRCObjectSafe(valAtAddr));
#endif
        RCObject *rc = (RCObject*)Pointer(*(RCObject**)address);
        if(rc != NULL) {
            rc->DecrementRef();
        }
        WriteField(address, value);
        rc = (RCObject*)Pointer(value);
        if(rc != NULL) {
            rc->IncrementRef();
        }
    }

    /*static private*/
    REALLY_INLINE void GC::InlineWriteBarrier(const void *address, const void *value)
    {
       if(Pointer(value) != NULL)
           GetGC(value)->InlineWriteBarrierGuardedTrap(address);
       WriteFieldNonRC(address, value);
    }

    /*static private*/
    REALLY_INLINE void GC::InlineWriteBarrierRC(const void *address, const void *value)
    {
        if(Pointer(value) != NULL)
            GetGC(value)->InlineWriteBarrierGuardedTrap(address);
        WriteFieldRC(address, value);
    }

    /*static private*/
    REALLY_INLINE void GC::InlineWriteBarrierRC_ctor(const void *address, const void *value)
    {
        GCAssert(*(void**)address == NULL);
        RCObject *rc = (RCObject*)Pointer(value);
        if(rc != NULL)
        {
            GCAssert(GetGC(rc)->IsRCObjectSafe(rc));
            GetGC(rc)->InlineWriteBarrierGuardedTrap(address);
            rc->IncrementRef();
        }
        WriteField(address, value);
    }

    /*static private*/
    REALLY_INLINE void GC::InlineWriteBarrierRC_dtor(const void *address)
    {
        RCObject *rc = (RCObject*)Pointer(*(RCObject**)address);
        if(rc != NULL)
        {
            GCAssert(GetGC(rc)->IsRCObjectSafe(rc));
            rc->DecrementRef();
        }
        WriteField(address, NULL);
     }
 
    REALLY_INLINE void GC::InlineWriteBarrierGuardedTrap(const void *address)
    {
#ifdef _DEBUG
        {
            const void* container = FindBeginningFast(address);

            GCAssert(IsPointerToGCPage(container));
            GCAssert(address >= container);
            GCAssert(address < (char*)container + Size(container));
        }
#endif
        if (BarrierActive()) {
            const void* container = FindBeginningFast(address);
            InlineWriteBarrierTrap(container);
        }
    }

    /*private*/
    REALLY_INLINE void GC::InlineWriteBarrierTrap(const void *container)
    {
        GCAssert(BarrierActive());
        GCAssert(IsPointerToGCPage(container));

        POLICY_PROFILING_ONLY(int stage=0;)
        // If the object is black then it needs to be gray, because we just stored
        // something into it.
        //
        // Testing shows that it's /sometimes/ useful to check the right hand side of the
        // assignment for NULL, but this depends on the program and for the time being
        // the right hand side isn't available here and isn't checked.

        if (IsMarkedThenMakeQueued(container))
        {
            POLICY_PROFILING_ONLY(stage=1;)
            WriteBarrierHit(container);
        }
        POLICY_PROFILING_ONLY( policy.signalWriteBarrierWork(stage); )
    }

    REALLY_INLINE void GC::privateInlineWriteBarrier(const void *container, const void *address, const void *value)
    {
        GCAssert(container != NULL);
        GCAssert(IsPointerToGCObject(GetRealPointer(container)));
        GCAssert(address >= container);
        GCAssert(address < (char*)container + Size(container));

        if (BarrierActive())
            InlineWriteBarrierTrap(container);
        WriteFieldNonRC(address, value);
    }

    REALLY_INLINE void GC::privateInlineWriteBarrierRC(const void *container, const void *address, const void *value)
    {
        GCAssert(container != NULL);
        GCAssert(IsPointerToGCObject(GetRealPointer(container)));
        GCAssert(address >= container);
        GCAssert(address < (char*)container + Size(container));

        if (BarrierActive())
            InlineWriteBarrierTrap(container);
        WriteFieldRC(address, value);
    }

    REALLY_INLINE void GC::privateInlineWriteBarrier(const void *address, const void *value)
    {
        InlineWriteBarrierGuardedTrap(address);
        WriteFieldNonRC(address, value);
    }

    REALLY_INLINE void GC::privateInlineWriteBarrierRC(const void *address, const void *value)
    {
        InlineWriteBarrierGuardedTrap(address);
        WriteFieldRC(address, value);
    }

    REALLY_INLINE void GC::ConservativeWriteBarrierNoSubstitute(const void *address, const void *value)
    {
        (void)value;  // Can't get rid of this parameter now; part of an existing API

        if (BarrierActive())
            privateConservativeWriteBarrierNoSubstitute(address);
    }

    template<class T>
    REALLY_INLINE WriteBarrier<T>::WriteBarrier() : t(0)
    {
#ifdef DEBUG
        GC::TracePointerCheck(&t);
#endif
    }

    template<class T>
    REALLY_INLINE WriteBarrier<T>::WriteBarrier(T _t)
    {
#ifdef DEBUG
        GC::TracePointerCheck(&t);
#endif
        set(_t);
    }

    // Always pay for a single real function call; then inline & optimize massively in WriteBarrier()
    template<class T>
    REALLY_INLINE T WriteBarrier<T>::set(const T tNew)
    {
        GC::WriteBarrier(&t, (const void*)tNew);    // updates 't'
        return tNew;
    }

    // Always pay for a single real function call; then inline & optimize massively in WriteBarrierRC()
    template<class T>
    REALLY_INLINE T WriteBarrierRC<T>::set(const T tNew)
    {
        GC::WriteBarrierRC(&t, (const void*)tNew);  // updates 't'
        return tNew;
    }
    
    template<class T>
    REALLY_INLINE WriteBarrierRC<T>::WriteBarrierRC() : t(0)
    {
#ifdef DEBUG
        GC::TracePointerCheck(&t);
#endif
    }

    template<class T>
    REALLY_INLINE WriteBarrierRC<T>::WriteBarrierRC(const T _t) // : t(0) -- not necessary, WriteBarrierRC_ctor handles it
    {
        GC::WriteBarrierRC_ctor(&t, (const void*)_t);
    }
    
    template<class T>
    REALLY_INLINE WriteBarrierRC<T>::~WriteBarrierRC()
    {
        GC::WriteBarrierRC_dtor(&t);
    }
    
    template<class T>
    REALLY_INLINE void WriteBarrierRC<T>::set(MMgc::GC* gc, void* container, T newValue)
    {
        WBRC(gc, container, &t, newValue);
    }

    REALLY_INLINE AtomWBCore::AtomWBCore(avmplus::Atom a) : m_atom(a)
    {
#ifdef DEBUG
        GC::TracePointerCheck(&m_atom);
#endif
    }
    
    REALLY_INLINE AtomWBCore::AtomWBCore()
    {
#ifdef DEBUG
        GC::TracePointerCheck(&m_atom);
#endif
    }
}

#endif // _WRITE_BARRIER_INLINES_H_

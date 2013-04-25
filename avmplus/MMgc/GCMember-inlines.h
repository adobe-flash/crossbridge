/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCMember_inlines__
#define __GCMember_inlines__

namespace MMgc
{    
#ifdef DEBUG
#ifdef WIN8_IAOS_WORKAROUND
    // On VS2011 on Win8 inlining this causes some compiler optimizations to
    // happen and in some cases this ends up erronously reporting that things
    // are not on stack. For now forcing this to not inline (but leaving 
    // REALLY_INLINE as without it a lot of linker errors.
    __declspec(noinline) REALLY_INLINE bool IsAddressOnStack(void *address)
#else
     REALLY_INLINE bool IsAddressOnStack(void *address)
#endif
    {
        uintptr_t stackBase = AVMPI_getThreadStackBase();
        char stackTop;
        return ((uintptr_t)address > (uintptr_t)&stackTop && (uintptr_t)address < stackBase);
    }
#endif
   
    template<class T>
    REALLY_INLINE void GCMemberBase<T>::set(const T* tNew)
    {
        T::WriteBarrier((void**)&(this->t), (void*)tNew);
    }

    template<class T>
    REALLY_INLINE T* GCMemberBase<T>::value() const
    {
        return this->t;
    }
    
    template<class T>
    REALLY_INLINE T* const* GCMemberBase<T>::location() const
    {
        return &(this->t);
    }

    template<class T>
    REALLY_INLINE GCMemberBase<T>::GCMemberBase() 
        : GCRef<T>()
    {
#ifdef DEBUG
        GCAssert(!IsAddressOnStack(this));
        GC::TracePointerCheck(&(this->t));
#endif
    }
    
    template<class T>
    template<class T2>
    REALLY_INLINE GCMemberBase<T>::GCMemberBase(const GCRef<T2>& other) 
        : GCRef<T>()
    {
#ifdef DEBUG
        GCAssert(!IsAddressOnStack(this));
        GC::TracePointerCheck(&(this->t));
#endif
        set(GCRef<T>::ProtectedGetOtherRawPtr(other));
    }

    // copy constructor
    template<class T>
    REALLY_INLINE GCMemberBase<T>::GCMemberBase(const GCMemberBase<T>& other) 
        : GCRef<T>()
    {
#ifdef DEBUG
        GCAssert(!IsAddressOnStack(this));
        GC::TracePointerCheck(&(this->t));
#endif
        set(GCRef<T>::ProtectedGetOtherRawPtr(other));
    }

    template<class T>
    REALLY_INLINE GCMemberBase<T>::~GCMemberBase()
    {
        T::WriteBarrier_dtor((void**)&(this->t));
    }

    template<class T>
    REALLY_INLINE GCMemberBase<T>& GCMemberBase<T>::operator=(const GCMemberBase& other)
    {
        set(GCRef<T>::ProtectedGetOtherRawPtr(other));
        return *this;
    }

    template<class T>
    template<class T2>
    REALLY_INLINE void GCMemberBase<T>::operator=(const GCRef<T2>& other) 
    {
        set(GCRef<T>::ProtectedGetOtherRawPtr(other));
    }	

    template<class T>
    REALLY_INLINE void GCMemberBase<T>::operator=(T* tNew)
    {
        set(tNew);
    }

    // -----------------------------------------
    // GCRoot::GCMember
    // -----------------------------------------
    
    template<class T>
    REALLY_INLINE void GCRoot::GCMember<T>::set(T* tNew)
    {
        T* tOld = NULL;
        if (valid())
        {
            tOld = this->t;
        }
        this->t = tNew;

        // Only call DecrementRef after installing new value to avoid
        // tripping up the DRC validator which doesn't like finding a
        // live zero count object its about to reap (in case tOld drops
        // to zero and a reap happens).
        if (tOld)
        {
            //  This is NOOP for GCObject and GCFinalizedObject
            tOld->DecrementRef();
        }
        if (valid())
        {
            //  This is NOOP for GCObject and GCFinalizedObject
            this->t->IncrementRef();
        }
    }
    
    template<class T>
    REALLY_INLINE bool GCRoot::GCMember<T>::valid()
    { 
        return (uintptr_t)(this->t) > 1; 
    }

    template<class T>
    REALLY_INLINE GCRoot::GCMember<T>::GCMember() 
        : GCRef<T>()
    { 
        GCAssert(!IsAddressOnStack(this));
    }

    template<class T>
    REALLY_INLINE GCRoot::GCMember<T>::GCMember(const GCMember<T>& other) 
        : GCRef<T>()
    { 
        GCAssert(!IsAddressOnStack(this));
        set(GCRef<T>::ProtectedGetOtherRawPtr(other));
    }

    template<class T>
    template<class T2>
    REALLY_INLINE GCRoot::GCMember<T>::GCMember(const GCRef<T2>& other) 
        : GCRef<T>()
    { 
        GCAssert(!IsAddressOnStack(this));
        set(GCRef<T>::ProtectedGetOtherRawPtr(other));
    }

    template<class T>
    REALLY_INLINE GCRoot::GCMember<T>::GCMember(T* valuePtr) 
        : GCRef<T>()
    { 
        GCAssert(!IsAddressOnStack(this));
        set(valuePtr);
    }

    template<class T>
    REALLY_INLINE GCRoot::GCMember<T>::~GCMember() 
    { 
        set(NULL);
    }
            
    template<class T>
    template<class T2>
    REALLY_INLINE void GCRoot::GCMember<T>::operator=(const GCRef<T2>& other) 
    { 
        set(GCRef<T>::ProtectedGetOtherRawPtr(other));
    }

    template<class T>
    REALLY_INLINE void GCRoot::GCMember<T>::operator=(T* tNew) 
    { 
        set(tNew);
    }

    template<class T>
    REALLY_INLINE GCRoot::GCMember<T>& GCRoot::GCMember<T>::operator=(const GCMember& other) 
    { 
        set(GCRef<T>::ProtectedGetOtherRawPtr(other));
        return *this;
    }

    // -----------------------------------------
    // GCInlineObject::GCMember
    // -----------------------------------------

    template<class T>
    template<class T2>
    REALLY_INLINE void GCInlineObject::GCMember<T>::operator=(const GCRef<T2>& other) 
    { 
        GCMemberBase<T>::operator=(other);
    }

    template<class T>
    REALLY_INLINE void GCInlineObject::GCMember<T>::operator=(T* tNew) 
    { 
        GCMemberBase<T>::operator=(tNew);
    }

    template<class T>
    REALLY_INLINE GCInlineObject::GCMember<T>::GCMember()
        : GCMemberBase<T>()
    { 
    }

    template<class T>
    template<class T2>
    REALLY_INLINE GCInlineObject::GCMember<T>::GCMember(const GCRef<T2>& other) 
        : GCMemberBase<T>(other)
    { 
    }

    template<class T>
    REALLY_INLINE GCInlineObject::GCMember<T>::GCMember(T* valuePtr) 
    { 
        GCMemberBase<T>::operator=(valuePtr);
    }

    template<class T>
    REALLY_INLINE GCInlineObject::GCMember<T>::GCMember(const GCMember<T>& other) 
        : GCMemberBase<T>(other)
    { 
    }

    // -----------------------------------------
    // GCObject::GCMember
    // -----------------------------------------

    template<class T>
    template<class T2>
    REALLY_INLINE void GCObject::GCMember<T>::operator=(const GCRef<T2>& other) 
    { 
        GCMemberBase<T>::operator=(other);
    }

    template<class T>
    REALLY_INLINE void GCObject::GCMember<T>::operator=(T* tNew) 
    { 
        GCMemberBase<T>::operator=(tNew);
    }

    template<class T>
    REALLY_INLINE GCObject::GCMember<T>::GCMember() 
    {
    }

    template<class T>
    template<class T2>
    REALLY_INLINE GCObject::GCMember<T>::GCMember(const GCRef<T2>& other) 
        : GCMemberBase<T>(other)
    { 
    }

    template<class T>
    REALLY_INLINE GCObject::GCMember<T>::GCMember(T* valuePtr) 
    { 
        GCMemberBase<T>::operator=(valuePtr);
    }

    template<class T>
    REALLY_INLINE GCObject::GCMember<T>::GCMember(const GCMember<T>& other) 
        : GCMemberBase<T>(other)
    { 
    }
    
    // -----------------------------------------
    // GCTraceableBase::GCMember
    // -----------------------------------------

    template<class T>
    template<class T2>
    REALLY_INLINE void GCTraceableBase::GCMember<T>::operator=(const GCRef<T2>& other) 
    { 
        GCMemberBase<T>::operator=(other);
    }

    template<class T>
    REALLY_INLINE void GCTraceableBase::GCMember<T>::operator=(T* tNew) 
    { 
        GCMemberBase<T>::operator=(tNew);
    }

    template<class T>
    REALLY_INLINE GCTraceableBase::GCMember<T>::GCMember() 
        : GCMemberBase<T>()
    { 
    }

    template<class T>
    template<class T2>
    REALLY_INLINE GCTraceableBase::GCMember<T>::GCMember(const GCRef<T2>& other) 
        : GCMemberBase<T>(other)
    { 
    }

    template<class T>
    REALLY_INLINE GCTraceableBase::GCMember<T>::GCMember(T* valuePtr) 
    { 
        GCMemberBase<T>::operator=(valuePtr);
    }

    template<class T>
    REALLY_INLINE GCTraceableBase::GCMember<T>::GCMember(const GCMember<T>& other) 
        : GCMemberBase<T>(other)
    { 
    }
}

#endif /* __GCMember_inlines__ */

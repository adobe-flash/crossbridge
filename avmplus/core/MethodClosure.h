/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_MethodClosure__
#define __avmplus_MethodClosure__


namespace avmplus
{
    class GC_AS3_EXACT(MethodClosureClass, ClassClosure)
    {
        MethodClosureClass(VTable* cvtable);

    public:
        REALLY_INLINE static MethodClosureClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) MethodClosureClass(cvtable);
        }

        // overrides ClassClosure::call
        Atom call(int argc, Atom* argv);

        MethodClosure* create(MethodEnv* env, Atom savedThis);
        
    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(MethodClosureClass)

        DECLARE_SLOTS_MethodClosureClass;
    // ------------------------ DATA SECTION END
    };

    /**
     * The MethodClosure class is used to represent method
     * closures; that is, a function which is a method of
     * a class.
     *
     * MethodClosure is how AVM+ supports the "method extraction"
     * behavior of ECMAScript Edition 4.  When a reference to
     * a method of a class is retrieved, a MethodClosure is
     * created.  The MethodClosure also remembers the object
     * instance that the method was extracted from.  When the
     * MethodClosure is invoked, the method is invoked with
     * "this" pointing to the remembered instance.
     */
    class GC_AS3_EXACT(MethodClosure, FunctionObject)
    {
    protected:
        REALLY_INLINE MethodClosure(VTable* cvtable, MethodEnv* call, Atom savedThis)
            : FunctionObject(cvtable, call)
            , m_savedThis(savedThis) // can be nullAtom from WeakMethodClosure
        {
            AvmAssert(traits()->getSizeOfInstance() == sizeof(MethodClosure));
            m_call_ptr = (FunctionProc) MethodClosure::callMethodClosure;
        }

    public:
        REALLY_INLINE static MethodClosure* create(MMgc::GC* gc, VTable* vtable, MethodEnv* call, Atom savedThis)
        {
            return new (gc, MMgc::kExact, vtable->getExtraSize()) MethodClosure(vtable, call, savedThis);
        }

        /**
         * Implements OP_call and ScriptObject::call for concrete MethodClosures.  Can be
         * invoked directly by jit code or by FunctionObject::call; do not override the latter.
         */
        static Atom callMethodClosure(MethodClosure*, int argc, Atom* argv);

        // ScriptObject method overrides
        virtual MethodClosure* toMethodClosure() const;
#ifdef AVMPLUS_VERBOSE
        virtual PrintWriter& print(PrintWriter& prw) const;
#endif

        // MethodClosure-specific methods
#ifdef _DEBUG
        virtual bool isWeak() const;
#endif
        virtual bool isValid() const;
        virtual MethodClosure* weaken() const;
        virtual Atom get_savedThis() const;
        virtual Atom get_savedThisOrNull() const; // returns nullAtom if invalid, no assert

        bool equals(const MethodClosure* that) const;
        uintptr_t hashKey() const;

    protected:
        // called by Function.AS3_call/apply
        virtual Atom get_coerced_receiver(Atom a) const;

    protected:
        GC_DATA_BEGIN(MethodClosure)
        
        // Note: this field is not used by the Weak subclass
        ATOM_WB GC_ATOM(m_savedThis); // can be null (e.g. WeakMethodClosure)
        
        GC_DATA_END(MethodClosure)

        DECLARE_SLOTS_MethodClosure;
    };

    // Note that this class doesn't have an AS3 declaration; from AS3 it is indistinguishable
    // from a normal MethodClosure. (In fact, this is currently never even exposed to AS3
    // code; it's used internally.) The only way to obtain one is to call weaken() on a normal
    // MethodClosure.
    class GC_CPP_EXACT(WeakMethodClosure, MethodClosure)
    {
        friend class MethodClosure;

    protected:
        REALLY_INLINE WeakMethodClosure(VTable* cvtable, MethodEnv* call, Atom savedThis)
            // Bugzilla 500538, 767410: do NOT pass savedThis nor the
            // call MethodEnv to the superclass ctor!
            : MethodClosure(cvtable, NULL, nullObjectAtom)
            , m_weakCallEnv(call->GetWeakRef())
            , m_weakSavedThis(AvmCore::atomToScriptObject(savedThis)->GetWeakRef())
        {
            AvmAssert(m_weakSavedThis != NULL);
            m_call_ptr = (FunctionProc) callWeakMethodClosure;
        }
 
        REALLY_INLINE static WeakMethodClosure* create(MMgc::GC* gc, VTable* vtable, MethodEnv* call, Atom savedThis)
        {
            return new (gc, MMgc::kExact, vtable->getExtraSize()) WeakMethodClosure(vtable, call, savedThis);
        }

    public:
        /**
         * Implements OP_call and ScriptObject::call for concrete MethodClosures.  Can be
         * invoked directly by jit code or by FunctionObject::call; do not override the latter.
         */
        static Atom callWeakMethodClosure(WeakMethodClosure*, int argc, Atom* argv);

    public:
        // MethodClosure-specific methods
#ifdef _DEBUG
        virtual bool isWeak() const;
#endif
        virtual bool isValid() const;
        virtual MethodClosure* weaken() const;
        virtual Atom get_savedThis() const;
        virtual Atom get_savedThisOrNull() const; // returns nullAtom if invalid, no assert
 
     protected:
        // called by Function.AS3_call/apply
        virtual Atom get_coerced_receiver(Atom a) const;

        virtual GCRef<MethodEnv> get_callEnv() const;
    
    private:
        GCRef<MethodEnv> _get_callEnv() const;
        Atom _get_savedThis() const;

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(WeakMethodClosure)

    private:
        GCMember<MMgc::GCWeakRef> GC_POINTER(m_weakCallEnv);    // WeakRef to a MethodEnv*
        GCMember<MMgc::GCWeakRef> GC_POINTER(m_weakSavedThis);  // WeakRef to a ScriptObject*

        GC_DATA_END(WeakMethodClosure)

    private:
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_MethodClosure__ */

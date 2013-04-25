/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_FunctionClass__
#define __avmplus_FunctionClass__


namespace avmplus
{
    /**
     * class Function
     */
    class GC_AS3_EXACT(FunctionClass, ClassClosure)
    {
    protected:
        FunctionClass(VTable* cvtable);
    public:
        REALLY_INLINE static FunctionClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) FunctionClass(cvtable);
        }

        Atom call(int argc, Atom* argv)
        {
            return construct(argc,argv);
        }

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(FunctionClass)

        DECLARE_SLOTS_FunctionClass;
    // ------------------------ DATA SECTION END
    };

    class GC_AS3_EXACT(FunctionObject, ClassClosure)
    {
    protected:
        FunctionObject(VTable* cvtable, MethodEnv* call);

    public:
        REALLY_INLINE static FunctionObject* create(MMgc::GC* gc, VTable* vtable, MethodEnv* call)
        {
            return new (gc, MMgc::kExact, vtable->getExtraSize()) FunctionObject(vtable, call);
        }

        /** Implements ScriptObject::call called directly via ptr */
        static Atom callFunction(FunctionObject*, int argc, Atom* argv);

        // AS3 native methods
        int32_t get_length();
        Atom AS3_call(Atom thisAtom, Atom *argv, int argc);
        Atom AS3_apply(Atom thisAtom, Atom argArray);

        // ScriptObject method overrides
#if defined(DEBUGGER) || defined(VMCFG_AOT)
        virtual MethodEnv* getCallMethodEnv();
#endif
        virtual Atom call(int argc, Atom* argv); // overrides ClassClosure.call, ScriptObject.call
        virtual CodeContext* getFunctionCodeContext() const;
        virtual Stringp implToString() const;
    protected:
        virtual Atom get_coerced_receiver(Atom a) const; // called by AS3_call/apply
    protected:
        // returns MethodEnv to use for call, or null if none present.
        // (see e.g. WeakMethodClosure for example overrides.)
        virtual GCRef<MethodEnv> get_callEnv() const { return m_callEnv; }
    private:
        /** Implements get_coerced_receiver specifically for concrete FunctionObject instances */
        Atom getFunctionReceiver(Atom a) const;

    // ------------------------ DATA SECTION BEGIN
    public:
        FunctionProc m_call_ptr; // stub to directly invoke this function.

    protected:
        GC_DATA_BEGIN(FunctionObject)
    private:
        GCMember<MethodEnv> GC_POINTER(m_callEnv); // can be null (e.g. WeakMethodClosure)
        GC_DATA_END(FunctionObject)

    private:
        DECLARE_SLOTS_FunctionObject;
    // ------------------------ DATA SECTION END
    };
}

namespace avm {
using avmplus::FunctionObject;
using avmplus::VTable;
using avmplus::ScriptObject;

/**
 * A HostFunctionObject is the base class for all host-supplied classes that
 * extend AS3 class Function.  Their C++ class must extend FunctionObject via this wrapper.
 */
class HostFunctionObject: public FunctionObject {
public:
    HostFunctionObject(VTable* vtable, ScriptObject* delegate);

    /**
     * callHostFunction() intercepts calls via m_call_ptr, then delegates back to the
     * HostFunctionObject::call virtual function, because HostFunctionObjects do not use
     * the FunctionProc / m_call_ptr protocol.
     */
    static Atom callHostFunction(HostFunctionObject* f, int argc, Atom* args);
};


}

#endif /* __avmplus_FunctionClass__ */

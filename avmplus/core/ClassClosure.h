/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ClassClosure__
#define __avmplus_ClassClosure__


namespace avmplus
{
    /**
     * a user defined class, ie class MyClass
     */
    class GC_AS3_EXACT(ClassClosure, ScriptObject)
    {
        friend class FunctionObject;
        friend class VTable;

    private:
        // This ctor is for use only by FunctionObject + MethodClosure
        // (and createClassClosure)
        ClassClosure(VTable* cvtable, CreateInstanceProc createInstanceProc);

    protected:
        // This ctor is used by all other (normal) subclasses
        ClassClosure(VTable* cvtable);

    public:

        // All ClassClosures get a default "constructObject" available
        // to them with no args; which does what you might expect: attempt
        // to call the ctor with no args. (If the ctor has nonoptional args
        // then an exception will be thrown.) This is useful in various places
        // in Flash, which expect subclasses of (e.g.) Font to be declared
        // with zero-arg constructors, and allows us to avoid exposing
        // naked calls to "construct()" with argc/argv usage.
        //
        // NOTE that this is *not* a virtual call.
        //
        // Note that this will never return NULL: if the constructor
        // returns null (or a non-object atom), we throw kCantInstantiateError instead.
        GCRef<ScriptObject> constructObject();

        static ClassClosure* FASTCALL createClassClosure(VTable* cvtable);
        
        Atom get_prototype();
        void set_prototype(Atom p);

        ScriptObject* prototypePtr();
        void FASTCALL setPrototypePtr(ScriptObject* p);

        void createVanillaPrototype();

        ScriptObject* newInstance();

        /**
         * called as function, as in C().  For user classes, this is the
         * the explicit coersion function.  For user functions, we
         * invoke m_call.
         *
         * NOTE: The contents of the given argument list can be modified during
         *       invocation of the function without further warning.
         *       Do not reuse an argument list AS-IS for multiple function calls,
         *       unless you make sure to reinitialize the contents of the argument list after each call.
         */
        virtual Atom call(int argc, Atom* argv);

        VTable* ivtable() const;

        /**
         * throwError is a convenience function for throwing
         * an exception with a formatted error message,
         * printf-style
         */
        void throwError(int errorID, Stringp arg1=0, Stringp arg2=0, Stringp arg3=0);

#ifdef DEBUGGER
        virtual uint64_t bytesUsed() const;
#endif

        virtual Stringp implToString() const;

        virtual ClassClosure* toClassClosure() { return this; }

        size_t getExtraSize() const;

        void initPrototypeConstructor();

    private:
        static CreateInstanceProc FASTCALL checkForRestrictedInheritance(VTable* ivtable, CreateInstanceProc p);

    protected:
        static CreateInstanceProc FASTCALL calcCreateInstanceProc(VTable* cvtable);

    protected:
        Atom construct_native(CreateInstanceProc ciproc, int argc, Atom* argv);

        bool FASTCALL isTypeImpl(Atom value);
        Atom FASTCALL asTypeImpl(Atom value);
        Atom FASTCALL coerceToTypeImpl(Atom value);

    public:
        static ScriptObject* FASTCALL createScriptObjectProc(ClassClosure* cls);
        static ScriptObject* FASTCALL cantInstantiateCreateInstanceProc(ClassClosure* cls);
        static ScriptObject* FASTCALL impossibleCreateInstanceProc(ClassClosure* cls);

    private:
        static ScriptObject* FASTCALL reinitNullPrototypeCreateInstanceProc(ClassClosure* cls);

#ifdef AVMPLUS_VERBOSE
    public:
        PrintWriter& print(PrintWriter& prw) const;
#endif
    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(ClassClosure)

    private:
        GCMember<ScriptObject>      GC_POINTER(m_prototype);
        CreateInstanceProc const    m_createInstanceProc;   // not GC memory

        GC_DATA_END(ClassClosure)

        DECLARE_SLOTS_ClassClosure;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_ClassClosure__ */

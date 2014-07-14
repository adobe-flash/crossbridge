/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_SystemClass__
#define __avmshell_SystemClass__


namespace avmshell
{
    class GC_AS3_EXACT(AbstractBaseClass, avmplus::ClassClosure)
    {
    public:
        AbstractBaseClass(avmplus::VTable* cvtable) : ClassClosure(cvtable) {}
        
        GC_NO_DATA(AbstractBaseClass)

        DECLARE_SLOTS_AbstractBaseClass;
    };

    class GC_AS3_EXACT(AbstractBaseObject, avmplus::ScriptObject)
    {
    public:
        AbstractBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_NO_DATA(AbstractBaseObject)

        DECLARE_SLOTS_AbstractBaseObject;
    };

    class GC_AS3_EXACT(NativeSubclassOfAbstractBaseClass, avmplus::ClassClosure)
    {
    public:
        NativeSubclassOfAbstractBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(NativeSubclassOfAbstractBaseClass)

        DECLARE_SLOTS_NativeSubclassOfAbstractBaseClass;
    };

    class GC_AS3_EXACT(NativeSubclassOfAbstractBaseObject, AbstractBaseObject)
    {
    public:
        NativeSubclassOfAbstractBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : AbstractBaseObject(vtable, prototype) {}

        GC_NO_DATA(NativeSubclassOfAbstractBaseObject)

        DECLARE_SLOTS_NativeSubclassOfAbstractBaseObject;
    };

    class GC_AS3_EXACT(RestrictedBaseClass, avmplus::ClassClosure)
    {
    public:
        RestrictedBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(RestrictedBaseClass)

        DECLARE_SLOTS_RestrictedBaseClass;
    };

    class GC_AS3_EXACT(RestrictedBaseObject, avmplus::ScriptObject)
    {
    public:
        RestrictedBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_NO_DATA(RestrictedBaseObject)

        DECLARE_SLOTS_RestrictedBaseObject;
    };

    class GC_AS3_EXACT(NativeSubclassOfRestrictedBaseClass, avmplus::ClassClosure)
    {
    public:
        NativeSubclassOfRestrictedBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(NativeSubclassOfRestrictedBaseClass)

        DECLARE_SLOTS_NativeSubclassOfRestrictedBaseClass;
    };

    class GC_AS3_EXACT(NativeSubclassOfRestrictedBaseObject, RestrictedBaseObject)
    {
    public:
        NativeSubclassOfRestrictedBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : RestrictedBaseObject(vtable, prototype) {}

        GC_NO_DATA(NativeSubclassOfRestrictedBaseObject)

        DECLARE_SLOTS_NativeSubclassOfRestrictedBaseObject;
    };

    class GC_AS3_EXACT(AbstractRestrictedBaseClass, avmplus::ClassClosure)
    {
    public:
        AbstractRestrictedBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(AbstractRestrictedBaseClass)

        DECLARE_SLOTS_AbstractRestrictedBaseClass;
    };

    class GC_AS3_EXACT(AbstractRestrictedBaseObject, avmplus::ScriptObject)
    {
    public:
        AbstractRestrictedBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_NO_DATA(AbstractRestrictedBaseObject)

        DECLARE_SLOTS_AbstractRestrictedBaseObject;
    };

    class GC_AS3_EXACT(NativeSubclassOfAbstractRestrictedBaseClass, avmplus::ClassClosure)
    {
    public:
        NativeSubclassOfAbstractRestrictedBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(NativeSubclassOfAbstractRestrictedBaseClass)

        DECLARE_SLOTS_NativeSubclassOfAbstractRestrictedBaseClass;
    };

    class GC_AS3_EXACT(NativeSubclassOfAbstractRestrictedBaseObject, AbstractRestrictedBaseObject)
    {
    public:
        NativeSubclassOfAbstractRestrictedBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : AbstractRestrictedBaseObject(vtable, prototype) {}

        GC_NO_DATA(NativeSubclassOfAbstractRestrictedBaseObject)

        DECLARE_SLOTS_NativeSubclassOfAbstractRestrictedBaseObject;
    };

    // this class exists solely to test native classes that use MI.
    class MIClass : public avmplus::ClassClosure
    {
    public:
        MIClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}
        ~MIClass() {}

        DECLARE_SLOTS_MIClass;
    };

    // this class exists solely to test native classes that use MI.
    class MixinClassThatDoesNotDescendFromScriptObject
    {
    public:
        const double factor;
        MixinClassThatDoesNotDescendFromScriptObject(double f) : factor(f) {}
        // evil, wrong version that we DO NOT WANT
        double plus(double v) { return v * factor; }
    };

    // this class exists solely to test native classes that use MI.
    class MIObjectImpl : public avmplus::ScriptObject
    {
    public:
        const double amount;
        MIObjectImpl(avmplus::VTable* vtable, avmplus::ScriptObject* prototype, double a) : avmplus::ScriptObject(vtable, prototype), amount(a) {}
        double plus(double v) { return v + amount; }
    };

    class GC_AS3_EXACT(CheckBaseClass, avmplus::ClassClosure)
    {
    public:
        CheckBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(CheckBaseClass)

        DECLARE_SLOTS_CheckBaseClass;
    };

    class GC_AS3_EXACT(CheckBaseObject, avmplus::ScriptObject)
    {
    public:
        CheckBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_NO_DATA(CheckBaseObject)

        DECLARE_SLOTS_CheckBaseObject;
    };

    class GC_AS3_EXACT(NativeBaseClass, avmplus::ClassClosure)
    {
    public:
        NativeBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(NativeBaseClass)

        DECLARE_SLOTS_NativeBaseClass;
    };

    class GC_AS3_EXACT(NativeBaseObject, avmplus::ScriptObject)
    {
    public:
        NativeBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_DATA_BEGIN(NativeBaseObject)
        char nine_bytes[9]; // deliberately ending on a misaligned boundary
        GC_DATA_END(NativeBaseObject)
        
        DECLARE_SLOTS_NativeBaseObject;
    };

    // this class exists solely to test native classes that use MI.
    class MIObject : public MIObjectImpl, public MixinClassThatDoesNotDescendFromScriptObject
    {
    public:
        MIObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : MIObjectImpl(vtable, prototype, 1), MixinClassThatDoesNotDescendFromScriptObject(2) {}
        ~MIObject() {}

        DECLARE_SLOTS_MIObject;
    };

    /**
     * A simple class that has some native methods.
     * Included as an example for writers of native methods,
     * and also to provide some useful QA instrumentation.
     */
    class GC_AS3_EXACT(SystemClass, avmplus::ClassClosure)
    {
        uint64_t initialTime;

        SystemClass(avmplus::VTable* cvtable);
    public:
        REALLY_INLINE static SystemClass* create(MMgc::GC* gc, avmplus::VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) SystemClass(cvtable);
        }

        ~SystemClass();

        // set by shell
        static int user_argc;
        static char **user_argv;

        /**
         * Implementation of System.exit
         * AS usage: System.exit(status);
         * Exits the VM with OS exit code specified by  status.
         */
        void exit(int status);

        /**
         * Implementation of System.sleep.
         */
        void sleep(int ms);

        /**
         * Implementation of System.getAvmplusVersion
         * AS usage: System.getAvmplusVersion();
         * Returns the current version of AVM+ in the form
         * "1.0 d100"
         */
        avmplus::Stringp getAvmplusVersion();
        
        /**
         * Implementation of System.getFeatures
         * AS usage: System.getFeatures();
         * Returns the compiled in features of AVM+
         */
        avmplus::Stringp getFeatures();
        
        /**
         * Implementation of System.getRunmode
         * AS usage: System.getRunmode();
         * Returns the current runmode
         */
        avmplus::Stringp getRunmode();

        /**
         * Implementation of System.exec
         * AS usage: exitCode = System.exec("command");
         * Executes the specified command line and returns
         * the status code
         */
        int exec(avmplus::Stringp command);

        void trace(avmplus::ArrayObject* a);
        void write(avmplus::Stringp s);


        /**
         * @name Debugging Extensions
         */
        /*@{*/
        void debugger();
        bool isDebugger();
        /*@}*/

                /**
         * @name ActionScript Extensions
         * ActionScript extensions to ECMAScript
         */
        /*@{*/
        double getNanosecondTimer();
        unsigned getTimer();
        /*@}*/

        avmplus::ArrayObject * getArgv();
        avmplus::ArrayObject * getEnviron();

        avmplus::Stringp readLine();

        double get_totalMemory();
        double get_freeMemory();
        double get_privateMemory();

        int32_t get_swfVersion();
        int32_t get_apiVersion();

        // Initiate a garbage collection; future versions will not return before completed.
        void forceFullCollection();

        // Queue a garbage collection request.
        void queueCollection();

        // function exists solely to test native-methods with custom namespaces
        void ns_example_nstest() { }

        // function exists solely to test ScriptObject::isGlobalObject
        bool isGlobal(avmplus::Atom o);

        void disposeXML(avmplus::XMLObject *xmlObject);

        void pauseForGCIfCollectionImminent(double imminence);

        // Support for VM tests that depend on representation of numeric atoms.
        // These are provided solely for testing purposes, as the semantics of an AS3 program
        // should not depend on the word size of the platform nor the internal encoding of atoms.

        // Return true if running on a 64-bit platform.
        bool is64bit();

        // Return true if the argument is an atom with tag kIntptrType.
        bool isIntptr(avmplus::Atom a);

        // Return argument value as a kIntptrType atom if the argument is a numeric
        // value that can be so represented, else return the argument unchanged.
        avmplus::Atom canonicalizeNumber(avmplus::Atom a);

        // DEOPT: Diagnostic scaffolding.
        void deopt(int32_t k);
        void runInSafepoint(avmplus::FunctionObject* closure);

        GC_NO_DATA(SystemClass)

        DECLARE_SLOTS_SystemClass;
    };
}

#endif /* __avmshell_SystemClass__ */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ScriptObject__
#define __avmplus_ScriptObject__


namespace avmplus
{
#define avmplus_ScriptObject_isExactInterlock 1

    /**
     * one actionscript object.  might or might not be a function.
     * Base class for all objects visible to script code.
     */
    class ScriptObject : public AvmPlusScriptableObject
    {
    protected:
        ScriptObject(VTable* vtable, ScriptObject* delegate);
        ScriptObject(VTable* vtable, ScriptObject* delegate, int htCapacity);
    public:
        REALLY_INLINE static ScriptObject* create(MMgc::GC* gc, VTable* vtable, ScriptObject* delegate);
        REALLY_INLINE static ScriptObject* create(MMgc::GC* gc, VTable* vtable, ScriptObject* delegate, int htCapacity);

        // Not actually entirely trivial, as it decrements reference counts
        // manually, not just by GCMember smart pointer destructors.
#ifdef DRC_TRIVIAL_DESTRUCTOR
        ~ScriptObject();
#endif

        virtual bool gcTrace(MMgc::GC* gc, size_t cursor);

        ScriptObject* getDelegate() const;
        void setDelegate(ScriptObject *d);
        Atom atom() const;
        virtual Atom toAtom() const;
        Traits* traits() const;
        AvmCore* core() const;
        MMgc::GC* gc() const;
        Toplevel* toplevel() const;
        /**
         * Returns a pointer to hashtable to use for dynamic
         * properties, this is a derived pointer and can never be
         * NULL.  This method should not be called until the full
         * construction sequence (C++ and AS3) is complete and will
         * assert if called on a Dictionary before its init method has
         * run.
         */
        InlineHashtable* getTable() const;
        /**
         * Like getTable but doesn't init the table if its empty, and
         * will return NULL and not assert on a yet to be initialized
         * Dictionary.
         */
        InlineHashtable* getTableNoInit() const;

        Atom getSlotAtom(uint32_t slot, AvmCore *core);

        Atom getSlotAtom(uint32_t slot);

        // like getSlotAtom, but assume the resulting Atom is a ScriptObject...
        // if it is not, return NULL. useful for callers that expect only
        // a ScriptObject and reject other types (eg callproperty). If the
        // slot in question is usually a ScriptObject, it's substantially faster
        // to call this (vs getSlotAtom).
        ScriptObject* getSlotObject(uint32_t slot);

        // NOTE, this now does the equivalent of Toplevel::coerce() internally;
        // it is not necessary to call coerce() prior to calling this!
        void coerceAndSetSlotAtom(uint32_t slot, Atom atom);

        virtual Atom getDescendants(const Multiname* name) const;

        // argv[0] = receiver
        virtual Atom callProperty(const Multiname* name, int argc, Atom* argv);

        // common set/set/has/delete/etc virtual methods renamed to explicitly name the expected arg types,
        // to avoid potentially hidden virtual functions
        virtual Atom getAtomProperty(Atom name) const;
        virtual void setAtomProperty(Atom name, Atom value);
        virtual bool deleteAtomProperty(Atom name);
        virtual bool hasAtomProperty(Atom name) const;
        virtual bool getAtomPropertyIsEnumerable(Atom name) const;
        virtual void setAtomPropertyIsEnumerable(Atom name, bool enumerable);

        virtual Atom getMultinameProperty(const Multiname* name) const;
        virtual void setMultinameProperty(const Multiname* name, Atom value);
        virtual bool deleteMultinameProperty(const Multiname* name);
        virtual bool hasMultinameProperty(const Multiname* name) const;

        virtual Atom getUintProperty(uint32_t i) const;
        virtual void setUintProperty(uint32_t i, Atom value);
        virtual bool delUintProperty(uint32_t i);
        virtual bool hasUintProperty(uint32_t i) const;

        // convenience wrappers for passing Stringp instead of Atom
        // inline, not virtual (should never need overriding)
        Atom getStringProperty(Stringp name) const;
        Atom getStringPropertyFromProtoChain(Stringp name, ScriptObject* protochain, Traits *origObjTraits) const;
        void setStringProperty(Stringp name, Atom value);
        bool deleteStringProperty(Stringp name);
        bool hasStringProperty(Stringp name) const;
        bool getStringPropertyIsEnumerable(Stringp name) const;
        void setStringPropertyIsEnumerable(Stringp name, bool enumerable);

        virtual Atom defaultValue();        // ECMA [[DefaultValue]]
        virtual Stringp toString();

        /**
         * Implmenets the behavior of this object when called as a function.
         * This function is only intended to be called by the VM, not from
         * C++ host code.  For the latter, use one of the avm::callFunction()
         * wrappers.
         *
         * argv[0] is not counted in argc and is the receiver (this) parameter
         * argc[1+] are the normal arguments
         */
        virtual Atom call(int argc, Atom* argv);

        /**
         * argv[0] = receiver(ignored)
         * arg1 = argv[1]
         * argN = argv[argc]
         *
         * called as constructor, as in new C().  for user classes this
         * invokes the implicit constructor followed by the user's constructor
         * if any.
         *
         * NOTE: The contents of the given argument list can be modified during
         *       invocation of the constructor without further warning.
         *       Do not reuse an argument list AS-IS for multiple constructor calls,
         *       unless you make sure to reinitialize the contents of the argument list after each call.
         *
         * NOTE: subclasses should never need to declare this method in their class;
         *       an override declaration will be provided for them iff
         *       construct="override" or construct="instance" is specified in the AS3 file.
         */
        virtual Atom construct(int argc, Atom* argv);

        // TODO make this const
        virtual Atom nextName(int index);

        // TODO make this const
        virtual Atom nextValue(int index);

        // TODO make this const
        virtual int nextNameIndex(int index);

        virtual Atom applyTypeArgs(int argc, Atom* argv);

        // This is a fast way to implement "istype(MethodClosure)"; it returns non-null if
        // this is an MethodClosure (or subclass thereof)
        virtual MethodClosure* toMethodClosure() const;
        bool isMethodClosure() const;

        // The maximum integer key we can use with our ScriptObject
        // HashTable must fit within 28 bits.  Any integer larger
        // than 28 bits will use a string key.
        static const uint32_t MAX_INTEGER_MASK = 0xF0000000;

        // return true iff the object is a toplevel script init object.
        bool isGlobalObject() const;

        virtual Stringp implToString() const;

        // this really shouldn't exist here, but AIR currently plays some games with subclassing
        // "Function" that use C++ classes that don't descend from FunctionObject. Easier to fix
        // by rooting this method here than by fixing AIR at this time.
        virtual CodeContext* getFunctionCodeContext() const;

        // This is a fast way to implement "istype(ARRAY)"; it returns non-null if
        // this is an ArrayObject (or subclass thereof)
        virtual ArrayObject* toArrayObject();

        // This gets the property named "length" (if any) on the object, coerces it to a uint,
        // and returns the value. (Since uint(undefined) == 0, undefined length is zero.)
        virtual uint32_t getLengthProperty();

        // This sets the property named "length" to the given uint value. If this
        // is a nondynamic object, or there is a read-only length property, or uint
        // can't be coerced to the type of "length", an exception will be thrown.
        virtual void setLengthProperty(uint32_t newLen);

        virtual ClassClosure* toClassClosure();

#ifdef AVMPLUS_VERBOSE
    public:
        virtual PrintWriter& print(PrintWriter& prw) const;
#endif

#ifdef DEBUGGER
    public:
        virtual uint64_t bytesUsed() const;
#endif
#if defined(DEBUGGER) || defined(VMCFG_AOT)
    public:
        virtual MethodEnv* getCallMethodEnv();
#endif
	
    protected:
        
        Atom getAtomPropertyFromProtoChain(Atom name, ScriptObject* protochain, Traits *origObjTraits) const;

        // If name defined in this (not its prototype chain), sets
        // *recv to this[name] and returns true.  Else returns false.
        // (Assumes this is dynamic; do not call on sealed instances.)
        virtual bool isOwnAtomPropertyHere(Atom name, Atom *recv) const;

        void throwWriteSealedError(Atom name);
        void throwWriteSealedError(const Multiname& name);
    
    public:
        void throwCantInstantiateError();

    private:
        void initHashtable(int capacity = InlineHashtable::kDefaultCapacity);

    // ------------------------ DATA SECTION BEGIN
    public:     VTable* const           vtable;
    private:    GCMember<ScriptObject>  delegate;     // __proto__ in AS2, archetype in semantics
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_ScriptObject__ */

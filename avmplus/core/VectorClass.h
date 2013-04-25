/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_VectorClass__
#define __avmplus_VectorClass__

/* Documentation on the structure of the Vector code in Tamarin (2011-10-20).
 *
 *
 * Five components: AS3, ABC, ASC, verifier, runtime.  These interact in various ways and a
 * linear narrative is not possible.
 *
 *
 * On the AS3 side we have this (core/Vector.as, core/VectorImpl.as):
 *
 *   package __AS3__.vec 
 *   {
 *     class Vector ...
 *     class Vector$int ...
 *     class Vector$uint ...
 *     class Vector$double ...
 *     class Vector$object ...
 *   }
 *
 * The namespace "__AS3__.vec" is an artifact of a time when we did not have good API
 * versioning and could not introduce new top-level names without the risk of breaking
 * existing code.  Today we would probably have made "Vector" public & versioned.
 *
 * "Vector" is public in its package but cannot be instantiated (a run-time error is thrown)
 * and has no members.  It is possible to capture its value and use it as part of a type 
 * application, eg, "var v=Vector; new v.<int>" will create a Vector.<int>.  It's also 
 * referenced in code using the APPLYTYPE instruction and CONSTANT_TypeName multiname kind (below).
 *
 * Vector$int, Vector$uint, Vector$double, and Vector$object are not public in their package.
 * They represent Vector.<int>, Vector.<uint>, Vector.<Number>, and Vector.<*> respectively;
 * the first three special cases are provided in order to specialize the representation
 * and the last is a catch-all.
 *
 * The Vector$int etc bindings are artifacts of an older design and are kept around for the
 * sake of backward compatibility with existing bytecode, which referenced those names
 * directly (see ASC notes below).
 *
 * The object resulting from the expression "Vector.<int>" is the same object as the Vector$int
 * object in the __AS3__.vec package, ditto for the other three base types.  Note carefully
 * that Vector$object maps to Vector.<*> and that Vector.<Object> is different.  (Vector$object
 * should really have been called Vector$any.)
 *
 * For other base types than int, uint, Number, and * the VM synthesizes new Vector classes at
 * run-time.  The new classes are effectively clones of Vector$object, but their class objects
 * are distinct from the object held in Vector$object.
 *
 *
 * ASC:
 *
 * If AS3 code references the name "Vector" and Vector is not bound lexically in the compilation
 * unit then ASC will generate code that opens the namespace "__AS3__.vec" so that the "Vector"
 * binding becomes available.
 *
 * The current ASC - unlike older ASC - does not reference the Vector$int etc names directly,
 * instead it emits the APPLYTYPE instruction and the CONSTANT_TypeName multiname kind to
 * reference parameterized types in expression and type contexts, respectively.
 *
 * 
 * ABC and verification:
 *
 * The ABC instruction APPLYTYPE creates a class object for a type application T0.<T1,...>.
 * It's a general instruction.  APPLYTYPE is effectively implemented as an a call to the 
 * applyTypeArgs() virtual on avmplus::ScriptObject.
 *
 * APPLYTYPE does not have any interesting verification constraints, all the checking is
 * done at runtime (see below); basically the factory must be Vector and there must be one
 * type argument.
 *
 * The ABC multiname kind CONSTANT_TypeName references an instantiated type and comprises a
 * base type (a factory), an argument count, and a number of arguments, all of which must be
 * multinames.
 *
 * The AbcParser checks that there is exactly one type parameter for CONSTANT_TypeName, this
 * simplifies the representation of Multiname as it only has to support the special case.
 * This constraint can be lifted later without invalidating existing code or needing to introduce
 * a new instruction for the general case.
 *
 * CONSTANT_TypeName names will eventually end up being passed through PoolObject::resolveParameterizedType,
 * which will fail (return NULL) if the base type is not Vector.  That will in turn lead to a
 * verification error, at least in type contexts (called from resolveTypeName).
 *
 * In sum, only Vector.<T> for exactly one T is supported at this time through a combination
 * of checks.
 *
 * 
 * Runtime.
 * 
 * APPLYTYPE ends up in VectorClass::applyTypeArgs(), which checks that there's one parameter 
 * and that it is NULL (representing *) or a normal class object and fails otherwise with
 * a VerifyError (that is a weird error but it's a fact of life at this point).  APPLYTYPE
 * then delegates to VectorClass::getTypedVectorClass().
 *
 * When Vector is instantiated in VectorClass::getTypedVectorClass() and a new Vector type must
 * be created then the new class is stored in a table in the Domain of the factory, the key
 * to the table is the string "Vector.<T>" where "T" is the full formatted name of the
 * parameter class.  The table is used by getTypedVectorClass() to ensure that only one type is
 * instantiated per class parameter.  getTypedVectorClass() has special cases for base types
 * int, uint, Number, and *: in those cases the built-in classes Vector$int, etc are returned.
 *
 * Issues with getTypedVectorClass: The string-based mechanism feels a little clunky, and it's not
 * obvious that a table in Domain is correct, it should probably be in DomainEnv.
 *
 * CONSTANT_TypeName triggers invocations of PoolObject::resolveParameterizedType(), which must
 * create Traits objects corresponding to the parameterized types.  These are stored with the
 * "loaded" traits in the Domain, again the key is the string "Vector.<T>" where "T" is the full
 * formatted name of the parameter class, and again there are special cases for instantiations
 * corresponding to the built-in Vector instantiations Vector$int etc.
 *
 *
 * On the C++ side we have these two hierarchies:
 *
 *   ClassClosure
 *     VectorClass
 *       TypedVectorClassBase
 *         TypedVectorClass<T>
 *           IntVectorClass = TypedVectorClass<IntVectorObject>
 *           UIntVectorClass = TypedVectorClass<UintVectorObject>
 *           DoubleVectorClass = TypedVectorClass<DoubleVectorObject>
 *           ObjectVectorClass = TypedVectorClass<ObjectVectorObject>
 *
 *    ScriptObject
 *      VectorBaseObject
 *        TypedVectorObject<T>
 *          IntVectorObject = TypedVectorObject< DataList<int32_t> >
 *          UIntVectorObject = TypedVectorObject< DataList<uint32_t> >
 *          DoubleVectorObject = TypedVectorObject< DataList<double> >
 *          ObjectVectorObject = TypedVectorObject< AtomList >
 *
 * IntVectorClass corresponds to Vector$int, etc, and IntVectorObject corresponds to instances of
 * Vector$int, ect, with the wrinkle that ObjectVectorClass and ObjectVectorObject are also
 * used internally to represent classes and instances for dynamically instantiated classes,
 * ie Vector.<T> for T not int, uint, Number, or *.
 *
 * The Toplevel's builtin class manifest has getters, get_VectorClass(), get_Vector_intClass(), etc,
 * to return the unique instances representing Vector, Vector$int, etc.  The traits for those
 * classes are stored on the AvmCore: traits.vectorint_itraits, etc.
 *
 *
 * There are several workhorse methods:
 *
 *  VectorClass::getTypedVectorClass      ->  ClassClosure*
 *    Resolves to the vector Class object given the Class object for the parameter type.
 *    Calls newParameterizedVTable.
 *
 *  VTable::newParameterizedVTable        ->  VTable*
 *    Orchestrates building new VTable, ITraits, and CTraits for a new Vector type, reusing
 *    any existing ITraits and CTraits.  Calls newParameterizedCTraits and resolveParameterizedType.
 *
 *  PoolObject::resolveParameterizedType  ->  Traits*
 *    Resolves to the instance traits, ie, "Vector.<T>", used to handle CONSTANT_TypeName.
 *    (Can return NULL if the factory is not Vector.)  Calls newParameterizedITraits.
 *
 *  Traits::newParameterizedITraits       ->  Traits*
 *    Creates a Traits representing a Vector.<T> instance
 *
 *  Traits::newParameterizedCTraits       ->  Traits*
 *    Creates a Traits representing the Vector.<T> class
 *
 * When the traits for a new Vector type has been created it is added to the set of loaded traits
 * in the domain under the name "Vector.<T>$".
 *
 *
 * Consider APPLYTYPE VectorClass, T:
 *
 *   - VectorClass::applyTypeArgs does some error checking and tail-calls VectorClass::getTypedVectorClass
 *   - VectorClass::getTypedVectorClass looks for a cached class object in the Domain
 *   - If found return it
 *   - If not found:
 *     - newParameterizedVTable is called with fullname="Vector.<T>"
 *       - Call DomainMgr::findTraitsInPoolByNameAndNS to look for a loaded trait for "Vector.<T>$"
 *         - If not found then create ctraits and itraits
 *           - That will register the itraits (in resolveParameterizedType) but not the ctraits
 *              [[[ The CTraits are never registered.  The lookup is redundant, I think,
 *                  for that reason.  That's OK, because the call to newParameterizedVTable
 *                  is guarded by the lookup in getTypedVectorClass, so we make one lookup
 *                  that fails but that's all.  I've left a comment in the code.  ]]]
 *       - Create and initialize a new vtable based on the ctraits/itraits
 *       - return the vtable
 *     - a new instance of ObjectVectorClass is created and initialized
 *     - the new instance is registered with the Domain
 *     - return the new instance
 *
 *
 * Consider CONSTANT_TypeName VectorClass, T:
 *
 *   - PoolObject::resolveParameterizedType is called to resolve the type and return an itraits
 *     - Call DomainMgr::findTraitsInPoolByNameAndNS to look for a loaded itrait for "Vector.<T>"
 *       - If not found then create it and register it
 *       - return it
 *
 * From all of that it follows that CONSTANT_TypeName and APPLYTYPE communicate about itraits via the
 * domain manager, and itraits are stored as loaded traits in the domain.  
 *
 *
 * Questions to resolve in the future (not all of these are hard, necessarily):
 *
 * Q: When does a traits represent a parameterized class, or an instance of a parameterized class?
 *    Ie, how do we recognize it in C++ code?
 *
 * Q: Given some traits representing a parameterized class, how can we find its class object?
 *    (Right now getTypedVectorClass keys on the class object of the parameter class.)
 *
 * Q: Given an instantiation of Vector with T (a class object), how do we find T (class object)?
 */

namespace avmplus
{
    // ----------------------------

    class GC_AS3_EXACT(VectorClass, ClassClosure)
    {
    protected:
        VectorClass(VTable* vtable);
    public:
        REALLY_INLINE static VectorClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) VectorClass(cvtable);
        }

        virtual Atom applyTypeArgs(int argc, Atom* argv);

        ClassClosure* getTypedVectorClass(ClassClosure* typeClass);

        static Stringp makeVectorClassName(AvmCore* core, Traits* t);

        // make a Vector of subtype Vector<typeClass>,
        // eg if typeClass == StringClass then return a new Vector<String>
        ObjectVectorObject* newVector(ClassClosure* typeClass, uint32_t length = 0);

    private:
        GC_NO_DATA(VectorClass)

        DECLARE_SLOTS_VectorClass;
    };

    // ----------------------------

    class GC_CPP_EXACT(TypedVectorClassBase, ClassClosure)
    {
    protected:
        explicit TypedVectorClassBase(VTable* vtable);

    public:
        Traits* getTypeTraits() const;

        // ClassClosure overrides
        virtual Atom call(int argc, Atom* argv);

        // AS3 native function implementations
        void _forEach(Atom thisAtom, ScriptObject* callback, Atom thisObject);
        bool _every(Atom thisAtom, ScriptObject* callback, Atom thisObject);
        bool _some(Atom thisAtom, ScriptObject* callback, Atom thisObject);
        Atom _sort(Atom thisAtom, ArrayObject* args);

    protected:
        virtual Atom createAndInitVectorFromObject(ScriptObject* so, uint32_t len) = 0;

    protected:
        GC_DATA_BEGIN(TypedVectorClassBase)

        GCMember<Traits> GC_POINTER(m_typeTraits);

        GC_DATA_END(TypedVectorClassBase)
    };

    // ----------------------------

    template<class OBJ>
    class TypedVectorClass : public TypedVectorClassBase
    {
        friend class VectorClass;

    protected:
        explicit TypedVectorClass(VTable* vtable);

    public:
        OBJ* newVector(uint32_t length = 0, bool fixed = false);

        virtual bool gcTrace(MMgc::GC* gc, size_t cursor)
        {
            return TypedVectorClassBase::gcTrace(gc, cursor);
        }

    protected:
        virtual Atom createAndInitVectorFromObject(ScriptObject* so, uint32_t len);
        Atom constructImpl(int argc, Atom* argv);
    };


    // ----------------------------

    class GC_AS3_EXACT(IntVectorClass, TypedVectorClass<IntVectorObject>)
    {
    private:
        explicit IntVectorClass(VTable* vtable);
    public:
        REALLY_INLINE static IntVectorClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) IntVectorClass(cvtable);
        }

        GC_NO_DATA(IntVectorClass)

        DECLARE_SLOTS_IntVectorClass;
    };

    // ----------------------------

    class GC_AS3_EXACT(UIntVectorClass, TypedVectorClass<UIntVectorObject>)
    {
    private:
        explicit UIntVectorClass(VTable* vtable);
    public:
        REALLY_INLINE static UIntVectorClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) UIntVectorClass(cvtable);
        }

        GC_NO_DATA(UIntVectorClass)

        DECLARE_SLOTS_UIntVectorClass;
    };

    // ----------------------------

    class GC_AS3_EXACT(DoubleVectorClass, TypedVectorClass<DoubleVectorObject>)
    {
    private:
        explicit DoubleVectorClass(VTable* vtable);
    public:
        REALLY_INLINE static DoubleVectorClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) DoubleVectorClass(cvtable);
        }

        GC_NO_DATA(DoubleVectorClass)

        DECLARE_SLOTS_DoubleVectorClass;
    };

    // ----------------------------

#ifdef VMCFG_FLOAT
    class GC_AS3_EXACT(FloatVectorClass, TypedVectorClass<FloatVectorObject>)
    {
    private:
        explicit FloatVectorClass(VTable* vtable);
    public:
        REALLY_INLINE static FloatVectorClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) FloatVectorClass(cvtable);
        }

        GC_NO_DATA(FloatVectorClass)

        DECLARE_SLOTS_FloatVectorClass;
    };

    class GC_AS3_EXACT(Float4VectorClass, TypedVectorClass<Float4VectorObject>)
    {
    private:
        explicit Float4VectorClass(VTable* vtable);
    public:
        REALLY_INLINE static Float4VectorClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) Float4VectorClass(cvtable);
        }
        
        GC_NO_DATA(Float4VectorClass)
        
        DECLARE_SLOTS_Float4VectorClass;
    };
#endif

    // ----------------------------

    class GC_AS3_EXACT(ObjectVectorClass, TypedVectorClass<ObjectVectorObject>)
    {
    private:
        explicit ObjectVectorClass(VTable* vtable);
    public:
        REALLY_INLINE static ObjectVectorClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) ObjectVectorClass(cvtable);
        }

        GC_NO_DATA(ObjectVectorClass)

        DECLARE_SLOTS_ObjectVectorClass;
    };

    // ----------------------------

    struct VectorIndex
    {
        explicit VectorIndex(AvmCore* core, Atom name);

        uint32_t index;
        enum { kNotNumber, kInvalidNumber, kValidNumber } status;
    };

    // ----------------------------

    template<class T>
    struct TypedVectorConstants
    {
        static T nullValue();
        static T undefinedValue();
    };

    // ----------------------------

#ifdef VMCFG_FLOAT
    template<typename T>
    struct typeIsFloat4
    {
        const static bool value = false;
    };

    template<>
    struct typeIsFloat4<float4_t>
    {
        const static bool value = true;
    };
#define IS_FLOAT4_TYPE(T) typeIsFloat4<T>::value
#else
#define IS_FLOAT4_TYPE(T) false
#endif 

    // ----------------------------

    class GC_CPP_EXACT(VectorBaseObject, ScriptObject)
    {
    protected:
        explicit VectorBaseObject(VTable* ivtable, ScriptObject* delegate);

    public:
        // AS3 native getter/setter implementations
        bool get_fixed() const;
        void set_fixed(bool fixed);

        // used by Flash code for AMF3 -- note that this is
        // the type that the Vector contains, not the Vector itself
        // (ie, "int" rather than "Vector<int>")
        Traits* getTypeTraits() const;

        // AIR needs to be able to get/set the length of an arbitrary
        // Vector without knowing its subclass; these simply forward into the
        // appropriate calls of the well-typed subclass.
        virtual uint32_t getLength() const = 0;
        virtual void setLength(uint32_t length) = 0;

    protected:
        
        Atom _mapImpl(ScriptObject* callback, Atom thisObject, VectorBaseObject* r, uint32_t len);
        Atom _filterImpl(ScriptObject* callback, Atom thisObject, VectorBaseObject* r, uint32_t len);

        VectorBaseObject* _newVector();

        void checkFixed() const;
        void FASTCALL throwFixedError() const;

        void atomToValue(Atom atom, int32_t& value);
        void atomToValueKnown(Atom atom, int32_t& value);
        Atom valueToAtom(const int32_t& value) const;

        void atomToValue(Atom atom, uint32_t& value);
        void atomToValueKnown(Atom atom, uint32_t& value);
        Atom valueToAtom(const uint32_t& value) const;

        void atomToValue(Atom atom, double& value);
        void atomToValueKnown(Atom atom, double& value);
        Atom valueToAtom(const double& value) const;

#ifdef VMCFG_FLOAT
        void atomToValue(Atom atom, float& value);
        void atomToValueKnown(Atom atom, float& value);
        Atom valueToAtom(const float& value) const;

        void atomToValue(Atom atom, float4_t& value);
        void atomToValueKnown(Atom atom, float4_t& value);
        Atom valueToAtom(const float4_t& value) const;
#endif // VMCFG_FLOAT

        void atomToValue(Atom atom, OpaqueAtom& value);
        void atomToValueKnown(Atom atom, OpaqueAtom& value);
        Atom valueToAtom(const OpaqueAtom& value) const;

        enum VectorIndexStatus { kNotNumber, kInvalidNumber, kValidNumber };
        VectorIndexStatus getVectorIndex(Atom name, uint32_t& index) const;
        static bool isNegativeVectorIndexAtom(Atom name);

        void throwGetDoubleException(double d, uint32_t length) const;
        void throwSetDoubleException(double d, uint32_t length) const;
    
        GC_DATA_BEGIN(VectorBaseObject)

        GCMember<TypedVectorClassBase>    GC_POINTER(m_vecClass);
        bool                            m_fixed;

        GC_DATA_END(VectorBaseObject)
    };

    // ----------------------------

    template<class TLIST>
    class TypedVectorObject : public VectorBaseObject
    {
        friend class CodegenLIR;
        friend class ObjectVectorObject;
        template<class OBJ> friend class TypedVectorClass;
        template<class TLISTVA, uintptr_t align> friend class VectorAccessor;

    public:
        typedef TLIST LIST;

    protected:
        explicit TypedVectorObject(VTable* ivtable, ScriptObject* delegate);

    public:
        // overrides
        virtual uint32_t getLength() const;
        virtual void setLength(uint32_t length);

        // AS3 native getter/setter implementations
        uint32_t get_length() const;
        void set_length(uint32_t newLength);

        // AS3 native function implementations
        Atom _map(ScriptObject* callback, Atom thisObject);
        Atom _filter(ScriptObject* callback, Atom thisObject);
        void _reverse();
        // First delete deleteCount entries, starting at insertPoint.
        // Then insert insertCount entries starting at insertPoint;
        // the insertCount entries are read from args, starting at offset.
        void _spliceHelper(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, Atom args, uint32_t offset);
        uint32_t AS3_push(Atom* argv, int argc);
        typename TLIST::TYPE AS3_pop();
#ifdef VMCFG_FLOAT
        void AS3_pop(typename TLIST::TYPE *);
#endif
        uint32_t AS3_unshift(Atom* argv, int argc);
        typename TLIST::TYPE AS3_shift();
#ifdef VMCFG_FLOAT
        void AS3_shift(typename TLIST::TYPE *);
#endif

        // ScriptObject method overrides
        virtual bool hasAtomProperty(Atom name) const;
        virtual Atom getAtomProperty(Atom name) const;
        virtual void setAtomProperty(Atom name, Atom value);
        virtual bool hasUintProperty(uint32_t index) const;
        virtual Atom getUintProperty(uint32_t index) const;
        virtual void setUintProperty(uint32_t index, Atom value);
        virtual Atom nextName(int index);
        virtual Atom nextValue(int index);
        virtual int nextNameIndex(int index);
        
        // "fast" methods that don't range-check in nondebug builds
        // and are inlined; for internal use by various bits of Flash glue code
        typename TLIST::TYPE getUintPropertyFast(uint32_t index) const;
        void setUintPropertyFast(uint32_t index, typename TLIST::TYPE value);

        // JIT helpers -- not for public use!
        // (declared public only to avoid a painful 'friend' declaration)
        typename TLIST::TYPE _getNativeIntProperty(int32_t index) const;
        void _setNativeIntProperty(int32_t index, typename TLIST::TYPE value);
#ifdef VMCFG_AOT
		void _setNativeIntPropertyFast(int32_t index, typename TLIST::TYPE value);
		void _setNativeIntPropertySlow(int32_t index, typename TLIST::TYPE value);
#endif
        typename TLIST::TYPE _getNativeUintProperty(uint32_t index) const;
        void _setNativeUintProperty(uint32_t index, typename TLIST::TYPE value);
        typename TLIST::TYPE _getNativeDoubleProperty(double index) const;
        void _setNativeDoubleProperty(double index, typename TLIST::TYPE value);
        bool _hasUintProperty(uint32_t index) const;
        Atom _getUintProperty(uint32_t index) const;
        void _setUintProperty(uint32_t index, Atom value);
        Atom _getIntProperty(int32_t index) const;
        void _setIntProperty(int32_t index, Atom value);
        Atom _getDoubleProperty(double index) const;
        void _setDoubleProperty(double index, Atom value);

#ifdef VMCFG_FLOAT
        /* Float4 helpers; these  helpers are only used for float4 values (i.e. when TLIST::TYPE is float4_t). 
           This is asserted in the implementations. */
        void _getFloat4IntProperty(typename TLIST::TYPE* retval, int32_t index) const;
        void _setFloat4IntProperty(int32_t index, const typename TLIST::TYPE& value);
        void _getFloat4UintProperty(typename TLIST::TYPE* retval, uint32_t index) const;
        void _setFloat4UintProperty(uint32_t index, const typename TLIST::TYPE& value);
        void _getFloat4DoubleProperty(typename TLIST::TYPE* retval, double index) const;
        void _setFloat4DoubleProperty(double index, const typename TLIST::TYPE& value);
        /* asserts used in the implementation, to verify that these helpers are used properly */
#endif
        
        // Optimized setters used by various JIT optimizations.  It is possible that
        // these ought to be moved into ObjectVectorObject in the manner of
        // _setKnown*PropertyWithPointer(), they only make sense for ObjectVectorObject.

        void _setKnownUintProperty(uint32_t index, Atom value);             // ObjectType <: VectorBaseType
        void _setKnownIntProperty(int32_t index, Atom value);               // ditto
        void _setKnownDoubleProperty(double index, Atom value);             // ditto

#ifdef DEBUGGER
        virtual uint64_t bytesUsed() const;
#endif
#ifdef VMCFG_AOT
		bool canTakeFastPath(int32_t index) const;
#endif
    private:

        void FASTCALL throwRangeError_u(uint32_t index) const;
        void FASTCALL throwRangeError_i(int32_t index) const;
        void FASTCALL throwRangeError_a(Atom index) const;

        void     checkReadIndex_u(uint32_t index) const;
        uint32_t checkReadIndex_i(int32_t index) const;
        uint32_t checkReadIndex_d(double index) const;
        void     checkWriteIndex_u(uint32_t index) const;
        uint32_t checkWriteIndex_i(int32_t index) const;
        uint32_t checkWriteIndex_d(double index) const;

        // variant of _spliceHelper with explicit array of Atom.
        // (Not exposed to AS3.)
        // Note:
        //      _splice(0, 1, 0, &foo)           == Array.unshift(foo)
        //      _splice(this.length, 1, 0, &foo) == Array.push(foo)
        void _splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const Atom* argv);
        
        // _spliceHelper for when we already know that we have an object that is *not* of our Vector type.
        void _spliceHelper_so(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, ScriptObject* so_args, uint32_t offset);

    public:
        virtual bool gcTrace(MMgc::GC* gc, size_t cursor)
        {
            (void)cursor;
            VectorBaseObject::gcTrace(gc, 0);
            m_list.gcTrace(gc);
            return false;
        }

    protected:
        TLIST                           m_list;
    };

    // Some code internal to Flash/AIR needs to directly get/set the contents of Vectors;
    // this class provides an implicit lock/unlock mechanism. We guarantee that
    // the value returned by addr() is valid for reading/writing for the lifespan of
    // the VectorAccessor (but only for entries 0...get_length()-1, of course).
    // length() is identical to VectorObject::get_length() but is provided here for symmetry.
    // This should obviously only be used in cases where performance is critical, or
    // other circumstances requires it (eg, to pass an array of numbers to a GPU
    // without intermediate copying). Note that it is explicitly legal to pass
    // a NULL VectorObject to the ctor (which will cause addr() to also return NULL
    // and length() to return 0). This class must be used only on the stack.
    template<class TLIST, uintptr_t align=0>
    class VectorAccessor
    {
    public:
        explicit VectorAccessor(TypedVectorObject<TLIST>* v);
        typename TLIST::TYPE* addr();
        uint32_t length();

    private:
        TypedVectorObject<TLIST>* m_vector;

    private:
        void* operator new(size_t size);                                        // unimplemented
    };


    // ----------------------------

    class GC_AS3_EXACT(IntVectorObject, TypedVectorObject< DataList<int32_t> >)
    {
    protected:
        explicit IntVectorObject(VTable* ivtable, ScriptObject* delegate);

    public:
        REALLY_INLINE static IntVectorObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* delegate)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) IntVectorObject(ivtable, delegate);
        }

        // AS3 native function implementations
        IntVectorObject* newThisType();

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(IntVectorObject)

        DECLARE_SLOTS_IntVectorObject;
    // ------------------------ DATA SECTION END
    };
    typedef VectorAccessor< DataList<int32_t> > IntVectorAccessor;

    // ----------------------------

    class GC_AS3_EXACT(UIntVectorObject, TypedVectorObject< DataList<uint32_t> >)
    {
    protected:
        explicit UIntVectorObject(VTable* ivtable, ScriptObject* delegate);

    public:
        REALLY_INLINE static UIntVectorObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* delegate)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) UIntVectorObject(ivtable, delegate);
        }

        // AS3 native function implementations
        UIntVectorObject* newThisType();


    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(UIntVectorObject)

        DECLARE_SLOTS_UIntVectorObject;
    // ------------------------ DATA SECTION END
    };
    typedef VectorAccessor< DataList<uint32_t> > UIntVectorAccessor;

    // ----------------------------

    class GC_AS3_EXACT(DoubleVectorObject, TypedVectorObject< DataList<double> >)
    {
    protected:
        explicit DoubleVectorObject(VTable* ivtable, ScriptObject* delegate);

    public:
        REALLY_INLINE static DoubleVectorObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* delegate)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) DoubleVectorObject(ivtable, delegate);
        }

        // AS3 native function implementations
        DoubleVectorObject* newThisType();

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(DoubleVectorObject)

        DECLARE_SLOTS_DoubleVectorObject;
    // ------------------------ DATA SECTION END
    };
    typedef VectorAccessor< DataList<double> > DoubleVectorAccessor;

    // ----------------------------

#ifdef VMCFG_FLOAT
    class GC_AS3_EXACT(FloatVectorObject, TypedVectorObject< DataList<float> >)
    {
    protected:
        explicit FloatVectorObject(VTable* ivtable, ScriptObject* delegate);

    public:
        REALLY_INLINE static FloatVectorObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* delegate)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) FloatVectorObject(ivtable, delegate);
        }

        // AS3 native function implementations
        FloatVectorObject* newThisType();

        // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(FloatVectorObject)

            DECLARE_SLOTS_FloatVectorObject;
        // ------------------------ DATA SECTION END
    };
    typedef VectorAccessor< DataList<float> > FloatVectorAccessor;

    // Macros and templates don't mix
    typedef TypedVectorObject< DataList<float4_t, 16> > Float4VectorObjectBaseClass;
    
    class GC_AS3_EXACT(Float4VectorObject, Float4VectorObjectBaseClass)
    {
    protected:
        explicit Float4VectorObject(VTable* ivtable, ScriptObject* delegate);
        
    public:
        REALLY_INLINE static Float4VectorObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* delegate)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) Float4VectorObject(ivtable, delegate);
        }
        
        // AS3 native function implementations
        Float4VectorObject* newThisType();
        
        // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(Float4VectorObject)
        
        DECLARE_SLOTS_Float4VectorObject;
        // ------------------------ DATA SECTION END
    };
    typedef VectorAccessor< DataList<float4_t, 16>, 16 > Float4VectorAccessor;
#endif

    // ----------------------------

    class GC_AS3_EXACT(ObjectVectorObject, TypedVectorObject< AtomList >)
    {
    protected:
        explicit ObjectVectorObject(VTable* ivtable, ScriptObject* delegate);

    public:
        REALLY_INLINE static ObjectVectorObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* delegate)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) ObjectVectorObject(ivtable, delegate);
        }

        // The JIT calls _setKnown*PropertyWithPointer when the Vector is known to contain only
        // RCObject values and the value to be stored is an RCObject subtype of the element type.
        // Note the call to m_list.setPointer() instead of m_list.set(); the former streamlines
        // the write barrier.
        //
        // These are here and not in TypedVectorObject because they only make sense
        // for ObjectVectorObject.

        void _setKnownUintPropertyWithPointer(uint32_t index, Atom value)
        {
            checkWriteIndex_u(index);
            AtomList::OPAQUE_TYPE tmp;
            atomToValueKnown(value, tmp);
            m_list.setPointer(index, (AtomList::TYPE)tmp);
        }
        
        void _setKnownIntPropertyWithPointer(int32_t index_i, Atom value)
        {
            uint32_t index = checkWriteIndex_i(index_i);
            AtomList::OPAQUE_TYPE tmp;
            atomToValueKnown(value, tmp);
            m_list.setPointer(index, (AtomList::TYPE)tmp);
        }
        
        void _setKnownDoublePropertyWithPointer(double index_d, Atom value)
        {
            uint32_t index = checkWriteIndex_d(index_d);
            AtomList::OPAQUE_TYPE tmp;
            atomToValueKnown(value, tmp);
            m_list.setPointer(index, (AtomList::TYPE)tmp);
        }


        // AS3 native function implementations
        ObjectVectorObject* newThisType();

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(ObjectVectorObject)

        DECLARE_SLOTS_ObjectVectorObject;
    // ------------------------ DATA SECTION END
    };
    // This is explicitly NOT provided, as
    // (1) there's currently no need for it, and
    // (2) it would require WB hackery on the part of the user, which is error-prone
    //typedef VectorAccessor<Atom> ObjectVectorAccessor;

    // ----------------------------
}

#endif /* __avmplus_VectorClass__ */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */



#include "avmplus.h"

#include "VectorClass-impl.h"

namespace avmplus
{
    // Force explicit instantiations for various non-inlined methods;
    // some compilers don't need this, but some do.

    template class TypedVectorClass<IntVectorObject>;
    template class TypedVectorClass<UIntVectorObject>;
    template class TypedVectorClass<DoubleVectorObject>;
#ifdef VMCFG_FLOAT
    template class TypedVectorClass<FloatVectorObject>;
    template class TypedVectorClass<Float4VectorObject>;
#endif
    template class TypedVectorClass<ObjectVectorObject>;

    template class TypedVectorObject< DataList<int32_t> >;
    template class TypedVectorObject< DataList<uint32_t> >;
    template class TypedVectorObject< DataList<double> >;
#ifdef VMCFG_FLOAT
    template class TypedVectorObject< DataList<float> >;
    template class TypedVectorObject< DataList<float4_t, 16> >;
#endif
    template class TypedVectorObject< AtomList >;
}

namespace avmplus
{
    // ----------------------------

    // helper method
    // sets index to the uint32_t value of name, if it can be converted to a vector index
    // isNumber is set to true if name was a number (whether it was a uint32_t value or not)
    VectorBaseObject::VectorIndexStatus VectorBaseObject::getVectorIndex(Atom name, uint32_t& index) const
    {
        // This is implicitly asserted by the call to AvmCore::getIndexFromAtom() below.
        // Make it explicit here.
        AvmAssert(atomIsIntptr(name) || AvmCore::isString(name));

        if (AvmCore::getIndexFromAtom(name, &index))
        {
            return kValidNumber;
        }
        else if (AvmCore::isString(name))
        {
            Stringp s = core()->string(name);
            wchar c = s->charAt(0);
            // Does it look like a number?
            if (s->length() > 0 && ((c >= '0' && c <= '9') || c == '-'))
            {
                double const index_d = s->toNumber();
                if (!MathUtils::isNaN(index_d))
                {
                    // Name is a string that looks like a number.
                    // Convert using int, not uint, as it's much faster.
                    int32_t const index_i = int32_t(index_d);
                    if (double(index_i) == index_d) {
                        index = uint32_t(index_i);
                        return (index_i >= 0 ? kValidNumber : kInvalidNumber);
                    }
                    #if 0
                    // If number cannot be converted to a int, try to convert
                    // to a uint.  This is slower, but we are going to throw
                    // an exception anyway at this point:  Either the number
                    // is non-integral, or it is larger than we can allocate
                    // for a (dense) vector.
                    uint32_t const index_u = uint32_t(index_d);
                    if (double(index_u) == index_d) {
                        index = index_u;
                        return kValidNumber;
                    }
                    #else
                    // Number is non-integral, negative, or too large for an int.
                    // Note that some larger values that fit in a uint are valid
                    // index values, and we would expect that kValidNumber would be
                    // returned.  In Tamarin, however, we cannot allocate a vector
                    // sufficiently large to make such an index legal, so such an
                    // index will always yield a ReferenceError whenever it is used
                    // to subscript a vector, just as for kInvalidNumber.  Furthermore,
                    // a similar quirk has been long-standing, and compatibility with
                    // SWF10 relies upon it.
                    return kInvalidNumber;
                    #endif
                }
            }
        }
        return kNotNumber;
    }

    // Return true if name is a negative number or a string denoting the same.
    // Assumes that getVectorIndex() has been previously invoked with argument 'name',
    // and returned with the 'isNumber' flag true.
    bool VectorBaseObject::isNegativeVectorIndexAtom(Atom name)
    {
        AvmAssert(atomIsIntptr(name) ||
                  (AvmCore::isString(name) && AvmCore::atomToString(name)->length() > 0));

        return (atomIsIntptr(name)
                ? atomGetIntptr(name) < 0
                : AvmCore::atomToString(name)->charAt(0) == '-');
    }

    // Helper for _getDoubleProperty and _getNativedoubleProperty
    // Called when vector index of type double is non-integral or negative, or too large to fit in an int32.
    void VectorBaseObject::throwGetDoubleException(double d, uint32_t length) const
    {
        // For SWF11 and later, throw RangeError.
        // For SWF10 and earlier, throw ReferenceError if the value is not legal ECMAscript Array index,
        // which must be in the range 0..2^32-2. Otherwise, throw RangeError.
        if ((double(uint32_t(d)) == d && uint32_t(d) != 0xffffffff) || core()->currentBugCompatibility()->bugzilla456852b)
        {
            toplevel()->throwRangeError(kOutOfRangeError, core()->doubleToString(d), core()->uintToString(length));
        }
        else
        {
            // NOTE: Backward compatibility for SWF10 and earlier requires that we search the
            // prototype chain.  Unfortunately, that might result in a value of type other than
            // the declared element type of the vector, as assumed here, so we punt and just throw.
            Multiname mn(core()->findPublicNamespace(), core()->internDouble(d));
            toplevel()->throwReferenceError(kReadSealedError, &mn, traits());
        }
    }

    // Helper for _setDoubleProperty and _setNativeDoubleProperty.
    // Called when vector index of type double is non-integral or negative, or too large to fit in an int32.
    void VectorBaseObject::throwSetDoubleException(double d, uint32_t length) const
    {
        // For SWF11 and later, throw RangeError.
        // For SWF10 and earlier, throw ReferenceError if the value is not legal ECMAscript Array index,
        // or one greater than the largest legal index, i.e., in the range 0..2^32-1.  Othewrwise, throw RangeError.
        if (double(uint32_t(d)) == d || core()->currentBugCompatibility()->bugzilla456852b)
        {
            toplevel()->throwRangeError(kOutOfRangeError, core()->doubleToString(d), core()->uintToString(length));
        }
        else
        {
            Multiname mn(core()->findPublicNamespace(), core()->internDouble(d));
            toplevel()->throwReferenceError(kWriteSealedError, &mn, traits());
        }
    }

    // ----------------------------

    VectorClass::VectorClass(VTable *vtable)
        : ClassClosure(vtable)
    {
        setPrototypePtr(toplevel()->objectClass->construct());
    }

    /*static*/ Stringp VectorClass::makeVectorClassName(AvmCore* core, Traits* t)
    {
        Stringp s = core->newConstantStringLatin1("Vector.<");
        s = s->append(t->formatClassName());
        s = s->append(core->newConstantStringLatin1(">"));
        // all callers want it interned, so let's do it here
        return core->internString(s);
    }

    ClassClosure* VectorClass::getTypedVectorClass(ClassClosure* typeClass)
    {
        ClassClosure* result = NULL;
        Toplevel* toplevel = this->toplevel();
        if (typeClass == NULL)
        {
            result = toplevel->objectVectorClass();
        }
        else if (typeClass == toplevel->intClass())
        {
            result = toplevel->intVectorClass();
        }
        else if (typeClass == toplevel->uintClass())
        {
            result = toplevel->uintVectorClass();
        }
        else if (typeClass == toplevel->numberClass())
        {
            result = toplevel->doubleVectorClass();
        }
#ifdef VMCFG_FLOAT
        else if (typeClass == toplevel->floatClass())
        {
            result = toplevel->floatVectorClass();
        }
        else if (typeClass == toplevel->float4Class())
        {
            result = toplevel->float4VectorClass();
        }
#endif // VMCFG_FLOAT
        else
        {
            // if we have an object, we must have an itraits (otherwise the typearg is not a Class)
            Traits* typeTraits = typeClass->vtable->traits->itraits;
            if (!typeTraits)
                toplevel->throwVerifyError(kCorruptABCError);

            Domain* typeDomain = typeClass->traits()->pool->domain;
            if ((result = typeDomain->getParameterizedType(typeClass)) == NULL)
            {
                Stringp fullname = VectorClass::makeVectorClassName(core(), typeTraits);

                VTable* vt = this->vtable->newParameterizedVTable(typeTraits, fullname);

                vt->ivtable->createInstanceProc = ClassClosure::impossibleCreateInstanceProc;
                ObjectVectorClass* parameterizedVector = ObjectVectorClass::create(vt->gc(), vt);
                parameterizedVector->m_typeTraits = typeClass ? (Traits*)(typeClass->traits()->itraits) : NULL;
                parameterizedVector->setDelegate(toplevel->classClass()->prototypePtr());

                // Is this right?  Should each instantiation get its own prototype?
                parameterizedVector->setPrototypePtr(toplevel->objectVectorClass()->prototypePtr());
                typeDomain->addParameterizedType(typeClass, parameterizedVector);

                result = parameterizedVector;
            }
        }
        AvmAssert(result != NULL);
        return result;
    }

    Atom VectorClass::applyTypeArgs(int argc, Atom* argv)
    {
        Toplevel* toplevel = this->toplevel();

        if (argc != 1)
        {
            // Vector only takes 1 type argument
            toplevel->typeErrorClass()->throwError(kWrongTypeArgCountError, traits()->formatClassName(), core()->toErrorString(1), core()->toErrorString(argc));
        }

        Atom const typeAtom = argv[0];

        ClassClosure* typeClass = NULL;
        if (!ISNULL(typeAtom))
        {
            if (atomKind(typeAtom) != kObjectType)
                toplevel->throwVerifyError(kCorruptABCError);

            typeClass = (ClassClosure*)AvmCore::atomToScriptObject(typeAtom);
            if (!typeClass->vtable->traits->itraits)
                toplevel->throwVerifyError(kCorruptABCError);
        }
        return getTypedVectorClass(typeClass)->atom();
    }

    Atom VectorClass::construct(int /*argc*/, Atom* /*argv*/)
    {
        toplevel()->throwTypeError(kConstructOfNonFunctionError);
        return undefinedAtom;
    }

    // FIXME: this could return a non-ObjectVectorObject, so we should really
    // return VectorBaseObject instead.
    ObjectVectorObject* VectorClass::newVector(ClassClosure* type, uint32_t length)
    {
        ClassClosure* vc = getTypedVectorClass(type);
        Atom args[2] = { nullObjectAtom, core()->uintToAtom(length) };
        return (ObjectVectorObject*)AvmCore::atomToScriptObject(vc->construct(1, args));
    }

    // ----------------------------

    IntVectorClass::IntVectorClass(VTable* vtable)
        : TypedVectorClass<IntVectorObject>(vtable)
    {
        // This is an ugly hack: Vector<> doesn't work properly with the ClassManifest setup,
        // because the name it returns for itself isn't the name listed for finddef.
        // To work around this (and maintain legacy internal behavior), we pre-emptively
        // enter the Vector classes into the table upon first creation. (Arguably the name
        // lookup issue should be fixed.)
        toplevel()->builtinClasses()->fillInClass(avmplus::NativeID::abcclass___AS3___vec_Vector_int, this);
        this->m_typeTraits = toplevel()->intClass()->traits()->itraits;
    }

    Atom IntVectorClass::construct(int argc, Atom* argv)
    {
        return constructImpl(argc, argv);
    }

    // ----------------------------

    UIntVectorClass::UIntVectorClass(VTable* vtable)
        : TypedVectorClass<UIntVectorObject>(vtable)
    {
        // This is an ugly hack: Vector<> doesn't work properly with the ClassManifest setup,
        // because the name it returns for itself isn't the name listed for finddef.
        // To work around this (and maintain legacy internal behavior), we pre-emptively
        // enter the Vector classes into the table upon first creation. (Arguably the name
        // lookup issue should be fixed.)
        toplevel()->builtinClasses()->fillInClass(avmplus::NativeID::abcclass___AS3___vec_Vector_uint, this);
        this->m_typeTraits = toplevel()->uintClass()->traits()->itraits;
    }

    Atom UIntVectorClass::construct(int argc, Atom* argv)
    {
        return constructImpl(argc, argv);
    }

    // ----------------------------

    DoubleVectorClass::DoubleVectorClass(VTable* vtable)
        : TypedVectorClass<DoubleVectorObject>(vtable)
    {
        // This is an ugly hack: Vector<> doesn't work properly with the ClassManifest setup,
        // because the name it returns for itself isn't the name listed for finddef.
        // To work around this (and maintain legacy internal behavior), we pre-emptively
        // enter the Vector classes into the table upon first creation. (Arguably the name
        // lookup issue should be fixed.)
        toplevel()->builtinClasses()->fillInClass(avmplus::NativeID::abcclass___AS3___vec_Vector_double, this);
        this->m_typeTraits = toplevel()->numberClass()->traits()->itraits;
    }

    Atom DoubleVectorClass::construct(int argc, Atom* argv)
    {
        return constructImpl(argc, argv);
    }

    // ----------------------------
#ifdef VMCFG_FLOAT
    FloatVectorClass::FloatVectorClass(VTable* vtable)
        : TypedVectorClass<FloatVectorObject>(vtable)
    {
        toplevel()->builtinClasses()->fillInClass(avmplus::NativeID::abcclass___AS3___vec_Vector_float, this); 
        this->m_typeTraits = toplevel()->floatClass()->traits()->itraits;
    }

    Atom FloatVectorClass::construct(int argc, Atom* argv)
    {
        return constructImpl(argc, argv);
    }

    Float4VectorClass::Float4VectorClass(VTable* vtable)
        : TypedVectorClass<Float4VectorObject>(vtable)
    {
        toplevel()->builtinClasses()->fillInClass(avmplus::NativeID::abcclass___AS3___vec_Vector_float4, this); 
        this->m_typeTraits = toplevel()->float4Class()->traits()->itraits;
    }
    
    Atom Float4VectorClass::construct(int argc, Atom* argv)
    {
        return constructImpl(argc, argv);
    }
#endif
    // ----------------------------

    ObjectVectorClass::ObjectVectorClass(VTable* vtable)
        : TypedVectorClass<ObjectVectorObject>(vtable)
    {
        // This is an ugly hack: Vector<> doesn't work properly with the ClassManifest setup,
        // because the name it returns for itself isn't the name listed for finddef.
        // To work around this (and maintain legacy internal behavior), we pre-emptively
        // enter the Vector classes into the table upon first creation. (Arguably the name
        // lookup issue should be fixed.)
        toplevel()->builtinClasses()->fillInClass(avmplus::NativeID::abcclass___AS3___vec_Vector_object, this);
        this->m_typeTraits = toplevel()->objectClass->traits()->itraits;
    }

    Atom ObjectVectorClass::construct(int argc, Atom* argv)
    {
        return constructImpl(argc, argv);
    }

    // ----------------------------

    IntVectorObject::IntVectorObject(VTable* ivtable, ScriptObject* delegate)
        : TypedVectorObject< DataList<int32_t> >(ivtable, delegate)
    {
    }

    IntVectorObject* IntVectorObject::newThisType()
    {
        return (IntVectorObject*)_newVector();
    }


    // ----------------------------

    UIntVectorObject::UIntVectorObject(VTable* ivtable, ScriptObject* delegate)
        : TypedVectorObject< DataList<uint32_t> >(ivtable, delegate)
    {
    }

    UIntVectorObject* UIntVectorObject::newThisType()
    {
        return (UIntVectorObject*)_newVector();
    }

    // ----------------------------

    DoubleVectorObject::DoubleVectorObject(VTable* ivtable, ScriptObject* delegate)
        : TypedVectorObject< DataList<double> >(ivtable, delegate)
    {
    }

    DoubleVectorObject* DoubleVectorObject::newThisType()
    {
        return (DoubleVectorObject*)_newVector();
    }

    // ----------------------------
#ifdef VMCFG_FLOAT
    FloatVectorObject::FloatVectorObject(VTable* ivtable, ScriptObject* delegate)
        : TypedVectorObject< DataList<float> >(ivtable, delegate)
    {
    }

    FloatVectorObject* FloatVectorObject::newThisType()
    {
        return (FloatVectorObject*)_newVector();
    }

    Float4VectorObject::Float4VectorObject(VTable* ivtable, ScriptObject* delegate)
        : TypedVectorObject< DataList<float4_t, 16> >(ivtable, delegate)
    {
    }
    
    Float4VectorObject* Float4VectorObject::newThisType()
    {
        return (Float4VectorObject*)_newVector();
    }
#endif // VMCFG_FLOAT
    // ----------------------------

    ObjectVectorObject::ObjectVectorObject(VTable* ivtable, ScriptObject* delegate)
        : TypedVectorObject< AtomList >(ivtable, delegate)
    {
    }

    ObjectVectorObject* ObjectVectorObject::newThisType()
    {
        return (ObjectVectorObject*)_newVector();
    }


}

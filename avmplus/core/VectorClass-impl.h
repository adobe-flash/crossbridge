/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_VectorClass_impl__
#define __avmplus_VectorClass_impl__

namespace avmplus
{
    // ----------------------------

    Atom TypedVectorClassBase::call(int argc, Atom* argv)
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();
        
        if (argc != 1)
            toplevel->throwArgumentError(kCoerceArgumentCountError, core->toErrorString(argc));

        Traits* itraits = ivtable()->traits;
        if (AvmCore::istype(argv[1], itraits))
            return argv[1];

        if (!AvmCore::isObject(argv[1]))
            toplevel->throwTypeError(kCheckTypeFailedError, core->atomToErrorString(argv[1]), core->toErrorString(itraits));

        ScriptObject* so_args = AvmCore::atomToScriptObject(argv[1]);
        uint32_t len = so_args->getLengthProperty();
        return createAndInitVectorFromObject(so_args, len);
    }

    void TypedVectorClassBase::_forEach(Atom thisAtom, ScriptObject* callback, Atom thisObject)
    {
        return ArrayClass::generic_forEach(toplevel(), thisAtom, callback, thisObject);
    }
    
    bool TypedVectorClassBase::_every(Atom thisAtom, ScriptObject* callback, Atom thisObject)
    {
        return ArrayClass::generic_every(toplevel(), thisAtom, callback, thisObject);
    }
    
    bool TypedVectorClassBase::_some(Atom thisAtom, ScriptObject* callback, Atom thisObject)
    {
        return ArrayClass::generic_some(toplevel(), thisAtom, callback, thisObject);
    }
    
    Atom TypedVectorClassBase::_sort(Atom thisAtom, ArrayObject* args)
    {
        return ArrayClass::generic_sort(toplevel(), thisAtom, args);
    }

    // ----------------------------

    template<class OBJ>
    TypedVectorClass<OBJ>::TypedVectorClass(VTable* vtable)
        : TypedVectorClassBase(vtable)
    {
        setPrototypePtr(toplevel()->objectClass->construct());
    }

    template<class OBJ>
    OBJ* TypedVectorClass<OBJ>::newVector(uint32_t length, bool fixed)
    {
        OBJ* v = (OBJ*)OBJ::create(gc(), ivtable(), prototypePtr());
        v->m_vecClass = this;
        if (length > 0)
            v->set_length(length);
        v->m_fixed = fixed;
        return v;
    }

    template<class OBJ>
    Atom TypedVectorClass<OBJ>::createAndInitVectorFromObject(ScriptObject* so_args, uint32_t len)
    {
        OBJ* v = newVector();
        v->_spliceHelper_so(0, len, 0, so_args, 0);
        return v->atom();
    }

    template<class OBJ>
    Atom TypedVectorClass<OBJ>::constructImpl(int argc, Atom* argv)
    {
        uint32_t len = 0;
        bool fixed = false;
        if (argc > 0)
        {
            len = AvmCore::toUInt32(argv[1]);
            if (argc > 1)
            {
                fixed = AvmCore::boolean(argv[2]) != 0;
                if (argc > 2)
                {
                    vtable->init->argcError(argc);
                }
            }
        }
        OBJ* v = newVector(len, fixed);
        return v->atom();
    }

    // ----------------------------

    VectorBaseObject::VectorBaseObject(VTable* ivtable, ScriptObject* delegate)
        : ScriptObject(ivtable, delegate)
        , m_vecClass(NULL)
        , m_fixed(false)
    {
    }

    VectorBaseObject* VectorBaseObject::_newVector()
    {
        AvmAssert(m_vecClass != NULL);
        Atom args[1] = { nullObjectAtom };
        return (VectorBaseObject*)AvmCore::atomToScriptObject(m_vecClass->construct(0, args));
    }

    void FASTCALL VectorBaseObject::throwFixedError() const
    {
        toplevel()->throwRangeError(kVectorFixedError);
    }

    Atom VectorBaseObject::_mapImpl(ScriptObject* callback, Atom thisObject, VectorBaseObject* r, uint32_t len)
    {
        AvmCore* core = this->core();
        if (callback)
        {
            ScriptObject* d = this;
            for (uint32_t i = 0; i < len; i++)
            {
                // If thisObject is null, the call function will substitute the global object
                // args are modified in place by callee
                Atom args[4] = {
                    thisObject,
                    d->getUintProperty(i), // element
                    core->uintToAtom(i), // index
                    this->atom()
                };
                Atom result = callback->call(3, args);
                r->setUintProperty(i, result);
            }
        }
        return r->atom();
    }

    Atom VectorBaseObject::_filterImpl(ScriptObject* callback, Atom thisObject, VectorBaseObject* r, uint32_t len)
    {
        AvmCore* core = this->core();
        if (callback)
        {
            ScriptObject* d = this;
            for (uint32_t i = 0, k = 0; i < len; i++)
            {
                // If thisObject is null, the call function will substitute the global object
                // args are modified in place by callee
                Atom element = d->getUintProperty(i);
                Atom args[4] = {
                    thisObject,
                    element,
                    core->uintToAtom(i), // index
                    this->atom()
                };
                Atom result = callback->call(3, args);
                if (result == trueAtom)
                    r->setUintProperty(k++, element);
            }
        }
        return r->atom();
    }

    // ----------------------------

    template<class TLIST>
    TypedVectorObject<TLIST>::TypedVectorObject(VTable* ivtable, ScriptObject* delegate)
        : VectorBaseObject(ivtable, delegate)
        , m_list(ivtable->gc(), 0)
    {
    }

    template<class TLIST>
    bool TypedVectorObject<TLIST>::hasAtomProperty(Atom name) const
    {
        uint32_t index;
        VectorIndexStatus status = getVectorIndex(name, index);
        if (status == kValidNumber)
        {
            return index < m_list.length();
        }
        else if (status == kInvalidNumber)
        {
            if (core()->currentBugCompatibility()->bugzilla456852b)
                return false;
            else if (!isNegativeVectorIndexAtom(name))
                // Prior to SWF11, isNumber was incorrectly set false for negative numbers,
                // so we fall through and allow searching the prototype chain for these.
                // See getAtomProperty() below.
                return false;
        }

        // status == kNotNumber, or index is negative and compatibility is earlier than SWF11.
        return ScriptObject::hasAtomProperty(name);
    }
        
    template<class TLIST>
    void TypedVectorObject<TLIST>::setAtomProperty(Atom name, Atom value)
    {
        uint32_t index;
        VectorIndexStatus status = getVectorIndex(name, index);
        if (status == kValidNumber)
        {
            setUintProperty(index, value);
        }
        else if (status == kInvalidNumber && core()->currentBugCompatibility()->bugzilla456852b)
        {
            throwRangeError_a(name);
        }
        else
        {
            toplevel()->throwReferenceError(kWriteSealedError, core()->string(name), traits());
        }
    }
        
    template<class TLIST>
    Atom TypedVectorObject<TLIST>::getAtomProperty(Atom name) const
    {
        uint32_t index;
        VectorIndexStatus status = getVectorIndex(name, index);
        if (status == kValidNumber)
        {
            return getUintProperty(index);
        }
        else if (status == kInvalidNumber)
        {
            if (core()->currentBugCompatibility()->bugzilla456852b)
            {
                throwRangeError_a(name);
                return undefinedAtom; // not reached
            }
            else if (!isNegativeVectorIndexAtom(name))
            {
                // Prior to SWF11, isNumber was incorrectly set false for negative numbers.
                toplevel()->throwReferenceError(kReadSealedError, core()->string(name), traits());
                return undefinedAtom; // not reached
            }
        }

        // status == kNotNumber, or index is negative and compatibility is earlier than SWF11.
        // Check the prototype chain - that will throw if there is no match.
        return getAtomPropertyFromProtoChain(name, getDelegate(), traits());
    }

    template<class TLIST>
    /*virtual*/ uint32_t TypedVectorObject<TLIST>::getLength() const
    {
        return get_length();
    }

    template<class TLIST>
    /*virtual*/ void TypedVectorObject<TLIST>::setLength(uint32_t length)
    {
        set_length(length);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::set_length(uint32_t newLength)
    {
        checkFixed();
        // this is what the old code did, so we'll do it too: set the capacity explicitly if we grow
        if (newLength > m_list.capacity())
        {
            m_list.set_capacity(newLength);
        }
        m_list.set_length(newLength);
    }
        
    template<class TLIST>
    Atom TypedVectorObject<TLIST>::nextName(int index)
    {
        AvmAssert(index > 0);
        if (uint32_t(index) <= m_list.length())
        {
            return core()->intToAtom(index-1);
        }
        else
        {
            return nullStringAtom;
        }
    }
        
    template<class TLIST>
    Atom TypedVectorObject<TLIST>::nextValue(int index)
    {
        AvmAssert(index > 0);
        if (uint32_t(index) <= m_list.length())
        {
            return getUintProperty(index-1);
        }
        else
        {
            return undefinedAtom;
        }
    }
        
    template<class TLIST>
    int TypedVectorObject<TLIST>::nextNameIndex(int index)
    {
        if (uint32_t(index) < m_list.length())
        {
            return index + 1;
        }
        else
        {
            return 0;
        }
    }
        
    template<class TLIST>
    Atom TypedVectorObject<TLIST>::_map(ScriptObject* callback, Atom thisObject)
    {
        TypedVectorObject<TLIST>* r = (TypedVectorObject<TLIST>*)_newVector();
        uint32_t const len = m_list.length();
        r->set_length(len);
        return _mapImpl(callback, thisObject, r, len);
    }

    template<class TLIST>
    Atom TypedVectorObject<TLIST>::_filter(ScriptObject* callback, Atom thisObject)
    {
        TypedVectorObject<TLIST>* r = (TypedVectorObject<TLIST>*)_newVector();
        uint32_t const len = m_list.length();
        return _filterImpl(callback, thisObject, r, len);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_spliceHelper_so(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, ScriptObject* so_args, uint32_t offset)
    {
        AvmAssert(!AvmCore::istype(so_args->atom(), vtable->traits));

        // NB: so_args could be any kind of object, but in practice,
        // it's almost always an ArrayObject; it might be profitable
        // to sniff for that case and optimize appropriately.

        // passing NULL for args inserts the proper space, but leaves the inserted entries empty
        m_list.splice(insertPoint, insertCount, deleteCount, NULL);
        for (uint32_t i = 0; i < insertCount; i++)
        {
            typename TLIST::OPAQUE_TYPE value;
            atomToValue(so_args->getUintProperty(i+offset), value);
            m_list.set(insertPoint+i, (typename TLIST::TYPE)value);
        }
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_spliceHelper(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, Atom args, uint32_t offset)
    {
        if (AvmCore::istype(args, vtable->traits))
        {
            TypedVectorObject<TLIST>* vec_args = (TypedVectorObject<TLIST>*)AvmCore::atomToScriptObject(args);
            m_list.splice(insertPoint, insertCount, deleteCount, vec_args->m_list, offset);
        }
        else if (AvmCore::isObject(args))
        {
            ScriptObject* so_args = AvmCore::atomToScriptObject(args);
            _spliceHelper_so(insertPoint, insertCount, deleteCount, so_args, offset);
        }
        else
        {
            AvmAssert(insertCount == 0);
            m_list.splice(insertPoint, 0, deleteCount, NULL);
        }
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const Atom* argv)
    {
        // passing NULL for args inserts the proper space, but leaves the inserted entries empty
        m_list.splice(insertPoint, insertCount, deleteCount, NULL);
        for (uint32_t i = 0; i < insertCount; i++)
        {
            typename TLIST::OPAQUE_TYPE value;
            atomToValue(argv[i], value);
            m_list.set(insertPoint+i, (typename TLIST::TYPE)value);
        }
    }

    template<class TLIST>
    uint32_t TypedVectorObject<TLIST>::AS3_push(Atom* argv, int argc)
    {
        checkFixed();
        _splice(m_list.length(), argc, 0, argv);
        return m_list.length();
    }

    template<class TLIST>
    typename TLIST::TYPE TypedVectorObject<TLIST>::AS3_pop()
    {
        checkFixed();
        return m_list.isEmpty() ?
                (typename TLIST::TYPE)TypedVectorConstants<typename TLIST::OPAQUE_TYPE>::undefinedValue() :
                m_list.removeLast();
    }

#ifdef VMCFG_FLOAT
    template<class TLIST>
    void TypedVectorObject<TLIST>::AS3_pop(typename TLIST::TYPE* retval)
    {
        checkFixed();
        *retval = m_list.isEmpty() ?
        (typename TLIST::TYPE)TypedVectorConstants<typename TLIST::OPAQUE_TYPE>::undefinedValue() :
        m_list.removeLast();
    }
#endif
    
    template<class TLIST>
    typename TLIST::TYPE TypedVectorObject<TLIST>::AS3_shift()
    {
        checkFixed();
        return m_list.isEmpty() ?
                (typename TLIST::TYPE)TypedVectorConstants<typename TLIST::OPAQUE_TYPE>::undefinedValue() :
                m_list.removeFirst();
    }

#ifdef VMCFG_FLOAT
    template<class TLIST>
    void TypedVectorObject<TLIST>::AS3_shift(typename TLIST::TYPE* retval)
    {
        checkFixed();
        *retval = m_list.isEmpty() ?
        (typename TLIST::TYPE)TypedVectorConstants<typename TLIST::OPAQUE_TYPE>::undefinedValue() :
        m_list.removeFirst();
    }
#endif
    
    template<class TLIST>
    uint32_t TypedVectorObject<TLIST>::AS3_unshift(Atom* argv, int argc)
    {
        checkFixed();
        _splice(0, argc, 0, argv);
        return m_list.length();
    }

    template<class TLIST>
    REALLY_INLINE typename TLIST::TYPE TypedVectorObject<TLIST>::_getNativeIntProperty(int32_t index_i) const
    {
        AvmAssertMsg(!IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _getNativeIntProperty helper used for Vector.<float4>");
        uint32_t const index = checkReadIndex_i(index_i);
        return m_list.get(index);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setNativeIntProperty(int32_t index_i, typename TLIST::TYPE value)
    {
        AvmAssertMsg(!IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _setNativeIntProperty helper used for Vector.<float4>");
        uint32_t const index = checkWriteIndex_i(index_i);
       	m_list.set(index, value);
    }

#ifdef VMCFG_AOT
    template<class TLIST>
    NO_INLINE void TypedVectorObject<TLIST>::_setNativeIntPropertySlow(int32_t index_i, typename TLIST::TYPE value)
    {
        uint32_t const length = m_list.length();
        AvmAssert(uint32_t(index_i) >= length);
        uint32_t const limit = length + 1 - uint32_t(m_fixed);
        if (uint32_t(index_i) >= limit)
            throwRangeError_i(index_i);
        AvmAssertMsg(!IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _setNativeIntProperty helper used for Vector.<float4>");
		m_list.set((uint32_t)index_i, value);
    }
#endif

    template<class TLIST>
    typename TLIST::TYPE TypedVectorObject<TLIST>::_getNativeUintProperty(uint32_t index) const
    {
        AvmAssertMsg(!IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _getNativeUIntProperty helper used for Vector.<float4>");
        checkReadIndex_u(index);
        return m_list.get(index);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setNativeUintProperty(uint32_t index, typename TLIST::TYPE value)
    {
        AvmAssertMsg(!IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _setNativeUIntProperty helper used for Vector.<float4>");
        checkWriteIndex_u(index);
        m_list.set(index, value);
    }

    template<class TLIST>
    typename TLIST::TYPE TypedVectorObject<TLIST>::_getNativeDoubleProperty(double index_d) const
    {
        AvmAssertMsg(!IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _getNativeDoubleProperty helper used for Vector.<float4>");
        uint32_t const index = checkReadIndex_d(index_d);
        return m_list.get(index);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setNativeDoubleProperty(double index_d, typename TLIST::TYPE value)
    {
        AvmAssertMsg(!IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _setNativeDoubleProperty helper used for Vector.<float4>");
        uint32_t const index = checkWriteIndex_d(index_d);
        m_list.set(index, value);
    }

#ifdef VMCFG_FLOAT
    template<class TLIST>
    void TypedVectorObject<TLIST>::_getFloat4IntProperty(typename TLIST::TYPE* result, int32_t index_i) const
    {
        AvmAssertMsg(IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _getNativeIntProperty helper used for Vector.<*>");
        uint32_t const index = checkReadIndex_i(index_i);
        if(result) 
            *result = m_list.get(index);
        return;
    }
    
    template<class TLIST>
    void TypedVectorObject<TLIST>::_setFloat4IntProperty(int32_t index_i, const typename TLIST::TYPE& value)
    {
        AvmAssertMsg(IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _setNativeIntProperty helper used for Vector.<*>");
        uint32_t const index = checkWriteIndex_i(index_i);
        m_list.set(index, value);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_getFloat4UintProperty(typename TLIST::TYPE* result, uint32_t index) const
    {
        AvmAssertMsg(IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _getNativeUIntProperty helper used for Vector.<*>");
        checkReadIndex_u(index);
        if(result) 
            *result = m_list.get(index);
        return;
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setFloat4UintProperty(uint32_t index, const typename TLIST::TYPE& value)
    {
        AvmAssertMsg(IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _setNativeUIntProperty helper used for Vector.<*>");
        checkWriteIndex_u(index);
        m_list.set(index, value);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_getFloat4DoubleProperty(typename TLIST::TYPE* result, double index_d) const
    {
        AvmAssertMsg(IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _getNativeDoubleProperty helper used for Vector.<*>");
        uint32_t const index = checkReadIndex_d(index_d);
        if(result) 
            *result = m_list.get(index);
        return;
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setFloat4DoubleProperty(double index_d, const typename TLIST::TYPE& value)
    {
        AvmAssertMsg(IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _setNativeDoubleProperty helper used for Vector.<*>");
        uint32_t const index = checkWriteIndex_d(index_d);
        m_list.set(index, value); // needed to compile for most types; hopefully the useless call will be optimized-away.
    }
#endif

    template<class TLIST>
    bool TypedVectorObject<TLIST>::_hasUintProperty(uint32_t index) const
    {
        return (index < m_list.length());
    }

    template<class TLIST>
    Atom TypedVectorObject<TLIST>::_getUintProperty(uint32_t index) const
    {
        checkReadIndex_u(index);
        return valueToAtom((typename TLIST::OPAQUE_TYPE)m_list.get(index));
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setUintProperty(uint32_t index, Atom value)
    {
        typename TLIST::OPAQUE_TYPE tmp;
        atomToValue(value, tmp);
        checkWriteIndex_u(index);
        m_list.set(index, (typename TLIST::TYPE)tmp);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setKnownUintProperty(uint32_t index, Atom value)
    {
        typename TLIST::OPAQUE_TYPE tmp;
        atomToValueKnown(value, tmp);
        checkWriteIndex_u(index);
        m_list.set(index, (typename TLIST::TYPE)tmp);
    }
    
    template<class TLIST>
    Atom TypedVectorObject<TLIST>::_getIntProperty(int32_t index_i) const
    {
        uint32_t const index = checkReadIndex_i(index_i);
        return valueToAtom((typename TLIST::OPAQUE_TYPE)m_list.get(index));
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setIntProperty(int32_t index_i, Atom value)
    {
        typename TLIST::OPAQUE_TYPE tmp;
        atomToValue(value, tmp);
        uint32_t index = checkWriteIndex_i(index_i);
        m_list.set(index, (typename TLIST::TYPE)tmp);
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setKnownIntProperty(int32_t index_i, Atom value)
    {
        typename TLIST::OPAQUE_TYPE tmp;
        atomToValueKnown(value, tmp);
        uint32_t index = checkWriteIndex_i(index_i);
        m_list.set(index, (typename TLIST::TYPE)tmp);
    }
    
    template<class TLIST>
    Atom TypedVectorObject<TLIST>::_getDoubleProperty(double index_d) const
    {
        uint32_t const index = checkReadIndex_d(index_d);
        return valueToAtom((typename TLIST::OPAQUE_TYPE)m_list.get(index));
    }

    template<class TLIST>
    void TypedVectorObject<TLIST>::_setDoubleProperty(double index_d, Atom value)
    {
        typename TLIST::OPAQUE_TYPE tmp;
        atomToValue(value, tmp);
        uint32_t index = checkWriteIndex_d(index_d);
        m_list.set(index, (typename TLIST::TYPE)tmp);
    }
    
    template<class TLIST>
    void TypedVectorObject<TLIST>::_setKnownDoubleProperty(double index_d, Atom value)
    {
        typename TLIST::OPAQUE_TYPE tmp;
        atomToValueKnown(value, tmp);
        uint32_t index = checkWriteIndex_d(index_d);
        m_list.set(index, (typename TLIST::TYPE)tmp);
    }
    
    template<class TLIST>
    void FASTCALL TypedVectorObject<TLIST>::throwRangeError_u(uint32_t index) const
    {
        toplevel()->throwRangeError(kOutOfRangeError, core()->uintToString(index), core()->uintToString(m_list.length()));
    }

    template<class TLIST>
    void FASTCALL TypedVectorObject<TLIST>::throwRangeError_i(int32_t index) const
    {
        toplevel()->throwRangeError(kOutOfRangeError, core()->intToString(index), core()->uintToString(m_list.length()));
    }

    template<class TLIST>
    void FASTCALL TypedVectorObject<TLIST>::throwRangeError_a(Atom index) const
    {
        toplevel()->throwRangeError(kOutOfRangeError, core()->string(index), core()->uintToString(m_list.length()));
    }

    // ----------------------------
}

#endif /* __avmplus_VectorClass_impl__ */

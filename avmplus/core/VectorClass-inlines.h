/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_VectorClass_inlines__
#define __avmplus_VectorClass_inlines__

namespace avmplus
{
    // For some hand-written tracers that are used by generated tracers.

#define avmplus_TypedVectorClassXFloatVectorObjectX_isExactInterlock 1
#define avmplus_TypedVectorClassXFloat4VectorObjectX_isExactInterlock 1
#define avmplus_TypedVectorClassXDoubleVectorObjectX_isExactInterlock 1
#define avmplus_TypedVectorClassXIntVectorObjectX_isExactInterlock 1
#define avmplus_TypedVectorClassXUIntVectorObjectX_isExactInterlock 1
#define avmplus_TypedVectorClassXObjectVectorObjectX_isExactInterlock 1
#define avmplus_TypedVectorObjectXDataListXfloatXX_isExactInterlock 1
#define avmplus_TypedVectorObjectXDataListXfloat4_tXX_isExactInterlock 1
#define avmplus_TypedVectorObjectXDataListXdoubleXX_isExactInterlock 1
#define avmplus_TypedVectorObjectXDataListXint32_tXX_isExactInterlock 1
#define avmplus_TypedVectorObjectXDataListXuint32_tXX_isExactInterlock 1
#define avmplus_TypedVectorObjectXAtomListX_isExactInterlock 1
    
    // To deal with a typedef workaround
#define avmplus_Float4VectorObjectBaseClass_isExactInterlock 1
    
    // ----------------------------

    REALLY_INLINE TypedVectorClassBase::TypedVectorClassBase(VTable* vtable)
        : ClassClosure(vtable)
    {
    }

    REALLY_INLINE Traits* TypedVectorClassBase::getTypeTraits() const
    {
        return m_typeTraits;
    }

    // ----------------------------

    template<>
    REALLY_INLINE OpaqueAtom TypedVectorConstants<OpaqueAtom>::nullValue()
    {
        return (OpaqueAtom)nullObjectAtom;
    }
    template<>
    REALLY_INLINE OpaqueAtom TypedVectorConstants<OpaqueAtom>::undefinedValue()
    {
        return (OpaqueAtom)undefinedAtom;
    }

    template<class T>
    REALLY_INLINE T TypedVectorConstants<T>::nullValue()
    {
        return 0;
    }

#ifdef VMCFG_FLOAT
    template<>
    REALLY_INLINE float4_t TypedVectorConstants<float4_t>::undefinedValue()
    {
        float4_t zero = { 0,0,0,0 };
        return zero;
    }
#endif

    template<class T>
    REALLY_INLINE T TypedVectorConstants<T>::undefinedValue()
    {
        return 0;
    }

    // ----------------------------

    REALLY_INLINE bool VectorBaseObject::get_fixed() const
    {
        return m_fixed;
    }

    REALLY_INLINE void VectorBaseObject::set_fixed(bool f)
    {
        m_fixed = f;
    }

    REALLY_INLINE void VectorBaseObject::checkFixed() const
    {
        if (m_fixed)
            throwFixedError();
    }

    REALLY_INLINE Traits* VectorBaseObject::getTypeTraits() const
    {
        return m_vecClass->getTypeTraits();
    }

    REALLY_INLINE void VectorBaseObject::atomToValue(Atom atom, int32_t& value)
    {
        value = AvmCore::integer(atom);
    }
    
    REALLY_INLINE void VectorBaseObject::atomToValueKnown(Atom atom, int32_t& value)
    {
        value = AvmCore::integer(atom);
    }
    
    REALLY_INLINE Atom VectorBaseObject::valueToAtom(const int32_t& value) const
    {
        return core()->intToAtom(value);
    }

    REALLY_INLINE void VectorBaseObject::atomToValue(Atom atom, uint32_t& value)
    {
        value = AvmCore::toUInt32(atom);
    }
    
    REALLY_INLINE void VectorBaseObject::atomToValueKnown(Atom atom, uint32_t& value)
    {
        value = AvmCore::toUInt32(atom);
    }
    
    REALLY_INLINE Atom VectorBaseObject::valueToAtom(const uint32_t& value) const
    {
        return core()->uintToAtom(value);
    }

    REALLY_INLINE void VectorBaseObject::atomToValue(Atom atom, double& value)
    {
        value = AvmCore::number(atom);
    }
    
    REALLY_INLINE void VectorBaseObject::atomToValueKnown(Atom atom, double& value)
    {
        value = AvmCore::number(atom);
    }
    
    REALLY_INLINE Atom VectorBaseObject::valueToAtom(const double& value) const
    {
        return core()->doubleToAtom(value);
    }

#ifdef VMCFG_FLOAT
    REALLY_INLINE void VectorBaseObject::atomToValue(Atom atom, float& value)
    {
        value = AvmCore::singlePrecisionFloat(atom);
    }

    REALLY_INLINE void VectorBaseObject::atomToValueKnown(Atom atom, float& value)
    {
        value = AvmCore::singlePrecisionFloat(atom);
    }
    
    REALLY_INLINE Atom VectorBaseObject::valueToAtom(const float& value) const
    {
        return core()->floatToAtom(value);
    }

    REALLY_INLINE void VectorBaseObject::atomToValue(Atom atom, float4_t& value)
    {
        AvmCore::float4(&value, atom);
    }
    
    REALLY_INLINE void VectorBaseObject::atomToValueKnown(Atom atom, float4_t& value)
    {
        AvmCore::float4(&value, atom);
    }
    
    REALLY_INLINE Atom VectorBaseObject::valueToAtom(const float4_t& value) const
    {
        return core()->float4ToAtom(value);
    }
#endif

    REALLY_INLINE void VectorBaseObject::atomToValue(Atom atom, OpaqueAtom& value)
    {
        AvmAssert(m_vecClass != NULL);
        atom = avmplus::coerce(toplevel(), atom, m_vecClass->getTypeTraits());
        value = (OpaqueAtom)atom;
    }
    
    REALLY_INLINE void VectorBaseObject::atomToValueKnown(Atom atom, OpaqueAtom& value)
    {
        AvmAssert(m_vecClass != NULL);
        AvmAssert(atom == avmplus::coerce(toplevel(), atom, m_vecClass->getTypeTraits()));
        value = (OpaqueAtom)atom;
    }
    
    REALLY_INLINE Atom VectorBaseObject::valueToAtom(const OpaqueAtom& value) const
    {
        return (Atom)value;
    }

    // ----------------------------

    template<class TLIST>
    REALLY_INLINE typename TLIST::TYPE TypedVectorObject<TLIST>::getUintPropertyFast(uint32_t index) const
    {
        return m_list.get(index);
    }
    
    template<class TLIST>
    REALLY_INLINE void TypedVectorObject<TLIST>::setUintPropertyFast(uint32_t index, typename TLIST::TYPE value)
    {
        m_list.set(index, value);
    }

    template<class TLIST>
    REALLY_INLINE uint32_t TypedVectorObject<TLIST>::get_length() const
    {
        return m_list.length();
    }

    template<class TLIST>
    REALLY_INLINE void TypedVectorObject<TLIST>::_reverse()
    {
        m_list.reverse();
    }

    template<class TLIST>
    REALLY_INLINE bool TypedVectorObject<TLIST>::hasUintProperty(uint32_t index) const
    {
        return _hasUintProperty(index);
    }

    template<class TLIST>
    REALLY_INLINE Atom TypedVectorObject<TLIST>::getUintProperty(uint32_t index) const
    {
        return _getUintProperty(index);
    }

    template<class TLIST>
    REALLY_INLINE void TypedVectorObject<TLIST>::setUintProperty(uint32_t index, Atom value)
    {
        _setUintProperty(index, value);
    }

#ifdef DEBUGGER
    template<class TLIST>
    REALLY_INLINE uint64_t TypedVectorObject<TLIST>::bytesUsed() const
    {
        return ScriptObject::bytesUsed() + m_list.bytesUsed();
    }
#endif

    template<class TLIST>
    REALLY_INLINE void TypedVectorObject<TLIST>::checkReadIndex_u(uint32_t index) const
    {
        uint32_t const limit = m_list.length();
        if (index >= limit)
            throwRangeError_u(index);
    }

    // For signed 'index', the test (index < 0 || uint32_t(index) >= limit) can be
    // accomplished by (uint32_t(index) >= limit), provided that limit < 2^31.  The
    // resulting speedup is worthwhile.
    //
    // We assert statically that the maximum object size divided by the element size will
    // never exceed 2^31-1.  This should always be true at the time of writing (Aug 2011),
    // because the smallest object to go into a Vector is four bytes and the largest object
    // size supported by MMgc is 4GB.
    
    template<class TLIST>
    REALLY_INLINE uint32_t TypedVectorObject<TLIST>::checkReadIndex_i(int32_t index) const
    {
        static_assert((MMgc::GCHeap::kMaxObjectSize / sizeof(typename TLIST::TYPE)) <= 0x7FFFFFFFU,
                      "Optimizations in the vector code depend on vectors being shorter than 2^31 elements");
        uint32_t const limit = m_list.length();
        // See comment above
        if (uint32_t(index) >= limit)   // See comment above
            throwRangeError_i(index);
        return uint32_t(index);
    }

    template<class TLIST>
    REALLY_INLINE uint32_t TypedVectorObject<TLIST>::checkReadIndex_d(double index) const
    {
        int32_t const index_i = int32_t(index);
        uint32_t const index_u = uint32_t(index_i);
        uint32_t const limit = m_list.length();
        if (double(index_i) == index && index_i >= 0)
        {
            if (index_u < limit)
                return index_u;
        }
        throwGetDoubleException(index, limit);
        return 0;
    }

    template<class TLIST>
    REALLY_INLINE void TypedVectorObject<TLIST>::checkWriteIndex_u(uint32_t index) const
    {
        // If we are 'fixed', we can't write past the end.
        // If we are not 'fixed', legal to write exactly 1 past the end, growing as needed.
        // Common case is we're in bounds, so fast-path that.  This matters.
        uint32_t const length = m_list.length();
        if (index >= length)
        {
            uint32_t const limit = length + 1 - uint32_t(m_fixed);
            if (index >= limit)
                throwRangeError_u(index);
        }
    }

#ifdef VMCFG_AOT
    template<class TLIST>
    REALLY_INLINE void TypedVectorObject<TLIST>::_setNativeIntPropertyFast(int32_t index_i, typename TLIST::TYPE value)
    {
        AvmAssertMsg(!IS_FLOAT4_TYPE(typename TLIST::TYPE), "wrong _setNativeIntProperty helper used for Vector.<float4>");
		m_list.replace((uint32_t)index_i, value);
    }

	template<class TLIST>
    REALLY_INLINE bool TypedVectorObject<TLIST>::canTakeFastPath(int32_t index) const
    {
				
		uint32_t const length = m_list.length();
		if (uint32_t(index) >= length)      // See comment above checkReadIndex_i
        {
			return false;
        }
		return true;
	}
#endif

    template<class TLIST>
    REALLY_INLINE uint32_t TypedVectorObject<TLIST>::checkWriteIndex_i(int32_t index) const
    {
        // If we are 'fixed', we can't write past the end.
        // If we are not 'fixed', legal to write exactly 1 past the end, growing as needed.
        // Common case is we're in bounds, so fast-path that.  This matters.
        uint32_t const length = m_list.length();
        if (uint32_t(index) >= length)      // See comment above checkReadIndex_i
        {
            uint32_t const limit = length + 1 - uint32_t(m_fixed);
            if (uint32_t(index) >= limit)   // See comment above checkReadIndex_i
                throwRangeError_i(index);
        }
        return uint32_t(index);
    }

    template<class TLIST>
    REALLY_INLINE uint32_t TypedVectorObject<TLIST>::checkWriteIndex_d(double index) const
    {
        // If we are 'fixed', we can't write past the end.
        // If we are not 'fixed', legal to write exactly 1 past the end, growing as needed.
        int32_t const index_i = int32_t(index);
        uint32_t const index_u = uint32_t(index_i);
        uint32_t const limit = m_list.length() + 1 - uint32_t(m_fixed);
        if (double(index_i) == index && index_i >= 0)
        {
            if (index_u < limit)
                return index_u;
        }
        throwSetDoubleException(index, limit);
        return 0;
    }


    // ----------------------------

    template<class TLIST, uintptr_t align>
    REALLY_INLINE VectorAccessor<TLIST, align>::VectorAccessor(TypedVectorObject<TLIST>* v) : m_vector(v)
    {
    }
    
    template<class TLIST, uintptr_t align>
    REALLY_INLINE typename TLIST::TYPE* VectorAccessor<TLIST, align>::addr()
    {
        if (m_vector == NULL)
            return (typename TLIST::TYPE*)NULL;
        if (align == 0)
            return m_vector->m_list.m_data->entries;
        return (typename TLIST::TYPE*)((uintptr_t(m_vector->m_list.m_data->entries) + (align-1)) & ~(align-1));
    }

    template<class TLIST, uintptr_t align>
    REALLY_INLINE uint32_t VectorAccessor<TLIST, align>::length()
    {
        return (m_vector != NULL) ? m_vector->m_list.length() : 0;
    }
}

#endif /* __avmplus_VectorClass__ */

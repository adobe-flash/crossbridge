/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ArrayObject_inlines__
#define __avmplus_ArrayObject_inlines__

namespace avmplus
{
    REALLY_INLINE Atom ArrayObject::pop()
    {
        return AS3_pop();
    }

    REALLY_INLINE uint32_t ArrayObject::push(Atom *args, int argc)
    {
        return AS3_push(args, argc);
    }

    REALLY_INLINE uint32_t ArrayObject::unshift(Atom *args, int argc)
    {
        return AS3_unshift(args, argc);
    }

    // Performance on some benchmarks (eg Euler.as) is highly sensitive to
    // ArrayObject::getUintProperty being fast; this exists in order to ensure
    // that the implementations of getUintProperty and _getUintProperty/_getIntProperty
    // (used by the JIT) contain inlined code rather than a tail-call.
    REALLY_INLINE Atom ArrayObject::getUintPropertyImpl(uint32_t index) const
    {
        // NB: we constrain the dense area to indices <= 0x7FFFFFFF,
        // so this test will always fail if the we are sealed or sparse,
        // due to IS_SEALED and IS_SPARSE being carefully chosen values.
        uint32_t const denseIdx = index - m_denseStart;
        if (denseIdx < m_denseArray.length())
        {
            Atom result = m_denseArray.get(denseIdx);
            if (result != atomNotFound)
                return result;
            // else, a hole in the dense area, must fall thru and search the hashtable/proto chain
        }

        Atom result = ScriptObject::getUintProperty(index);
        AvmAssert(result != atomNotFound);
        return result;
    }

    /*virtual*/
    REALLY_INLINE void ArrayObject::setUintProperty(uint32_t index, Atom value)
    {
        _setUintProperty(index, value);
    }

    /*virtual*/
    REALLY_INLINE Atom ArrayObject::getUintProperty(uint32_t index) const
    {
        return getUintPropertyImpl(index);
    }

    // Non-virtual members for ActionScript method implementation.
    // Always calls thru to the virtual method to allow subclasses to override in C++.
    REALLY_INLINE uint32_t ArrayObject::get_length() const
    {
        return getLength();
    }

    REALLY_INLINE void ArrayObject::set_length(uint32_t newLength)
    {
        setLength(newLength);
    }

} // namespace avmplus

#endif /* __avmplus_ArrayObject_inlines__ */

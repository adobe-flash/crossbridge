/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// inline functions that manipulate Atom go here.

#ifndef __avmplus_atom_inlines__
#define __avmplus_atom_inlines__

namespace avmplus
{
    class ScriptObject;
#ifdef DEBUG
    bool testIsObject(Atom atom);
#endif

    using namespace AtomConstants;

    // Macros for these are defined in atom.h.  Measurements show that macros have an edge
    // over inline functions with Visual C++ 2008, at least.
    //
    //inline AtomConstants::AtomKind atomKind(Atom a) { return AtomConstants::AtomKind(uintptr_t(a) & 7); }
    //inline void* atomPtr(Atom a) { return (void*)(uintptr_t(a) & ~7); }

    REALLY_INLINE bool atomIsIntptr(Atom atom)
    {
        return atomKind(atom) == kIntptrType;
    }

    REALLY_INLINE bool atomIsBothIntptr(Atom a, Atom b)
    {
        return ((((uintptr_t(a) ^ kIntptrType) | (uintptr_t(b) ^ kIntptrType)) & 7) == 0);
    }

    REALLY_INLINE bool atomCanBeInt32(Atom atom)
    {
        AvmAssert(atomIsIntptr(atom));
#ifdef AVMPLUS_64BIT
        intptr_t const i = atomGetIntptr(atom);
        int32_t const i32 = int32_t(i);
        return i == i32;
#else
        // int atoms always fit in int32_t on 32-bit
        (void)atom;
        return true;
#endif
    }

    REALLY_INLINE bool atomCanBeUint32(Atom atom)
    {
        AvmAssert(atomIsIntptr(atom));
#ifdef AVMPLUS_64BIT
        intptr_t const i = atomGetIntptr(atom);
        uint32_t const u32 = uint32_t(i);
        return i == intptr_t(u32);
#else
        // int atoms always fit in uint32_t on 32-bit, if they are >= 0
        return atom >= 0;
#endif
    }

    REALLY_INLINE intptr_t atomGetIntptr(Atom a)
    {
        AvmAssert(atomIsIntptr(a));
        AvmAssert(atomIsValidIntptrValue(intptr_t(a) >> 3));
        return intptr_t(a) >> 3;
    }

    REALLY_INLINE bool atomIsValidIntptrValue(const intptr_t i)
    {
        return (((i << atomSignExtendShift) >> atomSignExtendShift) == i);
    }

    REALLY_INLINE bool atomIsValidIntptrValue_u(const uintptr_t u)
    {
        const uintptr_t MASK = ~uintptr_t(atomMaxIntValue);
        return (u & MASK) == 0;
    }

    REALLY_INLINE Atom atomFromIntptrValue(const intptr_t i)
    {
        AvmAssert(atomIsValidIntptrValue(i));
        return ((Atom)i)<<3 | kIntptrType;
    }

    REALLY_INLINE Atom atomFromIntptrValue_u(const uintptr_t u)
    {
        AvmAssert(atomIsValidIntptrValue_u(u));
        return ((Atom)u)<<3 | kIntptrType;
    }

    // unwrap an atom and return a ScriptObject*.  Doesn't use atomPtr(), because
    // using subtract allows the expression to be folded with other pointer math,
    // unlike the & ~7 in atomPtr().
    REALLY_INLINE avmplus::ScriptObject* atomObj(Atom a)
    {
        AvmAssert(avmplus::testIsObject(a)); // proper type and not null or undefined
        return (avmplus::ScriptObject*) (uintptr_t(a) - kObjectType);
    }

    REALLY_INLINE bool atomGetBoolean(Atom a)
    {
        AvmAssert(a == trueAtom || a == falseAtom);
        return (a >> 3) != 0;
    }

#ifdef _DEBUG
    // In non-debug builds this is a macro in atom.h
    REALLY_INLINE uint8_t bibopKind(const Atom a)
    {
        AvmAssert(atomKind(a) == kSpecialBibopType);
        uint8_t const k = *(uint8_t*)(uintptr_t(a) & kBibopTypePtrMask);
        AvmAssert(k == kBibopUndefined || k == kBibopFloatType  || k == kBibopFloat4Type);
        return k;
    }
#endif
        
} // namespace

#endif // __avmplus_atom_inlines__

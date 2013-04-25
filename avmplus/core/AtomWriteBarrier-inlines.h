/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_AtomWriteBarrier_inlines__
#define __avmplus_AtomWriteBarrier_inlines__

namespace avmplus
{
    REALLY_INLINE AtomWB::AtomWB() : AtomWBCore(nullObjectAtom)
    {
        // nothing
    }

    REALLY_INLINE AtomWB::AtomWB(Atom t)  // : AtomWBCore(t) -- not necessary, atomWriteBarrier_ctor handles it
    {
        MMgc::GC* const gc = MMgc::GC::GetGC(this);
        void* const container = gc->FindBeginningFast(this);
        AvmCore::atomWriteBarrier_ctor(gc, container, &m_atom, t);
    }

    REALLY_INLINE AtomWB::~AtomWB()
    {
        AvmCore::atomWriteBarrier_dtor(&m_atom);
    }

    REALLY_INLINE Atom AtomWB::operator=(Atom tNew)
    {
        MMgc::GC* const gc = MMgc::GC::GetGC(this);
        void* const container = gc->FindBeginningFast(this);
        set(gc, container, tNew);
        return tNew;
    }

    // if you know the container, this saves a call to FindBeginningFast...
    // which adds up in (e.g.) heavy E4X usage
    REALLY_INLINE void AtomWB::set(MMgc::GC* gc, const void* container, Atom atomNew)
    {
        if (m_atom != atomNew)
        {
            AvmCore::atomWriteBarrier(gc, container, &m_atom, atomNew);
        }
    }

    REALLY_INLINE AtomWB::operator const Atom&() const
    {
        return m_atom;
    }
    
    REALLY_INLINE Atom AtomWB::set(Atom atomNew)
    {
        if (m_atom != atomNew)
        {
            MMgc::GC* gc = MMgc::GC::GetGC(this);
            AvmCore::atomWriteBarrier(gc, gc->FindBeginningFast(this), &m_atom, atomNew);
        }
        return atomNew;
    }
}

#endif /* __avmplus_AtomWriteBarrier_inlines__ */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_AtomWriteBarrier__
#define __avmplus_AtomWriteBarrier__

namespace avmplus
{
    // Optimized Atom write barrier
    class AtomWB : public MMgc::AtomWBCore
    {
    public:
        explicit AtomWB();
        explicit AtomWB(Atom t);
        ~AtomWB();
        Atom operator=(Atom tNew);
        // if you know the container, this saves a call to FindBeginningFast...
        // which adds up in (e.g.) heavy E4X usage
        void set(MMgc::GC* gc, const void* container, Atom atomNew);
        operator const Atom&() const;
        
    private:
        explicit AtomWB(const AtomWB& toCopy); // unimplemented
        void operator=(const AtomWB& wb); // unimplemented
        Atom set(Atom atomNew);
    };
#define ATOM_WB AtomWB
#define WBATOM(gc, c, a, v) AvmCore::atomWriteBarrier(gc, c, a, v)
}

#endif /* __avmplus_AtomWriteBarrier__ */

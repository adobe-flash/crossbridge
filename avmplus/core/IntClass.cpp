/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    IntClass::IntClass(VTable* cvtable) : ClassClosure(cvtable)
    {
        toplevel()->_intClass = this;
        // prototype objects are always vanilla objects.
        createVanillaPrototype();
    }

    Atom IntClass::construct(int argc, Atom* argv)
    {
        if (argc == 0)
            return zeroIntAtom;   // yep this is zero atom
        else
            return core()->intAtom(argv[1]);
        // TODO ArgumentError if argc > 1
    }

    UIntClass::UIntClass(VTable* cvtable) : ClassClosure(cvtable)
    {
        toplevel()->_uintClass = this;
        // prototype objects are always vanilla objects.
        createVanillaPrototype();
    }

    Atom UIntClass::construct(int argc, Atom* argv)
    {
        if (argc == 0)
            return zeroIntAtom;   // yep this is zero atom
        else
            return core()->uintAtom(argv[1]);
        // TODO ArgumentError if argc > 1
    }

}

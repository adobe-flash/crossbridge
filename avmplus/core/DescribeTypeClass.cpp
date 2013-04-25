/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"
#include "TypeDescriber.h"

namespace avmplus
{
    /*static*/ Atom DescribeTypeClass::describeTypeJSON(ScriptObject* self, Atom v, uint32_t flags)
    {
        TypeDescriber td(self->toplevel());
        ScriptObject* o = td.describeType(v, flags);
        return o ? o->atom() : nullObjectAtom;
    }

    /*static*/ Stringp DescribeTypeClass::getQualifiedClassName(ScriptObject* self, Atom v)
    {
        TypeDescriber td(self->toplevel());
        return td.getQualifiedClassName(v);
    }

    /*static*/ Stringp DescribeTypeClass::getQualifiedSuperclassName(ScriptObject* self, Atom v)
    {
        TypeDescriber td(self->toplevel());
        return td.getQualifiedSuperclassName(v);
    }
}

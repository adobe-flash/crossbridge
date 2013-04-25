/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ClassClosure_inlines__
#define __avmplus_ClassClosure_inlines__

namespace avmplus
{

REALLY_INLINE ScriptObject* ClassClosure::prototypePtr()
{
    return m_prototype;
}

REALLY_INLINE VTable* ClassClosure::ivtable() const
{
    return vtable->ivtable;
}

// Called from C++ to alloc a new instance.  Generated code calls createInstanceProc directly.
REALLY_INLINE ScriptObject* ClassClosure::newInstance()
{
    VTable* const ivtable = this->vtable->ivtable;
    return ivtable->createInstanceProc(this);
}

REALLY_INLINE size_t ClassClosure::getExtraSize() const
{
    VTable* const ivtable = this->vtable->ivtable;
    AvmAssert(ivtable != NULL);
    return ivtable->getExtraSize();
}

}

#endif /* __avmplus_ClassClosure_inlines__ */

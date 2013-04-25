/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ScriptObject_inlines__
#define __avmplus_ScriptObject_inlines__


namespace avmplus
{
REALLY_INLINE /*static*/ ScriptObject* ScriptObject::create(MMgc::GC* gc, VTable* vtable, ScriptObject* delegate)
{
    return new (gc, MMgc::kExact, vtable->getExtraSize()) ScriptObject(vtable, delegate);
}

REALLY_INLINE /*static*/ ScriptObject* ScriptObject::create(MMgc::GC* gc, VTable* vtable, ScriptObject* delegate, int htCapacity)
{
    return new (gc, MMgc::kExact, vtable->getExtraSize()) ScriptObject(vtable, delegate, htCapacity);
}

REALLY_INLINE ScriptObject* ScriptObject::getDelegate() const
{
    return delegate;
}

REALLY_INLINE void ScriptObject::setDelegate(ScriptObject *d)
{
    delegate = d;
}

REALLY_INLINE Atom ScriptObject::atom() const
{
    return kObjectType|(uintptr_t)this;
}

REALLY_INLINE /*virtual*/ Atom ScriptObject::toAtom() const
{
    return atom();
}

REALLY_INLINE Traits* ScriptObject::traits() const
{
    return vtable->traits;
}

REALLY_INLINE AvmCore* ScriptObject::core() const
{
    return vtable->traits->core;
}

REALLY_INLINE MMgc::GC* ScriptObject::gc() const
{
    return core()->GetGC();
}

REALLY_INLINE Toplevel* ScriptObject::toplevel() const
{
    return vtable->toplevel();
}

REALLY_INLINE InlineHashtable* ScriptObject::getTableNoInit() const
{
    InlineHashtable* iht = (InlineHashtable*)((uint8_t*)this + vtable->traits->getHashtableOffset());
    if(!vtable->traits->isDictionary())
    {
        return iht;
    }
    else
    {
        HeapHashtable** hht = (HeapHashtable**)iht;
        //DictionaryObjects store pointer to HeapHashtable at
        //the hashtable offset
        return ((*hht) ? (*hht)->get_ht() : NULL);
    }
}

REALLY_INLINE Atom ScriptObject::getStringProperty(Stringp name) const
{
    AvmAssert(name != NULL && name->isInterned());
    return getAtomProperty(name->atom());
}

REALLY_INLINE Atom ScriptObject::getStringPropertyFromProtoChain(Stringp name, ScriptObject* protochain, Traits *origObjTraits) const
{
    return getAtomPropertyFromProtoChain(name->atom(), protochain, origObjTraits);
}

REALLY_INLINE void ScriptObject::setStringProperty(Stringp name, Atom value)
{
    setAtomProperty(name->atom(), value);
}

REALLY_INLINE bool ScriptObject::deleteStringProperty(Stringp name)
{
    return deleteAtomProperty(name->atom());
}

REALLY_INLINE bool ScriptObject::hasStringProperty(Stringp name) const
{
    AvmAssert(name != NULL && name->isInterned());
    return hasAtomProperty(name->atom());
}

REALLY_INLINE bool ScriptObject::getStringPropertyIsEnumerable(Stringp name) const
{
    AvmAssert(name != NULL && name->isInterned());
    return getAtomPropertyIsEnumerable(name->atom());
}

REALLY_INLINE void ScriptObject::setStringPropertyIsEnumerable(Stringp name, bool enumerable)
{
    setAtomPropertyIsEnumerable(name->atom(), enumerable);
}

REALLY_INLINE /*virtual*/ MethodClosure* ScriptObject::toMethodClosure() const
{
    return NULL;
}

REALLY_INLINE bool ScriptObject::isMethodClosure() const
{
    return toMethodClosure() != NULL;
}

REALLY_INLINE /*virtual*/ CodeContext* ScriptObject::getFunctionCodeContext() const
{
    AvmAssert(0);
    return NULL;
}

REALLY_INLINE /*virtual*/ ArrayObject* ScriptObject::toArrayObject()
{
    return NULL;
}

#if defined(DEBUGGER) || defined(VMCFG_AOT)
REALLY_INLINE /*virtual*/ MethodEnv* ScriptObject::getCallMethodEnv()
{
    return NULL;
}
#endif

REALLY_INLINE /*virtual*/ ClassClosure* ScriptObject::toClassClosure()
{
    return NULL;
}

}

#endif /* __avmplus_ScriptObject_inlines__ */

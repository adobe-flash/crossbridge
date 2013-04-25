/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

using namespace MMgc;

namespace avmplus
{
    DictionaryClass::DictionaryClass(VTable *vtable)
    : ClassClosure(vtable)
    {
        createVanillaPrototype();
        vtable->traits->itraits->set_isDictionary();
    }

    DictionaryObject::DictionaryObject(VTable *vtable, ScriptObject *delegate)
        : ScriptObject(vtable, delegate)
    {
        GCAssert(vtable->traits->isDictionary());
    }

    void DictionaryObject::gcTraceHook_DictionaryObject(MMgc::GC *gc)
    {
        // This code was ripped out of ScriptObject::getTableNoInit
        union {
            uint8_t* p;
            HeapHashtable** hht;
        };
        p = (uint8_t*)this + vtable->traits->getHashtableOffset();
        gc->TraceLocation(hht);
    }
        
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=666720
    HeapHashtable* DictionaryObject::getHeapHashtable() const
    {
        uint32_t const htoffset = vtable->traits->getHashtableOffset();
        HeapHashtable** const p = (HeapHashtable**)(uintptr_t(this) + htoffset);
        HeapHashtable* ht = *p;
        if (ht == NULL)
        {
            MMgc::GC* gc = this->gc();
            // if we are lazy-initing here, it means our ctor was never called:
            // so, just assume weakKeys = false.
            ht = HeapHashtable::create(gc);
            WB(gc, this, p, ht);
        }
        AvmAssert(ht != NULL);
        return ht;
    }
#endif

    void DictionaryObject::init(bool weakKeys)
    {
        GCAssert(vtable->traits->isDictionary());
        MMgc::GC* gc = this->gc();

        HeapHashtable* ht = weakKeys ? WeakKeyHashtable::create(gc) : HeapHashtable::create(gc);

        //store pointer of newly created hashtable, encapsulated with writebarrier,
        //at the hashtable offset address of the corresponding traits
        union {
            uint8_t* p;
            HeapHashtable** hht;
        };
        p = (uint8_t*)this + vtable->traits->getHashtableOffset();
        WB(gc, this, hht, ht);
    }

    Atom FASTCALL DictionaryObject::getKeyFromObject(Atom key) const
    {
        AvmAssert(AvmCore::isObject(key));
        ScriptObject* obj = AvmCore::atomToScriptObject(key);
        AvmAssert(Traits::getBuiltinType(obj->traits()) != BUILTIN_qName);
        AvmAssert(MMgc::GC::Size(obj) >= sizeof(ScriptObject));
        (void)obj;

        // This commented-out code probably pertains to Bugzilla 507699:
        // "Dictionary key of Xml type are not matching."
        //
        // FIXME: this doesn't work, need to convert back to an XMLObject
        // on the way out or intern XMLObject's somehow
        //if(AvmCore::isXML(key))
        //  key = AvmCore::genericObjectToAtom(AvmCore::atomToXML(key));

        return key;
    }

    Atom DictionaryObject::getAtomProperty(Atom key) const
    {
        if (AvmCore::isObject(key))
        {
            return getHeapHashtable()->get(getKeyFromObject(key));
        }

        return ScriptObject::getAtomProperty(key);
    }

    bool DictionaryObject::hasAtomProperty(Atom key) const
    {
        if (AvmCore::isObject(key))
        {
            return getHeapHashtable()->contains(getKeyFromObject(key));
        }

        return ScriptObject::hasAtomProperty(key);
    }

    bool DictionaryObject::deleteAtomProperty(Atom key)
    {
        if (AvmCore::isObject(key))
        {
            getHeapHashtable()->remove(getKeyFromObject(key));
            return true;
        }

        return ScriptObject::deleteAtomProperty(key);
    }

    void DictionaryObject::setAtomProperty(Atom key, Atom value)
    {
        if (AvmCore::isObject(key))
        {
            getHeapHashtable()->add(getKeyFromObject(key), value);
            return;
        }

        ScriptObject::setAtomProperty(key, value);
    }

    Atom DictionaryObject::nextName(int index)
    {
        AvmAssert(index > 0);

        HeapHashtable* hht = getHeapHashtable();
        Atom m = hht->keyAt(index);
        Atom k = AvmCore::isNullOrUndefined(m) ? nullStringAtom : m;

        if (AvmCore::isGenericObject(k) && hht->weakKeys())
        {
            GCWeakRef* ref = (GCWeakRef*)AvmCore::atomToGenericObject(k);
            union {
                GCObject* key_o;
                ScriptObject* key;
            };
            key_o = ref->get();
            if (key)
            {
                AvmAssert(key->traits() != NULL);
                return key->atom();
            }

            return undefinedAtom;
        }

        return k;
    }

    Atom DictionaryObject::nextValue(int index)
    {
        AvmAssert(index > 0);

        HeapHashtable* hht = getHeapHashtable();
        Atom m = hht->keyAt(index);
        if (AvmCore::isNullOrUndefined(m))
            return nullStringAtom;
        return hht->valueAt(index);
    }

    int DictionaryObject::nextNameIndex(int index)
    {
        AvmAssert(index >= 0);

        // hht could be null if you break in debugger in a subclasses constructor before super
        // has been called -- let's do it in all builds, it's better than crashing.
        HeapHashtable* hht = getHeapHashtable();
        return hht ? hht->next(index) : 0;
    }
}

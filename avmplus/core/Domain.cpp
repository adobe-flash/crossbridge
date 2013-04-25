/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    Domain::Domain(AvmCore* core, Domain* base, uint32_t baseCount)
        : m_loadedTraits(MultinameTraitsHashtable::create(core->GetGC()))
        , m_cachedTraits(MultinameTraitsHashtable::create(core->GetGC()))
        , m_loadedScripts(MultinameMethodInfoHashtable::create(core->GetGC()))
        , m_cachedScripts(MultinameMethodInfoHashtable::create(core->GetGC()))
        , m_parameterizedTypes(WeakKeyHashtable::create(core->GetGC()))
        , m_baseCount(baseCount)
    {
        WB(core->GetGC(), this, &m_bases[0], this);
        for (uint32_t i = 1; i < baseCount; ++i)
        {
            WB(core->GetGC(), this, &this->m_bases[i], base->m_bases[i-1]);
        }
    }

    Domain* Domain::newDomain(AvmCore* core, Domain* base)
    {
        uint32_t baseCount = (base ? base->m_baseCount : 0) + 1;
        // Note that we deliberately overallocate by one here (the proper
        // amount is baseCount-1) so that we always have one, zeroed-out
        // entry at the end. This allows us to always use "m_bases[1]"
        // to get our immediate base, even if our base is NULL, thus
        // avoiding a check in the implementation of base().
        uint32_t extra = baseCount * sizeof(Domain*);
        return new (core->GetGC(), MMgc::kExact, extra) Domain(core, base, baseCount);
    }

    ClassClosure* Domain::getParameterizedType(ClassClosure* type)
    {
        ClassClosure* result = NULL;
        AvmAssert(type != NULL);
        if (type)
        {
            Atom value = m_parameterizedTypes->get(type->atom());
            if (AvmCore::isGenericObject(value))
            {
                MMgc::GCWeakRef* wr = (MMgc::GCWeakRef*)AvmCore::atomToGenericObject(value);
                if (wr)
                {
                    result = (ClassClosure*)wr->get();
                }
            }
        }
        return result;
    }

    bool Domain::isParameterizedTypePresent(ClassClosure* type)
    {
        bool result = false;
        if (type)
        {
            Atom value = m_parameterizedTypes->get(type->atom());
            if (AvmCore::isGenericObject(value))
            {
                MMgc::GCWeakRef* wr = (MMgc::GCWeakRef*)AvmCore::atomToGenericObject(value);
                if (wr)
                {
                    result = !(wr->isNull());
                }
            }
        }
        return result;
    }

    void Domain::addParameterizedType(ClassClosure* type, ClassClosure* parameterizedType)
    {
        AvmAssert(type && parameterizedType);
        if (type && parameterizedType)
        {
            AvmAssert(!isParameterizedTypePresent(type));
            // Note that what we really need here is fixing the problems with our weak tables.
            // If there exists a cycle in the weak table ('value' indirectly pointing to the weak 'key'),
            // the weak key will not get collected even if there are no references to 
            // it from elsewhere and hence the table entry will not be deleted, which
            // will in turn lead to value not getting collected.
            // 
            // As a short term fix, we'll just use WeakKeyHashtable with explicit GCWeakRef usage
            // for the value.
            MMgc::GCWeakRef* wr = parameterizedType ? parameterizedType->GetWeakRef() : NULL;
            m_parameterizedTypes->add(type->atom(), AvmCore::genericObjectToAtom(wr));
        }
    }
}



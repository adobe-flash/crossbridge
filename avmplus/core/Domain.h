/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Domain__
#define __avmplus_Domain__

namespace avmplus
{
    class GC_CPP_EXACT(Domain, MMgc::GCTraceableObject)
    {
        friend class DomainMgr;

    private:
        Domain(AvmCore* core, Domain* base, uint32_t baseCount);

    public:
        static Domain* newDomain(AvmCore* core, Domain* base);

        // see note in newDomainEnv about why this is always valid, even if m_baseCount == 1
        REALLY_INLINE Domain* base() const { return m_bases[1]; }

        // returns NULL if the type doesn't exist yet.
        ClassClosure* getParameterizedType(ClassClosure* type);
        void addParameterizedType(ClassClosure* type, ClassClosure* parameterizedType);
    private:
        // Returns false only if (getParameterizedType(type) == NULL).
        // Returns true when 'type' present (but it is weakly held;
        // later 'getParameterizedType(type)' calls could be null).
        //
        // Use when you want to know if the type is present, but do
        // not want inadvertantly to cause it to be retained by the GC
        // if it is otherwise reclaimable.
        bool isParameterizedTypePresent(ClassClosure* type);
    public:
        GCRef<MultinameMethodInfoHashtable> getLoadedScripts() { return m_loadedScripts; }

    GC_DATA_BEGIN(Domain)

    private:
        // "loaded" Traits/Scripts are the Traits/ScriptEnvs that are actually
        // defined in this Domain. "cached" Traits/Scripts are the ones that
        // actually should be used for a given name lookup; the cached versions
        // take precedence over the loaded ones (on a freeze-on-first-use basis)
        // to ensure that the types associated with a name can't change as new
        // Domains are loaded. See DomainMgr for more info.
        GCMember<MultinameTraitsHashtable>     GC_POINTER(m_loadedTraits);
        GCMember<MultinameTraitsHashtable>     GC_POINTER(m_cachedTraits);
        GCMember<MultinameMethodInfoHashtable> GC_POINTER(m_loadedScripts);
        GCMember<MultinameMethodInfoHashtable> GC_POINTER(m_cachedScripts);
        GCMember<WeakKeyHashtable>             GC_POINTER(m_parameterizedTypes);
        // note that m_baseCount is actually the number of bases, plus one:
        // we always add ourself (!) to the front of the list, to simplify
        // processing in DomainMgr.
        uint32_t const                      m_baseCount; // number of entries in m_bases
        Domain*                             GC_POINTERS_SMALL(m_bases[1], m_baseCount);

    GC_DATA_END(Domain)
    };

}

#endif /* __avmplus_Domain__ */

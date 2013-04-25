/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_NamespaceSet_inlines__
#define __avmplus_NamespaceSet_inlines__


namespace avmplus
{
    REALLY_INLINE NamespaceSetIterator::NamespaceSetIterator(const NamespaceSet* n) :
        nsset(n), i(0)
    {
    }

    REALLY_INLINE bool NamespaceSetIterator::hasNext() const
    {
        return i < nsset->count();
    }

    REALLY_INLINE Namespacep NamespaceSetIterator::next()
    {
        AvmAssert(i < nsset->count());
        return nsset->nsAt(i++);
    }

    REALLY_INLINE NamespaceSet::NamespaceSet()
    {
        /* nothing */
    }

    REALLY_INLINE bool NamespaceSet::containsAnyPublicNamespace() const
    {
        return (_countAndFlags & 1) != 0;
    }

    REALLY_INLINE bool NamespaceSet::contains(Namespace* ns) const
    {
        for (uint32_t i=0,n=count(); i < n; i++)
            if (_namespaces[i] == ns)
                return true;
        return false;
    }

    REALLY_INLINE uint32_t NamespaceSet::count() const
    {
        return (_countAndFlags >> 1);
    }

    REALLY_INLINE Namespacep NamespaceSet::nsAt(uint32_t i) const
    {
        AvmAssert(i < count());
        AvmAssert(_namespaces[i] != NULL);
        return _namespaces[i];
    }

    REALLY_INLINE void NamespaceSet::_initNsAt(uint32_t i, Namespacep ns)
    {
        AvmAssert(i < count());
        AvmAssert(_namespaces[i] == NULL);
        AvmAssert(ns != NULL);
        WBRC(MMgc::GC::GetGC(this), this, &_namespaces[i], ns);
        // note that this flag is 'sticky'; it relies on a given slot being
        // initialized exactly once. (NamespaceSets are immutable once created,
        // but creation involves multiple calls to this method.)
        if (ns->isPublic())
            _countAndFlags |= 1;
    }
}

#endif /* __avmplus_NamespaceSet_inlines__ */

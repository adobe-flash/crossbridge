/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_NamespaceSet__
#define __avmplus_NamespaceSet__


namespace avmplus
{
    class NamespaceSetIterator
    {
    private:
        const NamespaceSet* const nsset;
        uint32_t i;
    public:
        NamespaceSetIterator(const NamespaceSet* n);
        bool hasNext() const;
        Namespacep next();
    };

    /**
     * NamespaceSet is a reference to 0 or more namespaces.  It consists
     * of a list of namespaces.
     */
    class GC_CPP_EXACT(NamespaceSet, MMgc::GCTraceableObject)
    {
        friend class AbcParser;
        friend class AvmCore;
        friend class Toplevel;

        GC_DATA_BEGIN(NamespaceSet)
        
    private:
        // hi 31 bits: count
        // lo bit: containsAnyPublic flag
        uint32_t _countAndFlags;
        Namespacep GC_POINTERS_SMALL(_namespaces[1], "count()");

        GC_DATA_END(NamespaceSet)
        
    private:
        NamespaceSet();

        // these are only for use by friends
        static NamespaceSet* _create(MMgc::GC* gc, uint32_t count);
        void _initNsAt(uint32_t i, Namespacep ns);

    public:
        static const NamespaceSet* create(MMgc::GC* gc, Namespacep ns);
        bool contains(Namespace* ns) const;
        bool containsAnyPublicNamespace() const;
        uint32_t count() const;
        Namespacep nsAt(uint32_t i) const;

//#ifdef AVMPLUS_VERBOSE
    public:
        PrintWriter& print(PrintWriter& prw) const;
//#endif
    };
}

#endif /* __avmplus_NamespaceSet__ */

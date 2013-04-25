/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_MultinameHashtable_impl__
#define __avmplus_MultinameHashtable_impl__

using namespace MMgc;

namespace avmplus
{
    template <class VALUE_TYPE, class VALUE_WRITER>
    bool MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::gcTrace(MMgc::GC *gc, size_t cursor)
    {
        (void)cursor;
        gc->TraceLocation(&m_quads);
        return false;
    }
    
    template <class VALUE_TYPE, class VALUE_WRITER>
    void MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::grow()
    {
        // double our table
        int capacity = numQuads*2;
        MMgc::GC* gc = MMgc::GC::GetGC(this);
        MMGC_MEM_TYPE(this);
        QuadContainer<VALUE_TYPE>* newAtoms = QuadContainer<VALUE_TYPE>::create(gc, capacity);
        rehash(m_quads->quads, numQuads, newAtoms->quads, capacity);
        freeQuads(gc);
        WB(gc, this, &m_quads, newAtoms);
        numQuads = capacity;
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    VALUE_TYPE MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::getMulti(const Multiname* mname) const
    {
        // multiname must not be an attr name, have wildcards, or have runtime parts.
        AvmAssert(mname->isBinding() && !mname->isAnyName());

        if (!mname->isNsset())
            return get(mname->getName(), mname->getNamespace());
        else
            return get(mname->getName(), mname->getNsset());
    }

    // return the NS that unambigously matches in "match" (or null for none/ambiguous)
    template <class VALUE_TYPE, class VALUE_WRITER>
    VALUE_TYPE MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::getMulti(const Multiname& mname, Namespacep& match) const
    {
        // multiname must not be an attr name, have wildcards, or have runtime parts.
        AvmAssert(mname.isBinding() && !mname.isAnyName());

        if (!mname.isNsset())
        {
            VALUE_TYPE b = get(mname.getName(), mname.getNamespace());
            match = (b != NULL) ? mname.getNamespace() : NULL;
            return b;
        }
        else
        {
            const Quad<VALUE_TYPE>* q = getNSSet(mname.getName(), mname.getNsset());
            match = q->ns;
            return q->value;
        }
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::MultinameHashtable(int capacity) :
        m_quads(NULL),
        size(0),
        numQuads(0)
    {
        // We really want alignment and size of the Quad to be tightly controlled.
        MMGC_STATIC_ASSERT(sizeof(Quad<VALUE_TYPE>) == 4*sizeof(uintptr_t));

        // Code near the end of MultinameHashtable::put assumes that API will fit into 31 bits
        MMGC_STATIC_ASSERT(kApiVersion_count <= 0x7fffffff);

        Init(capacity);
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    void MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::Init(int capacity)
    {
        if (capacity)
        {
            numQuads = MathUtils::nextPowerOfTwo(capacity);

            MMgc::GC* gc = MMgc::GC::GetGC(this);

            AvmAssert(numQuads > 0);
            MMGC_MEM_TYPE(this);
            QuadContainer<VALUE_TYPE>* newAtoms = QuadContainer<VALUE_TYPE>::create(gc, numQuads);
            WB(gc, this, &m_quads, newAtoms);
        }
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    void MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::freeQuads(MMgc::GC* gc)
    {
        QuadContainer<VALUE_TYPE>* quads = m_quads;
        m_quads = NULL;     // Avoid dangling m_quad
        gc->Free(quads);
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    bool MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::isFull() const
    {
        // 0.80 load factor
        return 5*(size+1) >= numQuads*4;
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    int MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::find(Stringp name, Namespacep ns, const Quad<VALUE_TYPE>* t, unsigned m)
    {
        AvmAssert(ns->getURI()->isInterned());
        AvmAssert(name != NULL && ns != NULL);
        AvmAssert(name->isInterned());

        // this is a quadratic probe but we only hit every third slot since those hold keys.
        int n = 7;

        int bitmask = (m - 1);

        // Note: Mask off MSB to avoid negative indices.  Mask off bottom
        // 3 bits because it doesn't contribute to hash.  Quad it
        // because names, namespaces, and values are stored adjacently.
        unsigned i = ((0x7FFFFFF8 & (uintptr_t)name) >> 3) & bitmask;

        Stringp k;
        while (((k=t[i].name) != name || !t[i].matchNS(ns)) && k != NULL)
        {
            i = (i + (n++)) & bitmask;          // quadratic probe
        }
        return i;
    }

    template <class VALUE_TYPE>
    REALLY_INLINE QuadContainer<VALUE_TYPE>* QuadContainer<VALUE_TYPE>::create(MMgc::GC* gc, uint32_t capacity)
    {
        return new (gc, MMgc::kExact, sizeof(Quad<VALUE_TYPE>) * (capacity-1)) QuadContainer(capacity);
    }
    
    template <class VALUE_TYPE>
    bool QuadContainer<VALUE_TYPE>::gcTrace(MMgc::GC* gc, size_t cursor)
    {
        (void)cursor;
        for ( uint32_t i=0 ; i < capacity ; i++ ) {
            Quad<VALUE_TYPE>& q = quads[i];
            gc->TraceLocation(&q.name);
            gc->TraceLocation(&q.ns);
            gc->TraceConservativeLocation((uintptr_t*)&q.value);
        }
        return false;
    }

    /**
     * since identifiers are always interned strings, they can't be 0,
     * so we can use 0 as the empty value.
     */
    static const Stringp EMPTY = NULL;

    template <class VALUE_TYPE, class VALUE_WRITER>
    const Quad<VALUE_TYPE>* MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::getNSSet(Stringp mnameName, NamespaceSetp nsset) const
    {
        static const Quad<VALUE_TYPE> kBindNone = { NULL, NULL, NULL, 0 };
        static const Quad<VALUE_TYPE> kBindAmbiguous = { NULL, NULL, (VALUE_TYPE)-1, 0 };

        int nsCount = nsset->count();
        int j;

        const Quad<VALUE_TYPE>* match = &kBindNone;
        VALUE_TYPE matchValue = match->value;

        // this is a quadratic probe but we only hit every third slot since those hold keys.
        int n = 7;
        int bitMask = numQuads - 1;

        // Note: Mask off MSB to avoid negative indices.  Mask off bottom
        // 3 bits because it doesn't contribute to hash.  Quad it
        // because names, namespaces, and values are stored adjacently.
        unsigned i = ((0x7FFFFFF8 & (uintptr_t)mnameName)>>3) & bitMask;
        Stringp atomName;

        const Quad<VALUE_TYPE>* t = m_quads->quads;
        while ((atomName = t[i].name) != EMPTY)
        {
            if (atomName == mnameName)
            {
                for (j=0; j < nsCount; j++)
                {
                    Namespacep ns = nsset->nsAt(j);
                    AvmAssert(ns->getURI()->isInterned());
                    if (t[i].matchNS(ns))
                    {
                        match = &t[i];
                        matchValue = match->value;
                        goto found1;
                    }
                }
            }

            i = (i + (n++)) & bitMask;          // quadratic probe
        }

        return &kBindNone;

found1:
        if (t[i].multiNS())
        {
            int k = (i + (n++)) & bitMask;          // quadratic probe
            while ((atomName = t[k].name) != EMPTY)
            {
                if (atomName == mnameName)
                {
                    AvmAssert(t[k].ns->getURI()->isInterned());
                    for (j=0; j < nsCount; j++)
                    {
                        Namespacep ns = nsset->nsAt(j);
                        if (t[k].matchNS(ns) && matchValue != t[k].value)
                        {
                            return &kBindAmbiguous;

                        }
                    }
                }
                k = (k + (n++)) & bitMask;          // quadratic probe
            }
        }
        return match;
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    VALUE_TYPE MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::get(Stringp name, Namespacep ns) const
    {
        AvmAssert(ns->getURI()->isInterned());
        const Quad<VALUE_TYPE>* t = m_quads->quads;
        int i = find(name, ns, t, numQuads);
        if (t[i].name == name)
        {
            const Quad<VALUE_TYPE>& tf = t[i];
            AvmAssert(tf.matchNS(ns));
            return tf.value;
        }
        return (VALUE_TYPE)NULL;
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    VALUE_TYPE MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::getName(Stringp name, Namespacep* nsFoundOut) const
    {
        const Quad<VALUE_TYPE>* t = m_quads->quads;
        for (int i=0, n=numQuads; i < n; i++)
        {
            if (t[i].name == name)
            {
                const Quad<VALUE_TYPE>& tf = t[i];
                if (nsFoundOut)
                    *nsFoundOut = tf.ns;
                return tf.value;
            }
        }
        if (nsFoundOut)
            *nsFoundOut = NULL;
        return (VALUE_TYPE)NULL;
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    void MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::rehash(const Quad<VALUE_TYPE>* oldAtoms, int oldTriplet, Quad<VALUE_TYPE>* newAtoms, int newTriplet)
    {
        for (int i=0, n=oldTriplet; i < n; i++)
        {
            Stringp oldName;
            if ((oldName=oldAtoms[i].name) != EMPTY)
            {
                // inlined & simplified version of put()
                int j = find(oldName, oldAtoms[i].ns, newAtoms, newTriplet);
                // don't need WBRC/WB here because we are just moving pointers
                newAtoms[j].name = oldName;
                newAtoms[j].ns = oldAtoms[i].ns;
                newAtoms[j].value = oldAtoms[i].value;
                newAtoms[j].apiAndMultiNS = oldAtoms[i].apiAndMultiNS;
            }
        }
    }

    // call this method using the previous value returned
    // by this method starting with 0, until 0 is returned.
    template <class VALUE_TYPE, class VALUE_WRITER>
    int FASTCALL MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::next(int index) const
    {
        // Advance to first non-empty slot.
        const Quad<VALUE_TYPE>* t = m_quads->quads;
        while (index < numQuads) {
            if (t[index++].name != NULL) {
                return index;
            }
        }
        return 0;
    }

    template <class VALUE_TYPE, class VALUE_WRITER>
    void MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::add(Stringp name, Namespacep ns, VALUE_TYPE value)
    {
        if (isFull())
        {
            grow();
        }
        put(name, ns, value);
    }
    
    template <class VALUE_TYPE, class VALUE_WRITER>
    void MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::put(Stringp name, Namespacep ns, VALUE_TYPE value)
    {
        AvmAssert(!isFull());
        AvmAssert(name->isInterned());
        AvmAssert(ns->getURI()->isInterned());
        
        MMgc::GC* gc = MMgc::GC::GetGC(m_quads);
        
        uint32_t multiNS = 0;
        
        // inlined version of find(), so that we can sniff for the multiNS
        // case (and update as necessary) in a single pass (rather than the
        // two extra passes we used to do)... this relies on the fact that
        // the quadratic probe will walk thru every existing entry with the same
        // name in order to find an empty slot, thus if there are any existing
        // entries with a different ns than what we are adding, all of those name
        // entries should be marked as multiNS.
        Quad<VALUE_TYPE>* cur;
        Quad<VALUE_TYPE>* const quadbase = m_quads->quads;
        {
            int n = 7;
            int const bitmask = (numQuads - 1);
            unsigned i = ((0x7FFFFFF8 & (uintptr_t)name) >> 3) & bitmask;
            cur = quadbase + i;
            for (;;)
            {
                Stringp probeName = cur->name;
                if (!probeName)
                {
                    // found the hole.
                    break;
                }
                
                if (probeName == name)
                {
                    // there's at least one existing entry with this name in the MNHT.
                    if (cur->matchNS(ns))
                    {
                        // it's the one we're looking for, just update the value.
                        goto write_value;
                    }
                    
                    // it's not the one we're looking for, thus we are now multiNS on this name.
                    if (cur->ns->m_uriAndType != ns->m_uriAndType) {
                        cur->apiAndMultiNS |= 1;
                        multiNS = 1;
                    }
                }
                
                i = (i + (n++)) & bitmask;          // quadratic probe
                cur = quadbase + i;
            }
        }
        
        AvmAssert(cur->name == NULL);
        AvmAssert(cur->ns == NULL);
        
        // New table entry for this <name,ns> pair
        size++;
        // OPTIMIZEME: we know the entries are zero, so we could use WriteBarrierRC_ctor here...
        // except that it will call GetGC() on the address, which will be wrong for > 4k.
        // Need a version of WriteBarrierRC_ctor that takes explicit GC*.
        WBRC(gc, m_quads, &cur->name, name);
        WBRC(gc, m_quads, &cur->ns, ns);
        // Set the "ApiVersion" section to an impossibly large value, so that we always
        // set it to the proper value the first time through.
        AvmAssert(cur->apiAndMultiNS == 0);
        cur->apiAndMultiNS = (kApiVersion_count << 1) | multiNS;
        
    write_value:
        VALUE_WRITER::store(gc, m_quads, (void**)&cur->value, (void*)value);
        uintptr_t const v = (uintptr_t)ns->getApiVersion() << 1;
        if (v < (cur->apiAndMultiNS & ~1))
        {
            // Note that this assumes that both ApiVersions are in the same series;
            // this should always be the case as we build them that way, but let's double-check.
            cur->apiAndMultiNS = v | (cur->apiAndMultiNS & 1);
            AvmAssert((kApiVersionSeriesMembership[ns->getApiVersion()] & kApiVersionSeriesMembership[cur->apiVersion()]) != 0);
        }
    }
}

#endif /*__avmplus_MultinameHashtable_impl__ */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

template <class VALUE_TYPE>
REALLY_INLINE bool Quad<VALUE_TYPE>::matchNS(Namespacep ns) const
{
    AvmAssert(ns->getURI()->isInterned());
    AvmAssert(this->ns != NULL);

    // Avoid loading m_uriAndType if possible.
    if (this->ns == ns)
        return true;

    uintptr_t const u1 = this->ns->m_uriAndType;
    uintptr_t const u2 = ns->m_uriAndType;
    return  u1 == u2 &&
            (u1 & 7) == Namespace::NS_Public && // implies both are public, since u1 == u2
            this->apiVersion() <= ns->getApiVersion();
}

template <class VALUE_TYPE, class VALUE_WRITER>
REALLY_INLINE VALUE_TYPE MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::get(Stringp name, NamespaceSetp nsset) const
{
    return getNSSet(name, nsset)->value;
}

template <class VALUE_TYPE, class VALUE_WRITER>
REALLY_INLINE VALUE_TYPE MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::getMulti(const Multiname& name) const
{
    return getMulti(&name);
}

template <class VALUE_TYPE, class VALUE_WRITER>
REALLY_INLINE Stringp MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::keyAt(int index) const
{
    AvmAssert(m_quads->quads[index-1].name != NULL); return m_quads->quads[index-1].name;
}

template <class VALUE_TYPE, class VALUE_WRITER>
REALLY_INLINE Namespacep MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::nsAt(int index) const
{
    return m_quads->quads[index-1].ns;
}

template <class VALUE_TYPE, class VALUE_WRITER>
REALLY_INLINE VALUE_TYPE MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::valueAt(int index) const
{
    return m_quads->quads[index-1].value;
}

template <class VALUE_TYPE, class VALUE_WRITER>
REALLY_INLINE size_t MultinameHashtable<VALUE_TYPE, VALUE_WRITER>::allocatedSize() const
{
    return numQuads * sizeof(Quad<VALUE_TYPE>);
}

template<class MNHT>
REALLY_INLINE StMNHTIterator<MNHT>::StMNHTIterator(MNHT* mnht) :
    m_mnht(mnht),
    m_cur(mnht->m_quads->quads - 1),
    m_end(mnht->m_quads->quads + mnht->numQuads)
{ }

template<class MNHT>
REALLY_INLINE bool StMNHTIterator<MNHT>::next()
{
    return ++m_cur < m_end;
}

template<class MNHT>
REALLY_INLINE Stringp StMNHTIterator<MNHT>::key() const
{
    AvmAssert(m_cur < m_end);
    return m_cur->name;
}

template<class MNHT>
REALLY_INLINE Namespacep StMNHTIterator<MNHT>::ns() const
{
    return m_cur->ns;
}

template<class MNHT>
REALLY_INLINE typename MNHT::TYPE StMNHTIterator<MNHT>::value() const
{
    return m_cur->value;
}

template<class MNHT>
REALLY_INLINE ApiVersion StMNHTIterator<MNHT>::apiVersion() const
{
    return m_cur->apiVersion();
}

/*static*/
REALLY_INLINE void BindingType::store(MMgc::GC* gc, void *, void **addr, void *value)
{
    // In theory, this assert can fire if a binding index happens to
    // point to GC memory, highly unlikely and it is worth keeping us
    // honest in Binding usage by doing this.
    (void)gc;
    AvmAssert(!gc->IsPointerToGCPage(value));
    *addr = value;
}

/*static*/
REALLY_INLINE void GCObjectType::store(MMgc::GC* gc, void *container, void **addr, void *value)
{
    WB(gc, container, addr, value);
}
} // namespace avmplus

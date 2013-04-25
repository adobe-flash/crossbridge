/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_E4XNode_inlines__
#define __avmplus_E4XNode_inlines__

namespace avmplus
{
    REALLY_INLINE uint32_t ElementE4XNode::numAttributes() const
    {
        return (m_attributes ? m_attributes->list.length() : 0);
    }

    REALLY_INLINE E4XNode::HeapE4XNodeList* ElementE4XNode::getAttributes() const
    {
        return m_attributes;
    }

    REALLY_INLINE E4XNode *ElementE4XNode::getAttribute(uint32_t index) const
    {
        return m_attributes->list.get(index);
    }

    REALLY_INLINE uint32_t ElementE4XNode::numNamespaces() const
    {
        return (m_namespaces ? m_namespaces->list.length() : 0);
    }

    REALLY_INLINE HeapNamespaceList* ElementE4XNode::getNamespaces() const
    {
        return m_namespaces;
    }
}
#endif /* __avmplus_E4XNode_inlines__ */

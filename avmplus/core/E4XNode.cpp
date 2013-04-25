/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

using namespace MMgc;

namespace avmplus
{
    extern wchar *stripPrefix (const wchar *str, const char *pre);

    E4XNodeAux::E4XNodeAux(Stringp s, Namespace *ns, FunctionObject* notify) :
        m_name(s),
        m_ns(ns),
        m_notification(notify)
    {
    }

#ifdef DEBUGGER
    uint64_t E4XNodeAux::bytesUsed() const
    {
        uint64_t size = GC::Size(this);

        if (m_name)
            size += m_name->bytesUsed();

        if (m_ns)
            size += m_ns->bytesUsedDeep();

        if (m_notification)
            size += m_notification->bytesUsed();

        return size;
    }
#endif

    AttributeE4XNode::AttributeE4XNode (E4XNode *parent, String *value) : E4XNode(parent), m_value(value)
    {
    }

    CDATAE4XNode::CDATAE4XNode (E4XNode *parent, String *value) : E4XNode(parent), m_value(value)
    {
    }

    CommentE4XNode::CommentE4XNode (E4XNode *parent, String *value) : E4XNode(parent), m_value(value)
    {
    }

    PIE4XNode::PIE4XNode (E4XNode *parent, String *value) : E4XNode(parent), m_value(value)
    {
    }

    TextE4XNode::TextE4XNode (E4XNode *parent, String *value) : E4XNode(parent), m_value(value)
    {
    }

    ElementE4XNode::ElementE4XNode (E4XNode *parent) : E4XNode(parent)
    {
    }

    // Fast append with no checks for type, etc.
    void ElementE4XNode::_append (E4XNode *childNode)
    {
        childNode->setParent (this);
        if (!m_children)
        {
            WB(gc(), this, &m_children, uintptr_t(childNode) | SINGLECHILDBIT);
            return;
        }

        if (m_children & SINGLECHILDBIT)
        {
            convertToE4XNodeList();
        }

        HeapE4XNodeList* aa = ((HeapE4XNodeList*)(uintptr_t)m_children);
        aa->list.add(childNode);
    }

    uint32_t ElementE4XNode::numChildren() const
    {
        if (!m_children)
            return 0;

        if (m_children & SINGLECHILDBIT)
            return 1;
        else
        {
            HeapE4XNodeList* aa = ((HeapE4XNodeList*)(uintptr_t)m_children);
            return aa->list.length();
        }
    }

    void ElementE4XNode::clearChildren()
    {
        if (m_children & ~SINGLECHILDBIT)
        {
            // !!@ delete our AtomList
        }

        WB(gc(), this, &m_children, 0);
    }

    void ElementE4XNode::convertToE4XNodeList()
    {
        if (m_children & SINGLECHILDBIT)
        {
            E4XNode *firstChild = (E4XNode *) (m_children & ~SINGLECHILDBIT);
            HeapE4XNodeList* aa = HeapE4XNodeList::create(gc(), 2);
            aa->list.add(firstChild);
            WB(gc(), this, &m_children, uintptr_t(aa));
        }
        else if (!m_children)
        {
            WB(gc(), this, &m_children, uintptr_t(HeapE4XNodeList::create(gc(), 1)));
        }
    }

    void ElementE4XNode::insertChild (uint32_t i, E4XNode *x)
    {
        // m_children->insert (i, a)
        convertToE4XNodeList();
        HeapE4XNodeList* aa = ((HeapE4XNodeList*)(uintptr_t)m_children);
        aa->list.insert(i, x);
    }

    void ElementE4XNode::removeChild (uint32_t i)
    {
        // m_children->removeAt (i)
        convertToE4XNodeList();
        HeapE4XNodeList *aa = ((HeapE4XNodeList *)(uintptr_t)m_children);
        aa->list.removeAt (i);
    }

    void ElementE4XNode::setChildAt (uint32_t i, E4XNode *x)
    {
        if ((i == 0) && (m_children & SINGLECHILDBIT))
        {
            WB(gc(), this, &m_children, uintptr_t(x) | SINGLECHILDBIT);
        }
        else
        {
            convertToE4XNodeList();
            HeapE4XNodeList* aa = ((HeapE4XNodeList *)(uintptr_t)m_children);
            aa->list.set(i, x);
        }
    }

    E4XNode *ElementE4XNode::_getAt(uint32_t i) const
    {
        if (i >= _length())
            return 0;

        if (int(this->m_children) & SINGLECHILDBIT)
        {
            return (E4XNode *)(this->m_children & ~SINGLECHILDBIT);
        }
        else
        {
            HeapE4XNodeList* aa = (HeapE4XNodeList*)(uintptr_t)this->m_children;
            return aa->list.get(i);
        }
    }

    bool E4XNode::getQName(Multiname *mn, Namespacep publicNS) const
    {
        if (!m_nameOrAux)
            return false;

        uintptr_t nameOrAux = m_nameOrAux;
        if (AUXBIT & nameOrAux)
        {
            E4XNodeAux *aux = (E4XNodeAux *)(nameOrAux & ~AUXBIT);
            // We can have a notification only aux which won't have a real name
            if (aux->m_name)
            {
                mn->setName (aux->m_name);
                mn->setNamespace (aux->m_ns);
                mn->setQName();
            }
            else
            {
                return false;
            }
        }
        else
        {
            Stringp str = (String *)(nameOrAux);
            mn->setName (str);
            mn->setNamespace(publicNS);
        }

        if (getClass() == kAttribute)
            mn->setAttr();

        return true;
    }

    // FIXME avoid code duplication with getQName
    bool E4XNode::getQNameForeign(AvmCore* core, Multiname *mn, Namespacep publicNS) const
    {
        if (!m_nameOrAux)
            return false;

        uintptr_t nameOrAux = m_nameOrAux;
        if (AUXBIT & nameOrAux)
        {
            E4XNodeAux *aux = (E4XNodeAux *)(nameOrAux & ~AUXBIT);
            // We can have a notification only aux which won't have a real name
            if (aux->m_name)
            {
                mn->setName (core->internForeignString(aux->m_name));
                mn->setNamespace (core->internNamespace(core->cloneNamespace(aux->m_ns)));
                mn->setQName();
            }
            else
            {
                return false;
            }
        }
        else
        {
            Stringp str = (String *)(nameOrAux);
            mn->setName (core->internForeignString(str));
            mn->setNamespace(core->internNamespace(core->cloneNamespace(publicNS)));
        }

        if (getClass() == kAttribute)
            mn->setAttr();

        return true;
    }



    void E4XNode::setQName (AvmCore *core, Stringp name, Namespace *ns)
    {
        // name can be null!
        if (name && !name->isInterned())
            name = core->internString(name);

        // If we already have an aux, use it.  (It may have notification atom set)
        uintptr_t nameOrAux = m_nameOrAux;
        if (AUXBIT & nameOrAux)
        {
            E4XNodeAux *aux = (E4XNodeAux *)(nameOrAux & ~AUXBIT);
            aux->m_name = name;
            aux->m_ns = ns;
            return;
        }

        if (!name && !ns)
        {
            m_nameOrAux = 0;
            return;
        }

        if (!ns || ns->isPublic() ||
            (ns->getPrefix() == core->kEmptyString->atom() && ns->getURI()->isEmpty()))
        {
            //m_nameOrAux = int (name);
            WBRC(core->GetGC(), this, &m_nameOrAux, uintptr_t(name));
            return;
        }

        E4XNodeAux *aux = E4XNodeAux::create(core->GetGC(), name, ns);
        //m_nameOrAux = AUXBIT | int(aux);
        // Clear it out in case there's already a name there.
        WBRC_NULL(&m_nameOrAux);
        WB(core->GetGC(), this, &m_nameOrAux, AUXBIT | uintptr_t(aux));
    }

    void E4XNode::setQName (AvmCore *core, const Multiname *mn)
    {
        if (!mn)
        {
            m_nameOrAux = 0;
        }
        else
        {
            setQName (core, mn->getName(), mn->getNamespace());
        }
    }

    // E4X 9.1.1.3, pg 20
    void E4XNode::_addInScopeNamespace (AvmCore* /*core*/, Namespace* /*ns*/, Namespacep /*publicNS*/)
    {
        // do nothing for non-element nodes
    }

    void ElementE4XNode::_addInScopeNamespace (AvmCore *core, Namespace *ns, Namespacep publicNS)
    {
//      if (getClass() & (kText | kCDATA | kComment | kProcessingInstruction | kAttribute))
//          return;

        if (!ns || (ns->getPrefix() == undefinedAtom))
            return;

        Multiname m;
        getQName(&m, publicNS);

        if ((ns->getPrefix() == core->kEmptyString->atom()) &&
            (!m.isAnyNamespace()) && m.getNamespace()->getURI()->isEmpty())
            return;

        // step 2b + 2c
        int index = -1;
        for (unsigned int i = 0, nn = numNamespaces(); i < nn; i++)
        {
            Namespace *ns2 = getNamespaces()->list.get(i);
            if (ns2->getPrefix() == ns->getPrefix())
                index = i;
        }

        // step 2d
        if (index != -1)
        {
            Namespace *ns2 = getNamespaces()->list.get(index);
            if (ns2->getURI() != ns->getURI())
            {
                // remove match from inscopenamespaces
                m_namespaces->list.removeAt(index);
            }
        }

        // step 2e - add namespace to inscopenamespaces
        if (!m_namespaces)
            m_namespaces = HeapNamespaceList::create(core->GetGC(), 1);

        m_namespaces->list.add(ns);

        // step 2f
        // If this nodes prefix == n prefix
            // set this nodes prefix to undefined
        if  (!m.isAnyNamespace() && (m.getNamespace()->getPrefix() == ns->getPrefix()))
        {
            setQName(core, m.getName(), core->newNamespace(m.getNamespace()->getURI(), Namespace::NS_Public, kApiVersion_VM_ALLVERSIONS));
        }

        // step 2g
        // for all attributes
        // if their nodes prefix == n.prefix
        //     set the node prefix to undefined
        for (unsigned int i = 0, na = numAttributes(); i < na; i++)
        {
            E4XNode *curAttr = m_attributes->list.get(i);
            Multiname ma;
            curAttr->getQName(&ma, publicNS);
            if (!ma.isAnyNamespace() && ma.getNamespace()->getPrefix() == ns->getPrefix())
            {
                curAttr->setQName(core, ma.getName(), core->newNamespace(ma.getNamespace()->getURI(), Namespace::NS_Public, kApiVersion_VM_ALLVERSIONS));
            }
        }

        return;
    }

    int E4XNode::FindMatchingNamespace (AvmCore *core, Namespace *ns)
    {
        for (unsigned int i = 0, nn = numNamespaces(); i < nn; i++)
        {
            Namespace *ns2 = getNamespaces()->list.get(i);
            if (ns2->getURI() == ns->getURI())
            {
                if (ns->getPrefix() == undefinedAtom)
                    return i;

                if (ns2->getPrefix() == core->kEmptyString->atom())
                    return -1;

                if (ns2->getPrefix() == ns->getPrefix())
                    return i;
            }
        }

        return -1;
    }

    Namespace *E4XNode::FindNamespace(AvmCore *core, Toplevel *toplevel, Stringp& tagName, bool bAttribute)
    {
        int32_t pos = tagName->indexOfLatin1(":", 1, 0);
        // handle case of ":name"
        if (pos == 0)
            toplevel->throwTypeError(kXMLBadQName, tagName);

        Stringp prefix = core->kEmptyString;
        if (pos > 0)
        {
            prefix = core->internSubstring(tagName, 0, pos);
            tagName = core->internSubstring(tagName, pos + 1, tagName->length());
        }

        // An attribute without a prefix is unqualified and does not inherit a namespace
        // from its parent.
        if (bAttribute && prefix == core->kEmptyString)
            return 0;

        // search all in scope namespaces for a matching prefix.  If we find one
        // return that prefix, otherwise we need to throw an error.
        E4XNode *y = this;
        while (y)
        {
            for (unsigned int i = 0, nn = y->numNamespaces(); i < nn; i++)
            {
                Namespace *ns = y->getNamespaces()->list.get(i);
                if (((prefix == core->kEmptyString) && !ns->hasPrefix()) ||
                    (prefix->atom() == ns->getPrefix()))
                {
                    return ns;
                }
            }

            y = y->m_parent;
        }

        if (prefix == core->kxml)
        {
            Namespacep nsXML = core->newNamespace(core->kEmptyString->atom(), core->kXML1998NS->atom());
            return nsXML;
        }

        // throw error because we didn't match this prefix
        if (prefix != core->kEmptyString)
        {
            toplevel->throwTypeError(kXMLPrefixNotBound, prefix, tagName);
        }
        return 0;
    }

    void E4XNode::BuildInScopeNamespaceList(AvmCore* /*core*/, NamespaceList& inScopeNS) const
    {
        const E4XNode *y = this;
        while (y)
        {
            for (unsigned int i = 0, nn = y->numNamespaces(); i < nn; i++)
            {
                Namespace *ns1 = y->getNamespaces()->list.get(i);
                uint32_t j;
                for (j = 0; j < inScopeNS.length(); j++)
                {
                    Namespace *ns2 = inScopeNS.get(j);
                    if (ns1->getPrefix() == undefinedAtom)
                    {
                        if (ns1->getURI() == ns2->getURI())
                            break;
                    }
                    else
                    {
                        if (ns1->getPrefix() == ns2->getPrefix())
                            break;
                    }
                }

                if (j == inScopeNS.length()) // no match
                {
#ifdef STRING_DEBUG
                    Stringp u = ns1->getURI();
                    Stringp p = core->string(ns1->getPrefix());
#endif
                    inScopeNS.add(ns1);
                }
            }

            y = y->m_parent;
        }
    }

    void E4XNode::addAttribute (E4XNode* /*x*/)
    {
        AvmAssert(0);
    }

    void ElementE4XNode::addAttribute (E4XNode *x)
    {
        if (!m_attributes)
            m_attributes = HeapE4XNodeList::create(gc(), 1);

        m_attributes->list.add(x);
    }

    void ElementE4XNode::CopyAttributesAndNamespaces(AvmCore *core, Toplevel *toplevel, XMLTag& tag, Namespacep publicNS)
    {
        m_attributes = 0;
        m_namespaces = 0;

        uint32_t numAttr = 0;
        int32_t len;
        // We first handle namespaces because the a attribute tag can reference a namespace
        // defined farther on in the same node...
        // <ns2:table2 ns2:bar=\"last\" xmlns:ns2=\"http://www.macromedia.com/home\">...
        uint32_t index = 0;
        Stringp attributeName, attributeValue;
        while (tag.nextAttribute(index, attributeName, attributeValue))
        {
            Namespace *ns = NULL;
            len = attributeName->length();
            if (len >= 5)
            {
                // caseless match
                if (attributeName->matchesLatin1_caseless("xmlns", 5, 0))
                {
                    // a namespace xnlns:prefix="URI" or xmlns="URI"
                    if ((len > 5) && attributeName->charAt(5) == ':')
                    {
                        if (len == 6)
                            // xmlns:=uri -- throw exception because of badly formed XML???
                            toplevel->throwTypeError(kXMLBadQName, attributeName);
                        Stringp prefix = attributeName->substring (6, len);
                        ns = core->newNamespace(prefix->atom(), attributeValue->atom());
                    }
                    else if (len == 5) {
                        // xmlns=uri
                        ns = core->newNamespace(core->kEmptyString->atom(), attributeValue->atom());
                    }

                    // !!@ Don't intern these namespaces since the intern table ignores
                    // the prefix value of the namespace.
                    if (ns) // ns can be null if prefix is defined and attributeValue = ""
                        this->_addInScopeNamespace(core, ns, publicNS);
                }
            }
            if (!ns)
                numAttr++;
        }

        if (!numAttr)
            return;

        m_attributes = HeapE4XNodeList::create(core->GetGC(), numAttr);

        // Now we read the attributes
        index = 0;
        while (tag.nextAttribute(index, attributeName, attributeValue))
        {
            len = attributeName->length();
            // check for namespace declarations and ignore them
            if (len >= 5)
            {
                // caseless match
                if (attributeName->matchesLatin1_caseless("xmlns", 5, 0))
                {
                    // a namespace xnlns:prefix="URI" or xmlns="URI"
                    if ((len == 5) || ((len > 5) && attributeName->charAt(5) == ':'))
                        continue;
                }
            }

            // !!@ intern our attributeValue??
            E4XNode *attrObj = AttributeE4XNode::create(core->GetGC(), this, attributeValue);

            Namespace *ns = this->FindNamespace(core, toplevel, attributeName, true);
            if (!ns)
                ns = publicNS;

            attrObj->setQName(core, attributeName, ns);

            // check for a duplicate attribute here and throw a kXMLDuplicateAttribute if found

            Multiname m2;
            attrObj->getQName(&m2, publicNS);
            for (unsigned int i = 0, na = numAttributes(); i < na; i++)
            {
                E4XNode *curAttr = m_attributes->list.get(i);
                Multiname m;
                curAttr->getQName(&m, publicNS);
                if (m.matches(&m2))
                {
                    toplevel->typeErrorClass()->throwError(kXMLDuplicateAttribute, attributeName, tag.text, core->toErrorString(tag.text->length()));
                }
            }

            m_attributes->list.add(attrObj);
        }
    }

    //////////////////////////////////////////////////////////////////////
    // E4X Section 9.1.1
    //////////////////////////////////////////////////////////////////////

    // E4X 9.1.1.4, pg 15
    void E4XNode::_deleteByIndex (uint32_t i)
    {
        if (i < numChildren())
        {
            E4XNode *x = _getAt(i);
            if (x)
            {
                x->m_parent = NULL;
            }

            removeChild (i);
            AvmAssert(numChildren() ^ 0x80000000); // check for underflow
        }
    }

    // E4X 9.1.1.7, page 16
    E4XNode *E4XNode::_deepCopy (AvmCore *core, Toplevel *toplevel, Namespacep publicNS) const
    {
        core->stackCheck(toplevel);

        E4XNode *x = 0;
        switch (this->getClass())
        {
        case kAttribute:
            x = AttributeE4XNode::create(core->GetGC(), 0, getValue());
            break;
        case kText:
            x = TextE4XNode::create(core->GetGC(), 0, getValue());
            break;
        case kCDATA:
            x = CDATAE4XNode::create(core->GetGC(), 0, getValue());
            break;
        case kComment:
            x = CommentE4XNode::create(core->GetGC(), 0, getValue());
            break;
        case kProcessingInstruction:
            x = PIE4XNode::create(core->GetGC(), 0, getValue());
            break;
        case kElement:
            x = ElementE4XNode::create(core->GetGC(), 0);
            break;
        }

        Multiname m;
        if (this->getQName(&m, publicNS))
        {
            x->setQName (core, &m);
        }

        if (x->getClass() == kElement)
        {
            ElementE4XNode *y = (ElementE4XNode *) x;

            // step 2 - for each ns in inScopeNamespaces
            unsigned int nn = numNamespaces();
            if (nn)
            {
                y->m_namespaces = HeapNamespaceList::create(core->GetGC(), nn);
                uint32_t i;
                for (i = 0; i < nn; i++)
                {
                    y->m_namespaces->list.add(getNamespaces()->list.get(i));
                }
            }

            // step 3 - duplicate attribute nodes
            unsigned int na = numAttributes();
            if (na)
            {
                y->m_attributes = HeapE4XNodeList::create(core->GetGC(), na);
                uint32_t i;
                for (i = 0; i < na; i++)
                {
                    E4XNode *ax = getAttribute (i);
                    E4XNode *bx = ax->_deepCopy(core, toplevel, publicNS);
                    bx->setParent(y);
                    y->addAttribute(bx);
                }
            }

            // step 4 - duplicate children
            if (numChildren())
            {
                AvmAssert(y->m_children == 0);
                WB(gc(), y, &y->m_children, uintptr_t(HeapE4XNodeList::create(core->GetGC(), numChildren())));
                for (uint32_t k = 0; k < _length(); k++)
                {
                    E4XNode *child = _getAt(k);
                    if (((child->getClass() == E4XNode::kComment) && toplevel->xmlClass()->get_ignoreComments()) ||
                        ((child->getClass() == E4XNode::kProcessingInstruction) && toplevel->xmlClass()->get_ignoreProcessingInstructions()))
                    {
                        continue;
                    }

                    E4XNode *cx = child->_deepCopy (core, toplevel, publicNS);
                    cx->setParent (y);
                    //y->m_children->push (c);
                    y->_append (cx);
                }
            }
        }

        return x;
    }

    // FIXME: copy and paste

    E4XNode *E4XNode::_deepCopyForeign (AvmCore *core, Toplevel *toplevel, Namespacep publicNS) const
    {
        core->stackCheck(toplevel);

        E4XNode *x = 0;
        switch (this->getClass())
        {
        case kAttribute:
            x = AttributeE4XNode::create(core->GetGC(), 0, getValue()->clone(core));
            break;
        case kText:
            x = TextE4XNode::create(core->GetGC(), 0, getValue()->clone(core));
            break;
        case kCDATA:
            x = CDATAE4XNode::create(core->GetGC(), 0, getValue()->clone(core));
            break;
        case kComment:
            x = CommentE4XNode::create(core->GetGC(), 0, getValue()->clone(core));
            break;
        case kProcessingInstruction:
            x = PIE4XNode::create(core->GetGC(), 0, getValue()->clone(core));
            break;
        case kElement:
            x = ElementE4XNode::create(core->GetGC(), 0);
            break;
        }

        Multiname m;
        if (this->getQNameForeign(core, &m, publicNS))
        {
            x->setQName (core, &m);
        }

        if (x->getClass() == kElement)
        {
            ElementE4XNode *y = (ElementE4XNode *) x;

            // step 2 - for each ns in inScopeNamespaces
            if (numNamespaces())
            {
                y->m_namespaces = HeapNamespaceList::create(core->GetGC(), numNamespaces());
                uint32_t i;
                for (i = 0; i < numNamespaces(); i++)
                {
                    y->m_namespaces->list.add(core->cloneNamespace(getNamespaces()->list.get(i)));
                }
            }

            // step 3 - duplicate attribute nodes
            if (numAttributes())
            {
                y->m_attributes = HeapE4XNodeList::create(core->GetGC(), numAttributes());
                uint32_t i;
                for (i = 0; i < numAttributes(); i++)
                {
                    E4XNode *ax = getAttribute (i);
                    E4XNode *bx = ax->_deepCopyForeign(core, toplevel, publicNS);
                    bx->setParent(y);
                    y->addAttribute(bx);
                }
            }

            // step 4 - duplicate children
            if (numChildren())
            {
                AvmAssert(y->m_children == 0);
                y->m_children = uintptr_t(HeapE4XNodeList::create(core->GetGC(), numChildren()));
                for (uint32_t k = 0; k < _length(); k++)
                {
                    E4XNode *child = _getAt(k);
                    if (((child->getClass() == E4XNode::kComment) && toplevel->xmlClass()->get_ignoreComments()) ||
                        ((child->getClass() == E4XNode::kProcessingInstruction) && toplevel->xmlClass()->get_ignoreProcessingInstructions()))
                    {
                        continue;
                    }

                    E4XNode *cx = child->_deepCopyForeign (core, toplevel, publicNS);
                    cx->setParent (y);
                    //y->m_children->push (c);
                    y->_append (cx);
                }
            }
        }

        return x;
    }



#if 0
    // E4X 9.1.1.8, page 17
    Atom E4XNode::descendants(Atom P) const
    {
        Multiname m;
        toplevel->ToXMLName (P, m);
        return getDescendants (&m);
    }
#endif

    // E4X 9.1.1.9, page 17
    bool E4XNode::_equals(Toplevel* toplevel, AvmCore *core, E4XNode *v) const
    {
        core->stackCheck(toplevel);

        if (this == v)
            return true;

        if (this->getClass() != v->getClass())
            return false;

        Multiname m;
        Multiname m2;
        Namespacep publicNS = core->findPublicNamespace();
        if (this->getQName(&m, publicNS))
        {
            if (v->getQName(&m2, publicNS) == 0)
                return false;

            // QName/AttributeName comparision here
            if (!m.matches(&m2))
                return false;
        }
        else if (v->getQName(&m2, publicNS) != 0)
        {
            return false;
        }

// Not enabled after discussion with JeffD.  If the namespaces are important, they're
// used in the node names themselves.
#if 0
        // NOT part of the original spec.  Added in later (bug 144429)
        if (this->numNamespaces() != v->numNamespaces())
            return false;

        // Order of namespaces does not matter
        NamespaceList *ns1 = getNamespaces();
        NamespaceList *ns2 = v->getNamespaces();
        for (uint32_t n1 = 0; n1 < numNamespaces(); n1++)
        {
            Namespace *namespace1 = ns1->get(n1);
            for (uint32_t n2 = 0; n2 < numNamespaces(); n2++)
            {
                Namespace *namespace2 = ns2->get(n2);
                if (namespace1->EqualTo (namespace2))
                    break;
            }

            // A match was not found
            if (n2 == numNamespaces())
                return false;
        }
#endif

        if (this->numAttributes() != v->numAttributes())
            return false;

        if (this->numChildren() != v->numChildren())
            return false;

        if (this->getValue() != v->getValue() &&
            (this->getValue()==NULL || v->getValue()==NULL || *getValue() != *v->getValue()))
            return false;

        // step 8
        // for each a in x.attributes
        // if v does not containing matching attribute, return failure
        for (uint32_t k1 = 0, na1 = numAttributes(); k1 < na1; k1++)
        {
            E4XNode *x1 = getAttribute(k1);
            bool bFoundMatch = false;
            for (uint32_t k2 = 0, na2 = v->numAttributes(); k2 < na2; k2++)
            {
                if (x1->_equals(toplevel, core, v->getAttribute(k2)))
                {
                    bFoundMatch = true;
                    break;
                }
            }

            if (!bFoundMatch)
                return false;
        }

        // step 9
        for (uint32_t i = 0; i < _length(); i++)
        {
            E4XNode *x1 = _getAt(i);
            E4XNode *x2 = v->_getAt(i);
            if (!x1->_equals(toplevel, core, x2))
                return false;
        }

        return true;
    }

    // E4X 9.1.1.11, page 18
    void E4XNode::_insert (AvmCore* /*core*/, Toplevel* /*toplevel*/, uint32_t /*entry*/, Atom /*value*/)
    {
        return;
    }

    void ElementE4XNode::_insert (AvmCore *core, Toplevel *toplevel, uint32_t entry, Atom value)
    {
//      //step 1
//      if (m_class & (kText | kCDATA | kComment | kProcessingInstruction | kAttribute))
//          return;

        // Spec says to throw a typeError if entry is not a number
        // We handle that in callingn functions

        uint32_t n = 1;
        XMLListObject *xl = AvmCore::atomToXMLList(value);
        if (xl)
        {
            n = xl->_length();
        }
        else
        {
            E4XNode *x = AvmCore::atomToXML(value);
            if (x)
            {
                E4XNode *n = this;
                while (n)
                {
                    if (x == n)
                        toplevel->throwTypeError(kXMLIllegalCyclicalLoop);
                    n = n->getParent();
                }

            }
        }

        if (n == 0)
            return;

        if (!m_children)
        {
            WB(gc(), this, &m_children, uintptr_t(HeapE4XNodeList::create(core->GetGC(), n)));
        }

        if (xl)
        {
            // insert each element of our XMLList into our array
            for (uint32_t j = 0; j < xl->_length(); j++)
            {
                E4XNode *child = AvmCore::atomToXML(xl->_getAt(j)->atom());

                // !!@ Not in spec but seems like a good idea
                E4XNode *n = this;
                while (n)
                {
                    if (child == n)
                        toplevel->throwTypeError(kXMLIllegalCyclicalLoop);
                    n = n->getParent();
                }

                child->setParent(this);

                insertChild (entry + j, child);
            }
        }
        else
        {
            insertChild (entry, 0); // make room for our replace
            this->_replace (core, toplevel, entry, value);
        }

        return;
    }

    // E4X 9.1.1.12, page 19
    // Autoconverts V into an XML object
    E4XNode* E4XNode::_replace (AvmCore* /*core*/, Toplevel* /*toplevel*/, uint32_t /*i*/, Atom /*V*/, Atom /*pastValue*/)
    {
        return 0;
    }

    E4XNode* ElementE4XNode::_replace (AvmCore *core, Toplevel *toplevel, uint32_t i, Atom V, Atom pastValue)
    {
        //step 1
        //if (getClass() & (kText | kCDATA | kComment | kProcessingInstruction | kAttribute))
        //  return;

        // step 2 + 3
        // API throws a typeError if entry is not a number
        // This is always handled back in the caller.

        // step 4
        if (i >= _length())
        {
            i = _length();
            // add a blank spot for this child
            if (!m_children)
                WB(gc(), this, &m_children, uintptr_t(HeapE4XNodeList::create(core->GetGC(), 1)));
            convertToE4XNodeList();
            HeapE4XNodeList* aa = ((HeapE4XNodeList*)(uintptr_t)m_children);
            aa->list.add(NULL);
        }

        E4XNode *prior = _getAt(i);

        // step 5
        E4XNode *xml = AvmCore::atomToXML(V);
        if (xml && (xml->getClass() & (kElement | kComment | kProcessingInstruction | kText | kCDATA)))
        {
            //a.    If V.[[Class]] is "element" and (V is x or an ancestor of x) throw an Error exception
            if (xml->getClass() == kElement)
            {
                E4XNode *n = this;
                while (n)
                {
                    if (xml == n)
                        toplevel->throwTypeError(kXMLIllegalCyclicalLoop);
                    n = n->getParent();
                }
            }

            xml->setParent (this);
            if (i < this->numChildren())
            {
                if (prior)
                {
                    prior->setParent (NULL);
                }
            }

            this->setChildAt (i, xml);
        }
        else if (AvmCore::atomToXMLList(V))
        {
            _deleteByIndex (i);
            _insert (core, toplevel, i, V);
        }
        else
        {
            Stringp s = core->string(V);
            E4XNode *newXML = TextE4XNode::create(core->GetGC(), this, s);
            // if this[i] is going away, clear its parent
            if (prior)
            {
                prior->setParent (NULL);
            }

            setChildAt (i, newXML);

            if (XMLObject::notifyNeeded(newXML))
            {
                Atom detail = prior ? prior->getValue()->atom() : 0;
                if (!detail)
                    detail = pastValue;
                XMLObject* target = XMLObject::create(core->GetGC(), toplevel->xmlClass(), newXML);
                target->nonChildChanges(core->ktextSet, newXML->getValue()->atom(), detail);
            }
        }

        return prior;
    }

    void ElementE4XNode::setNotification(AvmCore *core, FunctionObject* f, Namespacep publicNS)
    {
        uintptr_t nameOrAux = m_nameOrAux;
        // We already have an aux structure
        if (AUXBIT & nameOrAux)
        {
            E4XNodeAux *aux = (E4XNodeAux *)(nameOrAux & ~AUXBIT);
            aux->m_notification = f;
        }
        // allocate one to hold our name and notification atom
        else
        {
            Stringp str = (String *)(nameOrAux);
            E4XNodeAux *aux = E4XNodeAux::create(core->GetGC(), str, publicNS, f);
            //m_nameOrAux = AUXBIT | int(aux);
            // Clear it out in case there's already a name there.
            WBRC_NULL(&m_nameOrAux);
            WB(core->GetGC(), this, &m_nameOrAux, AUXBIT | uintptr_t(aux));
        }
    }

    FunctionObject* ElementE4XNode::getNotification() const
    {
        uintptr_t nameOrAux = m_nameOrAux;
        if (AUXBIT & m_nameOrAux)
        {
            E4XNodeAux *aux = (E4XNodeAux *)(nameOrAux & ~AUXBIT);
            return aux->m_notification;
        }

        return 0;
    }

    bool E4XNode::hasSimpleContent() const
    {
        if (getClass() & (E4XNode::kComment | E4XNode::kProcessingInstruction))
            return false;

        // for each prop in x, if x.class == element, return false
        for (uint32_t i = 0; i < _length(); i++)
        {
            E4XNode *child = _getAt(i);

            if (child->getClass() == E4XNode::kElement)
            {
                return false;
            }
        }

        return true;
    }

    bool E4XNode::hasComplexContent() const
    {
        if (getClass() & (E4XNode::kText | E4XNode::kComment | E4XNode::kProcessingInstruction | E4XNode::kAttribute | E4XNode::kCDATA))
            return false;

        for (uint32_t i = 0; i < _length(); i++)
        {
            E4XNode *child = _getAt(i);

            if (child->getClass() == E4XNode::kElement)
            {
                return true;
            }
        }

        return false;
    }

    int E4XNode::childIndex() const
    {
        if ((m_parent == NULL) || (getClass() == E4XNode::kAttribute))
            return -1;

        // find this child in parent's children list - return ordinal

        AvmAssert(m_parent->_length()); // this child's parent does not contain itself???

        for (uint32_t i = 0; i < m_parent->_length(); i++)
        {
            E4XNode *x = m_parent->_getAt(i);
            if (x == this)
            {
                return i;
            }
        }

        // this child's parent does not contain itself???
        AvmAssert(0);
        return -1;
    }

    String *E4XNode::nodeKind(Toplevel* toplevel) const
    {
        AvmCore* core = toplevel->core();
        switch (getClass())
        {
            case E4XNode::kAttribute:
                return core->kattribute;
            case E4XNode::kText:
            case E4XNode::kCDATA:
                return core->ktext;
            case E4XNode::kComment:
                return core->kcomment;
            case E4XNode::kProcessingInstruction:
                return core->kprocessing_instruction;
            case E4XNode::kElement:
                return core->kelement;
            case E4XNode::kUnknown:
            default:
                AvmAssert(0);
                return 0;
        }
    }

    void E4XNode::dispose()
    {
        for(uint32_t i=0,n=numChildren();i < n; i++)
        {
            E4XNode *node = _getAt(i);
            node->dispose();
            node->setParent(NULL);
            node->clearChildren();
        }
    }

#ifdef DEBUGGER
    uint64_t E4XNode::bytesUsed() const
    {
        // find the root node of the tree
        const E4XNode* root = this;
        while (root->m_parent)
            root = root->m_parent;

        // now find the size going down from there
        return root->bytesUsedDown();
    }

    uint64_t E4XNode::bytesUsedDown() const
    {
        uint64_t size = GC::Size(this);

        if (m_nameOrAux)
        {
            uintptr_t nameOrAux = m_nameOrAux;
            if (AUXBIT & nameOrAux)
            {
                E4XNodeAux *aux = (E4XNodeAux *)(nameOrAux & ~AUXBIT);
                size += aux->bytesUsed();
            }
            else
            {
                Stringp str = (String *)(nameOrAux);
                size += str->bytesUsed();
            }
        }

        return size;
    }

    uint64_t ElementE4XNode::bytesUsedDown() const
    {
        // Add up all the memory taken by this element plus all the namespaces,
        // attributes, and elements under it.

        uint64_t size = E4XNode::bytesUsedDown();

        for (uint32_t i=0, nn=numNamespaces(); i<nn; i++)
        {
            Namespace *ns1 = getNamespaces()->list.get(i);
            size += ns1->bytesUsedDeep();
        }

        for (uint32_t i=0, na=numAttributes(); i<na; ++i)
            size += getAttribute(i)->bytesUsedDown();

        for (uint32_t i=0, n=_length(); i<n; ++i)
            size += _getAt(i)->bytesUsedDown();

        return size;
    }
#endif
}


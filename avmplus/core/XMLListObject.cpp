/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    XMLListObject::XMLListObject(XMLListClass *type, Atom tObject, const Multiname* tProperty)
        : ScriptObject(type->ivtable(), type->prototypePtr()), m_targetObject(tObject),
          m_appended(false), m_children(type->gc(), 0)
    {
        if (tProperty)
            m_targetProperty = *tProperty;
        // unnecessary: the default ctor for HeapMultiname (and Multiname) init to an anyName
        //else // should be equivalent to a null m_targetProperty
        //  m_targetProperty.setAnyName();
    }

    //////////////////////////////////////////////////////////////////////
    // E4X Section 9.2.1 below (internal methods)
    //////////////////////////////////////////////////////////////////////

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom XMLListObject::callProperty(const Multiname* multiname, int argc, Atom* argv)
    {
        // sec 11.2.2.1 CallMethod(r,args)

        Atom f = getDelegate()->getMultinameProperty(multiname);
        if (f == undefinedAtom)
        {
            f = getMultinameProperty(multiname);
            // If our method returned is a 0 element XMLList, it means that we did not
            // find a matching property for this method name.  In this case, if our XMLList
            // has one child, we get the child and callproperty on the child object.
            // This allows node elements to be treated as simple strings even if they
            // are XML or XMLList objects.  See 11.2.2.1 in the E4X spec for CallMethod.
            if (AvmCore::isXMLList(f) &&
                !AvmCore::atomToXMLList(f)->_length() &&
                (this->_length() == 1))
            {
                XMLObject *x0 = _getAt(0);
                return x0->callProperty (multiname, argc, argv);
            }
        }
        argv[0] = atom(); // replace receiver
        return avmplus::op_call(toplevel(), f, argc, argv);
    }

    Atom XMLListObject::getMultinameProperty(const Multiname* m) const
    {
        AvmCore *core = this->core();
        Toplevel* toplevel = this->toplevel();

        // step 1 - We have an integer argument - direct child lookup
        if (!m->isAnyName() && !m->isAttr())
        {
            uint32_t index;
            if (!m->isAnyName() && m->getName()->parseIndex(index))
            {
                return getUintProperty (index);
            }
        }

        // step 2
        //QNameObject* qname = new (core->GetGC()) QNameObject(toplevel->qnameClass(), *m);
        XMLListObject *l = XMLListObject::create(core->GetGC(), toplevel->xmlListClass(), this->atom(), m);

        // step 3
        for (uint32_t i = 0; i < _length(); i++)
        {
            E4XNode* node = _getNodeAt(i);
            if (node->getClass() == E4XNode::kElement)
            {
                // step 3ai
                XMLObject* xml = _getAt(i);
                Atom gq = xml->getMultinameProperty(m);
                if (AvmCore::atomToXML(gq))
                {
                    XMLObject *x = AvmCore::atomToXMLObject(gq);
                    if (x && x->_length())
                        l->_append (gq);
                }
                else
                {
                    XMLListObject *xl = AvmCore::atomToXMLList(gq);
                    if (xl && xl->_length())
                    {
                        l->_append (gq);
                    }
                }
            }
        }

        return l->atom();
    }

    void XMLListObject::setMultinameProperty(const Multiname* m, Atom V)
    {
        Toplevel *toplevel = this->toplevel();

        uint32_t i;
        if (!m->isAnyName() && !m->isAttr() && m->getName()->parseIndex(i))
        {
            setUintProperty (i, V);
            return;
        }
        // step 3
        else if (_length() <= 1)
        {
            //step3a
            if (_length() == 0)
            {
                Atom r = this->_resolveValue();
                if (AvmCore::isNull(r))
                    return;

                if (AvmCore::isXML(r) && (AvmCore::atomToXML(r)->_length() != 1))
                    return;

                if (AvmCore::isXMLList(r) && (AvmCore::atomToXMLList(r)->_length() != 1))
                    return;

                this->_append (r);
            }

            XMLObject *x = _getAt (0);
            x->setMultinameProperty(m, V);
        }
        else if (_length() > 1)
        {
            toplevel->throwTypeError(kXMLAssigmentOneItemLists);
        }

#ifdef _DEBUG
        // The only thing allowed in XMLLists is XML objects
        for (uint32_t z = 0; z < _length(); z++)
        {
            AvmAssert(AvmCore::isXML(m_children.get(z)));
        }
#endif

        return;
    }

    bool XMLListObject::deleteMultinameProperty(const Multiname* m)
    {
        if (!m->isAnyName() && !m->isAttr())
        {
            Stringp name = m->getName();
            uint32_t index;
            if (name->parseIndex(index))
            {
                return this->delUintProperty (index);
            }
        }

        for (uint32_t i = 0; i < numChildren(); i++)
        {
            if (_getNodeAt(i)->getClass() == E4XNode::kElement)
            {
                XMLObject *xm = _getAt(i);
                xm->deleteMultinameProperty(m);
            }
        }

        return true;
    }

    Atom XMLListObject::getDescendants(const Multiname* m) const
    {
        AvmCore *core = this->core();

        XMLListObject *l = XMLListObject::create(core->GetGC(), toplevel()->xmlListClass());
        for (uint32_t i = 0; i < numChildren(); i++)
        {
            if (_getNodeAt(i)->getClass() == E4XNode::kElement)
            {
                XMLObject *xm = _getAt(i);
                Atom dq = xm->getDescendants (m);
                XMLListObject *dqx = AvmCore::atomToXMLList(dq);
                if (dqx && dqx->_length())
                {
                    l->_append (dq);
                }
            }
        }

        return l->atom();
    }

    // E4X 9.2.1.1, page 21
    Atom XMLListObject::getAtomProperty(Atom P) const
    {
        Multiname m;
        toplevel()->ToXMLName (P, m);
        return getMultinameProperty(&m);
    }

    // E4X 9.2.1.2, pg 21 - [[PUT]]
    void XMLListObject::setAtomProperty(Atom P, Atom V)
    {
        Multiname m;
        toplevel()->ToXMLName (P, m);
        setMultinameProperty(&m, V);
    }

    Atom XMLListObject::getUintProperty(uint32_t index) const
    {
        if (index < _length())
        {
            return _getAt(index)->atom();
        }
        else
        {
            return undefinedAtom;
        }
    }

    void XMLListObject::setUintProperty(uint32_t i, Atom V)
    {
        AvmCore* core = this->core();
        Toplevel* toplevel = this->toplevel();

        Atom r = nullStringAtom;
        fixTargetObject();
        // step 2a
        if (!AvmCore::isNull(this->m_targetObject))
        {
            if (AvmCore::isXML(m_targetObject))
            {
                r = AvmCore::atomToXMLObject(m_targetObject)->_resolveValue();
            }
            else if (AvmCore::isXMLList(m_targetObject))
            {
                r = AvmCore::atomToXMLList(m_targetObject)->_resolveValue();
            }

            if (AvmCore::isNull(r))
                return;
        }

        // step 2c
        if (i >= _length())
        {
            // step 2 c i
            if (AvmCore::isXMLList(r))
            {
                XMLListObject *xl = AvmCore::atomToXMLList(r);
                if (xl->_length() != 1)
                    return;
                else
                    r = xl->_getAt(0)->atom();

                // r should now be an XMLObject atom from this point forward
            }

            // step 2cii - newer spec
            XMLObject *rx = AvmCore::atomToXMLObject(r);
            if (rx && rx->getClass() != E4XNode::kElement)
                return;

            E4XNode *y = 0;
            // step 2 c iii
            if (m_targetProperty.isAttr())
            {
                Atom attributesExist = rx->getMultinameProperty(m_targetProperty);
                if (AvmCore::isXMLList(attributesExist) && (AvmCore::atomToXMLList(attributesExist)->_length() > 0))
                    return;

                y = AttributeE4XNode::create(core->GetGC(), AvmCore::atomToXML(r), 0);
                y->setQName (core, m_targetProperty);

                // warning: it looks like this is sent prior to the attribute being added.
                rx->nonChildChanges(core->kattributeAdded, this->m_targetProperty.getName()->atom());
            }
            // if targetProperty is null or targetProperty.localName = "*"
            else if (m_targetProperty.isAnyName() ||
                // Added because of bug 145184 - appendChild with a text node not working right
                // Also handle attribute nodes
                ((AvmCore::isXML(V) && AvmCore::atomToXML(V)->getClass() == E4XNode::kAttribute)) ||
                ((AvmCore::isXML(V) && AvmCore::atomToXML(V)->getClass() == E4XNode::kText)))
            {
                y = TextE4XNode::create(core->GetGC(), AvmCore::atomToXML(r), 0);
            }
            else
            {
                y = ElementE4XNode::create(core->GetGC(), AvmCore::atomToXML(r));
                y->setQName (core, this->m_targetProperty);
            }

            i = _length();
            // step 2.c.viii
            if (y->getClass() != E4XNode::kAttribute)
            {
                AvmAssert(y->getParent() == (rx ? rx->getNode() : 0));
                if (y->getParent() != NULL)
                {
                    // y->m_parent = r = XMLObject *
                    E4XNode *parent = y->getParent();
                    AvmAssert(parent != 0);
                    uint32_t j = 0;
                    if (i > 0)
                    {
                        // step 2 c vii 1 ii
                        // spec says "y.[[parent]][j] is not the same object as x[i-1]" and I'm
                        // assuming that means pointer equality and not "equals" comparison
                        while ((j < (parent->_length() - 1)) && parent->_getAt (j) != _getNodeAt(i - 1))
                        {
                            j++;
                        }

                        j = j + 1; // slightly different than spec but our insert call does not have a + 1 on it
                    }
                    else
                    {
                        j = parent->_length(); // slightly different than spec but our insert call does not have a + 1 on it
                    }
                    // These two lines are equivalent to _insert which takes
                    // an Atom since we know that y is an XMLObject/E4XNode
                    y->setParent (parent);
                    parent->insertChild (j, y);

                }
                if (AvmCore::isXML(V))
                {
                    Multiname mV;
                    if (AvmCore::atomToXML(V)->getQName(&mV, core->findPublicNamespace()))
                        y->setQName (core, &mV);

                }
                else if (AvmCore::isXMLList(V))
                {
                    //ERRATA : 9.2.1.2 step 2(c)(vii)(3) what is V.[[PropertyName]]? s.b. [[TargetProperty]]
                    if (!AvmCore::atomToXMLList(V)->m_targetProperty.isAnyName())
                        y->setQName (core, AvmCore::atomToXMLList(V)->m_targetProperty);
                }
            }

            this->_appendNode (y);
        }

        // step 2d
        if (AvmCore::atomToXMLList(V))
        {
            XMLListObject *src = AvmCore::atomToXMLList(V);
            if ((src->_length() == 1) && src->_getNodeAt(0)->getClass() & (E4XNode::kText | E4XNode::kAttribute))
            {
                V = core->string(V)->atom();
            }
        }
        else if (AvmCore::atomToXML(V))
        {
            E4XNode *v = AvmCore::atomToXML(V);
            if (v->getClass() & (E4XNode::kText | E4XNode::kAttribute))
            {
                // This string is converted into a XML object below in step 2(g)(iii)
                V = core->string(V)->atom();
            }
        }
        else
        {
            V = core->string(V)->atom();
        }

        // step 2e
        E4XNode *xi = _getNodeAt(i);
        if (xi->getClass() == E4XNode::kAttribute)
        {
            XMLObject* xio = _getAt(i);
            XMLObject *parent = xio->getParent();
            Multiname mxi;
            xio->getQName (&mxi);
            parent->setMultinameProperty(&mxi, V);
            Atom attr = parent->getMultinameProperty(&mxi);
            XMLListObject *attrx = AvmCore::atomToXMLList(attr);
            // x[i] = attr[0];
            m_children.set(i, attrx->m_children.get(0));
        }
        // step 2f
        else if (AvmCore::isXMLList(V))
        {
            // create a shallow copy c of V
            XMLListObject *src = AvmCore::atomToXMLList(V);
            XMLListObject *c = XMLListObject::create(core->GetGC(), toplevel->xmlListClass());

            c->m_children.ensureCapacity(src->numChildren());
            for (uint32_t i2 = 0; i2 < src->numChildren(); i2++)
                c->m_children.add(src->m_children.get(i2));

            E4XNode *parent = _getNodeAt(i)->getParent();
            // step 2 f iii
            if (parent)
            {
                uint32_t q = 0;
                while (q < parent->numChildren())
                {
                    if (parent->_getAt(q) == _getNodeAt(i))
                    {
                        parent->_replace (core, toplevel, q, c->atom());
                        for (uint32_t j = 0; j < c->_length(); j++)
                        {
                            c->m_children.set(j, AvmCore::genericObjectToAtom(parent->_getAt(q + j)));
                        }

                        break;
                    }

                    q++;
                }
            }

            // resize our buffer to ([[Length]] + c.[[Length]]) (2 f vi)

            bool notify = (parent && XMLObject::notifyNeeded(parent));
            XMLObject* prior =  (notify) ? _getAt(i) : 0;
            XMLObject* target = (notify) ? XMLObject::create(core->GetGC(), toplevel->xmlClass(), parent) : 0;

            m_children.removeAt(i);
            for (uint32_t i2 = 0; i2 < src->numChildren(); i2++)
            {
                m_children.insert(i + i2, c->m_children.get(i2));

                E4XNode* node = c->_getNodeAt(i2);
                if (notify && (parent == node->getParent()))
                {
                    XMLObject* obj = c->_getAt(i2);
                    if (i2 == 0)
                    {
                        // @todo; is this condition ever true?
                        if (node != prior->getNode())
                            target->childChanges(core->knodeChanged, obj->atom(), prior->getNode());
                    }
                    else
                    {
                        target->childChanges(core->knodeAdded, obj->atom());
                    }
                }
            }
        }
        // step 2g
        // !!@ always going into this step supports how Rhino acts with setting XMLList props
        //var y = new XMLList("<alpha>one</alpha><bravo>two</bravo>");
        //y[0] = "five";
        //print(y);
        // <alpha>one</alpha> which is an element node is converted to a text node with "five" as it's label
        // !!@ but in this case, the node's contents are replaced and not the node itself...
        // o = <o><i id="1">A</i><i id="2">B</i><i id="3">C</i><i id="4">D</i></o>;
        // o.i[1] = "BB";
        // print(o);
        else if (AvmCore::isXML(V) || xi->getClass() & (E4XNode::kText | E4XNode::kCDATA | E4XNode::kComment | E4XNode::kProcessingInstruction))
        {
            E4XNode *parent = _getNodeAt(i)->getParent();
            if (parent)
            {
                // search our parent for a match between a child and x[i]
                uint32_t q = 0;
                while (q < parent->numChildren())
                {
                    if (parent->_getAt(q) == _getNodeAt(i))
                    {
                        parent->_replace (core, toplevel, q, V);
                        XMLObject *xo = XMLObject::create(core->GetGC(), toplevel->xmlClass(), parent->_getAt(q));
                        V = xo->atom();

                        if (XMLObject::notifyNeeded(parent))
                        {
                            XMLObject *po = XMLObject::create(core->GetGC(), toplevel->xmlClass(), parent);
                            po->childChanges(core->knodeAdded, xo->atom());
                        }
                        break;
                    }

                    q++;
                }
            }

            // From E4X errrata
            //9.2.1.2 step 2(g)(iii) erratum: _V_ may not be of type XML, but all index-named
            //      properties _x[i]_ in an XMLList _x_ must be of type XML, according to
            //      9.2.1.1 Overview and other places in the spec.

            //      Thanks to 2(d), we know _V_ (*vp here) is either a string or an
            //      XML/XMLList object.  If _V_ is a string, call ToXML on it to satisfy
            //      the constraint before setting _x[i] = V_.

            if (AvmCore::isXML(V))
                m_children.set(i, V);
            else
                m_children.set(i, xmlClass()->ToXML (V));
        }
        // step 2h
        else
        {
            _getAt(i)->setStringProperty(core->kAsterisk, V);
        }
    }

    bool XMLListObject::delUintProperty(uint32_t index)
    {
        if (index >= _length())
        {
            return true;
        }
        else
        {
            XMLObject *xi = _getAt(index);
            XMLObject *px = xi->getParent();
            if (px != NULL)
            {
                if (xi->getClass() == E4XNode::kAttribute)
                {
                    Multiname mx;
                    xi->getQName (&mx);
                    px->deleteMultinameProperty(&mx);
                }
                else
                {
                    // let q be the property of parent, where parent[q] is the same object as x[i].
                    int q = xi->childIndex ();

                    E4XNode *x = px->getNode()->_getAt(q);
                    px->getNode()->_deleteByIndex (q);

                    if (XMLObject::notifyNeeded(px->getNode()) && x->getClass() == E4XNode::kElement)
                    {
                        AvmCore *core = this->core();
                        XMLObject *r = XMLObject::create(core->GetGC(), xmlClass(), x);
                        px->childChanges(core->knodeRemoved, r->atom());
                    }
                }
            }

            // delete index from this list

            m_children.removeAt(index);
            return true;
        }
    }

    // E4X 9.2.1.3, pg 23
    bool XMLListObject::deleteAtomProperty(Atom P)
    {
        Multiname m;
        toplevel()->ToXMLName (P, m);
        return deleteMultinameProperty(&m);
    }

    bool XMLListObject::hasUintProperty(uint32_t index) const
    {
        return (index < _length());
    }

    bool XMLListObject::hasMultinameProperty(const Multiname* m) const
    {
        if (!m->isAnyName() && !m->isAttr())
        {
            Stringp name = m->getName();
            uint32_t index;
            if (name->parseIndex(index))
            {
                return (index < _length());
            }
        }

        for (uint32_t i = 0; i < numChildren(); i++)
        {
            if (_getNodeAt(i)->getClass() == E4XNode::kElement)
            {
                XMLObject *xm = _getAt(i);
                if (xm->hasMultinameProperty(m))
                    return true;
            }
        }

        return false;
    }

    // E4X 9.2.1.5, pg 24
    bool XMLListObject::hasAtomProperty(Atom P) const
    {
        Multiname m;
        toplevel()->ToXMLName (P, m);
        return hasMultinameProperty(&m);
    }

    /*
    This code is extracted from the former _append(E4XNode*) method.
    It is unclear why this code is required, but I left it in to not
    break any existing code. It does not make sense to execute the
    code for every call to _appendNode(). That method now sets m_appended,
    and code that accesses m_targetObject and/or m_targetProperty needs
    to call fixTargetObject() before.
    */
    void XMLListObject::fixTargetObject() const
    {
        if (m_appended && numChildren() > 0)
        {
            AvmCore *core = this->core();
            E4XNode* v = _getNodeAt(numChildren()-1);

            if (v->getParent())
            {
                XMLObject* target = AvmCore::atomToXMLObject(m_targetObject);
                if (target && target->getNode() != v->getParent())
                {
                    // create only a new target object if the old one does not match
                    XMLObject *p = XMLObject::create(core->GetGC(), toplevel()->xmlClass(), v->getParent());
                    setTargetObject(p->atom());
                }
            }
            else
                setTargetObject(nullObjectAtom);
            if (v->getClass() != E4XNode::kProcessingInstruction)
            {
                Multiname m;
                if (v->getQName (&m, core->findPublicNamespace()))
                    this->m_targetProperty = m;
            }
            m_appended = false;
        }
    }

    void XMLListObject::_appendNode(E4XNode *v)
    {
        m_children.add(AvmCore::genericObjectToAtom(v));
        m_appended = true;
    }

    // E4X 9.2.1.6, pg 24
    void XMLListObject::_append(Atom V)
    {
        // !!@ what the docs say
        // Atom children = this->get ("*");
        // children->put (children->length(), child)
        // return x;

        XMLListObject *v = AvmCore::atomToXMLList(V);
        if (v)
        {
            setTargetObject(v->m_targetObject);
            this->m_targetProperty = v->m_targetProperty;
            m_appended = false;

            if (v->_length())
            {
                m_children.ensureCapacity(m_children.length() + v->_length());
                for (uint32_t j = 0; j < v->_length(); j++)
                {
                    m_children.add(v->m_children.get(j));
                }
            }
        }
        else
        {
            XMLObject* v = AvmCore::atomToXMLObject(V);
            if (v)
            {
                m_appended = true;
                m_children.add(V);
            }
        }
    }

    // E4X 9.2.1.7, pg 25
    XMLListObject *XMLListObject::_deepCopy () const
    {
        AvmCore *core = this->core();

        fixTargetObject();
        XMLListObject *l = XMLListObject::create(core->GetGC(), toplevel()->xmlListClass(), m_targetObject, m_targetProperty);

        l->m_children.ensureCapacity(numChildren());
        for (uint32_t i = 0; i < numChildren(); i++)
        {
            l->m_children.add(_getAt(i)->_deepCopy()->atom());
        }

        return l;
    }

    // E4X 9.2.1.8, pg 25
    XMLListObject *XMLListObject::AS3_descendants (Atom P)
    {
        Multiname m;
        toplevel()->ToXMLName (P, m);
        return AvmCore::atomToXMLList (getDescendants (&m));
    }

    // E4X 9.2.1.9, pg 26
    Atom XMLListObject::_equals(Atom V) const
    {
        AvmCore *core = this->core();

        // null or "" return false
        if ((V == undefinedAtom) && (_length() == 0))
            return trueAtom;

        if (AvmCore::isXMLList(V))
        {
            XMLListObject *v = AvmCore::atomToXMLList(V);
            if (_length() != v->_length())
                return falseAtom;

            for (uint32_t i = 0; i < _length(); i++)
            {
                Atom a1 = m_children.get(i);
                Atom a2 = v->m_children.get(i);
                if ((a1 != a2) && core->equals (_getAt(i)->atom(), v->_getAt(i)->atom()) == falseAtom)
                    return falseAtom;
            }

            return trueAtom;
        }
        else if (_length() == 1)
        {
            return core->equals(_getAt(0)->atom(), V);
        }

        return falseAtom;
    }

    // E4X 9.2.1.10, pg 26
    Atom XMLListObject::_resolveValue()
    {
        if (_length() > 0)
            return this->atom();

        AvmCore *core = this->core();

        core->stackCheck(toplevel());

        fixTargetObject();
        if (AvmCore::isNull(m_targetObject) ||
            (m_targetProperty.isAttr()) ||
            (m_targetProperty.isAnyName()))
        {
            return nullObjectAtom;
        }

        Atom base = nullObjectAtom;
        XMLObject *x = AvmCore::atomToXMLObject(m_targetObject);
        if (x)
        {
            base = x->_resolveValue();
        }
        XMLListObject *xl = AvmCore::atomToXMLList(m_targetObject);
        if (xl)
        {
            base = xl->_resolveValue ();
        }
        if (AvmCore::isNull(base))
            return nullObjectAtom;

        XMLListObject *target = 0;
        if (AvmCore::isXML(base))
        {
            target = AvmCore::atomToXMLList(AvmCore::atomToXMLObject(base)->getMultinameProperty(m_targetProperty));
        }
        else if (AvmCore::isXMLList(base))
        {
            target = AvmCore::atomToXMLList(AvmCore::atomToXMLList(base)->getMultinameProperty(m_targetProperty));
        }
        else
        {
            AvmAssert(0);// base should be an XML or XMLList always
        }

        if (!target)
            return nullObjectAtom;

        if (!target->_length())
        {
            if (AvmCore::isXMLList(base) && AvmCore::atomToXMLList(base)->_length() > 1)
            {
                toplevel()->throwTypeError(kXMLAssigmentOneItemLists);
                return nullObjectAtom;
            }

            if (AvmCore::isXML(base))
            {
                AvmCore::atomToXMLObject(base)->setMultinameProperty(m_targetProperty, core->kEmptyString->atom());
                return AvmCore::atomToXMLObject(base)->getMultinameProperty(m_targetProperty);
            }
            else if (AvmCore::isXMLList(base))
            {
                AvmCore::atomToXMLList(base)->setMultinameProperty(m_targetProperty, core->kEmptyString->atom());
                return AvmCore::atomToXMLList(base)->getMultinameProperty(m_targetProperty);
            }
            else
            {
                AvmAssert(0); // b should be one or the other at this point since target is non-null
            }
        }

        return target->atom();
    }

    XMLObject *XMLListObject::_getAt (uint32_t i) const
    {
        if (i >= _length())
            return 0;

        Atom a = m_children.get(i);
        XMLObject* obj = AvmCore::atomToXMLObject(a);
        if (!obj)
        {
            obj = XMLObject::create(core()->GetGC(), toplevel()->xmlClass(), (E4XNode*) AvmCore::atomToGenericObject(a));
            m_children.set(i, obj->atom());
        }
        return obj;
    }

    E4XNode *XMLListObject::_getNodeAt (uint32_t i) const
    {
        if (i >= _length())
            return 0;

        Atom a = m_children.get(i);
        XMLObject* obj = AvmCore::atomToXMLObject(a);
        if (obj)
            return obj->getNode();
        return (E4XNode*) AvmCore::atomToGenericObject(a);
    }

    // E4X 12.2, page 59
    // Support for for-in, for-each for XMLListObjects
    Atom XMLListObject::nextName(int index)
    {
        AvmAssert(index > 0);

        if (index <= (int)_length())
        {
            AvmCore *core = this->core();
            return core->internInt (index-1)->atom();
        }
        else
        {
            return nullStringAtom;
        }
    }

    Atom XMLListObject::nextValue(int index)
    {
        AvmAssert(index > 0);

        if (index <= (int)_length())
            return _getAt (index-1)->atom();
        else
            return undefinedAtom;
    }

    int XMLListObject::nextNameIndex(int index)
    {
        AvmAssert(index >= 0);

        // XMLList types return the same number of indexes as children
        // We return a 1-N iterator (and then subtract off the one in
        // the nextName and nextValue functions).
        if (index < (int) _length())
            return index + 1;
        else
            return 0;
    }

    // E4X 13.5.4.21, page 93 which just refers to 10.2, page 31 (XMLList case)
    void XMLListObject::__toXMLString(PrintWriter &output, Atom, int /*indentLevel*/)
    {
        AvmCore *core = this->core();

        // For i = 0 to x.[[length]-1
        //  if (xml.prettyprinting == true and i is not equal to 0)
        //      s = s + line terminator
        //  s = s + toxmlstring(x[i], ancestor namespace)

        for (uint32_t i = 0; i < _length(); i++)
        {
            // iterate over entire array. If any prop is an "element" type, return false
            XMLObject *xm = _getAt(i);
            if (xm)
            {
                // !!@ This does not seem be affected by the prettyPrinting flag
                // but always occurs (comparing to Rhino)
                if (/*xmlClass()->getPrettyPrinting() && */i)
                    output << "\n";
                    NamespaceList AncestorNamespaces(core->GetGC(), kListInitialCapacity);
                    xm->__toXMLString(output, AncestorNamespaces, 0);
            }
        }

        return;
    }

    //////////////////////////////////////////////////////////////////////
    // E4X Section 13.5.4 below (AS methods)
    //////////////////////////////////////////////////////////////////////

    // E4X 13.5.4.2, pg 88
    XMLListObject *XMLListObject::AS3_attribute (Atom arg)
    {
        // name= ToAttributeName (attributeName);
        // return [[get]](name)

        return AvmCore::atomToXMLList(getAtomProperty(toplevel()->ToAttributeName(arg)->atom()));
    }

    // E4X 13.5.4.3, pg 88
    XMLListObject *XMLListObject::AS3_attributes ()
    {
        return AvmCore::atomToXMLList(getAtomProperty(toplevel()->ToAttributeName(core()->kAsterisk)->atom()));
    }

    // E4X 13.5.4.4, pg 88
    XMLListObject *XMLListObject::AS3_child (Atom propertyName)
    {
        AvmCore *core = this->core();

        XMLListObject *m = XMLListObject::create(core->GetGC(), toplevel()->xmlListClass(), this->atom());

        for (uint32_t i = 0; i < _length(); i++)
        {
            XMLObject *x = _getAt(i);
            XMLListObject *rxl = x->child (propertyName);
            if (rxl && rxl->_length())
            {
                m->_append (rxl->atom());
            }
        }

        return m;
    }

    XMLListObject *XMLListObject::AS3_children ()
    {
        return AvmCore::atomToXMLList(getStringProperty(core()->kAsterisk));
    }

    XMLListObject *XMLListObject::AS3_comments ()
    {
        AvmCore *core = this->core();

        XMLListObject *m = XMLListObject::create(core->GetGC(), toplevel()->xmlListClass(), this->atom());

        for (uint32_t i = 0; i < _length(); i++)
        {
            if (_getNodeAt(i)->getClass() == E4XNode::kElement)
            {
                XMLObject *x = _getAt(i);
                XMLListObject *rxl = x->comments();
                if (rxl && rxl->_length())
                {
                    m->_append (rxl->atom());
                }
            }
        }

        return m;
    }

    bool XMLListObject::AS3_contains (Atom value)
    {
        AvmCore *core = this->core();;
        for (uint32_t i = 0; i < _length(); i++)
        {
            // Spec says "comparison l[i] == value)" which is different than _equals
            if (core->equals(_getAt(i)->atom(), value) == trueAtom)
                return true;
        }

        return false;
    }

    XMLListObject *XMLListObject::AS3_copy ()
    {
        return _deepCopy();
    }

    // E4X 13.5.4.10, pg 90
    XMLListObject *XMLListObject::AS3_elements (Atom name) // name defaults to '*'
    {
        AvmCore *core = this->core();
        Toplevel* toplevel = this->toplevel();

        Multiname m;
        toplevel->ToXMLName (name, m);

        XMLListObject *xl = XMLListObject::create(core->GetGC(), toplevel->xmlListClass(), this->atom(), &m);

        for (uint32_t i = 0; i < _length(); i++)
        {
            if (_getNodeAt(i)->getClass() == E4XNode::kElement)
            {
                XMLObject *x = _getAt(i);
                XMLListObject *rxl = x->elements(name);
                if (rxl && rxl->_length())
                {
                    xl->_append (rxl->atom());
                }
            }
        }

        return xl;
    }

    // E4X 13.5.4.11, pg 90
    bool XMLListObject::XMLList_AS3_hasOwnProperty (Atom P)
    {
        if (hasAtomProperty(P))
            return true;

        // If x has a property with name ToString(P), return true;
        // !!@ (prototype different than regular object??

        return false;
    }

    // E4X 13.5.4.12, pg 90
    bool XMLListObject::AS3_hasComplexContent ()
    {
        if (_length() == 0)
            return false;

        if (_length() == 1)
        {
            return _getNodeAt(0)->hasComplexContent();
        }

        for (uint32_t i = 0; i < _length(); i++)
        {
            if (_getNodeAt(i)->getClass() == E4XNode::kElement)
            {
                return true;
            }
        }

        return false;
    }

    // E4X 13.5.4.13, pg 91
    bool XMLListObject::AS3_hasSimpleContent ()
    {
        if (!_length())
        {
            return true;
        }
        else if (_length() == 1)
        {
            return _getNodeAt(0)->hasSimpleContent();
        }
        else
        {
            for (uint32_t i = 0; i < _length(); i++)
            {
                if (_getNodeAt(i)->getClass() == E4XNode::kElement)
                {
                    return false;
                }
            }
        }

        return true;
    }

    uint32_t XMLListObject::AS3_length() const
    {
        return _length();
    }

    XMLListObject *XMLListObject::AS3_normalize ()
    {
        AvmCore *core = this->core();

        uint32_t i = 0;
        while (i < _length())
        {
            E4XNode* xn = _getNodeAt(i);
            if (xn->getClass() == E4XNode::kElement)
            {
                XMLObject *x = _getAt(i);
                x->normalize();
                i++;
            }
            else if ((xn->getClass() & (E4XNode::kText | E4XNode::kCDATA)))
            {
                while (((i + 1) < _length()) && ((_getNodeAt(i + 1)->getClass() & (E4XNode::kText | E4XNode::kCDATA))))
                {
                    xn->setValue (core->concatStrings(xn->getValue(), _getNodeAt(i + 1)->getValue()));
                    deleteAtomProperty(core->intToAtom(i + 1));
                }
                if (xn->getValue()->length() == 0)
                {
                    deleteAtomProperty(core->intToAtom(i));
                }
                else
                {
                    i++;
                }
            }
            else
            {
                i++;
            }
        }

        return this;
    }

    Atom XMLListObject::AS3_parent ()
    {
        if (!_length())
            return undefinedAtom;

        E4XNode *parent = _getNodeAt(0)->getParent();
        for (uint32_t i = 1; i < _length(); i++)
        {
            E4XNode *p = _getNodeAt(i)->getParent();
            if (parent != p)
                return undefinedAtom;
        }

        if (parent)
            return _getAt(0)->getParent()->atom();
        else
            return undefinedAtom;
    }

    XMLListObject *XMLListObject::AS3_processingInstructions (Atom name) // name defaults to '*'
    {
        AvmCore *core = this->core();
        XMLListObject *m = XMLListObject::create(core->GetGC(), toplevel()->xmlListClass(), this->atom());

        for (uint32_t i = 0; i < _length(); i++)
        {
            if (_getNodeAt(i)->getClass() == E4XNode::kElement)
            {
                XMLObject *x = _getAt(i);
                XMLListObject *rxl = x->processingInstructions(name);
                if (rxl && rxl->_length())
                {
                    m->_append (rxl->atom());
                }
            }
        }

        return m;
    }

    bool XMLListObject::XMLList_AS3_propertyIsEnumerable(Atom P) // NOT virtual, NOT an override
    {
        double index = AvmCore::number(P);
        if ((index >= 0.0) && (index < _length()))
            return true;

        return false;
    }

    XMLListObject *XMLListObject::AS3_text ()
    {
        AvmCore *core = this->core();

        XMLListObject *m = XMLListObject::create(core->GetGC(), toplevel()->xmlListClass(), this->atom());

        for (uint32_t i = 0; i < _length(); i++)
        {
            if (_getNodeAt(i)->getClass() == E4XNode::kElement)
            {
                XMLObject *x = _getAt(i);
                XMLListObject *rxl = x->text();
                if (rxl && rxl->_length())
                {
                    m->_append (rxl->atom());
                }
            }
        }

        return m;
    }

    // E4X 10.1.2, page 28
    Stringp XMLListObject::toString()
    {
        AvmCore *core = this->core();
        if (hasSimpleContent ())
        {
            Stringp output = core->kEmptyString;

            // s is the empty string
            // for all props in children array
            // if (class != comment or processing instruction)
            // s = s + toString(prop)
            for (uint32_t i = 0; i < _length(); i++)
            {
                E4XNode* xn = _getNodeAt(i);
                if ((xn->getClass() != E4XNode::kComment) && (xn->getClass() != E4XNode::kProcessingInstruction))
                {
                    XMLObject *xm = _getAt(i);
                    output = core->concatStrings(output, xm->toString());
                }
            }

            return output;
        }
        else
        {
            StringBuffer output(core);
            this->__toXMLString(output, nullStringAtom, 0);
            return core->newStringUTF8(output.c_str());
        }
    }

    Stringp XMLListObject::AS3_toString()
    {
        return toString();
    }

    String *XMLListObject::AS3_toXMLString ()
    {
        StringBuffer output(core());
        this->__toXMLString(output, nullStringAtom, 0);
        return core()->newStringUTF8(output.c_str());
    }

    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    // Routines below are not in the spec but work if XMLList has one element
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    XMLObject *XMLListObject::AS3_addNamespace (Atom _namespace)
    {
        if (_length() == 1)
        {
            return _getAt(0)->addNamespace(_namespace);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("addNamespace"));
            return 0;
        }
    }

    XMLObject *XMLListObject::AS3_appendChild (Atom child)
    {
        if (_length() == 1)
        {
            return _getAt(0)->appendChild (child);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("appendChild"));
            return 0;
        }
    }

    int XMLListObject::AS3_childIndex ()
    {
        if (_length() == 1)
        {
            return _getNodeAt(0)->childIndex ();
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("childIndex"));
            return -1;
        }
    }

    ArrayObject *XMLListObject::AS3_inScopeNamespaces ()
    {
        if (_length() == 1)
        {
            return _getAt(0)->inScopeNamespaces();
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("inScopeNamespaces"));
            return 0;
        }
    }

    Atom XMLListObject::AS3_insertChildAfter (Atom child1, Atom child2)
    {
        if (_length() == 1)
        {
            return _getAt(0)->insertChildAfter (child1, child2);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("insertChildAfter"));
            return undefinedAtom;
        }
    }

    Atom XMLListObject::AS3_insertChildBefore (Atom child1, Atom child2)
    {
        if (_length() == 1)
        {
            return _getAt(0)->insertChildBefore (child1, child2);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("insertChildBefore"));
            return undefinedAtom;
        }
    }

    Atom XMLListObject::AS3_name()
    {
        if (_length() == 1)
        {
            return _getAt(0)->name();
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("name"));
            return nullStringAtom;
        }
    }


    Atom XMLListObject::_namespace (Atom prefix, int argc) // prefix is optional
    {
        AvmAssert(argc == 0 || argc == 1);

        if (_length() == 1)
        {
            return _getAt(0)->_namespace(prefix, argc);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("namespace"));
            return nullStringAtom;
        }
    }

    Atom XMLListObject::AS3_localName ()
    {
        if (_length() == 1)
        {
            return _getAt(0)->localName();
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("localName"));
            return nullStringAtom;
        }
    }

    ArrayObject *XMLListObject::AS3_namespaceDeclarations ()
    {
        if (_length() == 1)
        {
            return _getAt(0)->namespaceDeclarations();
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("namespaceDeclarations"));
            return 0;
        }
    }

    String *XMLListObject::AS3_nodeKind ()
    {
        // if our list has one element, return the nodeKind of the first element
        if (_length() == 1)
        {
            return _getNodeAt(0)->nodeKind(toplevel());
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("nodeKind"));
            return 0;
        }
    }

    XMLObject *XMLListObject::AS3_prependChild (Atom value)
    {
        if (_length() == 1)
        {
            return _getAt(0)->prependChild(value);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("prependChild"));
            return 0;
        }
    }

    XMLObject *XMLListObject::AS3_removeNamespace (Atom _namespace)
    {
        if (_length() == 1)
        {
            return _getAt(0)->removeNamespace (_namespace);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("removeNamespace"));
            return 0;
        }
    }

    XMLObject *XMLListObject::AS3_replace (Atom propertyName, Atom value)
    {
        if (_length() == 1)
        {
            return _getAt(0)->replace(propertyName, value);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("replace"));
            return 0;
        }
    }

    XMLObject *XMLListObject::AS3_setChildren (Atom value)
    {
        if (_length() == 1)
        {
            return _getAt(0)->setChildren (value);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("setChildren"));
            return 0;
        }
    }

    void XMLListObject::AS3_setLocalName (Atom name)
    {
        if (_length() == 1)
        {
            _getAt(0)->setLocalName (name);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("setLocalName"));
        }
    }

    void XMLListObject::AS3_setName (Atom name)
    {
        if (_length() == 1)
        {
            _getAt(0)->setName(name);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("setName"));
        }
    }

    void XMLListObject::AS3_setNamespace (Atom ns)
    {
        if (_length() == 1)
        {
            _getAt(0)->setNamespace (ns);
        }
        else
        {
            // throw type error
            toplevel()->throwTypeError(kXMLOnlyWorksWithOneItemLists, core()->toErrorString("setNamespace"));
        }
    }

#ifdef XML_FILTER_EXPERIMENT
    XMLListObject * XMLListObject::filter (Atom propertyName, Atom value)
    {
        Multiname m;
        toplevel()->ToXMLName(propertyName, m);

        Multiname name;
        toplevel()->CoerceE4XMultiname(&m, name);

        // filter opcode experiment
        XMLListObject *l = XMLListObject::create(core()->gc, toplevel()->xmlListClass());
        for (uint32_t i = 0; i < _length(); i++)
        {
            XMLObject *xm = _getAt(i);
            xm->_filter (l, name, value);
        }

        return l;
    }
#endif // XML_FILTER_EXPERIMENT

    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    // reserved for now
    void XMLListObject::setNotification(ScriptObject* /*f*/)
    {
        // nop
    }

    ScriptObject* XMLListObject::getNotification()
    {
        return 0;
    }

#ifdef DEBUGGER
    uint64_t XMLListObject::bytesUsed() const
    {
        // To calculate the memory used by an XMLList, we add the memory taken
        // by each XML or E4XNode that we point to.
        //
        // FIXME As described in https://bugzilla.mozilla.org/show_bug.cgi?id=552307 ,
        // this implementation can cause the same memory to be accounted for more
        // than once in the profiler.  It is easy for the user to have multiple
        // XMLLists and XMLs that point into the same tree of E4XNodes.  For now,
        // there is not much we can do about this.  A possible fix is described in
        // https://bugzilla.mozilla.org/show_bug.cgi?id=558385

        uint64_t size = ScriptObject::bytesUsed();
        size += m_children.bytesUsed();

        // It's possible -- likely, in fact -- that several XML elements in an
        // XMLList will actually be pointers into the same tree of E4XNodes.
        // We don't want to count their memory twice, so this hashtable keeps
        // track of the root E4XNodes whose memory we have already accounted
        // for.
        HeapHashtable* seenXmlRoots = HeapHashtable::create(gc());

        for (uint32_t i=0, n=m_children.length(); i<n; ++i)
        {
            Atom child = m_children.get(i);

            // list members can be either XMLObjects or E4XNodes
            XMLObject* xmlobj = AvmCore::atomToXMLObject(child);
            E4XNode* node;
            if (xmlobj)
                node = xmlobj->getNode();
            else
                node = (E4XNode*)AvmCore::atomToGenericObject(child);

            if (node)
            {
                // find the root node of the tree
                E4XNode* root = node;
                while (root->getParent())
                    root = root->getParent();

                Atom atomizedRoot = AvmCore::genericObjectToAtom(root);

                // Have we already seen that root node, as the root node of
                // some other XML object in this list?
                if (!seenXmlRoots->contains(atomizedRoot))
                {
                    seenXmlRoots->add(atomizedRoot, trueAtom);

                    // Again, the XMLList might contain XMLObjects or it might
                    // contain E4XNodes
                    if (xmlobj)
                        size += xmlobj->bytesUsed();
                    else
                        size += node->bytesUsed();
                }
                else
                {
                    // We have already accounted for the memory taken by the
                    // tree of E4XNodes, but we still need to take into
                    // account the small amount of memory taken by the XMLObject
                    // that points to that tree.
                    if (xmlobj)
                        size += xmlobj->bytesUsedShallow();
                }
            }
        }

        delete seenXmlRoots;

        return size;
    }
#endif
}

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PROXYGLUE_INCLUDED
#define PROXYGLUE_INCLUDED

namespace avmplus
{
    //
    // ProxyClass
    //

    class GC_AS3_EXACT(ProxyClass, ClassClosure)
    {
        friend class ProxyObject;
    public:
        ProxyClass(VTable* vtable);

    private:
        Binding initOneBinding(GCRef<Namespace> ns, const char* nm);

    private:
        GC_DATA_BEGIN(ProxyClass);

        Binding m_getProperty;
        Binding m_setProperty;
        Binding m_hasProperty;
        Binding m_deleteProperty;

        Binding m_callProperty;

        Binding m_getDescendants;

        Binding m_nextNameIndex;
        Binding m_nextName;
        Binding m_nextValue;
        
        GC_DATA_END(ProxyClass);
        
        DECLARE_SLOTS_ProxyClass;
    };

    //
    // ProxyObject
    //

    class GC_AS3_EXACT(ProxyObject, ScriptObject)
    {
        friend class ProxyClass;
    protected:
        ProxyObject(VTable* ivtable, ScriptObject* delegate)
            : ScriptObject(ivtable, delegate)
        {
        }

    public:
        bool flash_proxy_isAttribute(Atom name);

        // overrides from ScriptObject
        virtual Atom getAtomProperty(Atom name) const;
        virtual void setAtomProperty(Atom name, Atom value);
        virtual bool deleteAtomProperty(Atom name);
        virtual bool hasAtomProperty(Atom name) const;

        virtual Atom getMultinameProperty(const Multiname* name) const;
        virtual void setMultinameProperty(const Multiname* name, Atom value);
        virtual bool deleteMultinameProperty(const Multiname* name);
        virtual bool hasMultinameProperty(const Multiname* name) const;

        virtual Atom getUintProperty(uint32_t i) const;
        virtual void setUintProperty(uint32_t i, Atom value);
        virtual bool delUintProperty(uint32_t i);
        virtual bool hasUintProperty(uint32_t i) const;

        virtual Atom callProperty(const Multiname* multiname, int argc, Atom* argv);

        virtual Atom getDescendants(const Multiname* name) const;

        virtual Atom nextName(int index);
        virtual Atom nextValue(int index);
        virtual int nextNameIndex(int index);
        
    private:
        GC_NO_DATA(ProxyObject);
        DECLARE_SLOTS_ProxyObject;
    };
}

#endif /* PROXYGLUE_INCLUDED */

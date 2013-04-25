/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"


namespace avmplus
{
    using namespace avmplus::NativeID;

    //
    // ProxyClass
    //

    ProxyClass::ProxyClass(VTable *vtable)
        : ClassClosure(vtable)
    {
        createVanillaPrototype();

        AvmCore* const core = this->core();
        Atom const uri = core->internConstantStringLatin1("http://www.adobe.com/2006/actionscript/flash/proxy")->atom();
        GCRef<Namespace> ns = core->internNamespace(core->newNamespace(uri));

        m_getProperty       = initOneBinding(ns, "getProperty");
        m_setProperty       = initOneBinding(ns, "setProperty");
        m_hasProperty       = initOneBinding(ns, "hasProperty");
        m_deleteProperty    = initOneBinding(ns, "deleteProperty");
        m_callProperty      = initOneBinding(ns, "callProperty");
        m_getDescendants    = initOneBinding(ns, "getDescendants");
        m_nextNameIndex     = initOneBinding(ns, "nextNameIndex");
        m_nextValue         = initOneBinding(ns, "nextValue");
        m_nextName          = initOneBinding(ns, "nextName");
    }

    Binding ProxyClass::initOneBinding(GCRef<Namespace> ns, const char* nm)
    {
        Multiname mn(ns, core()->internConstantStringLatin1(nm));
        Binding b = toplevel()->getBinding(this->vtable->traits->itraits, &mn);
        AvmAssert(AvmCore::isMethodBinding(b));
        return b;
    }

    //
    // ProxyObject
    //
    
    //
    // Native methods for class
    //

    bool ProxyObject::flash_proxy_isAttribute(Atom name)
    {
        if (!AvmCore::isQName(name))
            return false;

        GCRef<QNameObject> q = AvmCore::atomToQName(name);
        return q->isAttr();
    }

    //
    // Virtual methods from ScriptObject invoked by the VM to reference and/or modify members.
    // Each of these is redirected to an ActionScript method of the same name, so a subclass
    // of Proxy can handle them. If unhandled, a native method (of the same name) will get invoked,
    // which forwards to our superclass's method.
    //

    Atom ProxyObject::getAtomProperty(Atom name) const
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Binding b = builtinClasses->get_ProxyClass()->m_getProperty;
        Atom argv[2] = { atom(), name };
        int const argc = 1;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv);
    }

    void ProxyObject::setAtomProperty(Atom name, Atom value)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Binding b = builtinClasses->get_ProxyClass()->m_setProperty;
        Atom argv[3] = { atom(), name, value };
        int const argc = 2;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        method->coerceEnter(argc, argv);
    }

    bool ProxyObject::deleteAtomProperty(Atom name)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Binding b = builtinClasses->get_ProxyClass()->m_deleteProperty;
        Atom argv[] = { atom(), name };
        int const argc = 1;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv) != falseAtom;
    }

    bool ProxyObject::hasAtomProperty(Atom name) const
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Binding b = builtinClasses->get_ProxyClass()->m_hasProperty;
        Atom argv[2] = { atom(), name };
        int const argc = 1;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv) != falseAtom;
    }

    Atom ProxyObject::getMultinameProperty(const Multiname* name) const
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        GCRef<QNameObject> q = QNameObject::create(core()->GetGC(), builtinClasses->get_QNameClass(), *name);
        Atom argv[] = { atom(), q->atom() };
        int const argc = 1;
        
        Binding b = builtinClasses->get_ProxyClass()->m_getProperty;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv);
    }

    void ProxyObject::setMultinameProperty(const Multiname*  name, Atom value)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        GCRef<QNameObject> q = QNameObject::create(core()->GetGC(), builtinClasses->get_QNameClass(), *name);
        Atom argv[] = { atom(), q->atom(), value };
        int const argc = 2;
        
        Binding b = builtinClasses->get_ProxyClass()->m_setProperty;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        method->coerceEnter(argc, argv);
    }

    bool ProxyObject::deleteMultinameProperty(const Multiname*  name)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        GCRef<QNameObject> q = QNameObject::create(core()->GetGC(), builtinClasses->get_QNameClass(), *name);
        Atom argv[] = { atom(), q->atom() };
        int const argc = 1;
        
        Binding b = builtinClasses->get_ProxyClass()->m_deleteProperty;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv) != falseAtom;
    }

    bool ProxyObject::hasMultinameProperty(const Multiname*  name) const
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        GCRef<QNameObject> q = QNameObject::create(core()->GetGC(), builtinClasses->get_QNameClass(), *name);
        Atom argv[] = { atom(), q->atom() };
        int const argc = 1;
        
        Binding b = builtinClasses->get_ProxyClass()->m_hasProperty;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv) != falseAtom;
    }

    Atom ProxyObject::getUintProperty(uint32_t i) const
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Atom argv[] = { atom(), core()->internUint32(i)->atom() };
        int const argc = 1;
        
        Binding b = builtinClasses->get_ProxyClass()->m_getProperty;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv);
    }

    void ProxyObject::setUintProperty(uint32_t i, Atom value)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Atom argv[] = { atom(), core()->internUint32(i)->atom(), value };
        int const argc = 2;
        
        Binding b = builtinClasses->get_ProxyClass()->m_setProperty;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        method->coerceEnter(argc, argv);
    }

    bool ProxyObject::delUintProperty(uint32_t i)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Binding b = builtinClasses->get_ProxyClass()->m_deleteProperty;
        Atom argv[] = { atom(), core()->internUint32(i)->atom() };
        int const argc = 1;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv) != falseAtom;
    }

    bool ProxyObject::hasUintProperty(uint32_t i) const
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Binding b = builtinClasses->get_ProxyClass()->m_hasProperty;
        Atom argv[] = { atom(), core()->internUint32(i)->atom() };
        int const argc = 1;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv) != falseAtom;
    }

    Atom ProxyObject::callProperty(const Multiname*  name, int argc, Atom* argv)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        int proxy_argc = argc+1;
        MMgc::GC::AllocaAutoPtr proxy_argv_autoptr;
        Atom* proxy_argv = (Atom*) avmStackAllocArray(core(), proxy_argv_autoptr, proxy_argc+1, sizeof(Atom));

        GCRef<QNameObject> q = QNameObject::create(core()->GetGC(), builtinClasses->get_QNameClass(), *name);

        proxy_argv[0] = atom();
        proxy_argv[1] = q->atom();
        for (int i = 1; i <= argc; i++)
        {
            proxy_argv[i+1] = argv[i];
        }
        Binding b = builtinClasses->get_ProxyClass()->m_callProperty;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(proxy_argc, proxy_argv);
    }

    Atom ProxyObject::getDescendants(const Multiname*  name) const
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        GCRef<QNameObject> q = QNameObject::create(core()->GetGC(), builtinClasses->get_QNameClass(), *name);
        Atom argv[] = { atom(), q->atom() };
        int const argc = 1;

        Binding b = builtinClasses->get_ProxyClass()->m_getDescendants;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv);
    }

    Atom ProxyObject::nextName(int index)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Atom argv[] = { atom(), core()->intToAtom (index) };
        int const argc = 1;
        
        Binding b = builtinClasses->get_ProxyClass()->m_nextName;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv);
    }

    Atom ProxyObject::nextValue(int index)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Atom argv[] = { atom(), core()->intToAtom (index) };
        int const argc = 1;
        
        Binding b = builtinClasses->get_ProxyClass()->m_nextValue;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return method->coerceEnter(argc, argv);
    }

    int ProxyObject::nextNameIndex(int index)
    {
        GCRef<Toplevel> toplevel = this->toplevel();
        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        Atom argv[] = { atom(), core()->intToAtom (index) };
        int const argc = 1;
        
        Binding b = builtinClasses->get_ProxyClass()->m_nextNameIndex;

        GCRef<MethodEnv> method = vtable->methods[AvmCore::bindingToMethodId(b)];
        Atom result = method->coerceEnter(argc, argv);
        return AvmCore::integer(result);
    }
}


/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmshell.h"

#ifndef AVMSHELL_BUILD
#error "This file is only for use with avmshell"
#endif

namespace avmplus
{
    using namespace avmshell;
    
    DomainObject::DomainObject(VTable *vtable, ScriptObject *delegate)
        : ScriptObject(vtable, delegate)
        , domainEnv(delegate->core()->codeContext()->domainEnv())
        , domainToplevel(delegate->toplevel())
    {
    }

    void DomainObject::init(DomainObject* parentDomain)
    {
        ShellCore* core = (ShellCore*) this->core();

        DomainEnv* baseDomainEnv = parentDomain ?
                                (DomainEnv*)parentDomain->domainEnv :
                                (DomainEnv*)NULL;
        Domain* baseDomain = baseDomainEnv ?
                                baseDomainEnv->domain() :
                                NULL;

        domainToplevel = parentDomain ?
                            (Toplevel*)parentDomain->domainToplevel :
                            core->createShellToplevel();

        Domain* domain = Domain::newDomain(core, baseDomain);
        domainEnv = DomainEnv::newDomainEnv(core, domain, parentDomain ? (DomainEnv*)parentDomain->domainEnv : (DomainEnv*)NULL);
    }

    Atom DomainObject::loadBytes(ByteArrayObject* b, uint32_t swfVersion)
    {
        AvmCore* core = this->core();
        if (!b)
            toplevel()->throwTypeError(kNullArgumentError, core->toErrorString("bytes"));

        // parse new bytecode
        size_t len = b->get_length();
        ScriptBuffer code = core->newScriptBuffer(len);
        VMPI_memcpy(code.getBuffer(), &b->GetByteArray()[0], len);

        Toplevel* toplevel = domainToplevel;
        ApiVersion apiVersion = core->getApiVersionFromCallStack();

        // parse constants and attributes.
        PoolObject* pool = core->parseActionBlock(code,
                                /*start*/0,
                                toplevel,
                                domainEnv->domain(),
                                /*ninit*/NULL,
                                apiVersion);


        // by default, use the same bugCompatibility as the builtins use
        const BugCompatibility* bugCompatibility = toplevel->abcEnv()->codeContext()->bugCompatibility();
        if (swfVersion != 0)
        {
            // ...unless specified otherwise.
            for (int j = 0; j < BugCompatibility::VersionCount; ++j)
            {
                if (BugCompatibility::kNames[j] == swfVersion)
                {
                    bugCompatibility = core->createBugCompatibility((BugCompatibility::Version)j);
                    goto done;
                }
            }
            // if we get here, didn't find a valid name
            toplevel->throwTypeError(kInvalidArgumentError, core->toErrorString("swfVersion"));
        }
done:

        ShellCodeContext* codeContext = new (core->GetGC()) ShellCodeContext(domainEnv, bugCompatibility);
        return core->handleActionPool(pool, toplevel, codeContext);
    }

    ScriptObject* DomainObject::finddef(const Multiname& multiname,
                                        DomainEnv* domainEnv)
    {
        Toplevel* toplevel = this->toplevel();

        ScriptEnv* script = core()->domainMgr()->findScriptEnvInDomainEnvByMultiname(domainEnv, multiname);
        if (script == (ScriptEnv*)BIND_AMBIGUOUS)
            toplevel->throwReferenceError(kAmbiguousBindingError, multiname);

        if (script == (ScriptEnv*)BIND_NONE)
            toplevel->throwReferenceError(kUndefinedVarError, multiname);

        if (script->global == NULL)
        {
            script->initGlobal();
            script->coerceEnter(script->global->atom());
        }

        return script->global;
    }

    ClassClosure* DomainObject::getClass(Stringp name)
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        if (name == NULL) {
            toplevel->throwArgumentError(kNullArgumentError, core->toErrorString("name"));
        }


        // Search for a dot from the end.
        int dot = name->lastIndexOf(core->cachedChars[(int)'.']);

        // If there is a '.', this is a fully-qualified
        // class name in a package.  Must turn it into
        // a namespace-qualified multiname.
        Namespace* ns;
        Stringp className;
        if (dot >= 0) {
            Stringp uri = core->internString(name->substring(0, dot));
            ns = core->internNamespace(core->newNamespace(uri, Namespace::NS_Public, core->getApiVersionFromCallStack()));
            className = core->internString(name->substring(dot+1, name->length()));
        } else {
            ns = core->findPublicNamespace();
            className = core->internString(name);
        }

        Multiname multiname(ns, className);

        ScriptObject *container = finddef(multiname, domainEnv);
        if (!container) {
            toplevel->throwTypeError(kClassNotFoundError, core->toErrorString(&multiname));
        }
        Atom atom = toplevel->getproperty(container->atom(),
                                            &multiname,
                                            container->vtable);

        // Note: this used to throw "kClassNotFoundError" if the class wasn't
        // found; now it throws "kCheckTypeFailedError" (by way of coerceToType).
        // This probably doesn't matter since this is a shell-only class, and it
        // gives us a testcase for coerceToType.
        return toplevel->builtinClasses()->get_ClassClass()->coerceToType(atom);
    }

    DomainClass::DomainClass(VTable *cvtable)
        : ClassClosure(cvtable)
    {
        createVanillaPrototype();
    }

    DomainObject* DomainClass::get_currentDomain()
    {
        return (DomainObject*) newInstance();
    }

    int DomainClass::get_MIN_DOMAIN_MEMORY_LENGTH()
    {
        return DomainEnv::GLOBAL_MEMORY_MIN_SIZE;
    }

    ByteArrayObject* DomainObject::get_domainMemory() const
    {
        return (ByteArrayObject*)domainEnv->get_globalMemory();
    }

    void DomainObject::set_domainMemory(ByteArrayObject* mem)
    {
        if(!domainEnv->set_globalMemory(mem))
            toplevel()->throwError(kEndOfFileError);
    }
}

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_DomainMgr__
#define __avmplus_DomainMgr__

namespace avmplus
{

/**
 *  The Domain manager implementation
 *  is responsible for all aspects of
 *  finding names in a given Domain stack.
 */
class DomainMgr : public MMgc::GCObject
{
public:
    DomainMgr(AvmCore* core);

    // DomainMgr methods:
    void addNamedTraits(PoolObject* pool, Stringp name, Namespacep ns, Traits* traits);
    void addNamedInstanceTraits(PoolObject* pool, Stringp name, Namespacep ns, Traits* itraits);
    Traits* findBuiltinTraitsByName(PoolObject* pool, Stringp name);
    Traits* findTraitsInPoolByNameAndNS(PoolObject* pool, Stringp name, Namespacep ns);
    Traits* findTraitsInPoolByMultiname(PoolObject* pool, const Multiname& multiname);

    void addNamedScript(PoolObject* pool, Stringp name, Namespacep ns, MethodInfo* script);
    MethodInfo* findScriptInPoolByMultiname(PoolObject* pool, const Multiname& multiname);

    void addNamedScriptEnvs(AbcEnv* abcEnv, const GCList<ScriptEnv>& envs);
    ScriptEnv* findScriptEnvInDomainEnvByMultiname(DomainEnv* domainEnv, const Multiname& multiname);
    ScriptEnv* findScriptEnvInAbcEnvByMultiname(AbcEnv* abcEnv, const Multiname& multiname);

#ifdef DEBUGGER
    ScriptEnv* findScriptEnvInAbcEnvByNameOnly(AbcEnv* abcEnv, Stringp name);
#endif

private:
    Traits* findTraitsInDomainByNameAndNSImpl(Domain* domain, Stringp name, Namespacep ns, bool cacheIfFound);
    Traits* findTraitsInPoolByNameAndNSImpl(PoolObject* pool, Stringp name, Namespacep ns, bool cacheIfFound);
    MethodInfo* findScriptInDomainByNameAndNSImpl(Domain* domain, Stringp name, Namespacep ns, bool cacheIfFound);
    MethodInfo* findScriptInDomainByMultinameImpl(Domain* domain, const Multiname& multiname, Namespace*& nsFound);
    MethodInfo* findScriptInPoolByNameAndNSImpl(PoolObject* pool, Stringp name, Namespacep ns, bool cacheIfFound);
    ScriptEnv* findScriptEnvInDomainEnvByMultinameImpl(DomainEnv* domainEnv, const Multiname& multiname);
#ifdef DEBUGGER
    ScriptEnv* findScriptEnvInDomainEnvByNameOnlyImpl(DomainEnv* domainEnv, Stringp name);
#endif

    ScriptEnv* mapScriptToScriptEnv(DomainEnv* domainEnv, MethodInfo* mi);
    MethodInfo* findScriptInPoolByMultinameImpl(PoolObject* pool, const Multiname& multiname);
#ifdef DEBUGGER
    MethodInfo* findScriptInDomainByNameOnlyImpl(Domain* domain, Stringp name, Namespace*& nsFound);
    MethodInfo* findScriptInPoolByNameOnlyImpl(PoolObject* pool, Stringp name);
#endif

private:
    AvmCore* const core;
};


}
#endif // __avmplus_DomainMgr__

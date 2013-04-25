/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{

/*
    Theory of Operation

    Definitions:
    ------------
    parent - reference to parent Domain. Will be null in the "root" (system) domain

    loaded - a map from class names to Classdef objects. (Classdef objects represent ready-to-use
    class definitions.) This map is populated with classes loaded from SWFs via the load method.
    
    cache - another map from class names to Classdef objects. The cache is populated as classes
    are found during lookups via the find method.

    There are two general classes of operations:
    
    "add" :     these methods are used when we load a chunk of ABC bytecode; they populate the "loaded"
                maps, and simply indicate what name is associated with what Traits/Script definition
                at a given Domain level.
                
                Note that, as an optimization, these methods check for reachability; attempting to
                add a name that is already defined at the current level will simply be ignored. This
                includes names that are "shadowed" by a name definition from a parent domain, and is the
                source of the "cacheIfFound" flag seen on various "find" methods: when adding new
                definitions, we want to examine any already-cached items (to exclude shadowed names),
                but we don't want to add to the cache prematurely (we want to defer that until
                we know a definition is likely to be used, via the "find" methods).

    "find" :    look up a Traits/Script definition by name. This is the analog in our scheme of Java’s
                ClassLoader.loadClass method, and implements the same lookup algorithm, which is:
                
                - look in our cache for an already-resolved class by this name
                - if none is found, immediately call find in the parent domain
                - if this doesn’t find a class, look in our currently loaded classes
                - if a class has been found, add it to our cache
                - return the resolved class, or null.

                Important properties of this algorithm:
                
                1. Because of the immediate recursive call to the parent domain, the "topmost" class resident in
                memory at the time of the lookup will be found. Here "topmost" means the class closest to the
                root domain, along the path from the root to the domain in which find is called. (Note
                that our actual implementation is iterative, not recursive.)
                
                2. Because of caching, once a given class has been returned from a find request in a given
                domain, that class will always be returned from subsequent calls to find in that domain. (This
                guarantees the binding stability property that we want.)
                
                3. When a class is requested from a "descendant" domain, and found in an "ancestor" domain,
                the found class will be added to the cache of both the descendent and ancestor domains.
                (It is not necessary to add to any Domains that may be in between the descendant and ancestor,
                as any lookups to intermediate domains would find the cached value in the ancestor as well,
                thus lazily getting the same effect.)

*/

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=636565
inline bool isValidBinding(MethodInfo* mi)
{
    return mi != NULL && mi != (MethodInfo*)BIND_AMBIGUOUS;
}
#endif

DomainMgr::DomainMgr(AvmCore* _core) : core(_core)
{
}

void DomainMgr::addNamedTraits(PoolObject* pool, Stringp name, Namespacep ns, Traits* traits)
{
    // if we can find an existing trait by this name, the one we're adding
    // if unreachable -- don't bother adding it. (Note that this examines
    // only the Domain's table, not the Pool's)
    // Note: It's important not to cache on find here; see earlier comments for details.
    if (findTraitsInDomainByNameAndNSImpl(pool->domain, name, ns, /*cacheIfFound*/false) == NULL)
    {
        pool->domain->m_loadedTraits->add(name, ns, traits);
    }
}

void DomainMgr::addNamedInstanceTraits(PoolObject* pool, Stringp name, Namespacep ns, Traits* itraits)
{
    // if we can find an existing trait by this name, the one we're adding
    // if unreachable -- don't bother adding it.
    // Note: It's important not to cache on find here; see earlier comments for details.
    if (findTraitsInPoolByNameAndNSImpl(pool, name, ns, /*cacheIfFound*/false) == NULL)
    {
        pool->m_loadedTraits->add(name, ns, itraits);
    }
}

Traits* DomainMgr::findBuiltinTraitsByName(PoolObject* pool, Stringp name)
{
    return pool->m_loadedTraits->getName(name);
}

Traits* DomainMgr::findTraitsInDomainByNameAndNSImpl(Domain* domain, Stringp name, Namespacep ns, bool cacheIfFound)
{
    Traits* traits = NULL;
    
    // First, look bottom-up to find the first cached instance.
    for (uint32_t i = 0, n = domain->m_baseCount; i < n; ++i)
    {
        Domain* d = domain->m_bases[i];
        if ((traits = (Traits*)d->m_cachedTraits->get(name, ns)) != NULL)
        {
            if (cacheIfFound)
            {
                if (i > 0)
                {
                    // If cacheIfFound is true, we need to ensure the result
                    // is cached in both the "d" and "domain". But we know
                    // it's already in "d", so we only need to do anything
                    // if d != domain (which is true iff i > 0, since base[0] == domain)
                    AvmAssert(d != domain);
                    domain->m_cachedTraits->add(name, ns, traits);
                }
            }
            return traits;
        }
    }

    // No instance ever cached, so look top-down to find the first loaded instance.
    // Note that this goes from N...1 (rather than N-1...0) so we can re-use the same unsigned var.
    for (uint32_t i = domain->m_baseCount; i > 0; --i)
    {
        Domain* d = domain->m_bases[i-1];
        if ((traits = (Traits*)d->m_loadedTraits->get(name, ns)) != NULL)
        {
            if (cacheIfFound)
            {
                // If cacheIfFound is true, we need to ensure the result
                // is cached in both the "d" and "domain". But we know
                // it's already in "d", so we only need to do anything
                // if d != domain (which is true iff i > 1, since base[1-1] == domain)
                if (i > 1)
                {
                    AvmAssert(d != domain);
                    d->m_cachedTraits->add(name, ns, traits);
                }
                domain->m_cachedTraits->add(name, ns, traits);
            }
            return traits;
        }
    }

    return NULL;
}

Traits* DomainMgr::findTraitsInPoolByNameAndNSImpl(PoolObject* pool, Stringp name, Namespacep ns, bool cacheIfFound)
{
    Traits* traits = (Traits*)pool->m_cachedTraits->get(name, ns);
    if (traits == NULL)
    {
        traits = findTraitsInDomainByNameAndNSImpl(pool->domain, name, ns, cacheIfFound);
        if (traits == NULL)
        {
            traits = (Traits*)pool->m_loadedTraits->get(name, ns);
        }

        if (cacheIfFound && traits != NULL)
        {
            pool->m_cachedTraits->add(name, ns, traits);
        }
    }
    return traits;
}

Traits* DomainMgr::findTraitsInPoolByNameAndNS(PoolObject* pool, Stringp name, Namespacep ns)
{
    return findTraitsInPoolByNameAndNSImpl(pool, name, ns, /*cacheIfFound*/true);
}

Traits* DomainMgr::findTraitsInPoolByMultiname(PoolObject* pool, const Multiname& multiname)
{
    // do full lookup of multiname, error if more than 1 match
    // return Traits if 1 match, NULL if 0 match, BIND_AMBIGUOUS >1 match
    Traits* found = NULL;
    if (multiname.isBinding())
    {
        // multiname must not be an attr name, have wildcards, or have runtime parts.
        for (int32_t i=0, n=multiname.namespaceCount(); i < n; i++)
        {
            Traits* t = findTraitsInPoolByNameAndNSImpl(pool, multiname.getName(), multiname.getNamespace(i), /*cacheIfFound*/true);
            if (t != NULL)
            {
                if (found == NULL)
                {
                    found = t;
                }
                else if (found != t)
                {
                    // ambiguity
                    return (Traits*)BIND_AMBIGUOUS;
                }
            }
        }
    }
    return found;
}

void DomainMgr::addNamedScript(PoolObject* pool, Stringp name, Namespacep ns, MethodInfo* script)
{
    // if we can find an existing trait by this name, the one we're adding
    // if unreachable -- don't bother adding it. (Note that this examines
    // only the Domain's table, not the Pool's)
    // Note: It's important not to cache on find here; see earlier comments for details.
    if (ns->isPrivate())
    {
        if (findScriptInPoolByNameAndNSImpl(pool, name, ns, /*cacheIfFound*/false) == NULL)
            pool->m_loadedScripts->add(name, ns, script);
    }
    else
    {
        if (findScriptInDomainByNameAndNSImpl(pool->domain, name, ns, /*cacheIfFound*/false) == NULL)
            pool->domain->m_loadedScripts->add(name, ns, script);
    }
}

MethodInfo* DomainMgr::findScriptInDomainByNameAndNSImpl(Domain* domain, Stringp name, Namespacep ns, bool cacheIfFound)
{
    MethodInfo* mi = NULL;
    
    // First, look bottom-up to find the first cached instance.
    for (uint32_t i = 0, n = domain->m_baseCount; i < n; ++i)
    {
        Domain* d = domain->m_bases[i];
        if ((mi = (MethodInfo*)d->m_cachedScripts->get(name, ns)) != NULL)
        {
            if (cacheIfFound)
            {
                if (i > 0)
                {
                    AvmAssert(d != domain);
                    domain->m_cachedScripts->add(name, ns, mi);
                }
            }
            return mi;
        }
    }

    // No instance ever cached, so look top-down to find the first loaded instance.
    for (uint32_t i = domain->m_baseCount; i > 0; --i)
    {
        Domain* d = domain->m_bases[i-1];
        if ((mi = (MethodInfo*)d->m_loadedScripts->get(name, ns)) != NULL)
        {
            if (cacheIfFound)
            {
                if (i > 1)
                {
                    AvmAssert(d != domain);
                    d->m_cachedScripts->add(name, ns, mi);
                }
                domain->m_cachedScripts->add(name, ns, mi);
            }
            return mi;
        }
    }

    return NULL;
}

MethodInfo* DomainMgr::findScriptInDomainByMultinameImpl(Domain* domain, const Multiname& multiname, Namespace*& nsFound)
{
    MethodInfo* mi = NULL;
    
    // First, look bottom-up to find the first cached instance.
    for (uint32_t i = 0, n = domain->m_baseCount; i < n; ++i)
    {
        Domain* d = domain->m_bases[i];
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=636565
        mi = (MethodInfo*)d->m_cachedScripts->getMulti(multiname, /*out*/nsFound);
        if (isValidBinding(mi))
#else
        if ((mi = (MethodInfo*)d->m_cachedScripts->getMulti(multiname, /*out*/nsFound)) != NULL)
#endif
        {
            // if (cacheIfFound) -- always true here
            {
                if (i > 0)
                {
                    AvmAssert(d != domain);
                    domain->m_cachedScripts->add(multiname.getName(), nsFound, mi);
                }
            }
            return mi;
        }
    }

    // No instance ever cached, so look top-down to find the first loaded instance.
    for (uint32_t i = domain->m_baseCount; i > 0; --i)
    {
        Domain* d = domain->m_bases[i-1];
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=636565
        mi = (MethodInfo*)d->m_loadedScripts->getMulti(multiname, /*out*/nsFound);
        if (isValidBinding(mi))
#else
        if ((mi = (MethodInfo*)d->m_loadedScripts->getMulti(multiname, /*out*/nsFound)) != NULL)
#endif
        {
            // if (cacheIfFound) -- always true here
            {
                if (i > 1)
                {
                    AvmAssert(d != domain);
                    d->m_cachedScripts->add(multiname.getName(), nsFound, mi);
                }
                domain->m_cachedScripts->add(multiname.getName(), nsFound, mi);
            }
            return mi;
        }
    }

    return NULL;
}

MethodInfo* DomainMgr::findScriptInPoolByNameAndNSImpl(PoolObject* pool, Stringp name, Namespacep ns, bool cacheIfFound)
{
    MethodInfo* mi = (MethodInfo*)pool->m_cachedScripts->getName(name);
    if (mi == NULL)
    {
        mi = findScriptInDomainByNameAndNSImpl(pool->domain, name, ns, cacheIfFound);
        if (mi == NULL)
        {
            mi = (MethodInfo*)pool->m_loadedScripts->get(name, ns);
        }
        
        if (cacheIfFound && mi != NULL)
        {
            pool->m_cachedScripts->add(name, ns, mi);
        }
    }
    return mi;
}

MethodInfo* DomainMgr::findScriptInPoolByMultiname(PoolObject* pool, const Multiname& multiname)
{
    return findScriptInPoolByMultinameImpl(pool, multiname);
}

void DomainMgr::addNamedScriptEnvs(AbcEnv* abcEnv, const GCList<ScriptEnv>& envs)
{
    // If the MethodInfo for this ScriptEnv isn't in the Domain's or Pool's
    // map, then we must have filtered it out as unreachable: don't
    // bother adding the ScriptEnv, as we'll never need to look it up.
    // (Note that we don't need to bother checking the parent Domains
    // for this, since we want to check loaded, not cached.) We can't rely
    // on looking up by name, since scripts all tend to be named "global",
    // so instead we make a temporary map of all the entries in the relevant
    // Pool and Domain.

    PoolObject* pool = abcEnv->pool();
    DomainEnv* domainEnv = abcEnv->domainEnv();
    Domain* domain = domainEnv->domain();

    // we have no generic "set" type, so let's use a hashtable for the same purpose
    // (a bit more mem, but short-lived and better average lookup time than using List<>)
    HeapHashtable* ht = HeapHashtable::create(core->GetGC());
    for (StMNHTMethodInfoIterator iter(pool->m_loadedScripts); iter.next(); )
    {
        if (!iter.key()) continue;
        Atom const a = AvmCore::genericObjectToAtom(iter.value());
        ht->add(a, a);
    }
    for (StMNHTMethodInfoIterator iter(domain->m_loadedScripts); iter.next(); )
    {
        if (!iter.key()) continue;
        Atom const a = AvmCore::genericObjectToAtom(iter.value());
        ht->add(a, a);
    }

    for (uint32_t i = 0, n = envs.length(); i < n; ++i)
    {
        ScriptEnv* se = envs[i];
        AvmAssert(se->abcEnv() == abcEnv);
        MethodInfo* mi = se->method;
        AvmAssert(domainEnv->m_scriptEnvMap->get(mi) == NULL);
        if (ht->get(AvmCore::genericObjectToAtom(mi)) == undefinedAtom)
            continue;
        domainEnv->m_scriptEnvMap->add(mi, se);
    }

    delete ht;
}

ScriptEnv* DomainMgr::mapScriptToScriptEnv(DomainEnv* domainEnv, MethodInfo* mi)
{
    ScriptEnv* se = NULL;
    if (mi != NULL)
    {
        se = domainEnv->m_scriptEnvMap->get(mi);
        if (se == NULL)
        {
            // This is an item from a parent domain that's not in the MI->SE map yet.
            // walk up the parent chain and find it. This can happen if
            // we find a (cached) entry from a parent Domain; since addNamedScriptEnvs()
            // only added mappings for the MI/SE's in this Domain/DomainEnv pair,
            // we won't have the proper mapping. However, we know it must be in
            // a parent (which is why we start at 1, not 0: no point in looking
            // in our own map again), so lazily find it.
            for (uint32_t i = 1, n = domainEnv->m_baseCount; i < n; ++i)
            {
                DomainEnv* de = domainEnv->m_bases[i];
                if (mi->pool()->domain == de->domain())
                {
                    se = de->m_scriptEnvMap->get(mi);
                    AvmAssert(se != NULL);
                    domainEnv->m_scriptEnvMap->add(mi, se);
                    break;
                }
            }
        }
        // If the MethodInfo was entered, but the script then failed verification,
        // we might not have a matching ScriptEnv... so don't crash in Debug builds.
        // (Caller already expects NULL and checks for it.)
        AvmAssert(se == NULL || se->method == mi);
    }
    return se;
}

MethodInfo* DomainMgr::findScriptInPoolByMultinameImpl(PoolObject* pool, const Multiname& multiname)
{
    MethodInfo* mi = (MethodInfo*)pool->m_cachedScripts->getMulti(multiname);
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=694698
    if (!isValidBinding(mi))
#else
    if(mi == NULL)
#endif
    {
        Namespacep nsFound;
        mi = findScriptInDomainByMultinameImpl(pool->domain, multiname, /*out*/nsFound);
        if (mi == NULL)
        {
            mi = (MethodInfo*)pool->m_loadedScripts->getMulti(multiname, /*out*/nsFound);
        }
        
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=636565
        if (isValidBinding(mi))
#else
        if (mi != NULL)
#endif
        {
            AvmAssert(nsFound != NULL);
            pool->m_cachedScripts->add(multiname.getName(), nsFound, mi);
        }
    }
    return mi;
}

#ifdef DEBUGGER

MethodInfo* DomainMgr::findScriptInDomainByNameOnlyImpl(Domain* domain, Stringp name, Namespace*& nsFound)
{
    MethodInfo* mi = NULL;
    
    // First, look bottom-up to find the first cached instance.
    for (uint32_t i = 0, n = domain->m_baseCount; i < n; ++i)
    {
        Domain* d = domain->m_bases[i];
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=636565
        mi = (MethodInfo*)d->m_cachedScripts->getName(name, &nsFound);
        if (isValidBinding(mi))
#else
        if ((mi = (MethodInfo*)d->m_cachedScripts->getName(name, &nsFound)) != NULL)
#endif
        {
            // if (cacheIfFound) -- always true here
            {
                if (i > 0)
                {
                    AvmAssert(d != domain);
                    domain->m_cachedScripts->add(name, nsFound, mi);
                }
            }
            return mi;
        }
    }

    // No instance ever cached, so look top-down to find the first loaded instance.
    for (uint32_t i = domain->m_baseCount; i > 0; --i)
    {
        Domain* d = domain->m_bases[i-1];
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=636565
        mi = (MethodInfo*)d->m_loadedScripts->getName(name, &nsFound);
        if (isValidBinding(mi))
#else
        if ((mi = (MethodInfo*)d->m_loadedScripts->getName(name, &nsFound)) != NULL)
#endif
        {
            // if (cacheIfFound) -- always true here
            {
                if (i > 1)
                {
                    AvmAssert(d != domain);
                    d->m_cachedScripts->add(name, nsFound, mi);
                }
                domain->m_cachedScripts->add(name, nsFound, mi);
            }
            return mi;
        }
    }

    return NULL;
}

MethodInfo* DomainMgr::findScriptInPoolByNameOnlyImpl(PoolObject* pool, Stringp name)
{
    MethodInfo* mi = (MethodInfo*)pool->m_cachedScripts->getName(name);
    if (mi == NULL)
    {
        Namespacep nsFound = NULL;
        mi = findScriptInDomainByNameOnlyImpl(pool->domain, name, /*out*/nsFound);
        if (mi == NULL)
        {
            mi = (MethodInfo*)pool->m_loadedScripts->getName(name, /*out*/&nsFound);
        }
        
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=636565
        if (isValidBinding(mi))
#else
        if (mi != NULL)
#endif
        {
            AvmAssert(nsFound != NULL);
            pool->m_cachedScripts->add(name, nsFound, mi);
        }
    }
    return mi;
}

#endif // DEBUGGER


ScriptEnv* DomainMgr::findScriptEnvInDomainEnvByMultinameImpl(DomainEnv* domainEnv, const Multiname& multiname)
{
    Namespacep nsFound = NULL;
    MethodInfo* mi = findScriptInDomainByMultinameImpl(domainEnv->domain(), multiname, /*out*/nsFound);
    ScriptEnv* se = mapScriptToScriptEnv(domainEnv, mi);
    return se;
}

ScriptEnv* DomainMgr::findScriptEnvInDomainEnvByMultiname(DomainEnv* domainEnv, const Multiname& multiname)
{
    return findScriptEnvInDomainEnvByMultinameImpl(domainEnv, multiname);
}

ScriptEnv* DomainMgr::findScriptEnvInAbcEnvByMultiname(AbcEnv* abcEnv, const Multiname& multiname)
{
    MethodInfo* mi = findScriptInPoolByMultinameImpl(abcEnv->pool(), multiname);
    ScriptEnv* se = mapScriptToScriptEnv(abcEnv->domainEnv(), mi);
    return se;
}

#ifdef DEBUGGER

ScriptEnv* DomainMgr::findScriptEnvInDomainEnvByNameOnlyImpl(DomainEnv* domainEnv, Stringp name)
{
    Namespacep nsFound = NULL;
    MethodInfo* mi = findScriptInDomainByNameOnlyImpl(domainEnv->domain(), name, /*out*/nsFound);
    ScriptEnv* se = mapScriptToScriptEnv(domainEnv, mi);
    return se;
}

ScriptEnv* DomainMgr::findScriptEnvInAbcEnvByNameOnly(AbcEnv* abcEnv, Stringp name)
{
    MethodInfo* mi = findScriptInPoolByNameOnlyImpl(abcEnv->pool(), name);
    ScriptEnv* se = mapScriptToScriptEnv(abcEnv->domainEnv(), mi);
    return se;
}

#endif // DEBUGGER

} // namespace avmplus

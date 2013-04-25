/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


namespace avmplus
{

REALLY_INLINE ScopeTypeChain::ScopeTypeChain(int32_t _size, int32_t _fullsize, Traits* traits)
    : size(_size), fullsize(_fullsize), _traits(traits)
{
    AvmAssert(size == fullsize || size+1 == fullsize);
}

REALLY_INLINE const ScopeTypeChain* ScopeTypeChain::createEmpty(MMgc::GC* gc, Traits* traits)
{
    return create(gc, traits, NULL, NULL, NULL, NULL);
}

REALLY_INLINE Traits* ScopeTypeChain::traits() const
{
    return _traits;
}

REALLY_INLINE Traits* ScopeTypeChain::getScopeTraitsAt(uint32_t i) const
{
    return (Traits*)(_scopes[i] & ~ISWITH);
}

REALLY_INLINE bool ScopeTypeChain::getScopeIsWithAt(uint32_t i) const
{
    return (_scopes[i] & ISWITH) != 0;
}

REALLY_INLINE void ScopeTypeChain::setScopeAt(uint32_t i, Traits* t, bool w)
{
    _scopes[i] = uintptr_t(t) | (w ? ISWITH : 0);
}

REALLY_INLINE ScopeChain::ScopeChain(VTable* vtable, AbcEnv* abcEnv, const ScopeTypeChain* scopeTraits, Namespacep dxns)
    : _vtable(vtable), _abcEnv(abcEnv), _scopeTraits(scopeTraits), _defaultXmlNamespace(dxns)
{ }

#ifndef VMCFG_AOT  // Avoid premature inlining for AOT; it prevents CSE
REALLY_INLINE VTable* ScopeChain::vtable() const
{
    return _vtable;
}
#endif

REALLY_INLINE AbcEnv* ScopeChain::abcEnv() const
{
    return _abcEnv;
}

REALLY_INLINE const ScopeTypeChain* ScopeChain::scopeTraits() const
{
    return _scopeTraits;
}

REALLY_INLINE int32_t ScopeChain::getSize() const
{
    return _scopeTraits->size;
}

REALLY_INLINE Atom ScopeChain::getScope(int32_t i) const
{
    AvmAssert(i >= 0 && i < _scopeTraits->size);
    return _scopes[i];
}

REALLY_INLINE Namespacep ScopeChain::getDefaultNamespace() const
{
    return _defaultXmlNamespace;
}

} // namespace avmplus

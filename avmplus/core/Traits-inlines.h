/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE bool isAtomOrRCObjectSlot(SlotStorageType sst)
{
    return sst <= SST_scriptobject;
}

REALLY_INLINE bool isRCObjectSlot(SlotStorageType sst)
{
    return isAtomOrRCObjectSlot(sst) && sst != SST_atom;
}

REALLY_INLINE SlotStorageType TraitsBindings::SlotInfo::sst() const
{
    return SlotStorageType(offsetAndSST & 0xf);
}

REALLY_INLINE uint32_t TraitsBindings::SlotInfo::offset() const
{
    return (offsetAndSST >> 4) << 2;
}

REALLY_INLINE TraitsBindings::TraitsBindings(Traits* _owner,
                    TraitsBindingsp _base,
                    MultinameBindingHashtable* _bindings,
                    uint32_t _slotCount,
                    uint32_t _methodCount,
                    bool typesValid) :
    owner(_owner),
    base(_base),
    m_bindings(_bindings),
    slotCount(_slotCount),
    methodCount(_methodCount),
    m_slotSize(0),
    m_typesValid(typesValid?1:0)
{ }

REALLY_INLINE Traitsp TraitsBindings::getSlotTraits(uint32_t i) const
{
    AvmAssert(m_typesValid);
    AvmAssert(i < slotCount);
    return getSlots()[i].type;
}

REALLY_INLINE uint32_t TraitsBindings::getSlotOffset(uint32_t i) const
{
    AvmAssert(m_typesValid);
    AvmAssert(i < slotCount);
    return getSlots()[i].offset();
}

REALLY_INLINE SlotStorageType TraitsBindings::calcSlotAddrAndSST(uint32_t i, void* pin, void*& pout) const
{
    AvmAssert(m_typesValid);
    AvmAssert(i < slotCount);
    uint32_t offsetAndSST = getSlots()[i].offsetAndSST;
    pout = (void*)(((uint32_t*)pin) + (offsetAndSST >> 4));
    return SlotStorageType(offsetAndSST & 0xf);
}

REALLY_INLINE MethodInfo* TraitsBindings::getMethod(uint32_t i) const
{
    AvmAssert(m_typesValid);
    AvmAssert(i < methodCount);
    return getMethods()[i].f;
}

REALLY_INLINE int32_t TraitsBindings::next(int32_t index) const
{
    return m_bindings->next(index);
}

REALLY_INLINE Stringp TraitsBindings::keyAt(int32_t index) const
{
    return m_bindings->keyAt(index);
}

REALLY_INLINE Namespacep TraitsBindings::nsAt(int32_t index) const
{
    return m_bindings->nsAt(index);
}

REALLY_INLINE Binding TraitsBindings::valueAt(int32_t index) const
{
    return m_bindings->valueAt(index);
}

REALLY_INLINE TraitsBindings::SlotInfo* TraitsBindings::getSlots()
{
    AvmAssert(m_typesValid);
    return (SlotInfo*)(this + 1);
}

REALLY_INLINE const TraitsBindings::SlotInfo* TraitsBindings::getSlots() const
{
    AvmAssert(m_typesValid);
    return (const SlotInfo*)(this + 1);
}

REALLY_INLINE TraitsBindings::BindingMethodInfo* TraitsBindings::getMethods()
{
    AvmAssert(m_typesValid);
    return (BindingMethodInfo*)(getSlots() + slotCount);
}

REALLY_INLINE const TraitsBindings::BindingMethodInfo* TraitsBindings::getMethods() const
{
    AvmAssert(m_typesValid);
    return (const BindingMethodInfo*)(getSlots() + slotCount);
}

REALLY_INLINE void TraitsBindings::setSlotInfo(uint32_t i, Traits* t, SlotStorageType sst, uint32_t offset)
{
    AvmAssert(m_typesValid);
    AvmAssert(i < slotCount);
    // don't need WB here
    getSlots()[i].type = t;
    // offset is always a multiple of 4 so skip those, gives us a max of 1<<30-1
    AvmAssert((offset & 3) == 0);
    AvmAssert(offset <= MAX_SLOT_OFFSET);
    getSlots()[i].offsetAndSST = (offset<<2) | uint32_t(sst);
}

REALLY_INLINE void TraitsBindings::setMethodInfo(uint32_t i, MethodInfo* f)
{
    AvmAssert(m_typesValid);
    AvmAssert(i < methodCount);
    // don't need WB here
    getMethods()[i].f = f;
}

REALLY_INLINE StTraitsBindingsIterator::StTraitsBindingsIterator(TraitsBindingsp tb)
    : StMNHTBindingIterator(tb->m_bindings), _tb(tb)
{ }

REALLY_INLINE TraitsMetadata::TraitsMetadata(TraitsMetadatap _base, PoolObject* _residingPool, MetadataPtr _metadata_pos, uint32_t _slotCount, uint32_t _methodCount)
    : base(_base),
    residingPool(_residingPool),
    slotCount(_slotCount),
    methodCount(_methodCount),
    metadataPos(_metadata_pos),
    slotMetadataPos(NULL),
    methodMetadataPos(NULL)
{ }

REALLY_INLINE TraitsMetadata::MetadataPtr TraitsMetadata::getMetadataPos(PoolObject*& residingPool) const
{
    residingPool = this->residingPool;
    return metadataPos;
}

REALLY_INLINE const ScopeTypeChain* Traits::declaringScope() const
{
    // don't assert, some callers need to know
    // AvmAssert(m_declaringScope != NULL);
    return m_declaringScope;
}

REALLY_INLINE uint16_t Traits::getSizeOfInstance() const
{
    return m_sizeofInstance;
}

REALLY_INLINE uint32_t Traits::getHashtableOffset() const
{
    AvmAssert(m_resolved);
    return m_hashTableOffset;
}

REALLY_INLINE uint32_t Traits::getTotalSize() const
{
    AvmAssert(m_resolved);
    return m_totalSize;
}

REALLY_INLINE uint32_t Traits::getSlotAreaSize() const
{
    AvmAssert(m_resolved);
    return getTotalSize() - m_sizeofInstance - (m_hashTableOffset ? sizeof(InlineHashtable) : 0);
}

// in bytes. includes size for all base classes too.
REALLY_INLINE uint32_t Traits::getExtraSize() const
{
    AvmAssert(m_resolved);
    AvmAssert(getTotalSize() >= m_sizeofInstance);
    return getTotalSize() - m_sizeofInstance;
}

// sadly, it's still more efficient to stash this in Traits itself, as it's nontrivial to recover when
// we rebuild the TraitMethodInfo.
REALLY_INLINE void Traits::setMetadataPos(const uint8_t* pos)
{
    AvmAssert(metadata_pos == NULL);
    metadata_pos = pos;
}

REALLY_INLINE Traits* Traits::newParameterizedITraits(Stringp name, Namespacep ns, Traits* param_traits)
{
    return _newParameterizedTraits(name, ns, this, param_traits);
}

REALLY_INLINE Traits* Traits::newParameterizedCTraits(Stringp name, Namespacep ns)
{
    return _newParameterizedTraits(name, ns, this->base, NULL);
}

REALLY_INLINE TraitsBindingsp Traits::getTraitsBindings()
{
    AvmAssert(m_tbref != NULL);
    TraitsBindings* tb;
    if ((tb = (TraitsBindings*)m_tbref->get()) == NULL)
        tb = _getTraitsBindings();
    return tb;
}

REALLY_INLINE TraitsMetadatap Traits::getTraitsMetadata()
{
    AvmAssert(m_tmref != NULL);
    TraitsMetadata* tm;
    if ((tm = (TraitsMetadata*)m_tmref->get()) == NULL)
        tm = _getTraitsMetadata();
    return tm;
}

REALLY_INLINE bool Traits::containsInterface(Traitsp t)
{
    return subtypeof(t);
}

REALLY_INLINE bool Traits::subtypeof(Traitsp t)
{
    // test primary supertypes or positive cache hit
    size_t off = t->m_supertype_offset;
    if (t == *((Traits**)(uintptr_t(this)+off)))
        return true;

    // return false if t was primary and not found, or if negative cache hit
    if (off != offsetof(Traits, m_supertype_cache) || t == m_supertype_neg_cache)
        return false;

    // check self because it won't be in m_secondary_supertypes
    if (this == t)
        return true;

    return secondary_subtypeof(t);
}

REALLY_INLINE Traits** Traits::get_secondary_supertypes() const
{
    return m_secondary_supertypes->AsArray();
}

REALLY_INLINE bool Traits::isPrimary() const
{
    return m_supertype_offset != offsetof(Traits, m_supertype_cache);
}

REALLY_INLINE BuiltinType Traits::getBuiltinType(const Traitsp t)
{
    return t ? BuiltinType(t->builtinType) : BUILTIN_any;
}

REALLY_INLINE bool Traits::hasComplexEqualityRules() const
{
    return (((1<<builtinType) & HAS_COMPLEX_EQUALITY_RULES) != 0);
}

REALLY_INLINE bool Traits::isMachineType() const
{
    return ((1<<builtinType) & MACHINE_TYPE_MASK) != 0;
}

REALLY_INLINE bool Traits::isNumeric() const
{
    return ((1<<builtinType) & NUMERIC_TYPE_MASK) != 0;
}

REALLY_INLINE bool Traits::isNumberType() const
{
    return ((1<<builtinType) & NUMBER_TYPE_MASK) != 0;
}

REALLY_INLINE bool Traits::isXMLType() const
{
    return ((1<<builtinType) & XML_TYPE_MASK) != 0;
}

REALLY_INLINE bool Traits::isInstanceType() const
{
    return posType() == TRAITSTYPE_INSTANCE || posType() == TRAITSTYPE_INTERFACE;
}

REALLY_INLINE bool Traits::isInterface() const
{
    return posType() == TRAITSTYPE_INTERFACE;
}

REALLY_INLINE TraitsPosType Traits::posType() const
{
    return (TraitsPosType)(uint32_t)m_posType;
}

REALLY_INLINE bool Traits::isResolved() const
{
    return m_resolved;
}

REALLY_INLINE bool Traits::isActivationTraits() const
{
    return posType() == TRAITSTYPE_ACTIVATION;
}

REALLY_INLINE bool Traits::needsHashtable() const
{
    AvmAssert(m_resolved);
    return m_needsHashtable;
}

REALLY_INLINE void Traits::set_needsHashtable(bool v)
{
    AvmAssert(!m_resolved);
    m_needsHashtable = v;
}

REALLY_INLINE void Traits::setCreateClassClosureProc(CreateClassClosureProc p)
{
    this->m_createClassClosure = p;
}

REALLY_INLINE CreateClassClosureProc Traits::getCreateClassClosureProc() const
{
    return m_createClassClosure;
}

REALLY_INLINE Namespacep Traits::ns() const
{
    return _ns;
}

REALLY_INLINE Stringp Traits::name() const
{
    return _name;
}

REALLY_INLINE void Traits::set_names(Namespacep p_ns, Stringp p_name)
{
    _ns = p_ns;
    _name = p_name;
}

// this returns true iff we implement an interface that is not implemented by our parent.
// essential for efficient building of IMT thunks.
REALLY_INLINE bool Traits::implementsNewInterfaces() const
{
    AvmAssert(m_resolved);
    return m_implementsNewInterfaces;
}

REALLY_INLINE void Traits::setDeclaringScopes(const ScopeTypeChain* stc)
{
    AvmAssert(m_declaringScope == NULL || m_declaringScope->equals(stc));
    AvmAssert(stc != NULL);
    m_declaringScope = stc;
}

REALLY_INLINE bool Traits::isDictionary() const
{
    return m_isDictionary != 0;
}

REALLY_INLINE void Traits::set_isDictionary()
{
    m_isDictionary = true;
}

REALLY_INLINE InterfaceIterator::InterfaceIterator(Traits* t)
{
    st = t->get_secondary_supertypes();
}

REALLY_INLINE InterfaceIterator::InterfaceIterator(const TraitsBindings* tb)
{
    st = tb->owner->get_secondary_supertypes();
}

REALLY_INLINE bool InterfaceIterator::hasNext()
{
    while (*st != NULL && !(*st)->isInterface())
        st++;
    return *st != NULL;
}

REALLY_INLINE Traits* InterfaceIterator::next()
{
    AvmAssert(*st != NULL && (*st)->isInterface());
    return *st++;
}

} // namespace avmplus

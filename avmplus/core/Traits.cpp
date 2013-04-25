/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    using namespace MMgc;

    // -------------------------------------------------------------------
    // -------------------------------------------------------------------
    // -------------------------------------------------------------------
    // -------------------------------------------------------------------

    /*static*/ TraitsBindings* TraitsBindings::alloc(GC* gc,
                                                Traits* _owner,
                                                TraitsBindingsp _base,
                                                MultinameBindingHashtable* _bindings,
                                                uint32_t slotCount,
                                                uint32_t methodCount,
                                                bool typesValid)
    {
        const uint32_t extra = typesValid ?
                                slotCount * sizeof(SlotInfo) + methodCount * sizeof(MethodInfo) :
                                0;

        TraitsBindings* tb = new (gc, MMgc::kExact, extra) TraitsBindings(_owner, _base, _bindings, slotCount, methodCount, typesValid);
        if (_base && typesValid)
        {
            if (_base->slotCount)
            {
                const SlotInfo* src = &_base->getSlots()[0];
                SlotInfo* dst = &tb->getSlots()[0];
                VMPI_memcpy(dst, src, _base->slotCount * sizeof(SlotInfo));
                AvmAssert(((_owner->isMachineType()) || (tb->owner->m_sizeofInstance >= _base->owner->m_sizeofInstance)));
            }
            if (_base->methodCount)
                VMPI_memcpy(&tb->getMethods()[0], &_base->getMethods()[0], _base->methodCount * sizeof(MethodInfo));
        }
        return tb;
    }

    void TraitsBindings::gcTraceHook_TraitsBindings(MMgc::GC* gc)
    {
        if (m_typesValid)
        {
            SlotInfo* slots = getSlots();
            for ( uint32_t i=0 ; i < slotCount ; i++ )
                gc->TraceLocation(&slots[i].type);
            TraitsBindings::BindingMethodInfo* methods = getMethods();
            for ( uint32_t i=0 ; i < methodCount ; i++ )
                gc->TraceLocation(&methods[i].f);
        }
    }

    Binding TraitsBindings::findBinding(Stringp name) const
    {
        for (TraitsBindingsp self = this; self; self = self->base)
        {
            const Binding b = self->m_bindings->getName(name);
            if (b != BIND_NONE)
                return b;
        }
        return BIND_NONE;
    }

    Binding TraitsBindings::findBinding(Stringp name, Namespacep ns) const
    {
        for (TraitsBindingsp self = this; self; self = self->base)
        {
            const Binding b = self->m_bindings->get(name, ns);
            if (b != BIND_NONE)
                return b;
        }
        return BIND_NONE;
    }

    Binding TraitsBindings::findBinding(Stringp name, NamespaceSetp nsset) const
    {
        for (TraitsBindingsp self = this; self; self = self->base)
        {
            const Binding b = self->m_bindings->get(name, nsset);
            if (b != BIND_NONE)
                return b;
        }
        return BIND_NONE;
    }

    Binding TraitsBindings::findBindingAndDeclarer(const Multiname& mn, Traitsp& declarer) const
    {
        if (mn.isBinding())
        {
            for (TraitsBindingsp self = this; self; self = self->base)
            {
                Namespacep foundns = NULL;
                Binding const b = self->m_bindings->getMulti(mn, foundns);
                if (b != BIND_NONE)
                {
                    declarer = self->owner;

                    // if the member is 'protected' then we have to do extra work,
                    // as we may have found it in a descendant's protected namespace --
                    // we have to bounce up the inheritance chain and check our parent's
                    // protected namespace.
                    while (foundns == declarer->protectedNamespace)
                    {
                        Traitsp declParent = declarer->base;
                        if (!declParent || declParent->protectedNamespace == NULL)
                            break;

                        Binding const bp = declParent->getTraitsBindings()->findBinding(mn.getName(), declParent->protectedNamespace);
                        if (bp != b)
                            break;

                        // self->owner->core->console<<"bounce "<<declarer<<" to "<<declParent<<"\n";
                        declarer = declParent;
                        foundns = declParent->protectedNamespace;
                    }

                    // self->owner->core->console<<"final declarer is "<<declarer<<"\n";
                    return b;
                }
            }
        }
        declarer = NULL;
        return BIND_NONE;
    }

    void TraitsBindings::buildSlotDestroyInfo(MMgc::GC* gc, FixedBitSet& slotDestroyInfo, uint32_t slotAreaCount, uint32_t slotAreaSize) const
    {
        MMGC_STATIC_ASSERT(kUnusedAtomTag == 0 && kObjectType == 1 && kStringType == 2 && kNamespaceType == 3);
        AvmAssert(slotAreaCount <= slotCount);

        // not the same as slotCount since a slot of type double
        // takes two bits (in 32-bit builds). note that the bits are
        // always 4-byte chunks even in 64-bit builds!
        const uint32_t bitsNeeded = slotAreaSize / sizeof(uint32_t);    // not sizeof(Atom)!
        AvmAssert(bitsNeeded * sizeof(uint32_t) == slotAreaSize);       // should be even multiple!
        // allocate one extra bit and use it for "all-zero"
        slotDestroyInfo.resize(gc, bitsNeeded+1);
        if (slotAreaSize > 0)
        {
            const uint32_t sizeofInstance = this->owner->getSizeOfInstance();
            const TraitsBindings::SlotInfo* tbs     = getSlots() + (slotCount - slotAreaCount);
            const TraitsBindings::SlotInfo* tbs_end = tbs + slotAreaCount;
            for ( ; tbs < tbs_end; ++tbs)
            {
                // offset is pointed off the end of our object
                if (isAtomOrRCObjectSlot(tbs->sst()))
                {
                    //owner->core->console<<"SDI "<<owner<<" "<<sizeofInstance<<" "<<tbs->type<<" "<<tbs->offset()<<"\n";
                    AvmAssert(tbs->offset() >= sizeofInstance);
                    const uint32_t off = tbs->offset() - sizeofInstance;
                    AvmAssert((off % 4) == 0);
                    // if slot is "big" then this is the bit of the first 4 bytes. that's fine.
                    slotDestroyInfo.set((off>>2)+1);    // +1 to leave room for bit 0
                    slotDestroyInfo.set(0);             // bit 0 is "anyset" flag
                }
                // otherwise leave the bit zero
            }
        }
        else
        {
            AvmAssert(slotAreaCount == 0);
            AvmAssert(bitsNeeded == 0);
        }

        // if nothing set, blow away what we built and realloc as single clear bit -- smaller and faster
        if (!slotDestroyInfo.test(0))
        {
            slotDestroyInfo.resize(gc, 1);
            AvmAssert(!slotDestroyInfo.test(0));
        }
    }

    bool TraitsBindings::checkOverride(AvmCore* core, MethodInfo* virt, MethodInfo* over) const
    {
        if (over == virt)
            return true;

        MethodSignaturep overms = over->getMethodSignature();
        MethodSignaturep virtms = virt->getMethodSignature();

        Traits* overTraits = overms->returnTraits();
        Traits* virtTraits = virtms->returnTraits();

        if (overTraits != virtTraits)
        {
#ifdef AVMPLUS_VERBOSE
            core->console << "\n";
            core->console << "return types dont match\n";
            core->console << "   virt " << virtTraits << " " << virt << "\n";
            core->console << "   over " << overTraits << " " << over << "\n";
#endif
            return false;
        }

        if (overms->param_count() != virtms->param_count() ||
            overms->optional_count() != virtms->optional_count())
        {
#ifdef AVMPLUS_VERBOSE
            core->console << "\n";
            core->console << "param count mismatch\n";
            core->console << "   virt params=" << virtms->param_count() << " optional=" << virtms->optional_count() << " " << virt << "\n";
            core->console << "   over params=" << overms->param_count() << " optional=" << overms->optional_count() << " " << virt << "\n";
#endif
            return false;
        }

        // allow subclass param 0 to implement or extend base param 0
        virtTraits = virtms->paramTraits(0);
        if (!owner->subtypeof(virtTraits) || !Traits::isMachineCompatible(this->owner, virtTraits))
        {
            if (!this->owner->isMachineType() && virtTraits == core->traits.object_itraits)
            {
                over->setUnboxThis();
            }
            else
            {
                #ifdef AVMPLUS_VERBOSE
                core->console << "\n";
                core->console << "param 0 incompatible\n";
                core->console << "   virt " << virtTraits << " " << virt << "\n";
                core->console << "   over " << this->owner << " " << over << "\n";
                #endif
                return false;
            }
        }

        for (int k=1, p=overms->param_count(); k <= p; k++)
        {
            overTraits = overms->paramTraits(k);
            virtTraits = virtms->paramTraits(k);
            if (overTraits != virtTraits)
            {
                #ifdef AVMPLUS_VERBOSE
                core->console << "\n";
                core->console << "param " << k << " incompatible\n";
                core->console << "   virt " << virtTraits << " " << virt << "\n";
                core->console << "   over " << overTraits << " " << over << "\n";
                #endif
                return false;
            }
        }

        if (virt->unboxThis())
        {
            // the _unboxThis flag is sticky, all the way down the inheritance tree
            over->setUnboxThis();
        }

        return true;
    }

    static bool isCompatibleOverrideKind(BindingKind baseKind, BindingKind overKind)
    {
        static const uint8_t kCompatibleBindingKinds[8] =
        {
            0,                                                      // BKIND_NONE
            (1<<BKIND_METHOD),                                      // BKIND_METHOD
            0,                                                      // BKIND_VAR
            0,                                                      // BKIND_CONST
            0,                                                      // unused
            (1<<BKIND_GET) | (1<<BKIND_SET) | (1<<BKIND_GETSET),    // BKIND_GET
            (1<<BKIND_GET) | (1<<BKIND_SET) | (1<<BKIND_GETSET),    // BKIND_SET
            (1<<BKIND_GET) | (1<<BKIND_SET) | (1<<BKIND_GETSET)     // BKIND_GETSET
        };
        return (kCompatibleBindingKinds[baseKind] & (1<<overKind)) != 0;
    }

    bool TraitsBindings::checkLegalInterfaces(AvmCore* core) const
    {
        // make sure every interface method is implemented
        for (InterfaceIterator ifc_iter(this); ifc_iter.hasNext();)
        {
            Traits* ifc = ifc_iter.next();

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=788296
            // We must check the validity of overrides for the derived classes.
            // Checking only that an interface was once correctly implemented is not sufficient.
#else
             // don't need to bother checking interfaces in our parent.
             if (this->base && this->base->owner->subtypeof(ifc))
                 continue;
#endif

            TraitsBindingsp ifcd = ifc->getTraitsBindings();
            StTraitsBindingsIterator iter(ifcd);
            while (iter.next())
            {
                Stringp name = iter.key();
                if (!name) continue;
                Namespacep ns = iter.ns();
                Binding iBinding = iter.value();
                const BindingKind iBindingKind = AvmCore::bindingKind(iBinding);

                Binding cBinding = this->findBinding(name, ns);

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=788296
                // The second lookup is unnecessary. The name would have been brought out for
                // this namespace by fixOneInterfaceBindings().
#else
                if (!isCompatibleOverrideKind(iBindingKind, AvmCore::bindingKind(cBinding)))
                {
                    // Try again with public namespace that matches the version of the current traits
                    const Binding pBinding = this->findBinding(name, core->getPublicNamespace(owner->pool));
                    if (isCompatibleOverrideKind(iBindingKind, AvmCore::bindingKind(pBinding)))
                        cBinding = pBinding;
                }
#endif

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=668052
                // We used to do something like:
                //
                //      if (iBinding == cBinding) continue;
                //
                // here, presumably on the theory that if the bindings are the same, we don't
                // need to check any further. THIS IS WRONG: we must check the actual argument
                // types in all cases.
#else
                if (iBinding == cBinding)
                    continue;
#endif

                if (!isCompatibleOverrideKind(iBindingKind, AvmCore::bindingKind(cBinding)))
                    return false;

                switch (iBindingKind)
                {
                    default:
                    {
                        AvmAssert(0);   // interfaces shouldn't have anything but methods, getters, and setters
                        return false;
                    }
                    case BKIND_METHOD:
                    {
                        MethodInfo* virt = ifcd->getMethods()[AvmCore::bindingToMethodId(iBinding)].f;
                        MethodInfo* over = this->getMethods()[AvmCore::bindingToMethodId(cBinding)].f;
                        if (!checkOverride(core, virt, over))
                            return false;
                        break;
                    }
                    case BKIND_GET:
                    case BKIND_SET:
                    case BKIND_GETSET:
                    {
                        // check getter & setter overrides
                        if (AvmCore::hasGetterBinding(iBinding))
                        {
                            if (!AvmCore::hasGetterBinding(cBinding))
                                return false;

                            MethodInfo* virt = ifcd->getMethods()[AvmCore::bindingToGetterId(iBinding)].f;
                            MethodInfo* over = this->getMethods()[AvmCore::bindingToGetterId(cBinding)].f;
                            if (!checkOverride(core, virt, over))
                                return false;
                        }

                        if (AvmCore::hasSetterBinding(iBinding))
                        {
                            if (!AvmCore::hasSetterBinding(cBinding))
                                return false;

                            MethodInfo* virt = ifcd->getMethod(AvmCore::bindingToSetterId(iBinding));
                            MethodInfo* over = this->getMethod(AvmCore::bindingToSetterId(cBinding));
                            if (!checkOverride(core, virt, over))
                                return false;
                        }
                    }
                    break;
                } // switch
            } // for j
        } // for tbi
        return true;
    }

    void TraitsBindings::fixOneInterfaceBindings(Traitsp ifc)
    {
        AvmAssert(!owner->isInterface());

        TraitsBindingsp ifcd = ifc->getTraitsBindings();
        StTraitsBindingsIterator iter(ifcd);
        while (iter.next())
        {
            Stringp name = iter.key();
            if (!name) continue;
            Namespacep ns = iter.ns();
            Binding iBinding = iter.value();
            const BindingKind iBindingKind = AvmCore::bindingKind(iBinding);
            const Binding cBinding = this->findBinding(name, ns);
            if (!isCompatibleOverrideKind(iBindingKind, AvmCore::bindingKind(cBinding)))
            {
                // Try again with public namespace that matches the version of the current traits
                const Binding pBinding = this->findBinding(name, ifc->core->getPublicNamespace(owner->pool));
                if (isCompatibleOverrideKind(iBindingKind, AvmCore::bindingKind(pBinding)))
                {
                    this->m_bindings->add(name, ns, pBinding);
                }
            }
        }
    }

    void Traits::addVersionedBindings(MultinameBindingHashtable* bindings,
                                      Stringp name,
                                      NamespaceSetp nss,
                                      Binding binding) const
    {
        // find the lowest version within the active series.
        ApiVersion apiVersion = kApiVersion_VM_INTERNAL;
        for (NamespaceSetIterator iter(nss); iter.hasNext();)
        {
            Namespacep ns = iter.next();
            ApiVersion a = core->getValidApiVersion(ns->getApiVersion());
            if (a < apiVersion)
                apiVersion = a;
        }
        Namespacep ns = core->getVersionedNamespace(nss->nsAt(0), apiVersion);
        bindings->add(name, ns, binding);
    }

    // -------------------------------------------------------------------
    // -------------------------------------------------------------------

    TraitsMetadata::MetadataPtr TraitsMetadata::getSlotMetadataPos(uint32_t i, PoolObject*& residingPool) const
    {
        AvmAssert(i < slotCount);
        residingPool = NULL;
        for (TraitsMetadatap self = this; self && (i < self->slotCount); self = self->base)
        {
            MetadataPtr pos = self->slotMetadataPos[i];
            if (pos)
            {
                residingPool = self->residingPool;
                return pos;
            }
        }
        return NULL;
    }

    TraitsMetadata::MetadataPtr TraitsMetadata::getMethodMetadataPos(uint32_t i, PoolObject*& residingPool) const
    {
        AvmAssert(i < methodCount);
        residingPool = NULL;
        for (TraitsMetadatap self = this; self && (i < self->methodCount); self = self->base)
        {
            MetadataPtr pos = self->methodMetadataPos[i];
            if (pos)
            {
                residingPool = self->residingPool;
                return pos;
            }
        }
        return NULL;
    }

    // -------------------------------------------------------------------
    // -------------------------------------------------------------------

    Traits::Traits(PoolObject* _pool,
                   Traits* _base,
                   uint16_t _sizeofInstance,
                   uint16_t _offsetofSlots,
                   TraitsPosPtr traitsPos,
                   TraitsPosType posType)
        : core(_pool->core)
        , base(_base)
        , m_paramTraits(NULL)
        , pool(_pool)
        , m_traitsPos(traitsPos)
        , m_tbref(_pool->core->GetGC()->emptyWeakRef)
        , m_tmref(_pool->core->GetGC()->emptyWeakRef)
        , m_sizeofInstance(_sizeofInstance)
        , m_offsetofSlots(_offsetofSlots)
        , builtinType(BUILTIN_none)
        , m_posType(uint8_t(posType))
        // Assume builtins initialize this correctly.  This is verified
        // by checkCustomConstruct() in MethodEnv.cpp.
        , hasCustomConstruct(0)
#ifdef VMCFG_AOT
        , m_interfaceBindingFunction(NULL)
#endif
    {
        AvmAssert(m_tbref->isNull());
        AvmAssert(m_tmref->isNull());
        AvmAssert(BUILTIN_COUNT <= 32);
        AvmAssert(m_slotDestroyInfo.allocatedSize() == 0);
#ifdef _DEBUG
        switch (posType)
        {
            case TRAITSTYPE_NVA:
            case TRAITSTYPE_RT:
                AvmAssert(m_traitsPos == 0);
                break;
            default:
                AvmAssert(m_traitsPos != 0);
                break;
        }
#endif
        build_primary_supertypes();
        build_secondary_supertypes();
    }

    /*static*/ Traits* Traits::newTraits(PoolObject* pool,
                            Traits *base,
                            uint16_t objectSize,
                            uint16_t offsetOfSlots,
                            TraitsPosPtr traitsPos,
                            TraitsPosType posType)
    {
        AvmAssert(posType != TRAITSTYPE_CATCH);
        AvmAssert(pool != NULL);
        Traits* traits = new (pool->core->GetGC(), MMgc::kExact) Traits(pool, base, objectSize, offsetOfSlots, traitsPos, posType);
        return traits;
    }

    /*static*/ Traits* Traits::newCatchTraits(const Toplevel* toplevel, PoolObject* pool, TraitsPosPtr traitsPos, Stringp name, Namespacep ns)
    {
        AvmAssert(pool != NULL);
        Traits* traits = new (pool->core->GetGC(), MMgc::kExact) Traits(pool, NULL, sizeof(ScriptObject), sizeof(ScriptObject), traitsPos, TRAITSTYPE_CATCH);
        traits->final = true;
        traits->set_names(ns, name);
        traits->verifyBindings(toplevel);
        traits->resolveSignatures(toplevel);
        return traits;
    }

    Traits* Traits::_newParameterizedTraits(Stringp name, Namespacep ns, Traits* _base, Traits* _param_traits)
    {
        Traits* newtraits = Traits::newTraits(this->pool, _base, this->getSizeOfInstance(), this->m_offsetofSlots, NULL, TRAITSTYPE_RT);
        newtraits->m_needsHashtable = this->m_needsHashtable;
        newtraits->m_paramTraits = _param_traits;
        newtraits->set_names(ns, name);
        return newtraits;
    }

    TraitsPosPtr Traits::traitsPosStart() const
    {
        TraitsPosPtr pos = m_traitsPos;
        switch (posType())
        {
            case TRAITSTYPE_INTERFACE:
            case TRAITSTYPE_INSTANCE:
                pos = skipToInstanceInitPos(pos);
                // fall thru, no break

            case TRAITSTYPE_CLASS:
            case TRAITSTYPE_SCRIPT:
                AvmCore::skipU32(pos, 1);       // skip in init_index
                break;

            case TRAITSTYPE_ACTIVATION:
                // nothing to do
                break;

            case TRAITSTYPE_CATCH:
            case TRAITSTYPE_NVA:
            case TRAITSTYPE_RT:
                pos = NULL;
                break;
        }
        return pos;
    }

    TraitsPosPtr Traits::skipToInstanceInitPos(TraitsPosPtr pos) const
    {
        AvmAssert(isInstanceType() && pos != NULL);
        AvmCore::skipU32(pos, 2);       // skip the QName & base traits
        const uint8_t theflags = *pos++;
        const bool hasProtected = (theflags & 8) != 0;
        if (hasProtected)
        {
            AvmCore::skipU32(pos);
        }
        const uint32_t interfaceCount = AvmCore::readU32(pos);
        AvmCore::skipU32(pos, interfaceCount);
        return pos;
    }

#ifdef VMCFG_FLOAT
    static bool is16ByteSlot(Traits* slotTE)
    {
        return Traits::getBuiltinType(slotTE) == BUILTIN_float4;
    }
#endif // VMCFG_FLOAT

    static bool is8ByteSlot(Traits* slotTE)
    {
        #ifdef AVMPLUS_64BIT
        const uint32_t BIG_TYPE_MASK = (~(NUMERIC_TYPE_MASK | (1U<<BUILTIN_boolean))) | (1U<<BUILTIN_number); // number, plus all non-numeric, non-boolean types (i.e. pointers, essentially)
        #else
        const uint32_t BIG_TYPE_MASK = (1U<<BUILTIN_number);
        #endif

        return ((1 << Traits::getBuiltinType(slotTE)) & BIG_TYPE_MASK) != 0;
    }

    // Sun compilers don't allow static and REALLY_INLINE
    /*static*/ REALLY_INLINE int32_t pad8(int32_t nextSlotOffset)
    {
        AvmAssert((nextSlotOffset % 4) == 0);   // should always be a multiple of 4
        nextSlotOffset += 7;
        nextSlotOffset &= ~7;
        return nextSlotOffset;
    }

#ifdef VMCFG_FLOAT
    // Sun compilers don't allow static and REALLY_INLINE
    /*static*/ REALLY_INLINE int32_t pad16(int32_t nextSlotOffset)
    {
        AvmAssert((nextSlotOffset % 4) == 0);   // should always be a multiple of 4
        nextSlotOffset += 0xf;
        nextSlotOffset &= ~0xf;
        return nextSlotOffset;
    }
#endif // VMCFG_FLOAT

    SlotStorageType valueStorageType(BuiltinType bt)
    {
        switch (bt)
        {
            case BUILTIN_int:       return SST_int32;
            case BUILTIN_uint:      return SST_uint32;
            case BUILTIN_number:    return SST_double;
#ifdef VMCFG_FLOAT
            case BUILTIN_float:     return SST_float;
            case BUILTIN_float4:    return SST_float4;
#endif // VMCFG_FLOAT
            case BUILTIN_boolean:   return SST_bool32;
            case BUILTIN_void:      return SST_atom;
            case BUILTIN_any:       return SST_atom;
            case BUILTIN_object:    return SST_atom;
            case BUILTIN_string:    return SST_string;
            case BUILTIN_namespace: return SST_namespace;
            default:                return SST_scriptobject;
        }
    }

    REALLY_INLINE SlotStorageType slotStorageType(BuiltinType bt)
    {
        AvmAssert(bt != BUILTIN_void); // actual slots cannot be type void.
        return valueStorageType(bt);
    }

    // Sun compilers don't allow static and REALLY_INLINE
    /*static*/ REALLY_INLINE bool isPointerSlot(Traits* slotTE)
    {
        BuiltinType bt = Traits::getBuiltinType(slotTE);
        AvmAssert(bt != BUILTIN_void);

        MMGC_STATIC_ASSERT(BUILTIN_COUNT < (sizeof(int) * 8));
        int const IS_POINTER = ~(NUMERIC_TYPE_MASK | (1<<BUILTIN_boolean));

        return ((1 << bt) & IS_POINTER) != 0;
    }

    // the logic for assigning slot id's is used in several places, so it's now collapsed here
    // rather than redundantly sprinkled thru several bits of code.
    class SlotIdCalcer
    {
    private:
        uint32_t m_slotCount;
        bool m_earlySlotBinding;
    public:
        SlotIdCalcer(uint32_t _baseSlotCount, bool _earlySlotBinding) :
            m_slotCount(_baseSlotCount),
            m_earlySlotBinding(_earlySlotBinding)
        {
        }

        uint32_t calc_id(uint32_t id)
        {
            if (!id || !m_earlySlotBinding)
            {
                id = ++m_slotCount;
            }
            if (m_slotCount < id)
                m_slotCount = id;
            return id - 1;
        }

        uint32_t slotCount() const { return m_slotCount; }
        bool earlySlotBinding() const { return m_earlySlotBinding; }
    };

    struct NameEntry
    {
        const uint8_t* meta_pos;
        uint32_t qni, id, info, value_index;
        CPoolKind value_kind;
        TraitKind kind;
        uint8_t tag;

        void readNameEntry(const uint8_t*& pos);
    };

    void NameEntry::readNameEntry(const uint8_t*& pos)
    {
        qni = AvmCore::readU32(pos);
        tag = *pos++;
        kind = (TraitKind) (tag & 0x0f);
        value_kind = CONSTANT_unused_0x00;
        value_index = 0;

        // Read in the trait entry.
        switch (kind)
        {
            case TRAIT_Slot:
            case TRAIT_Const:
                id = AvmCore::readU32(pos);             // slot id
                info = AvmCore::readU32(pos);           // value type
                value_index = AvmCore::readU32(pos);    // value index
                if (value_index)
                    value_kind = (CPoolKind)*pos++;     // value kind
                break;
            case TRAIT_Class:
                id = AvmCore::readU32(pos);     // slot id
                info = AvmCore::readU32(pos);   // classinfo
                break;
            case TRAIT_Getter:
            case TRAIT_Setter:
            case TRAIT_Method:
                AvmCore::skipU32(pos);          // disp id (never used)
                id = AvmCore::readU32(pos);     // method index
                info = 0;
                break;
            default:
                // unsupported traits type -- can't happen, caught in AbcParser::parseTraits
                id = 0;
                info = 0;
                AvmAssert(0);
                break;
        }
        meta_pos = pos;
        if (tag & ATTR_metadata)
        {
            uint32_t metaCount = AvmCore::readU32(pos);
            AvmCore::skipU32(pos, metaCount);
        }
    }

    bool Traits::allowEarlyBinding() const
    {
        // the compiler can early bind to a type's slots when it's defined
        // or when the base class came from another abc file and has zero slots
        // this ensures you cant use the early opcodes to access an external type's
        // private members.
        TraitsBindingsp tb = this->base ? this->base->getTraitsBindings() : NULL;
        while (tb != NULL && tb->slotCount > 0)
        {
            if (tb->owner->pool != this->pool && tb->slotCount > 0)
            {
                return false;
            }
            tb = tb->base;
        }
        return true;
    }

    void Traits::buildBindings(TraitsBindingsp basetb,
                                MultinameBindingHashtable* bindings,
                                uint32_t& slotCount,
                                uint32_t& methodCount,
                                SlotSizeInfo* slotSizeInfo,
                                const Toplevel* toplevel) const
    {
        const uint8_t* pos = traitsPosStart();

        const uint32_t baseSlotCount = basetb ? basetb->slotCount : 0;
        const uint32_t baseMethodCount = basetb ? basetb->methodCount : 0;

        //slotCount = baseSlotCount;
        methodCount = baseMethodCount;

        SlotIdCalcer sic(baseSlotCount, this->allowEarlyBinding());

#ifdef _DEBUG
        uint32_t pointerSlots = 0;
#endif

        NameEntry ne;
        const uint32_t nameCount = pos ? AvmCore::readU32(pos) : 0;
        for (uint32_t i = 0; i < nameCount; i++)
        {
            ne.readNameEntry(pos);
            Multiname mn;
            this->pool->resolveBindingNameNoCheck(ne.qni, mn, toplevel);
            Stringp name = mn.getName();
            Namespacep ns;
            NamespaceSetp compat_nss;
            if (mn.namespaceCount() > 1) {
                ns = mn.getNsset()->nsAt(0);
                compat_nss = mn.getNsset();
            }
            else {
                ns = mn.getNamespace();
                compat_nss = NamespaceSet::create(core->GetGC(), ns);
            }

            switch (ne.kind)
            {
                case TRAIT_Slot:
                case TRAIT_Const:
                case TRAIT_Class:
                {
                    uint32_t slot_id = sic.calc_id(ne.id);
                    if (toplevel)
                    {
                        AvmAssert(!this->m_resolved);

                        // when we resolve, we must do some additional verification checks... these were formerly
                        // done in AbcParser::parseTraits but require the base class to be resolved first, so we
                        // now defer it to here.

                        // illegal raw slot id -- reject IF we are early binding. (theoretically it's always illegal,
                        // but won't cause any problems if earlySlotBinding=false, and rejecting in this case
                        // will break existing content.)
                        if (ne.id > nameCount && sic.earlySlotBinding())
                            toplevel->throwVerifyError(kCorruptABCError);

                        // slots are final.
                        if (basetb && slot_id < basetb->slotCount)
                            toplevel->throwVerifyError(kIllegalOverrideError, core->toErrorString(mn), core->toErrorString(base));

                        // a slot cannot override anything else.
                        if (bindings->get(name, ns) != BIND_NONE)
                            toplevel->throwVerifyError(kCorruptABCError);

                        // In theory we should reject duplicate slots here;
                        // in practice we don't, as it causes problems with some existing content
                        //if (basetb->findBinding(name, ns) != BIND_NONE)
                        //  toplevel->throwVerifyError(kIllegalOverrideError, toplevel->core()->toErrorString(qn), toplevel->core()->toErrorString(this));

                        // Interfaces cannot have slots.
                        if (this->isInterface())
                            toplevel->throwVerifyError(kIllegalSlotError, core->toErrorString(this));

                    }
                    // these assertions are commented out because the new lazy-init code can
                    // fire them inapppriately -- they all indicate malformed ABC, but they
                    // *can* occur on a pre-resolve build of TraitsBindings. We'll just let
                    // them slide by since we check for them all at resolve time.
                    // AvmAssert(!(ne.id > nameCount));                     // unhandled verify error
                    // AvmAssert(!(basetb && slot_id < basetb->slotCount)); // unhandled verify error
                    // AvmAssert(!(bindings->get(name, ns) != BIND_NONE));      // unhandled verify error
                    addVersionedBindings(bindings, name, compat_nss, AvmCore::makeSlotBinding(slot_id, ne.kind==TRAIT_Slot ? BKIND_VAR : BKIND_CONST));
                    if (slotSizeInfo != NULL)
                    {
                            // only do this if we need it -- doing it too early could cause us to reference a not-yet-loaded
                            // type which we don't need yet, causing an unnecessary verification failure
                        Traitsp slotType = (ne.kind == TRAIT_Class) ?
                                            pool->getClassTraits(ne.info) :
                                            pool->resolveTypeName(ne.info, toplevel);
                        if (!isPointerSlot(slotType))
                        {
#ifdef VMCFG_FLOAT
                            if (is16ByteSlot(slotType))
                                slotSizeInfo->nonPointer128BitSlotCount += 1;
                            else 
#endif // VMCFG_FLOAT
                            if (is8ByteSlot(slotType))
                                slotSizeInfo->nonPointer64BitSlotCount += 1;
                            else
                                slotSizeInfo->nonPointer32BitSlotCount += 1;
                        }
                        else
                        {
#ifdef _DEBUG
                            pointerSlots += 1;
#endif
                        }
                    }
                    break;
                }
                case TRAIT_Method:
                {
                    Binding baseBinding = this->getOverride(basetb, ns, name, ne.tag, toplevel);
                    if (baseBinding == BIND_NONE)
                    {
                        addVersionedBindings(bindings, name, compat_nss, AvmCore::makeMGSBinding(methodCount, BKIND_METHOD));
                        // accessors require 2 vtable slots, methods only need 1.
                        methodCount += 1;
                    }
                    else if (AvmCore::isMethodBinding(baseBinding))
                    {
                        // something got overridden, need new name entry for this subclass
                        // but keep the existing disp_id
                        addVersionedBindings(bindings, name, compat_nss, baseBinding);
                    }
                    else
                    {
                        if (toplevel)
                            toplevel->throwVerifyError(kCorruptABCError);
                        AvmAssert(!"unhandled verify error");
                    }
                    break;
                }
                case TRAIT_Getter:
                case TRAIT_Setter:
                {
                    // if nothing already is defined in this class, use base class in case getter/setter has already been defined.
                    Binding baseBinding = bindings->get(name, ns);
                    if (baseBinding == BIND_NONE)
                        baseBinding = this->getOverride(basetb, ns, name, ne.tag, toplevel);

                    const BindingKind us = (ne.kind == TRAIT_Getter) ? BKIND_GET : BKIND_SET;
                    const BindingKind them = (ne.kind == TRAIT_Getter) ? BKIND_SET : BKIND_GET;
                    if (baseBinding == BIND_NONE)
                    {
                        addVersionedBindings(bindings, name, compat_nss, AvmCore::makeMGSBinding(methodCount, us));
                        // accessors require 2 vtable slots, methods only need 1.
                        methodCount += 2;
                    }
                    else if (AvmCore::isAccessorBinding(baseBinding))
                    {
                        // something maybe got overridden, need new name entry for this subclass
                        // but keep the existing disp_id
                        // both get & set bindings use the get id.  set_id = get_id + 1.
                        if (AvmCore::bindingKind(baseBinding) == them)
                        {
                            baseBinding = AvmCore::makeGetSetBinding(baseBinding);
                        }
                        addVersionedBindings(bindings, name, compat_nss, baseBinding);
                    }
                    else
                    {
                        if (toplevel)
                            toplevel->throwVerifyError(kCorruptABCError);
                        AvmAssert(!"unhandled verify error");
                    }
                    break;
                }

                default:
                    // unsupported traits type -- can't happen, caught in AbcParser::parseTraits
                    AvmAssert(0);
                    break;
            }
        } // for i
        slotCount = sic.slotCount();
        if (slotSizeInfo)
        {
            uint32_t const c = slotSizeInfo->nonPointer32BitSlotCount + slotSizeInfo->nonPointer64BitSlotCount + baseSlotCount
                               + IFFLOAT(slotSizeInfo->nonPointer128BitSlotCount,0);
            // We calculate this at the end because we might have a sparse slot table...
            // the unused slots will be, well, unused, but we still have to reserve space
            // for them to guard against pathological code.
            slotSizeInfo->pointerSlotCount = slotCount - c;
            // Don't do this assertion: if this situation occurs, we'll catch it (and throw
            // a VerifyError) in finishSlotsAndMethods; asserting here will just spoil acceptance test runs.
            // AvmAssert(slotSizeInfo->pointerSlotCount >= pointerSlots);
        }
    }

    namespace
    {
        // Don't mess with the order of the members of the
        // following struct.
        //
        // This struct is never actually instantiated, it is
        // only used to determine the default padding that C++
        // compiler will insert between 32 bit member variables and
        // 64 bit member variables.
        struct GlueClassTest_Slots
        {
            int32_t m_intSlot;
            double  m_numberSlot;
            int32_t m_otherIntSlot;
            void*   m_ptrSlot;
        };
        static const bool align8ByteSlots = (offsetof(GlueClassTest_Slots, m_numberSlot) == 8);
        static const bool alignPointersTo8Bytes = (offsetof(GlueClassTest_Slots, m_ptrSlot) == 24);
        static const bool is64Bit = sizeof(void*) == 8;
#ifdef VMCFG_FLOAT
        struct GlueClassTest128_Slots
        {
            int32_t m_intSlot;
            float4_t m_float4Slot;
        };
        static const bool align16ByteSlots = offsetof(GlueClassTest128_Slots, m_float4Slot) >= 8;
#ifdef _DEBUG
        struct AlmostStaticAsserts 
        {
            AlmostStaticAsserts()
            {
                AvmAssert((offsetof(GlueClassTest_Slots, m_numberSlot) == 4)     || 
                          (offsetof(GlueClassTest_Slots, m_numberSlot) == 8)     );
                
                AvmAssert((offsetof(GlueClassTest_Slots, m_otherIntSlot) == 12)  ||
                          (offsetof(GlueClassTest_Slots, m_otherIntSlot) == 16)  );
                
                AvmAssert((offsetof(GlueClassTest_Slots, m_ptrSlot) == 16)       ||
                          (offsetof(GlueClassTest_Slots , m_ptrSlot) == 20)      ||
                          (offsetof(GlueClassTest_Slots, m_ptrSlot) == 24)       );
                
                AvmAssert((offsetof(GlueClassTest128_Slots, m_float4Slot) == 4)  ||
                          (offsetof(GlueClassTest128_Slots, m_float4Slot) == 16) ||
                          (offsetof(GlueClassTest128_Slots, m_float4Slot) == 8)  );
                /* Is it EVER the case that pointers are 8-byte aligned, but doubles are not? 
                   We currently assume "no", but let's double-test this assumption */
                struct GlueClassTestPtr_Slots
                {
                    int32_t m_intSlot;
                    void*   m_ptrSlot;
                };
                AvmAssert( ((offsetof(GlueClassTestPtr_Slots, m_ptrSlot) == 8) && alignPointersTo8Bytes) ||
                           ((offsetof(GlueClassTestPtr_Slots, m_ptrSlot) == 4) && !alignPointersTo8Bytes));
            }
        } __unused_var;
#endif
#endif // VMCFG_FLOAT
    }

    // Sun compilers don't allow static and REALLY_INLINE
#ifdef VMCFG_FLOAT
    /*static*/ REALLY_INLINE int32_t computeSlotOffset(Traits* slotType, int32_t& next32BitSlotOffset, int32_t& nextPointerSlotOffset, int32_t& next64BitSlotOffset, int32_t& next128BitSlotOffset)
#else
    /*static*/ REALLY_INLINE int32_t computeSlotOffset(Traits* slotType, int32_t& next32BitSlotOffset, int32_t& nextPointerSlotOffset, int32_t& next64BitSlotOffset)
#endif // VMCFG_FLOAT
    {
        if (isPointerSlot(slotType))
        {
            int32_t const result = nextPointerSlotOffset;
            nextPointerSlotOffset += sizeof(void*);
            return result;
        }
        else if (is8ByteSlot(slotType))
        {
            int32_t const result = next64BitSlotOffset;
            next64BitSlotOffset += 8;
            return result;
        }
#ifdef VMCFG_FLOAT
        else if (is16ByteSlot(slotType))
        {
            int32_t const result = next128BitSlotOffset;
            next128BitSlotOffset += 16;
            return result;
        }
#endif // VMCFG_FLOAT
        else
        {
            int32_t const result = next32BitSlotOffset;
            next32BitSlotOffset += 4;
            return result;
        }
    }

    void Traits::computeNonNativeSlotAreaCountAndSize(TraitsBindings* tb, uint32_t& slotCount, uint32_t& size) const
    {
        const TraitsBindings* prevBindings = tb;
        const TraitsBindings* currBindings = tb->base;
        uint32_t thisSize = getSizeOfInstance();
        while ((currBindings != NULL) && (currBindings->owner->getSizeOfInstance() == thisSize))
        {
            const TraitsBindings* baseBindings = currBindings->base;
            AvmAssert((baseBindings == 0) || (baseBindings->owner->getSizeOfInstance() <= thisSize)); (void)baseBindings;
            prevBindings = currBindings;
            currBindings = currBindings->base;
        }

        // currBindings is first ancestor class that is native with at least one slot or native member variable
        // or null if there is no native ancenstor class

        if (currBindings == NULL)
        {
            // We could not find a ancestor class that is native.
            slotCount = tb->slotCount;
            size = tb->m_slotSize;
        }
        else if (currBindings == tb->base)
        {
            AvmAssert(tb->base != NULL);
            AvmAssert((tb->base->slotCount == 0) || (tb->base->owner->getSizeOfInstance() <= thisSize));
            // "this" is a native class.
            slotCount = 0;
            size = 0;
        }
        else
        {
            AvmAssert(tb->base != NULL);
            AvmAssert(currBindings != NULL);
            AvmAssert(currBindings->owner->getSizeOfInstance() <= thisSize);
            AvmAssert(tb->slotCount >= prevBindings->slotCount);
            AvmAssert(tb->m_slotSize >= prevBindings->m_slotSize);
            slotCount = tb->slotCount - prevBindings->slotCount;
            size = tb->m_slotSize - prevBindings->m_slotSize;
        }
    }

    uint32_t Traits::finishSlotsAndMethods(TraitsBindingsp basetb,
                                    TraitsBindings* tb,
                                    const Toplevel* toplevel,
                                    const SlotSizeInfo& sizeInfo) const
    {
        const uint8_t* pos = traitsPosStart();

        SlotIdCalcer sic(basetb ? basetb->slotCount : 0, this->allowEarlyBinding());

        /*
            A word of caution about calculating slot layout:

            Say you have two classes like so:

                class A { ... };
                class B : public A { int32 foo; };

            You might assume that sizeof(B) > sizeof(A).

            However... if the contents of A are

                class A { char foo[9]; };
                class B : public A { int32 bar; };

            The compiler may decide that sizeof(A) should be rounded 
            up to the nearest pointer-sized boundary (e.g. if it 
            contains virtual methods)... so for a 64-bit build, 
            sizeof(A) could be 16. So sizeof(B) is 20, right?

            Nope! Not necesssarily. Apparently GCC is clever enough to 
            realize that there is enough unused (and appropriately aligned) 
            space at the end of A to fit the extra fields in B... 
            so sizeof(A) == sizeof(B) == 16.
            
            Similarly, we could have

                class A { char foo[9]; };
                class B : public A { int32 bar[3]; };
                
            which could yield sizeof(A) == 16, sizeof(B) == 24, which violates
            the previously-assumed invariant of (sizeof(B)-sizeof(A)>=slotsize(B)).
            
            (Note that this behavior is theoretically possible on 32-bit builds as well,
            but since the slots only contain 32-bit-or-larger fields, there can't ever be
            enough properly-aligned padding for this to happen.)
            
            Anyway, the point of this digression is to point out that you can't ever assume
            that our slotArea starts "after" our parent; there might be overlap. m_offsetofSlots
            is computed for all native classes using C++ offsetof(), and thus correctly
            accounts for any overlap of this sort. See bugzilla 655300 for more info.
            
        */

        int32_t slotAreaStart = m_offsetofSlots;
        if (slotAreaStart == 0 && base != NULL)
        {
            // 0 means "put the slots at the end of my immediate parent"
            slotAreaStart = base->getHashtableOffset() ? 
                            base->getHashtableOffset() : 
                            base->getTotalSize();
        }


        int32_t next32BitSlotOffset = slotAreaStart;
        int32_t endOf32BitSlots = next32BitSlotOffset + (sizeInfo.nonPointer32BitSlotCount * 4);
        int32_t nextPointerSlotOffset = alignPointersTo8Bytes && (sizeInfo.pointerSlotCount != 0) ? pad8(endOf32BitSlots) : endOf32BitSlots;
        int32_t endOfPointerSlots = nextPointerSlotOffset + (sizeInfo.pointerSlotCount * sizeof(void*));
        int32_t next64BitSlotOffset = align8ByteSlots && (sizeInfo.nonPointer64BitSlotCount != 0) ? pad8(endOfPointerSlots) : endOfPointerSlots;
        int32_t endOf64BitSlots = next64BitSlotOffset + (sizeInfo.nonPointer64BitSlotCount * 8);
#ifdef VMCFG_FLOAT
        int32_t next128BitSlotOffset = align16ByteSlots && (sizeInfo.nonPointer128BitSlotCount != 0) ? pad16(endOf64BitSlots) : endOf64BitSlots;
        int32_t endOf128BitSlots = next128BitSlotOffset + (sizeInfo.nonPointer128BitSlotCount * 16);
#endif // VMCFG_FLOAT

        NameEntry ne;
        const uint32_t nameCount = pos ? AvmCore::readU32(pos) : 0;
        for (uint32_t i = 0; i < nameCount; i++)
        {
            AvmAssert(next32BitSlotOffset <= endOf32BitSlots);
            AvmAssert(nextPointerSlotOffset <= endOfPointerSlots);
            AvmAssert(next64BitSlotOffset <= endOf64BitSlots);
            FLOAT_ONLY(AvmAssert(next128BitSlotOffset <= endOf128BitSlots));
            (void)IFFLOAT(endOf128BitSlots,endOf64BitSlots);
            ne.readNameEntry(pos);
            Multiname mn;
            this->pool->resolveBindingNameNoCheck(ne.qni, mn, toplevel);
            Namespacep ns = mn.getNamespace();
            Stringp name = mn.getName();
            // NOTE only one versioned namespace from the set needed here

            switch (ne.kind)
            {
                case TRAIT_Slot:
                case TRAIT_Const:
                case TRAIT_Class:
                {
                    uint32_t slotid = sic.calc_id(ne.id);
                    if (slotid >= tb->slotCount || tb->getSlotOffset(slotid) > 0)
                    {
                        if (toplevel)
                            toplevel->throwVerifyError(kCorruptABCError);
                        AvmAssert(!"unhandled verify error");
                    }
                    // note, for TRAIT_Class, AbcParser::parseTraits has already verified that pool->cinits[ne.info] is not null
                    Traitsp slotType = (ne.kind == TRAIT_Class) ?
                                        pool->getClassTraits(ne.info) :
                                        pool->resolveTypeName(ne.info, toplevel);
                    uint32_t slotOffset = computeSlotOffset(slotType, next32BitSlotOffset, nextPointerSlotOffset, next64BitSlotOffset FLOAT_ONLY(, next128BitSlotOffset));
                    AvmAssert(slotOffset >= sizeof(ScriptObject));
                    tb->setSlotInfo(slotid, slotType, slotStorageType(getBuiltinType(slotType)), slotOffset);
                    break;
                }
                case TRAIT_Getter:
                case TRAIT_Setter:
                case TRAIT_Method:
                {
                    const Binding b = tb->m_bindings->get(name, ns);
                    if (b != BIND_NONE)
                    {
                        const uint32_t disp_id = uint32_t(uintptr_t(b) >> 3) + (ne.kind == TRAIT_Setter);
                        Binding base = this->getOverride(basetb, ns, name, ne.tag, toplevel);
                        if (AvmCore::isMethodBinding(base)
                            || (AvmCore::hasGetterBinding(base) && (ne.kind == TRAIT_Getter))
                            || (AvmCore::hasSetterBinding(base) && (ne.kind == TRAIT_Setter))) {
                            ensureNonFinal(basetb->getMethod(disp_id), toplevel);
                        }
                        MethodInfo* f = this->pool->getMethodInfo(ne.id);
                        //AvmAssert(f->declaringTraits() == this);
                        tb->setMethodInfo(disp_id, f);
                    }
                    else
                    {
                        // This should only be BIND_NONE if the attribute was elided via Api Versioning.
                        AvmAssert(!core->isValidApiVersion(ns->getApiVersion()));
                    }
                    break;
                }

                default:
                    // unsupported traits type -- can't happen, caught in AbcParser::parseTraits
                    AvmAssert(0);
                    break;
            }
        } // for i

        // check for sparse slot table -- anything not specified will default to * (but we must allocate space for it)
        for (uint32_t i = 0; i < tb->slotCount; i++)
        {
            if (tb->getSlotOffset(i) > 0)
                continue;

            #ifdef AVMPLUS_VERBOSE
            if (pool->isVerbose(VB_traits))
            {
                core->console << "WARNING: slot " << i+1 << " on " << this << " not defined by compiler.  Using *\n";
            }
            #endif

            const Traitsp slotType = NULL;
            const uint32_t slotOffset = nextPointerSlotOffset;
            nextPointerSlotOffset += sizeof(void*);
            AvmAssert(slotOffset >= sizeof(ScriptObject));
            tb->setSlotInfo(i, slotType, SST_atom, slotOffset);
        }
        if (!(next32BitSlotOffset == endOf32BitSlots &&
                nextPointerSlotOffset == endOfPointerSlots &&
                next64BitSlotOffset == endOf64BitSlots &&
                FLOAT_ONLY(next128BitSlotOffset == endOf128BitSlots &&)
                IFFLOAT(endOf128BitSlots,endOf64BitSlots) >= slotAreaStart))
        {
            // Verify that we used exactly the space we predicted; if not, we may
            // have a fuzzed file that duplicate slots in certain orders. This is unacceptable,
            // as it may allow access to the same memory location with different type expectations.
            if (toplevel)
                toplevel->throwVerifyError(kCorruptABCError);
            AvmAssert(!"unhandled verify error");
        }
        return IFFLOAT(endOf128BitSlots,endOf64BitSlots) - slotAreaStart;
    }

    static const uint8_t* skipToInterfaceCount(const uint8_t* pos)
    {
        AvmAssert(pos != NULL);
        AvmCore::skipU32(pos, 2);       // skip the QName + basetraits
        const uint8_t theflags = *pos++;
        if (theflags & 8)
            AvmCore::skipU32(pos);  // skip protected namespace
        return pos;
    }

    // Apps often have many interfaces redundantly listed, so first time thru,
    // eliminate redundant ones.  We do this by only adding new interfaces to
    // the "seen" list and only traversing new super-interfaces.  An interface
    // is NEW if our base class does not implement it.
    uint32_t Traits::countNewInterfaces(GCList<Traits>& seen)
    {
        // each Traits* added to this list is rooted via Domain and PoolObject,
        // so it is okay for this Stack to be opaque to the GC.
        Stack<Traits*> pending;
        pending.add(this);
        while (!pending.isEmpty()) {
            Traits* t = pending.pop();
            const uint8_t* pos = skipToInterfaceCount(t->m_traitsPos);
            const uint32_t interfaceCount = AvmCore::readU32(pos);
            for (uint32_t j = 0; j < interfaceCount; j++) {
                // we know all interfaces have been resolved already, it is done
                // before traits construction in AbcParser::parseInstanceInfos().
                Traitsp intf = t->pool->resolveTypeName(pos, NULL);
                AvmAssert(intf && intf->isInterface() && intf->base == NULL);
                // an interface can "extend" multiple other interfaces, so we must recurse here.
                if ((!base || !base->subtypeof(intf)) && seen.indexOf(intf) < 0) {
                    seen.add(intf);
                    pending.add(intf);
                }
            }
        }
        return seen.length();
    }

    static uint8_t calcLog2(uint32_t cap)
    {
        uint8_t capLog = 1; // start with at least 2 entries
        while ((1U<<capLog) < cap)
        {
            ++capLog;
        }
        AvmAssert((1U<<capLog) >= cap);
        return capLog;
    }

    TraitsBindings* Traits::_buildTraitsBindings(const Toplevel* toplevel, bool includeTypes)
    {
#ifdef AVMPLUS_VERBOSE
        if (pool->isVerbose(VB_traits))
        {
            core->console << "Generate TraitsBindings for "<<this<<"\n";
        }
#endif

        TraitsBindings* thisData = NULL;

        // if we know the cap we need, go there right away, otherwise start at small power of 2
        // this saves tremendously on subsequent builds of this set of bindings since we don't have to
        // waste time growing the MNHT as we build it
        const int32_t bindingCap = m_bindingCapLog2 ? (1 << m_bindingCapLog2) : 2;

        MMgc::GC* gc = core->GetGC();
        MultinameBindingHashtable* bindings = MultinameBindingHashtable::create(gc, bindingCap);
        AvmAssert(bindings->numQuads == bindingCap);

        if (this->posType() == TRAITSTYPE_CATCH)
        {
            const uint8_t* pos = m_traitsPos;

            Traits* t = this->pool->resolveTypeName(pos, toplevel);

            // this assumes we save name/ns in all builds, not just verbose
            NamespaceSetp nss = NamespaceSet::create(core->GetGC(), this->ns());
            NamespaceSetp compat_nss = nss;
            addVersionedBindings(bindings, this->name(), compat_nss, AvmCore::makeSlotBinding(0, BKIND_VAR));
            // bindings just need room for one slot binding
            thisData = TraitsBindings::alloc(gc, this, /*base*/NULL, bindings, /*slotCount*/1, /*methodCount*/0, true);
            thisData->setSlotInfo(0, t, slotStorageType(getBuiltinType(t)), this->m_sizeofInstance);
            thisData->m_slotSize = is8ByteSlot(t) ? 8 : 4;
        }
        else
        {
            TraitsBindingsp basetb = this->base ? this->base->getTraitsBindings() : NULL;

            // Copy protected traits from base class into new protected namespace
            if (basetb && base->protectedNamespace && this->protectedNamespace)
            {
                StTraitsBindingsIterator iter(basetb);
                while (iter.next())
                {
                    if (!iter.key()) continue;
                    if (iter.ns() == base->protectedNamespace)
                    {
                        bindings->add(iter.key(), this->protectedNamespace, iter.value());
                    }
                }
            }

            SlotSizeInfo _slotSizeInfo;
            SlotSizeInfo* slotSizeInfo = includeTypes ? &_slotSizeInfo : NULL;

            uint32_t slotCount = 0;
            uint32_t methodCount = 0;
            buildBindings(basetb, bindings, slotCount, methodCount, slotSizeInfo, toplevel);

            thisData = TraitsBindings::alloc(gc, this, basetb, bindings, slotCount, methodCount, includeTypes);
            if (slotSizeInfo)
            {
                thisData->m_slotSize = finishSlotsAndMethods(basetb, thisData, toplevel, *slotSizeInfo);
                if (basetb)
                    thisData->m_slotSize += basetb->m_slotSize;
            }

            if (!isInterface() && m_implementsNewInterfaces) {
                // fix up interface bindings
                for (InterfaceIterator it(this); it.hasNext(); ) {
                    Traits* t = it.next();
                    if (!base || !base->subtypeof(t)) {
                        // new interface not implemented in base.
                        thisData->fixOneInterfaceBindings(t);
                    }
                }
            }
        }

        // hashtable (if we have one) must start on pointer-sized boundary...
        // much easier to always round slotsize up unconditionally rather than
        // only for cases with hashtable, so that's what we'll do. (MMgc currently
        // allocate in 8-byte increments anyway, so we're not really losing any space.)
        // (only really necessary to do this if we calced slotSizeInfo, but simpler & harmless
        // to just do it unconditionally)
        thisData->m_slotSize = (thisData->m_slotSize + (sizeof(uintptr_t)-1)) & ~(sizeof(uintptr_t)-1);

        // remember the cap we need
        if (m_bindingCapLog2 == 0)
            m_bindingCapLog2 = calcLog2(thisData->m_bindings->numQuads);    // remember capacity, not count
        AvmAssert(m_bindingCapLog2 > 0);

#ifdef AVMPLUS_VERBOSE
        if (pool->isVerbose(VB_traits))
        {
            core->console << this << " bindings\n";
            StTraitsBindingsIterator iter(thisData);
            while (iter.next())
            {
                core->console << iter.key() << ":" << (uint32_t)(uintptr_t)(iter.value()) << "\n";
            }
            core->console << this << " end bindings \n";
        }
#endif

        AvmAssert(m_tbref->isNull());
        m_tbref = thisData->GetWeakRef();
        core->tbCache()->add(thisData);
        return thisData;
    }

    TraitsMetadata* Traits::_buildTraitsMetadata()
    {
#ifdef AVMPLUS_VERBOSE
        if (pool->isVerbose(VB_traits))
        {
            core->console << "Generate TraitsMetadata for "<<this<<"\n";
        }
#endif
        TraitsBindingsp td = this->getTraitsBindings();

        MMgc::GC* gc = core->GetGC();

        TraitsMetadatap basetm = this->base ? this->base->getTraitsMetadata() : NULL;

        const uint32_t extra = td->slotCount * sizeof(TraitsMetadata::MetadataPtr) + td->methodCount * sizeof(TraitsMetadata::MetadataPtr);

        TraitsMetadata* tm = TraitsMetadata::create(gc, extra, basetm, this->pool, this->metadata_pos, td->slotCount, td->methodCount);
        tm->slotMetadataPos = (TraitsMetadata::MetadataPtr*)(tm + 1);
        tm->methodMetadataPos = (TraitsMetadata::MetadataPtr*)(tm->slotMetadataPos + tm->slotCount);

        const uint8_t* pos = traitsPosStart();
        const uint32_t nameCount = pos ? AvmCore::readU32(pos) : 0;
        SlotIdCalcer sic(td->base ? td->base->slotCount : 0, this->allowEarlyBinding());
        NameEntry ne; 
        for (uint32_t i = 0; i < nameCount; i++)
        {
            ne.readNameEntry(pos);
            switch (ne.kind)
            {
                case TRAIT_Class:
                    AvmAssert(0);
                    // classes shouldn't have metadata, but just fall thru just in case
                case TRAIT_Slot:
                case TRAIT_Const:
                {
                    const uint32_t slot_id = sic.calc_id(ne.id);
                    if (ne.tag & ATTR_metadata)
                        tm->slotMetadataPos[slot_id] = ne.meta_pos;
                    break;
                }
                case TRAIT_Getter:
                case TRAIT_Setter:
                case TRAIT_Method:
                {
                    if (ne.tag & ATTR_metadata)
                    {
                        Multiname qn;
                        // passing NULL for toplevel here, since it's only used if a verification error occurs --
                        // but if there was one, we would have encountered it during AbcParser::parseTraits.
                        this->pool->resolveBindingNameNoCheck(ne.qni, qn, /*toplevel*/NULL);
                        const Binding b = td->findBinding(qn.getName(), qn.getNamespace());
                        AvmAssert(b != BIND_NONE);
                        const uint32_t disp_id = uint32_t(uintptr_t(b) >> 3) + (ne.kind == TRAIT_Setter);
                        tm->methodMetadataPos[disp_id] = ne.meta_pos;
                    }
                    break;
                }

                default:
                    // unsupported traits type -- can't happen, caught in AbcParser::parseTraits
                    AvmAssert(0);
                    break;
            }
        } // for i

        AvmAssert(m_tmref->isNull());
        m_tmref = tm->GetWeakRef();
        core->tmCache()->add(tm);
        return tm;
    }

    /**
     * add t to pending[] ahead of any existing entries that are
     * subtypes or implementers of t, so that when we iterate through
     * pending[] we will traverse the inheritance DAG top-down.
     */
    void insertSupertype(Traits* t, GCList<Traits> &pending)
    {
        uint32_t i = 0;
        for (uint32_t n = pending.length(); i < n; i++) {
            if (pending[i]->subtypeof(t)) {
                pending.insert(i, t);
                return;
            }
        }
        pending.add(t);
    }

    /**
     * resolve this traits signatures by visiting all supertypes in
     * top-down toplogical order, then resolving our own signatures.  The top-down
     * order ensures that we resolve base classes and interfaces first, while
     * avoiding recursion.
     */
    void Traits::resolveSignatures(const Toplevel* toplevel)
    {
        // toplevel actually can be null, when resolving the builtin classes...
        // but they should never cause verification errors in functioning builds
        if (m_resolved)
            return;

        GCList<Traits> pending(core->gc, kListInitialCapacity);
        // copy primary supertypes into pending.
        // primary_supertypes[] is already in top-down order.
        for (int32_t i = 0; i < MAX_PRIMARY_SUPERTYPE; i++) {
            Traits* t = m_primary_supertypes[i];
            if (t == NULL || t == this)
                break;
            if (!t->m_resolved)
                pending.add(t);
        }

        // copy other base types, and interfaces, into pending[] by
        // and maintaining the partial ordering that each type's bases and
        // interfaces are visited before that type itself is visited.
        for (Traits** st = get_secondary_supertypes(); *st != NULL; st++) {
            Traits* t = *st;
            if (t != this && !t->m_resolved)
                insertSupertype(t, pending);
        }

        for (uint32_t i = 0, n = pending.length(); i < n; i++) {
            AvmAssert(!pending[i]->m_resolved);
            pending[i]->resolveSignaturesSelf(toplevel);
        }

        this->resolveSignaturesSelf(toplevel);
    }

    void Traits::verifyBindings(const Toplevel* toplevel)
    {
        // builtin Traits get called before a Toplevel is created, so they
        // get a free pass. Everyone else better pass a good value.
        AvmAssert(toplevel != NULL || pool->isBuiltin);

#ifdef DEBUG
        AvmAssert(!m_bindingsVerified);
        // make sure our supertypes have verified their bindings. (must be done before calling _buildTraitsBindings)
        for (Traits* t = this->base; t != NULL; t = t->base)
            AvmAssert(t->m_bindingsVerified);
        for (Traits** st = this->get_secondary_supertypes(); *st != NULL; st++)
            AvmAssert(*st == this || (*st)->m_bindingsVerified);
#endif

        AvmAssert(m_tbref == core->GetGC()->emptyWeakRef);
        // force buildTraitsBindings to run now, so that any errors will
        // also be reported now, instead of later when called on a QCache
        // miss, when we don't have a valid Toplevel* for exception-throwing context.
        _buildTraitsBindings(toplevel, /*includeTypes*/false);
#ifdef DEBUG
        this->m_bindingsVerified = true;
#endif
    }

    /**
     * This must be called before any method is verified or any
     * instances are created.  It is not done eagerly in AbcParser
     * because doing so would prevent circular type references between
     * slots of cooperating classes.
     *
     * Resolve the type and position/width of each slot.
     */
    void Traits::resolveSignaturesSelf(const Toplevel* toplevel)
    {
        // builtin Traits get called before a Toplevel is created, so they
        // get a free pass. Everyone else better pass a good value.
        AvmAssert(toplevel != NULL || pool->isBuiltin);

        AvmAssert(m_bindingsVerified);

#ifdef DEBUG
        AvmAssert(!m_resolved);
        // make sure our supertypes are resolved. (must be done before calling _buildTraitsBindings)
        for (Traits* t = this->base; t != NULL; t = t->base)
            AvmAssert(t->m_resolved);
        for (Traits** st = this->get_secondary_supertypes(); *st != NULL; st++)
            AvmAssert(*st == this || (*st)->m_resolved);
#endif

        // we might already have one -- if so, toss it
        m_tbref = core->GetGC()->emptyWeakRef;
        TraitsBindings* tb = _buildTraitsBindings(toplevel, /*includeTypes*/true);
        genInitBody(toplevel, tb);

        // leave m_tmref as empty, we don't need it yet

        uint32_t slotAreaSize = 0;
        uint32_t slotAreaCount = 0;

        switch (posType())
        {
            case TRAITSTYPE_INTERFACE:
            case TRAITSTYPE_INSTANCE:
            case TRAITSTYPE_CLASS:
            case TRAITSTYPE_SCRIPT:
            case TRAITSTYPE_ACTIVATION:
            case TRAITSTYPE_CATCH:
                computeNonNativeSlotAreaCountAndSize(tb, slotAreaCount, slotAreaSize);
                m_totalSize = getSizeOfInstance() + slotAreaSize;
                break;
            case TRAITSTYPE_NVA:
            case TRAITSTYPE_RT:
                m_totalSize = getSizeOfInstance();
                break;
        }
        AvmAssert(m_totalSize >= m_sizeofInstance);
        if (m_needsHashtable || (base && base->base && base->m_hashTableOffset && !isXMLType()))
        {
            // round up total size to multiple of pointer size.
            m_totalSize = ((m_totalSize+(sizeof(uintptr_t)-1))&~(sizeof(uintptr_t)-1));
            m_hashTableOffset = m_totalSize;
            m_totalSize += sizeof(InlineHashtable);
            AvmAssert(builtinType == BUILTIN_boolean ? true : (m_hashTableOffset & 3) == 0);
            AvmAssert((m_hashTableOffset & (sizeof(uintptr_t)-1)) == 0);
            AvmAssert((m_totalSize & (sizeof(uintptr_t)-1)) == 0);
        }

        // make sure all the methods have resolved types
        if (tb->methodCount)
        {
            const TraitsBindings::BindingMethodInfo* tbm        = tb->getMethods();
            const TraitsBindings::BindingMethodInfo* tbm_end    = tbm + tb->methodCount;
            for ( ; tbm < tbm_end; ++tbm)
            {
                // don't assert: could be null if only one of a get/set pair is implemented
                //AvmAssert(tbm->f != NULL);
                if (tbm->f != NULL)
                {
                    tbm->f->resolveSignature(toplevel);
                }
            }
        }

        if (this->init != NULL)
            this->init->resolveSignature(toplevel);

        bool legal = true;
        TraitsBindingsp tbbase = tb->base;  // might be null

        if (tbbase && tbbase->methodCount > 0)
        {
            // check concrete overrides
            const TraitsBindings::BindingMethodInfo* basetbm        = tbbase->getMethods();
            const TraitsBindings::BindingMethodInfo* basetbm_end    = basetbm + tbbase->methodCount;
            const TraitsBindings::BindingMethodInfo* tbm            = tb->getMethods();
            for ( ; basetbm < basetbm_end; ++basetbm, ++tbm)
            {
                if (basetbm->f != NULL && basetbm->f != tbm->f)
                    legal &= tb->checkOverride(core, basetbm->f, tbm->f);
            }
        }

        if (legal && !this->isInterface())
        {
            legal &= tb->checkLegalInterfaces(core);
        }

        if (!legal)
        {
            AvmAssert(!m_resolved);
            Multiname qname(ns(), name());
            if (toplevel)
                toplevel->throwVerifyError(kIllegalOverrideError, core->toErrorString(&qname), core->toErrorString(this));
#ifdef VMCFG_VERIFYALL
            else {
                if (core->config.verifyonly)
                    core->console << "ILLEGAL OVERRIDE\n";
            }
#endif
            AvmAssert(!"unhandled verify error");
        }

        tb->buildSlotDestroyInfo(core->GetGC(), m_slotDestroyInfo, slotAreaCount, slotAreaSize);

        m_resolved = true;
#ifdef AVMPLUS_VERBOSE
        if (pool->isVerbose(VB_traits)) {
            core->console << "Resolved ";
            printExtended(core->console) << "\n";
        }
#endif
    }

#ifdef AVMPLUS_VERBOSE
    PrintWriter& Traits::printExtended(PrintWriter& pw)
    {
        const char* desc = NULL;
        switch (posType()) {
        case TRAITSTYPE_INTERFACE:
            desc = "interface"; break;
        case TRAITSTYPE_INSTANCE:
            desc = "instance"; break;
        case TRAITSTYPE_CLASS:
            desc = "class"; break;
        case TRAITSTYPE_SCRIPT:
            desc = "script"; break;
        case TRAITSTYPE_ACTIVATION:
            desc = "activation"; break;
        case TRAITSTYPE_CATCH:
            desc = "catch"; break;
        case TRAITSTYPE_NVA: // null/void/any
            desc = "singleton"; break;
        case TRAITSTYPE_RT:
            desc = "rt"; break;
        default:
            AvmAssert(false);
        }
        const char* domainString = core->identifyDomain(pool->domain);
        pw << ns() << "::" << name() << ", type:" << desc << ",domain:";
        if (domainString) {
            pw << domainString;
        } else {
            // not a well known domain, print the address to help distinguish distinct domains
            pw << "@" << hexAddr((intptr_t)(Domain*)pool->domain);
        }
        return pw;
    }

#endif


#ifdef VMCFG_AOT
    static inline void hookUpActivationTraitsInitMethodForTraitsMethod(AvmCore* core, MethodInfo* m)
    {
        AvmAssert(m->needActivation());

        const AOTInfo* aotInfo = m->pool()->aotInfo;
        Traits* activationTraits = m->activationTraits();
        AvmAssert(activationTraits != NULL);
        AvmAssert(m->method_id() < (int)aotInfo->nActivationTraits);
        AvmAssert(aotInfo->activationTraits != NULL);
        AvmAssert(aotInfo->activationTraits[m->method_id()] == activationTraits);
        AvmAssert(aotInfo->activationInfo != NULL);

        // See comment in initActivationTraits about why this can be called more than once per Traits
        if (activationTraits->init == NULL) {
            if (aotInfo->activationInfo[m->method_id()].initHandler) {
                // NativeMethodInfo.handler is a union of
                // pointer to function and pointer to member function.
                // Zero the structure so that the entire thing is
                // initialized.
                // See bugzilla#647660
                NativeMethodInfo compiledMethodInfo = {{0}, 0, 0};
                compiledMethodInfo.thunker = aotThunker;
                AvmThunkNativeHandler nhandler;
                nhandler.function = aotInfo->activationInfo[m->method_id()].initHandler;
                activationTraits->init = MethodInfo::create(core->GetGC(), MethodInfo::kInitMethodStub, activationTraits, &compiledMethodInfo, nhandler, aotInfo->activationInfo[m->method_id()].initMethodId);
            }
            // The following comes from Verifier::write() TODO: refactor so we can share this code
            const ScopeTypeChain *scope = m->activationScope();
            if (scope == NULL) {
                scope = m->declaringScope()->cloneWithNewTraits(core->GetGC(), activationTraits);
                activationTraits->setDeclaringScopes(scope);
                m->init_activationScope(scope);
            }
        }
    }

    void Traits::initActivationTraits()
    {
        // Note: this can be called multiple times per Traits from initScript, which must call this in case it's needed
        // but is itself called once per Toplevel
        const uint8_t* pos = traitsPosStart();

        if (this->init->needActivation()) {
            hookUpActivationTraitsInitMethodForTraitsMethod(core, this->init);
        }

        NameEntry ne;
        const uint32_t nameCount = pos ? AvmCore::readU32(pos) : 0;
        for (uint32_t i = 0; i < nameCount; i++)
        {
            ne.readNameEntry(pos);

            switch (ne.kind)
            {
            case TRAIT_Slot:
            case TRAIT_Const:
            case TRAIT_Class:
                break;
            case TRAIT_Method:
            case TRAIT_Getter:
            case TRAIT_Setter:
            {
                MethodInfo* m = pool->getMethodInfo(ne.id);
                if (m->needActivation()) {
                    hookUpActivationTraitsInitMethodForTraitsMethod(core, m);
                }
                break;
            }
            default:
                // unsupported traits type -- can't happen, caught in AbcParser::parseTraits
                AvmAssert(0);
                break;
            }
        }
    }
#endif

    // static
    bool Traits::isMachineCompatible(const Traits* a, const Traits* b)
    {
        return (a == b) ||
            // *, Object, and Void are each represented as Atom
            ((!a || a->builtinType == BUILTIN_object || a->builtinType == BUILTIN_void) &&
            (!b || b->builtinType == BUILTIN_object || b->builtinType == BUILTIN_void)) ||
            // all other non-pointer types have unique representations
            (a && b && !a->isMachineType() && !b->isMachineType());
    }

    PrintWriter& Traits::print(PrintWriter& prw, bool includeAllNamespaces) const
    {
        if (name() != NULL) {
            Multiname::print(prw, ns(), name(), false, !includeAllNamespaces);
        } else {
            prw << "Traits@" << asAtomHex((Atom)this);
        }
        return prw;
    }

    void Traits::genInitBody(const Toplevel* toplevel, TraitsBindings* tb)
    {
#if defined(VMCFG_AOT)
        if (this->init && this->init->isAotCompiled())
            return;
#endif

        struct ValidateInitVisitor: public InitVisitor
        {
            bool hasDefaults;
            ValidateInitVisitor() : hasDefaults(false) {}
            virtual ~ValidateInitVisitor() {}
            void defaultVal(Atom, uint32_t, Traits*)
            {
                hasDefaults = true;
            }
        };
        ValidateInitVisitor visitor;
        visitInitBody(&visitor, toplevel, tb);

        // if initialization code gen is required, create a new method body and write it to traits->init->body_pos
        if (visitor.hasDefaults && !this->init)
        {
            // Make a do-nothing init method.
            this->init = MethodInfo::create(core->gc, MethodInfo::kInitMethodStub, this);
            AvmAssert(this->init->declaringTraits() == this);
            static const uint8_t body[] = {
                2, // max_stack
                1, // local_count
                1, // init_scope_depth
                1, // max_scope_depth
                1, // code_length
                (uint8_t)OP_returnvoid, // code
                0 // exception_count
                // the verifier and interpreter don't read the activation traits so stop here
            };

            // The synthetic MethodInfo points directly to the static const ABC above.
            this->init->set_abc_body_pos(body);
        }
    }

    void Traits::visitInitBody(InitVisitor *visitor, const Toplevel* toplevel, const TraitsBindings* tb)
    {
        const uint8_t* pos = traitsPosStart();
        SlotIdCalcer sic(tb->base ? tb->base->slotCount : 0, this->allowEarlyBinding());
        NameEntry ne; 
        const uint32_t nameCount = pos ? AvmCore::readU32(pos) : 0;
        for (uint32_t i = 0; i < nameCount; i++)
        {
            ne.readNameEntry(pos);
            switch (ne.kind)
            {
            case TRAIT_Slot:
            case TRAIT_Const:
            case TRAIT_Class:
            {
                uint32_t slotid = sic.calc_id(ne.id);
                // note, for TRAIT_Class, AbcParser::parseTraits has already verified that pool->cinits[ne.info] is not null
                Traitsp slotType = (ne.kind == TRAIT_Class) ?
                                    pool->getClassTraits(ne.info) :
                                    pool->resolveTypeName(ne.info, NULL);
                uint32_t value_index = ne.value_index;
                CPoolKind kind = ne.value_kind;
                Atom value = pool->getLegalDefaultValue(toplevel, value_index, kind, slotType);
                switch (Traits::getBuiltinType(slotType))
                {
                    case BUILTIN_any:
                    case BUILTIN_object:
                        if (value == 0)
                            continue;
                        break;
#ifdef VMCFG_FLOAT
                    case BUILTIN_float:
                        if (*(int32_t*)atomPtr(value) == 0)
                            continue;
                        break;
                    case BUILTIN_float4:
                    {   
                        int32_t * fptr = (int32_t*)atomPtr(value);
                        if (fptr[0] == 0 && fptr[1] == 0 && fptr[2] == 0 && fptr[3] == 0)
                            continue;
                        break;
                    }
#endif // VMCFG_FLOAT
                    case BUILTIN_number:
                    {
                        double val = AvmCore::number_d(value);
                        if ( val == 0 && !MathUtils::isNegZero(val))
                            continue;
                        break;
                    }
                    case BUILTIN_boolean:
                        AvmAssert((uintptr_t(falseAtom)>>3) == 0);
                        if (value == falseAtom)
                            continue;
                        AvmAssert(value == trueAtom);
                        break;
                    case BUILTIN_uint:
                    case BUILTIN_int:
                        if (value == zeroIntAtom)
                            continue;
                        break;
                    case BUILTIN_namespace:
                    case BUILTIN_string:
                    default:
                        if (AvmCore::isNull(value))
                            continue;
                        break;
                }
                visitor->defaultVal(value, slotid, slotType);
                break;
            }
            }
        } // for i
    }

    void Traits::destroyInstance(ScriptObject* obj) const
    {
        AvmAssert(m_resolved);

        InlineHashtable* ht = m_hashTableOffset ? obj->getTableNoInit() : NULL;

        // start by clearing native space to zero (except baseclasses)
        union {
            char* p_8;
            uint32_t* p;
        };
        p_8 = (char*)obj + sizeof(AvmPlusScriptableObject);
        AvmAssert((uintptr_t(p) % sizeof(uint32_t)) == 0);

        if (!m_slotDestroyInfo.test(0))
        {
            AvmAssert(m_slotDestroyInfo.cap() == 1);
            AvmAssert(m_totalSize >= (sizeof(AvmPlusScriptableObject) + (ht ? sizeof(InlineHashtable) : 0)));
            uint32_t sizeToZero = m_totalSize - (sizeof(AvmPlusScriptableObject) + (ht ? sizeof(InlineHashtable) : 0));
            AvmAssert((sizeToZero % sizeof(uint32_t)) == 0); // we assume all sizes are multiples of sizeof(uint32_t)

            // no RCObjects, so just zero it all... my, that was easy
            VMPI_memset(p, 0, sizeToZero);
        }
        else
        {
            uint32_t sizeToZero = m_sizeofInstance - uint32_t(sizeof(AvmPlusScriptableObject));
            AvmAssert((sizeToZero % sizeof(uint32_t)) == 0); // we assume all sizes are multiples of sizeof(uint32_t)
            VMPI_memset(p, 0, sizeToZero);
            p += (sizeToZero / sizeof(uint32_t));

            AvmAssert(m_slotDestroyInfo.cap() >= 1);
            AvmAssert((uintptr_t(p) % sizeof(uint32_t)) == 0);

            const uint32_t slotAreaSize = getSlotAreaSize();
            const uint32_t bitsUsed = slotAreaSize / sizeof(uint32_t);  // not sizeof(Atom)!
            for (uint32_t bit = 1; bit <= bitsUsed; bit++)
            {
                if (m_slotDestroyInfo.test(bit))
                {
                    #ifdef AVMPLUS_64BIT
                    AvmAssert((uintptr_t(p) & 7) == 0);     // we had better be on an 8-byte boundary...
                    #endif
                    Atom a = *(const Atom*)p;
                    RCObject* rc = NULL;
                    if (atomKind(a) <= kNamespaceType)
                    {
                        rc = (RCObject*)atomPtr(a);
                        if (rc)
                        {
                            AvmAssert(GC::GetGC(obj)->IsRCObject(rc));
                            rc->DecrementRef();
                        }
                    }
                }
                *p++ = 0;
            }
        }

        // finally, zap the hashtable (if any)
        if(ht)
        {
            ht->destroy();
        }
        //for DictionaryObject also zero out the
        //hashtable pointer stored at the offset address;
        if(isDictionary())
        {
            union {
                char* p_8;
                uintptr_t* ptr;
            };
            p_8 = (char*)obj + m_hashTableOffset;
            *ptr = 0;
        }
    }

    void Traits::traceSlots(MMgc::GC* gc, ScriptObject* obj) const
    {
        if (m_slotDestroyInfo.test(0))
        {
            const uint32_t slotAreaSize = getSlotAreaSize();
            union {
                char* p_8;
                uint32_t* p;
            };
            p_8 = (char*)obj + m_sizeofInstance;

            AvmAssert(m_slotDestroyInfo.cap() >= 1);
            AvmAssert((uintptr_t(p) & 3) == 0);
            const uint32_t bitsUsed = slotAreaSize / sizeof(uint32_t);  // not sizeof(Atom)!

            traceSlotsFromBitmap(gc, p, m_slotDestroyInfo, bitsUsed);
        }
    }

    // OPTIMIZEME: We can do better than traceInfo.test, which is probably
    // going to be about a dozen instructions on interesting architectures.
    // A fairly trivial optimization would be to provide an API on FixedBitSet
    // that provides uint32_t values representing sets of 32 bits, and
    // make the loop here doubly nested.  That would also allow us to skip
    // quickly areas of objects that have all non-pointer data.

    void Traits::traceSlotsFromBitmap(MMgc::GC* gc, uint32_t* p, const FixedBitSet& traceInfo, uint32_t bitsUsed) const
    {
        for (uint32_t bit = 1; bit <= bitsUsed; bit++)
        {
            if (traceInfo.test(bit))
            {
#ifdef AVMPLUS_64BIT
                AvmAssert((uintptr_t(p) & 7) == 0); // we had better be on an 8-byte boundary...
#endif
                avmplus::Atom a = *(const avmplus::Atom*)p;
                if (atomKind(a) <= avmplus::AtomConstants::kNamespaceType)
                {
                    // This includes untagged pointers to String, Namespace, ScriptObject.
                    AvmAssert(atomPtr(a) == NULL || gc->IsRCObject(atomPtr(a)));
                    gc->TracePointer(atomPtr(a) HEAP_GRAPH_ARG((uintptr_t*)p));
                }
                else if (atomKind(a) == avmplus::AtomConstants::kDoubleType)
                {
                    AvmAssert(atomPtr(a) != NULL);
                    gc->TracePointer(atomPtr(a) HEAP_GRAPH_ARG((uintptr_t*)p));
                }
                else if (atomKind(a) == avmplus::AtomConstants::kSpecialBibopType && a != undefinedAtom)
                {   
                    AvmAssert(bibopKind(a) == avmplus::AtomConstants::kBibopFloatType || bibopKind(a) == avmplus::AtomConstants::kBibopFloat4Type);
                    gc->TracePointer(atomPtr(a) HEAP_GRAPH_ARG((uintptr_t*)p));
                }
            }
            p++;
        }
    }

    Stringp Traits::formatClassName()
    {
#ifdef VMCFG_CACHE_GQCN
        if (_fullname != NULL)
            return _fullname;
#endif

        Multiname qname(ns(), name());
        qname.setQName();
        StringBuffer buffer(core);
        buffer << qname;
        int length = buffer.length();
        if (length && buffer.c_str()[length-1] == '$')
        {
            length--;
        }
#ifndef VMCFG_CACHE_GQCN
        Stringp _fullname;
#endif
        _fullname = core->newStringUTF8(buffer.c_str(), length);
        return _fullname;
    }


    Binding Traits::getOverride(TraitsBindingsp basetb, Namespacep ns, Stringp name, int tag, const Toplevel* toplevel) const
    {
        Binding baseBinding = BIND_NONE;
        if (base)
        {
            const Namespacep lookupNS = (protectedNamespace == ns && base->protectedNamespace) ? (Namespacep)base->protectedNamespace : ns;
            AvmAssert(basetb != NULL);
            baseBinding = basetb->findBinding(name, lookupNS);
        }
        const BindingKind baseBindingKind = AvmCore::bindingKind(baseBinding);

        const TraitKind kind = TraitKind(tag & 0x0f);
        // some extra-picky compilers will complain about the values being out of
        // range for the comparison (if we use "kind") even with explicit int casting.
        // so recycle the expression for the assert.
        AvmAssert((tag & 0x0f) >= 0 && (tag & 0x0f) < TRAIT_COUNT);

        static const uint8_t kDesiredKind[TRAIT_COUNT] =
        {
            BKIND_NONE,                     // TRAIT_Slot
            BKIND_METHOD,                   // TRAIT_Method
            BKIND_GET,                      // TRAIT_Getter
            BKIND_SET,                      // TRAIT_Setter
            BKIND_NONE,                     // TRAIT_Class
            BKIND_NONE,                     // TRAIT_Function
            BKIND_NONE                      // TRAIT_Const
        };

        const BindingKind desiredKind = BindingKind(kDesiredKind[kind]);
        const uint8_t dkMask = uint8_t(1 << desiredKind);

        // given baseBindingKind, what are legal desiredKinds?
        static const uint8_t kLegalBaseKinds[8] =
        {
            (1<<BKIND_METHOD) | (1<<BKIND_GET) | (1<<BKIND_SET),    // BKIND_NONE
            (1<<BKIND_METHOD),                                      // BKIND_METHOD
            0,                                                      // BKIND_VAR
            0,                                                      // BKIND_CONST
            0,                                                      // unused
            (1<<BKIND_GET) | (1<<BKIND_SET),                        // BKIND_GET
            (1<<BKIND_GET) | (1<<BKIND_SET),                        // BKIND_SET
            (1<<BKIND_GET) | (1<<BKIND_SET)                         // BKIND_GETSET
        };

        // given baseBindingKind, which desiredKinds *require* override?
        static const uint8_t kOverrideRequired[8] =
        {
            0,                                                      // BKIND_NONE
            (1<<BKIND_METHOD),                                      // BKIND_METHOD
            0,                                                      // BKIND_VAR
            0,                                                      // BKIND_CONST
            0,                                                      // unused
            (1<<BKIND_GET),                                         // BKIND_GET
            (1<<BKIND_SET),                                         // BKIND_SET
            (1<<BKIND_GET) | (1<<BKIND_SET)                         // BKIND_GETSET
        };

        if ((kLegalBaseKinds[baseBindingKind] & dkMask) == 0)
            goto failure;

        if (((kOverrideRequired[baseBindingKind] & dkMask) ? ATTR_override : 0) != (tag & ATTR_override))
            goto failure;

        return baseBinding;

failure:

#ifdef AVMPLUS_VERBOSE
        if (pool->isVerbose(VB_traits) || pool->core->config.verifyonly)
            core->console << "illegal override in "<< this << ": " << Multiname(ns,name) <<"\n";
#endif
        if (toplevel)
            toplevel->throwVerifyError(kIllegalOverrideError, toplevel->core()->toErrorString(Multiname(ns,name)), toplevel->core()->toErrorString(this));
        AvmAssert(!"unhandled verify error");
        return BIND_NONE;
    }

    void Traits::ensureNonFinal(MethodInfo* minfo, const Toplevel* toplevel) const
    {
        if (!minfo->isFinal())
            return;
#ifdef AVMPLUS_VERBOSE
        if (pool->isVerbose(VB_traits))
            core->console << "illegal override of final "<< minfo << " in " << this <<"\n";
#endif
        toplevel->throwVerifyError(kIllegalOverrideError, toplevel->core()->toErrorString(minfo), toplevel->core()->toErrorString(this));
        AvmAssert(!"unhandled verify error");
    }

    TraitsBindings* FASTCALL Traits::_getTraitsBindings()
    {
        AvmAssert(m_bindingsVerified);
        TraitsBindings* tb = _buildTraitsBindings(/*toplevel*/NULL, /*includeTypes*/m_resolved);
        return tb;
    }

    TraitsMetadata* FASTCALL Traits::_getTraitsMetadata()
    {
        TraitsMetadata* tm = _buildTraitsMetadata();
        return tm;
    }

    // Count supertypes in the given list, like strlen().
    static uint32_t countSupertypes(Traits** list)
    {
        uint32_t n = 0;
        for (Traits** t = list; *t != NULL; t++)
            n++;
        return n;
    }

    // Initialize the m_primary_supertypes array by copying down the entries
    // from our base class (if it exists), and adding this traits if there is room.
    void Traits::build_primary_supertypes()
    {
        MMGC_STATIC_ASSERT(offsetof(Traits, m_primary_supertypes) + sizeof(m_primary_supertypes) < 256);
        MMGC_STATIC_ASSERT(offsetof(Traits, m_supertype_cache) < 256);

        // compute m_supertype_offset and fill in m_primary_supertypes
        if (!base) {
            // class roots and interfaces
            m_supertype_offset = isInterface() ? offsetof(Traits, m_supertype_cache) : offsetof(Traits, m_primary_supertypes);
            WB(core->gc, this, &m_primary_supertypes[0], this);
        } else {
            // single inherited classes
            AvmAssert(!isInterface());
            for (int i=0; i < MAX_PRIMARY_SUPERTYPE; i++)
                WB(core->gc, this, &m_primary_supertypes[i], base->m_primary_supertypes[i]);
            size_t off = base->m_supertype_offset;
            if (off != offsetof(Traits, m_supertype_cache) &&
                (off += sizeof(Traits*)) - offsetof(Traits, m_primary_supertypes) < sizeof(m_primary_supertypes)) {
                AvmAssert(off == (uint8_t) off);
                m_supertype_offset = uint8_t(off);
                WB(core->gc, this, (Traits*)(uintptr_t(this)+off), this);
            } else {
                // Inheritance is too deep to add this traits to m_primary_supertypes.
                // Make this traits "secondary", set m_supertype_offset to the cache.
                m_supertype_offset = offsetof(Traits, m_supertype_cache);
            }
        }
    }

    // Initialize the m_secondary_supertypes array as follows:
    //   * if this traits adds a new interface, create a new list with all
    //     the entries from the base class, plus the base class itself,
    //     plus the new interfaces
    //   * if we do not add new interfaces and there's no base class,
    //     use the empty list.
    //   * if there is a base class, and the base class is primary, just
    //     copy base->m_secondary_supertypes (containing any base interfaces)
    //   * otherwise create a new secondary_supertypes list with everything
    //     from the base class, plus the base class itself.  Install this list
    //     back in the base class so it can be shared by other leaf classes.
    //
    // allocSupertypeList returns un-scanned memory as we expect the
    // super types to be otherwise reachable to the GC, we also use
    // WB_SKIP to tell the GC we're skipping the WB on purpose.
    void Traits::build_secondary_supertypes()
    {
        MMgc::GC* gc = core->GetGC();
        GCList<Traits> seen(gc, kListInitialCapacity);
        uint32_t count;
        if (!isInstanceType() || (count = countNewInterfaces(seen)) == 0) {
            // no new interfaces, attempt to share the base type's secondary list
            if (!base) {
                WB(gc, this, &this->m_secondary_supertypes, core->_emptySupertypeList);
            } else {
                Traits** base_list = base->get_secondary_supertypes();
                // If we require base in our secondary_supertypes list, so will other
                // sibling leaf types.  Try to share the seconary_supertypes list by
                // inserting base at position 0.
                if (base->isPrimary() || base_list[0] == base) {
                    // just copy the base list.
                    m_secondary_supertypes = base->m_secondary_supertypes;
                } else {
                    // must prepend base to base_list, save the copy on this type and base.
                    count = countSupertypes(base_list);
                    UnscannedTraitsArray* list = allocSupertypeList(gc, count + 1);
                    Traits** raw_list = list->AsArray();
                    WB_SKIP(gc, raw_list, raw_list, base);
                    for (uint32_t i=0; i < count; i++)
                        WB_SKIP(gc, raw_list, raw_list+i+1, base_list[i]);
                    base->m_secondary_supertypes = list;
                    m_secondary_supertypes = list;
                }
            }
        } else {
            // this type implements new interfaces so we need a new list
            this->m_implementsNewInterfaces = true;
            if (base && !base->isPrimary() && base->get_secondary_supertypes()[0] != base) {
                seen.add(base);
                count++;
            }
            UnscannedTraitsArray* list;
            uint32_t baseCount = base ? countSupertypes(base->get_secondary_supertypes()) : 0;
            if (baseCount > 0) {
                uint32_t total = count + baseCount;
                list = allocSupertypeList(gc, total);
                for (Traits **d = list->AsArray(), **s = base->get_secondary_supertypes(); *s != NULL; s++, d++)
                    WB_SKIP(gc, list, d, *s);
            } else {
                list = allocSupertypeList(gc, count);
            }
            m_secondary_supertypes = list;
            Traits** raw_list = list->AsArray();
            for (uint32_t i=0; i < count; i++) {
                WB_SKIP(gc, raw_list, raw_list+baseCount+i, seen[i]);
            }
        }

#ifdef DEBUG
        // sanity check to make sure we don't have any duplicate supertypes.
        GCList<Traits> supertypes(gc, kListInitialCapacity);
        for (int i = 0; i < MAX_PRIMARY_SUPERTYPE; i++) {
            Traits* t = m_primary_supertypes[i];
            if (t != NULL) {
                if (supertypes.indexOf(t) != -1) {
                    core->console << "t " << this << " dup primary " << t << "\n";
                    AvmAssert(false);
                }
                supertypes.add(t);
            }
        }
        for (Traits** st = get_secondary_supertypes(); *st != NULL; st++) {
            Traits* t = *st;
            if (supertypes.indexOf(t) != -1) {
                core->console << "t " << this << " dup secondary " << t << "\n";
                AvmAssert(false);
            }
            supertypes.add(t);
        }
#endif
    }

    // search interfaces and bases that didn't fit in m_primary_supertypes,
    // and cache positive/negative results
    bool Traits::secondary_subtypeof(Traits* t)
    {
        for (Traits** s = get_secondary_supertypes(); *s != NULL; s++) {
            if (t == *s) {
                m_supertype_cache = t;
                return true;
            }
        }
        m_supertype_neg_cache = t;
        return false;
    }

    // create a new supertype list of the given length
    UnscannedTraitsArray* Traits::allocSupertypeList(GC* gc, uint32_t size)
    {
        // This memory is intentionally unscanned, see comments in header.
        return UnscannedTraitsArray::New(gc, size+1);
    }

    // Returns true if a value of type rhs can be assigned to a variable
    // or parameter of type lhs without modifying the representation of the value.
    // It is not just a subtypeof test, for example int is a subtype of Object
    // but they use different representations.
    bool Traits::canAssign(Traits* lhs, Traits* rhs)
    {
        if (!Traits::isMachineCompatible(lhs,rhs)) {
            // no machine type is assignment-compatible with any other
            return false;
        }

        if (!lhs)
            return true;

        // type on right must be same class or subclass of type on left.
        Traits* t = rhs;
        while (t != lhs && t != NULL)
            t = t->base;
        return t != NULL;
    }

    // Return the slot type (if slot), return type (if getter), or NULL.
    Traits* Traits::readBinding(Traits* traits, Binding b)
    {
        AvmAssert((!traits && AvmCore::bindingKind(b) == BKIND_NONE) ||
                  (traits && traits->isResolved()));
        switch (AvmCore::bindingKind(b))
        {
        default:
            AvmAssert(false); // internal error - illegal binding type
        case BKIND_GET:
        case BKIND_GETSET:
        {
            int m = AvmCore::bindingToGetterId(b);
            MethodInfo *f = traits->getTraitsBindings()->getMethod(m);
            MethodSignaturep fms = f->getMethodSignature();
            return fms->returnTraits();
        }
        case BKIND_SET:
            // TODO lookup type here. get/set must have same type.
        case BKIND_NONE:
            // dont know what this is
            // fall through
        case BKIND_METHOD:
            // extracted method or dynamic data, don't know which
            return NULL;
        case BKIND_VAR:
        case BKIND_CONST:
            return traits->getTraitsBindings()->getSlotTraits(AvmCore::bindingToSlotId(b));
        }
    }

}

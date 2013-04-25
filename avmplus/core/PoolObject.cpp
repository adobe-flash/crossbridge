/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

#ifdef VMCFG_NANOJIT
#  include "CodegenLIR.h"
#endif

namespace avmplus
{
    // This assert is here to ensure that "GCDouble" is the same
    // bit value as "double"; GCDouble exists solely to provide
    // a GCObject-descended type to satisfy new constraints on List.
    MMGC_STATIC_ASSERT(sizeof(GCDouble) == sizeof(double));
#ifdef VMCFG_FLOAT
   // same for float4_t/GCFloat4
    MMGC_STATIC_ASSERT(sizeof(GCFloat4) == sizeof(float4_t) && sizeof(GCFloat4)%16 == 0 );
    // GCFloat is trickier: allocations in heap are 8-byte aligned.
    // The static assertions here will verify that
    // GCFloat's size is greater than float, 8-byte aligned, and
    // the "value" is right at the beginning of a "GCFloat" so that it is 
    // safe to reinterpret-cast a GCFLoat* to float*.
    MMGC_STATIC_ASSERT(sizeof(GCFloat)%8==0 && sizeof(GCFloat)>=sizeof(float)
                       && offsetof(GCFloat,value)==0 );
#endif 

    PoolObject::PoolObject(AvmCore* core, ScriptBuffer& sb, const uint8_t* startPos, ApiVersion apiVersion) :
        core(core),
        cpool_int(core->GetGC(), 0),
        cpool_uint(core->GetGC(), 0),
        cpool_double(core->GetGC(), 0),
#ifdef VMCFG_FLOAT
        cpool_float(core->GetGC(), 0),
        cpool_float4(core->GetGC(), 0),
#endif
        cpool_ns(core->GetGC(), 0),
        cpool_ns_set(core->GetGC(), 0),
#ifdef VMCFG_HALFMOON
        cpool_const_double(core->GetGC(), 0),
        cpool_const_string(core->GetGC(), 0),
#endif
#ifndef AVMPLUS_64BIT
        cpool_int_atoms(core->GetGC(), 0),
        cpool_uint_atoms(core->GetGC(), 0),
#endif
        cpool_mn_offsets(core->GetGC(), 0),
        metadata_infos(core->GetGC(), 0),
        m_loadedTraits(MultinameTraitsHashtable::create(core->GetGC())),
        m_cachedTraits(MultinameTraitsHashtable::create(core->GetGC())),
        m_loadedScripts(MultinameMethodInfoHashtable::create(core->GetGC())),
        m_cachedScripts(MultinameMethodInfoHashtable::create(core->GetGC())),
        _code(sb.getImpl()),
        _abcStart(startPos),
        _abcStringStart(NULL),
        _abcStringEnd(NULL),
        _abcStrings(NULL),
        _classes(core->GetGC(), 0),
        _scripts(core->GetGC(), 0),
        _methods(core->GetGC(), 0)
#ifdef DEBUGGER
        , _method_dmi(core->GetGC(), 0)
#endif
        , _method_name_indices(core->GetGC(), 0)
        , _apiVersion(apiVersion)
        , _uniqueId(-1)
#ifdef VMCFG_AOT
        , aotInfo(NULL)
        , aotRoot(NULL)
#endif
    {
        version = AvmCore::readU16(&code()[0]) | AvmCore::readU16(&code()[2])<<16;
        core->addLivePool(this);
    }

    PoolObject::~PoolObject()
    {
        #ifdef VMCFG_AOT
        if (aotRoot)
            delete aotRoot;
        #endif
#ifdef VMCFG_NANOJIT
        mmfx_delete(codeMgr);
#endif
    }

    void PoolObject::dynamicizeStrings()
    {
        if (!MMgc::GC::GetGC(this)->Destroying())
        {
            // make all strings created so far dynamic,
            // making sure that no pointers into ABC data persist
            // (string 0 is always core->kEmptyString: skip it)
            ConstantStringData* dataP = _abcStrings->data;
            for (uint32_t i = 1; i < constantStringCount; i++)
            {
                ++dataP;

                if (dataP->abcPtr >= _abcStringStart && dataP->abcPtr < _abcStringEnd)
                {
                    // it's still a raw ABC ptr, not a String
                    continue;
                }

                Stringp s = dataP->str;

                // in theory, only index 0 should be the empty string... but in practice,
                // any index could be empty, and makeDynamic doesn't work on zero-length strings.
                // since that call doesn't have easy access to an AvmCore, do the check here.
                // (s can be null in the obscure case of a fuzzed ABC)
                if (!s || s->isEmpty())
                {
                    // all zero-length strings should be kEmptyString.
                    AvmAssert(!s || s == core->kEmptyString);
                    continue;
                }

                s->makeDynamic(_abcStringStart, uint32_t(_abcStringEnd - _abcStringStart));
            }
        }
    }

    Namespace* PoolObject::getNamespace(int32_t index) const
    {
        return cpool_ns[index];
    }

    NamespaceSetp PoolObject::getNamespaceSet(int32_t index) const
    {
        return cpool_ns_set[index];
    }

    ////////////////////////////////////////////////////////////////////

    bool ConstantStringContainer::gcTrace(MMgc::GC* gc, size_t cursor)
    {
        gc->TraceLocation(&pool);
        size_t cap = pool->constantStringCount;
        const uint32_t work_increment = 2000/sizeof(void*);
        if (work_increment * cursor >= cap)
            return false;

        size_t work = work_increment;
        bool more = true;
        if (work_increment * (cursor + 1) >= cap) {
            work = cap - (work_increment * cursor);
            more = false;
        }

        const uint8_t *start = pool->_abcStringStart;
        const uint8_t *end = pool->_abcStringEnd;

        // Skip strings into the ABC data, everything else is a real String pointer
        for ( size_t i=0 ; i < work ; i++ ) {
            ConstantStringData& item = data[(work_increment * cursor) + i];
            if (item.abcPtr >= start && item.abcPtr < end)
                continue;
            gc->TraceLocation(&item.str);
        }
        return more;
    }

    void PoolObject::setupConstantStrings(uint32_t count)
    {
        // Always allocate slot 0 in the data array, which will be
        // initialized to an empty String* in AbcParser::parseCpool().
        // We also avoid invoking Calloc() with a size of zero,
        // which will cause an assertion to fail.
        // TODO: If there are no strings, we should be able to set
        // data = NULL, size = 0, and avoid calling the allocator.
        // Slot 0 should never be accessed, and the code guards against
        // such accesses generally, but there are presently exceptions.
        // With the guards spread over much code, it is safer to do
        // as we do here.  This problem extends to other constant pool
        // entry types, and should be cleaned up.  See bug 557684.
        //
        // Be sure to zero the memory or garbage will be touched by
        // the presweep handler that cleans up the string pool, see
        // Bugzilla 574427.
        if (count == 0)
            count = 1;
        // FIXME: worry about overflow in the 'extra' computation
        _abcStrings = ConstantStringContainer::create(core->gc, (count-1) * sizeof(ConstantStringData), this);
        constantStringCount = count;
    }

    Stringp PoolObject::getString(int32_t index) const
    {
        ConstantStringData* dataP = _abcStrings->data + index;
        if (dataP->abcPtr >= _abcStringStart && dataP->abcPtr < _abcStringEnd)
        {
            // String not created yet; grab the pointer to the (verified) ABC data
            uint32_t len = AvmCore::readU32(dataP->abcPtr);
            // strict=false for bug-compatibility with swfs with incorrect utf8 encoding of strings
            Stringp s = core->internStringUTF8((const char*) dataP->abcPtr, len, true, false);
            s->Stick();             // FIXME - Bugzilla 596918:  The Stick() call is dodgy.
            dataP->abcPtr = NULL;   // Important to clear it - what's there is not an RCObject*, don't let WBRC see it
            WBRC(core->gc, _abcStrings, &dataP->str, s);
        }
        return dataP->str;
    }

    /*static*/ bool PoolObject::isLegalDefaultValue(BuiltinType bt, Atom value)
    {
        switch (bt)
        {
            case BUILTIN_any:
                return true;

            case BUILTIN_object:
                return (value != undefinedAtom);

            case BUILTIN_number:
                return AvmCore::isNumber(value);

#ifdef VMCFG_FLOAT
            case BUILTIN_float:
                return AvmCore::isFloat(value);

            case BUILTIN_float4:
                return AvmCore::isFloat4(value);
#endif

            case BUILTIN_boolean:
                return AvmCore::isBoolean(value);

            case BUILTIN_uint:
                if (AvmCore::isDouble(value))
                {
                    const double d = AvmCore::number_d(value);
                    if (d == (uint32_t)d)
                        return true;
                }

                return atomIsIntptr(value) && atomCanBeUint32(value);

            case BUILTIN_int:
                if (AvmCore::isDouble(value))
                {
                    const double d = AvmCore::number_d(value);
                    if (d == (int32_t)d)
                        return true;
                }

                return atomIsIntptr(value) && atomCanBeInt32(value);

            case BUILTIN_string:
                return AvmCore::isNull(value) || AvmCore::isString(value);

            case BUILTIN_namespace:
                return AvmCore::isNull(value) || AvmCore::isNamespace(value);

            default:
                return AvmCore::isNull(value);
        }

        //return false; // unreachable
    }

    Atom PoolObject::getLegalDefaultValue(const Toplevel* toplevel, uint32_t index, CPoolKind kind, Traits* t)
    {
        // toplevel actually can be null, when resolving the builtin classes...
        // but they should never cause verification errors in functioning builds
        //AvmAssert(toplevel != NULL);

        const BuiltinType bt = Traits::getBuiltinType(t);
        uint32_t maxcount = 0;
        Atom value;
        if (index)
        {
            // Look in the cpool specified by kind
            switch (kind)
            {
            case CONSTANT_Int:
            {
                if (index >= (maxcount = constantIntCount))
                    goto range_error;
                const int32_t i = cpool_int[index];
#ifdef AVMPLUS_64BIT
                value = core->intToAtom(i);
                AvmAssert(atomIsIntptr(value) && atomCanBeInt32(value));
#else
                // LIR relies on the return values from this being "sticky" so it can insert them inline.
                // that's true for everything but int/uints that overflow, so special-case them.
                // @todo this can/should go away when we convert to 64-bit Box atoms.
                if (!cpool_int_atoms.length())
                {
                    cpool_int_atoms.ensureCapacity(constantIntCount);
                    for (uint32_t j = 0; j < constantIntCount; ++j)
                        cpool_int_atoms.set(j, 0);
                    AvmAssert(cpool_int_atoms.length() == constantIntCount);
                }
                value = (Atom)cpool_int_atoms[index];
                if (value == 0)
                {
                    value = core->intToAtom(i);
                    if (AvmCore::isDouble(value))
                    {
                        cpool_int_atoms.set(index, value);
                    }
                    AvmAssert(AvmCore::isNumber(value));
                }
                else
                {
                    AvmAssert(AvmCore::isDouble(value));
                }
#endif
                break;
            }
            case CONSTANT_UInt:
            {
                if (index >= (maxcount = constantUIntCount))
                    goto range_error;
                const int32_t u = cpool_uint[index];
#ifdef AVMPLUS_64BIT
                value = core->uintToAtom(u);
                AvmAssert(atomIsIntptr(value) && atomCanBeUint32(value));
#else
                // LIR relies on the return values from this being "sticky" so it can insert them inline.
                // that's true for everything but int/uints that overflow, so special-case them.
                // @todo this can/should go away when we convert to 64-bit Box atoms.
                if (!cpool_uint_atoms.length())
                {
                    cpool_uint_atoms.ensureCapacity(constantUIntCount);
                    for (uint32_t j = 0; j < constantUIntCount; ++j)
                        cpool_uint_atoms.set(j, 0);
                    AvmAssert(cpool_uint_atoms.length() == constantUIntCount);
                }
                value = (Atom)cpool_uint_atoms[index];
                if (value == 0)
                {
                    value = core->uintToAtom(u);
                    if (AvmCore::isDouble(value))
                    {
                        cpool_uint_atoms.set(index, value);
                    }
                    AvmAssert(AvmCore::isNumber(value));
                }
                else
                {
                    AvmAssert(AvmCore::isDouble(value));
                }
#endif
                break;
            }
            case CONSTANT_Double:
                if (index >= (maxcount = constantDoubleCount))
                    goto range_error;
                value = kDoubleType|(uintptr_t)cpool_double[index];
                break;
#ifdef VMCFG_FLOAT
            case CONSTANT_Float:
                if (index >= (maxcount = constantFloatCount))
                    goto range_error;
                value = kSpecialBibopType|(uintptr_t)cpool_float[index];
                break;
            case CONSTANT_Float4:
                if (index >= (maxcount = constantFloat4Count))
                    goto range_error;
                value = kSpecialBibopType|(uintptr_t)cpool_float4[index];
                break;
#endif
            case CONSTANT_Utf8:
                if (index >= (maxcount = constantStringCount))
                    goto range_error;
                value = getString(index)->atom();
                break;

            case CONSTANT_True:
                value = trueAtom;
                break;

            case CONSTANT_False:
                value = falseAtom;
                break;

            case CONSTANT_Namespace:
            case CONSTANT_PackageNamespace:
            case CONSTANT_PackageInternalNs:
            case CONSTANT_ProtectedNamespace:
            case CONSTANT_ExplicitNamespace:
            case CONSTANT_StaticProtectedNs:
            case CONSTANT_PrivateNs:
                if (index >= (maxcount = constantNsCount))
                    goto range_error;
                value = cpool_ns[index]->atom();
                break;

            case CONSTANT_Null:
                value = nullObjectAtom;
                break;

            default:
                // Multinames & NamespaceSets are invalid default values.
                goto illegal_default_error;
            }
        }
        else
        {
            switch (bt)
            {
                case BUILTIN_any:
                    value = undefinedAtom;
                    break;
#ifdef VMCFG_FLOAT
                case BUILTIN_float:  
                    value = core->kFltNaN;
                    break;
                case BUILTIN_float4:  
                    value = core->kFlt4NaN;
                    break;
#endif
                case BUILTIN_number:
                    value = core->kNaN;
                    break;
                case BUILTIN_boolean:
                    value = falseAtom;
                    break;
                case BUILTIN_int:
                case BUILTIN_uint:
                    value = (zeroIntAtom);
                    break;
                case BUILTIN_string:
                    value = nullStringAtom;
                    break;
                case BUILTIN_namespace:
                    value = nullNsAtom;
                    break;
                case BUILTIN_object:
                default:
                    value = nullObjectAtom;
                    break;
            }
        }

        if (!isLegalDefaultValue(bt, value))
            goto illegal_default_error;

        return value;

illegal_default_error:
        if (toplevel)
        {
            if (t)
            {
                toplevel->throwVerifyError(kIllegalDefaultValue, core->toErrorString(Multiname(t->ns(), t->name())));
            }
            else
            {
                toplevel->throwVerifyError(kCorruptABCError);
            }
        }
        AvmAssert(!"unhandled verify error");
        return undefinedAtom; // not reached

range_error:
        if (toplevel)
            toplevel->throwVerifyError(kCpoolIndexRangeError, core->toErrorString(index), core->toErrorString(maxcount));
        AvmAssert(!"unhandled verify error");
        return undefinedAtom; // not reached
    }

    void PoolObject::parseMultiname(const uint8_t *pos, Multiname& m) const
    {
        // the multiname has already been validated so we don't do
        // any checking here, we just fill in the Multiname object
        // with the information we have parsed.

        int32_t index;
        CPoolKind kind = (CPoolKind) *(pos++);
        switch (kind)
        {
        case CONSTANT_Qname:
        case CONSTANT_QnameA:
        {
            // U16 namespace_index
            // U16 name_index
            // parse a multiname with one namespace (aka qname)

            index = AvmCore::readU32(pos);
            if (!index)
                m.setAnyNamespace();
            else
                m.setNamespace(getNamespace(index));

            index = AvmCore::readU32(pos);
            if (!index)
                m.setAnyName();
            else
                m.setName(getString(index));

            m.setQName();
            m.setAttr(kind==CONSTANT_QnameA);
            break;
        }

        case CONSTANT_RTQname:
        case CONSTANT_RTQnameA:
        {
            // U16 name_index
            // parse a multiname with just a name; ns fetched at runtime

            index = AvmCore::readU32(pos);
            if (!index)
                m.setAnyName();
            else
                m.setName(getString(index));

            m.setQName();
            m.setRtns();
            m.setAttr(kind==CONSTANT_RTQnameA);
            break;
        }

        case CONSTANT_RTQnameL:
        case CONSTANT_RTQnameLA:
        {
            m.setQName();
            m.setRtns();
            m.setRtname();
            m.setAttr(kind==CONSTANT_RTQnameLA);
            break;
        }

        case CONSTANT_Multiname:
        case CONSTANT_MultinameA:
        {
            index = AvmCore::readU32(pos);
            if (!index)
                m.setAnyName();
            else
                m.setName(getString(index));

            index = AvmCore::readU32(pos);
            AvmAssert(index != 0);
            m.setNsset(getNamespaceSet(index));
            m.setAttr(kind==CONSTANT_MultinameA);
            break;
        }

        case CONSTANT_MultinameL:
        case CONSTANT_MultinameLA:
        {
            m.setRtname();

            index = AvmCore::readU32(pos);
            AvmAssert(index != 0);
            m.setNsset(getNamespaceSet(index));

            m.setAttr(kind==CONSTANT_MultinameLA);
            break;
        }

        case CONSTANT_TypeName:
        {
            index = AvmCore::readU32(pos);
            // Note that AbcParser ensures that the Multiname we're parsing can't
            // be a CONSTANT_TypeName, thus the recursion here can't be more
            // than a single level deep.
            parseMultiname(_abcStart + cpool_mn_offsets[index], m);
            index = AvmCore::readU32(pos);
            AvmAssert(index==1);
            m.setTypeParameter(AvmCore::readU32(pos));
            break;
        }

        default:
            AvmAssert(false);
        }

        return;
    }

    void PoolObject::resolveBindingNameNoCheck(uint32_t index, Multiname &m, const Toplevel* toplevel) const
    {
        // FIXME: should only assert and not throw, pending correctness verification, see bug https://bugzilla.mozilla.org/show_bug.cgi?id=557541

        if (index == 0 || index >= cpool_mn_offsets.length())
        {
            if (toplevel)
                toplevel->throwVerifyError(kCpoolIndexRangeError, core->toErrorString(index), core->toErrorString(cpool_mn_offsets.length()));
            AvmAssert(!"unhandled verify error");
        }

        parseMultiname(m, index);

        if (!(m.isBinding() && (m.isQName() || isBuiltin)))
        {
            if (toplevel)
                toplevel->throwVerifyError(kCpoolEntryWrongTypeError, core->toErrorString(index));
            AvmAssert(!"unhandled verify error");
        }
    }

    Traits* PoolObject::resolveTypeName(uint32_t index, const Toplevel* toplevel, bool allowVoid/*=false*/)
    {
        // only save the type name for now.  verifier will resolve to traits
        if (index == 0)
        {
            return NULL;
        }

        // check contents is a multiname.  in the cpool, and type system, kObjectType means multiname.
        if (index >= cpool_mn_offsets.length())
        {
            if (toplevel)
                toplevel->throwVerifyError(kCpoolIndexRangeError, core->toErrorString(index), core->toErrorString(cpool_mn_offsets.length()));
            AvmAssert(!"unhandled verify error");
        }

        Multiname m;
        parseMultiname(m, index);

        Traits* t = core->domainMgr()->findTraitsInPoolByMultiname(this, m);
        if (t == (Traits*)BIND_AMBIGUOUS)
        {
            if (toplevel)
                toplevel->throwReferenceError(kAmbiguousBindingError, m);
            AvmAssert(!"unhandled verify error");
        }
        if(m.isParameterizedType())
        {
            core->stackCheck(const_cast<Toplevel*>(toplevel));
            Traits* param_traits = resolveTypeName(m.getTypeParameter(), toplevel);
            t = resolveParameterizedType(toplevel, t, param_traits);
        }
        if (!t)
        {
            #ifdef AVMPLUS_VERBOSE
            if (!toplevel || !toplevel->verifyErrorClass())
                core->console << "class not found: " << m << " index=" << (uint32_t)index << "\n";
            #endif
            if (toplevel)
                toplevel->throwVerifyError(kClassNotFoundError, core->toErrorString(&m));
            AvmAssert(!"unhandled verify error");
        }
        if (!allowVoid && t == VOID_TYPE)
        {
            if (toplevel)
                toplevel->throwVerifyError(kIllegalVoidError);
            AvmAssert(!"unhandled verify error");
        }

        return t;
    }

    Traits* PoolObject::resolveParameterizedType(const Toplevel* toplevel, Traits* base, Traits* param_traits) const
    {
        Traits* r = NULL;
        if (base == core->traits.vector_itraits)
        {
            // Only vector is parameterizable for now...
            switch (Traits::getBuiltinType(param_traits))
            {
                case BUILTIN_any:
                    r = core->traits.vectorobj_itraits;
                    break;
                case BUILTIN_int:
                    r = core->traits.vectorint_itraits;
                    break;
                case BUILTIN_uint:
                    r = core->traits.vectoruint_itraits;
                    break;
#ifdef VMCFG_FLOAT
                case BUILTIN_float: 
                    r = core->traits.vectorfloat_itraits;
                    break;
                case BUILTIN_float4: 
                    r = core->traits.vectorfloat4_itraits;
                    break;
#endif
                case BUILTIN_number:
                    r = core->traits.vectordouble_itraits;
                    break;
                default:
                {
                    PoolObject* pool = param_traits->pool;
                    Stringp fullname = VectorClass::makeVectorClassName(core, param_traits);
                    r = core->domainMgr()->findTraitsInPoolByNameAndNS(pool, fullname, base->ns());
                    if (!r)
                    {
                        r = core->traits.vectorobj_itraits->newParameterizedITraits(fullname, base->ns(), param_traits);
                        r->verifyBindings(toplevel);
                        core->domainMgr()->addNamedTraits(pool, fullname, base->ns(), r);
                    }
                    break;
                }
            }
        }
        return r;
    }

    // search metadata record at meta_pos for name, return true if present
    bool PoolObject::hasMetadataName(const uint8_t* meta_pos, const String* name)
    {
        AvmAssert(meta_pos && name->isInterned());
        uint32_t metadata_count = AvmCore::readU32(meta_pos);
        for (uint32_t i=0; i < metadata_count; i++) {
            uint32_t metadata_index = AvmCore::readU32(meta_pos);
            const uint8_t* metadata_pos = getMetadataInfoPos(metadata_index);
            if (metadata_pos) {
                uint32_t name_index = AvmCore::readU32(metadata_pos);
                AvmCore::skipU32(metadata_pos, 1); // skip val_count
                if (name_index > 0 && name_index < constantStringCount &&
                        getString(name_index) == name)
                    return true;
            }
        }
        return false;
    }

    void PoolObject::initPrecomputedMultinames()
    {
        if (precompNames == NULL)
        {
            MMgc::GC* gc = core->GetGC();
            uint32_t nNames = cpool_mn_offsets.length();
            precompNames = ExactStructContainer<HeapMultiname>::create(gc, destroyPrecomputedMultinames, nNames);
            for (uint32_t i=1; i < nNames; i++) {
                Multiname mn;
                parseMultiname(mn, i);
                precompNames->get(i).setMultiname(gc, precompNames, mn);
            }
        }
    }

    void PoolObject::destroyPrecomputedMultinames(ExactStructContainer<HeapMultiname>* self)
    {
        // Destroy each HeapMultiname to properly decrement reference counts on
        // RC'd parts of HeapMultiname as soon as possible.
        MMgc::GC* gc = MMgc::GC::GetGC(self);
        Multiname mn;
        uint32_t nNames = self->capacity();
        for (uint32_t i=1; i < nNames; i++)
            self->get(i).setMultiname(gc, self, mn);
    }

    Stringp PoolObject::getMethodInfoName(uint32_t i)
    {
        Stringp name = NULL;
        if (core->config.methodNames && (uint32_t(i) < uint32_t(this->_method_name_indices.length())))
        {
            const int32_t index = this->_method_name_indices[i];
            if (index >= 0)
            {
                if (hasString(index))
                    name = this->getString(index);
            }
            else
            {
                // Precomputed multinames may not be inited yet, but we'll need them eventually,
                // so go ahead and init them now
                this->initPrecomputedMultinames();
                const Multiname& mn = precompNames->get(-index);
                StringBuffer sb(core);
                sb << Multiname::Format(&mn);
                name = sb.toString();
            }
        }
        return name;
    }
}
